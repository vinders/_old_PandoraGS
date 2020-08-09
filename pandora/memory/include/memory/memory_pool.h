/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <memory>
#include <array>
#include <stdexcept>
#include <type_traits>
#include <pattern/class_traits.h>

namespace pandora {
  namespace memory {
    /// @brief Memory allocation type for memory pool
    enum class MemoryPoolAllocation: uint32_t {
      automatic = 0,
      onStack = 1,
      onHeap = 2,
    };

#   define __P_HEAP_POOL(_bytes,_alloc) (_alloc==MemoryPoolAllocation::onHeap || (_alloc==MemoryPoolAllocation::automatic && _BytesCapacity > 8000u))

    /// ---
    
    /// @class MemoryPool
    /// @brief Fixed-size pre-allocated memory pool, for efficient real-time memory management.
    template <size_t _BytesCapacity, MemoryPoolAllocation _Alloc = MemoryPoolAllocation::automatic>
    class MemoryPool final : public pandora::pattern::ConstructorEnabler<true,__P_HEAP_POOL(_BytesCapacity,_Alloc)>,
                             public pandora::pattern::AssignmentEnabler<true,__P_HEAP_POOL(_BytesCapacity,_Alloc)> {
    public:
      using value_type = uint8_t;
      using size_type = size_t;
      using pool_type = typename std::conditional<__P_HEAP_POOL(_BytesCapacity,_Alloc),
                                                  std::unique_ptr<value_type[]>,
                                                  std::array<value_type,_BytesCapacity> >::type;
      using Type = MemoryPool<_BytesCapacity,_Alloc>;
      static_assert(_BytesCapacity > 0u, "MemoryPool: _BytesCapacity must be > 0.");

      
      MemoryPool() { _allocate(this->_pool); clear(); }
      ~MemoryPool() noexcept = default;
      
      MemoryPool(Type&& rhs) noexcept 
      : pandora::pattern::ConstructorEnabler<true,__P_HEAP_POOL(_BytesCapacity,_Alloc)>(std::move(rhs)),
        _pool(std::move(rhs._pool)) {}
      Type& operator=(Type&& rhs) noexcept { pandora::pattern::AssignmentEnabler<true,__P_HEAP_POOL(_BytesCapacity,_Alloc)>(std::move(rhs));
        this->_pool = std::move(rhs._pool);
      }

      template<bool _HeapAlloc = __P_HEAP_POOL(_BytesCapacity,_Alloc), typename std::enable_if<_HeapAlloc,int>::type = 0>
      void swap(Type& rhs) noexcept { std::swap(this->_pool, rhs._pool); }


      /// @brief Clone content of current instance
      /// @warning If objects are allocated, the cloned objects will be invalidated.
      ///          Only use 'clone()' with pod-types or for debug/trace purpose.
      Type clone() const noexcept { return Type{*this}; }
      
      // -- getters --

      static constexpr inline MemoryPoolAllocation allocationType() noexcept { ///< Actual allocation type
        return (__P_HEAP_POOL(_BytesCapacity,_Alloc)) ? MemoryPoolAllocation::onHeap : MemoryPoolAllocation::onStack;
      }
      
      static constexpr inline size_t size() noexcept { return _BytesCapacity; } ///< Get full capacity of memory pool (bytes)
      template <typename _DataType>
      static constexpr inline size_t length() noexcept { return _BytesCapacity / sizeof(_DataType); } ///< Get object capacity of memory pool

      inline uint8_t* get() noexcept { return &(this->_pool[0]); }             ///< Read raw pool data (bytes)
      inline const uint8_t* get() const noexcept { return &(this->_pool[0]); } ///< Read raw pool data (bytes)

      /// @brief Read raw data (bytes) at a custom position (byte index)
      template <bool _ThrowIfOutOfRange = false>
      inline uint8_t* get(size_t byteIndex) { 
        return (_verifyIndex<_ThrowIfOutOfRange>(byteIndex)) ? &(this->_pool[byteIndex]) : nullptr; 
      }
      template <bool _ThrowIfOutOfRange = false>
      inline const uint8_t* get(size_t byteIndex) const { 
        return (_verifyIndex<_ThrowIfOutOfRange>(byteIndex)) ? &(this->_pool[byteIndex]) : nullptr; 
      }

      /// @brief Read object data at a custom position (byte index)
      template <typename _DataType, bool _ThrowIfOutOfRange = false>
      inline _DataType* get(size_t byteIndex) {
        return (_verifyLength<_ThrowIfOutOfRange>(byteIndex + sizeof(_DataType))) ? reinterpret_cast<_DataType*>(this->_pool[byteIndex]) : nullptr;
      }
      template <typename _DataType, bool _ThrowIfOutOfRange = false>
      inline const _DataType* get(size_t byteIndex) const {
        return (_verifyLength<_ThrowIfOutOfRange>(byteIndex + sizeof(_DataType))) ? reinterpret_cast<_DataType*>(this->_pool[byteIndex]) : nullptr;
      }

      /// @brief Read object data at a custom position (object index)
      template <typename _DataType, bool _ThrowIfOutOfRange = false>
      inline _DataType* at(size_t dataTypeIndex) {
        return (_verifyLength<_ThrowIfOutOfRange>((dataTypeIndex+1u)*sizeof(_DataType))) 
               ? reinterpret_cast<_DataType*>(this->_pool[0]) + dataTypeIndex  : nullptr;
      }
      template <typename _DataType, bool _ThrowIfOutOfRange = false>
      inline const _DataType* at(size_t dataTypeIndex) const  {
        return (_verifyLength<_ThrowIfOutOfRange>((dataTypeIndex+1u)*sizeof(_DataType))) 
               ? reinterpret_cast<_DataType*>(this->_pool[0]) + dataTypeIndex  : nullptr;
      }

      /// @brief Read object data at a custom position (object index) after a custom byte offset
      template <typename _DataType, bool _ThrowIfOutOfRange = false>
      inline _DataType* at(size_t byteOffset, size_t dataTypeIndexAfterOffset) {
        return (_verifyLength<_ThrowIfOutOfRange>(byteOffset + (dataTypeIndexAfterOffset+1u)*sizeof(_DataType))) 
               ? reinterpret_cast<_DataType*>(this->_pool[byteOffset]) + dataTypeIndexAfterOffset  : nullptr;
      }
      template <typename _DataType, bool _ThrowIfOutOfRange = false>
      inline const _DataType* at(size_t byteOffset, size_t dataTypeIndexAfterOffset) const {
        return (_verifyLength<_ThrowIfOutOfRange>(byteOffset + (dataTypeIndexAfterOffset+1u)*sizeof(_DataType))) 
               ? reinterpret_cast<_DataType*>(this->_pool[byteOffset]) + dataTypeIndexAfterOffset  : nullptr;
      }

      inline uint8_t& operator[](size_t byteIndex) noexcept { return *get(byteIndex); }
      inline const uint8_t& operator[](size_t byteIndex) const noexcept { return *get(byteIndex); }

      // -- comparisons --

      /// @brief Compare entire pool with another pool
      template <MemoryPoolAllocation _RhsAlloc>
      inline int32_t compare(const MemoryPool<_BytesCapacity,_RhsAlloc>& rhs) const noexcept {
        return memcmp((void*)&(this->_pool[0]), (void*)&(rhs._pool[0]), size());
      }
      /// @brief Compare subset of pool (byte index) with same subset of another pool
      template <MemoryPoolAllocation _RhsAlloc>
      inline int32_t compare(const MemoryPool<_BytesCapacity,_RhsAlloc>& rhs, size_t offset, size_t length) const noexcept {
        return compare<value_type>(&(rhs._pool[offset]), offset, length);
      }
      /// @brief Compare subset of pool (byte index) with an object value (or array of objects)
      template <typename _DataType>
      inline int32_t compare(const _DataType* rhs, size_t byteIndex, size_t sourceArraySize = 1u) const noexcept {
        assert(rhs != nullptr && byteIndex < size());
        sourceArraySize *= sizeof(_DataType);
        if (byteIndex + sourceArraySize >= size())
          sourceArraySize = size() - byteIndex;
        return memcmp((void*)&(this->_pool[byteIndex]), (void*)rhs, sourceArraySize);
      }
      /// @brief Compare subset of pool (object index) with an object value (or array of objects)
      template <typename _DataType>
      inline int32_t compareAt(const _DataType* rhs, size_t dataTypeIndex, size_t sourceArraySize = 1u) const noexcept {
        assert(rhs != nullptr);
        return compare<_DataType>(rhs, dataTypeIndex*sizeof(_DataType), sourceArraySize);
      }
      /// @brief Compare subset of pool (byte offset + object index) with an object value (or array of objects)
      template <typename _DataType>
      inline int32_t compareAt(const _DataType* rhs, size_t byteOffset, size_t dataTypeIndexAfterOffset, size_t sourceArraySize = 1u) const noexcept {
        assert(rhs != nullptr);
        return compare<_DataType>(rhs, byteOffset + dataTypeIndexAfterOffset*sizeof(_DataType), sourceArraySize);
      }

      template <MemoryPoolAllocation _RhsAlloc>
      inline bool operator==(const MemoryPool<_BytesCapacity,_RhsAlloc>& rhs) const noexcept { return (compare<_RhsAlloc>(rhs) == 0); }
      template <MemoryPoolAllocation _RhsAlloc>
      inline bool operator!=(const MemoryPool<_BytesCapacity,_RhsAlloc>& rhs) const noexcept { return (compare<_RhsAlloc>(rhs) != 0); }
      template <MemoryPoolAllocation _RhsAlloc>
      inline bool operator<(const MemoryPool<_BytesCapacity,_RhsAlloc>& rhs) const noexcept { return (compare<_RhsAlloc>(rhs) < 0); }
      template <MemoryPoolAllocation _RhsAlloc>
      inline bool operator<=(const MemoryPool<_BytesCapacity,_RhsAlloc>& rhs) const noexcept { return (compare<_RhsAlloc>(rhs) <= 0); }
      template <MemoryPoolAllocation _RhsAlloc>
      inline bool operator>(const MemoryPool<_BytesCapacity,_RhsAlloc>& rhs) const noexcept { return (compare<_RhsAlloc>(rhs) > 0); }
      template <MemoryPoolAllocation _RhsAlloc>
      inline bool operator>=(const MemoryPool<_BytesCapacity,_RhsAlloc>& rhs) const noexcept { return (compare<_RhsAlloc>(rhs) >= 0); }

      // -- operations --
      
      /// @brief Clear entire pool
      inline void clear() noexcept {
        memset((void*)&(this->_pool[0]), 0, size());
      }
      /// @brief Clear a subset of the pool
      inline bool clear(size_t byteOffset, size_t byteLength) noexcept {
        if (byteOffset + byteLength < size()) {
          memset((void*)&(this->_pool[byteOffset]), 0, byteLength);
          return true;
        }
        return false;
      }
      /// @brief Fill entire pool with a value
      inline void fill(value_type value) noexcept {
        memset((void*)&(this->_pool[0]), value, size());
      }
      /// @brief Fill a subset of the pool with a value
      inline bool fill(value_type value, size_t byteOffset, size_t byteLength) noexcept {
        if (byteOffset + byteLength < size()) {
          memset((void*)&(this->_pool[byteOffset]), value, byteLength);
          return true;
        }
        return false;
      }
      
      /// @brief Write raw data from another pool (at a custom position) at the same position (byte index)
      template <MemoryPoolAllocation _RhsAlloc>
      inline bool set(const MemoryPool<_BytesCapacity,_RhsAlloc>& sourceData, size_t offset, size_t length) noexcept {
        return set<value_type>(&(sourceData._pool[offset]), offset, length);
      }
      /// @brief Write raw object data (or array) at a custom position (byte index)
      template <typename _DataType>
      inline bool set(const _DataType* sourceData, size_t byteIndex, size_t sourceArraySize = 1u) noexcept {
        assert(sourceData != nullptr && byteIndex + sourceArraySize <= size());
        sourceArraySize *= sizeof(_DataType);
        if (byteIndex + sourceArraySize <= size()) {
          memcpy((void*)&(this->_pool[byteIndex]), (void*)sourceData, sourceArraySize);
          return true;
        }
        return false;
      }
      /// @brief Write raw object data (or array) at a custom position (object index)
      template <typename _DataType>
      inline bool setAt(const _DataType* sourceData, size_t dataTypeIndex, size_t sourceArraySize = 1u) noexcept {
        return set<_DataType>(sourceData, dataTypeIndex*sizeof(_DataType), sourceArraySize);
      }
      /// @brief Write raw object data (or array) at a custom position (object index after byte offset)
      template <typename _DataType>
      inline bool setAt(const _DataType* sourceData, size_t byteOffset, size_t dataTypeIndexAfterOffset, size_t sourceArraySize = 1u) noexcept {
        return set<_DataType>(sourceData, byteOffset + dataTypeIndexAfterOffset*sizeof(_DataType), sourceArraySize);
      }
      
      // -- unmanaged object operations (unsafe!) --
      
      /// @brief Create an instance - copied (byte offset)
      /// @warning Will not be destroyed automatically!
      template <typename _DataType, bool _ThrowIfOutOfRange = false>
      inline bool create(size_t byteIndex, const _DataType& value) { return _emplace(get<_DataType,_ThrowIfOutOfRange>(byteIndex), value); }
      /// @brief Create an instance - copied (object index)
      /// @warning Will not be destroyed automatically!
      template <typename _DataType, bool _ThrowIfOutOfRange = false>
      inline bool createAt(size_t dataTypeIndex, const _DataType& value) { return _emplace(at<_DataType,_ThrowIfOutOfRange>(dataTypeIndex), value); }
      /// @brief Create an instance - copied (object index after byte offset)
      /// @warning Will not be destroyed automatically!
      template <typename _DataType, bool _ThrowIfOutOfRange = false>
      inline bool createAt(size_t byteOffset, size_t dataTypeIndexAfterOffset, const _DataType& value) {
        return _emplace(at<_DataType,_ThrowIfOutOfRange>(byteOffset, dataTypeIndexAfterOffset), value);
      }

      /// @brief Create an instance - moved (byte offset)
      /// @warning Will not be destroyed automatically!
      template <typename _DataType, bool _ThrowIfOutOfRange = false>
      inline bool create(size_t byteIndex, _DataType&& value) { return _emplace(get<_DataType,_ThrowIfOutOfRange>(byteIndex), std::forward(value)); }
      /// @brief Create an instance - moved (object index)
      /// @warning Will not be destroyed automatically!
      template <typename _DataType, bool _ThrowIfOutOfRange = false>
      inline bool createAt(size_t dataTypeIndex, _DataType&& value) { return _emplace(at<_DataType,_ThrowIfOutOfRange>(dataTypeIndex), std::forward(value)); }
      /// @brief Create an instance - moved (object index after byte offset)
      /// @warning Will not be destroyed automatically!
      template <typename _DataType, bool _ThrowIfOutOfRange = false>
      inline bool createAt(size_t byteOffset, size_t dataTypeIndexAfterOffset, _DataType&& value) {
        return _emplace(at<_DataType,_ThrowIfOutOfRange>(byteOffset, dataTypeIndexAfterOffset), std::forward(value));
      }

      /// @brief Create an instance in place (byte offset)
      /// @warning Will not be destroyed automatically!
      template <typename _DataType, typename ... _Args, bool _ThrowIfOutOfRange = false>
      bool emplace(size_t byteIndex, _Args... args) { return _emplace(get<_DataType,_ThrowIfOutOfRange>(byteIndex), std::forward<_Args>(args)...); }
      /// @brief Create an instance in place (object index)
      /// @warning Will not be destroyed automatically!
      template <typename _DataType, typename ... _Args, bool _ThrowIfOutOfRange = false>
      bool emplaceAt(size_t dataTypeIndex, _Args... args) { return _emplace(at<_DataType,_ThrowIfOutOfRange>(dataTypeIndex), std::forward<_Args>(args)...); }
      /// @brief Create an instance in place (object index after byte offset)
      /// @warning Will not be destroyed automatically!
      template <typename _DataType, typename ... _Args, bool _ThrowIfOutOfRange = false>
      bool emplaceAt(size_t byteOffset, size_t dataTypeIndexAfterOffset, _Args... args) {
        return _emplace(at<_DataType,_ThrowIfOutOfRange>(byteOffset, dataTypeIndexAfterOffset), std::forward<_Args>(args)...);
      }

      /// @brief Destroy current instance (if any) (byte offset)
      /// @warning Will not verify if the instance exists!
      template <typename _DataType, bool _ThrowIfOutOfRange = false>
      inline bool destroy(size_t byteIndex) { return _destroy(get<_DataType,_ThrowIfOutOfRange>(byteIndex)); }
      /// @brief Destroy current instance (if any) (object index)
      /// @warning Will not verify if the instance exists!
      template <typename _DataType, bool _ThrowIfOutOfRange = false>
      inline bool destroyAt(size_t dataTypeIndex) { return _destroy(at<_DataType,_ThrowIfOutOfRange>(dataTypeIndex)); }
      /// @brief Destroy current instance (if any) (object index after byte offset)
      /// @warning Will not verify if the instance exists!
      template <typename _DataType, bool _ThrowIfOutOfRange = false>
      inline bool destroyAt(size_t byteOffset, size_t dataTypeIndexAfterOffset) {
        return _destroy(at<_DataType,_ThrowIfOutOfRange>(byteOffset, dataTypeIndexAfterOffset));
      }
      
      
    private:
      MemoryPool(const Type& rhs) { 
        _allocate(this->_pool); 
        memcpy((void*)&(this->_pool[0]), &(rhs._pool[0]), size()); 
      }
      Type& operator=(const Type& rhs) { memcpy((void*)&(this->_pool[0]), &(rhs._pool[0]), size()); }

      static inline void _allocate(const std::array<value_type, _BytesCapacity>&) {} // stack: no allocation
      static inline void _allocate(std::unique_ptr<value_type[]>& pool) { // heap: dynamic allocation
        pool = nullptr;
        pool = std::make_unique<value_type[]>(_BytesCapacity);
      }

      template <bool _ThrowIfOutOfRange>
      inline bool _verifyIndex(size_t byteIndex) {
        if (byteIndex >= size()) {
          if (_ThrowIfOutOfRange)
            throw std::out_of_range("MemoryPool: index out of range");
          assert(false);
          return false;
        }
        return true;
      }
      template <bool _ThrowIfOutOfRange>
      inline bool _verifyLength(size_t byteLength) {
        if (byteLength > size()) {
          if (_ThrowIfOutOfRange)
            throw std::out_of_range("MemoryPool: position out of range");
          assert(false);
          return false;
        }
        return true;
      }

      template <typename _DataType, typename ... _Args>
      inline bool _emplace(_DataType* destination, _Args... args) {
        if (destination != nullptr) {
          new(destination) _DataType(std::forward<_Args>(args)...);
          return true;
        }
        return false;
      }
      template <typename _DataType>
      inline bool _destroy(_DataType* destination) { 
        if (destination != nullptr) {
          (*destination).~_DataType();
          return true;
        }
        return false;
      }

    private:
      pool_type _pool;
    };
    
  }
}
#undef __P_HEAP_POOL
