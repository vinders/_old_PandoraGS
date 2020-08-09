/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <memory>
#include <type_traits>
#include <pattern/class_traits.h>
#include "./memory_pool.h"

namespace pandora {
  namespace memory {
    /// @class MemoryPoolRange
    /// @brief Range manager for memory pool, with object life-time management
    template <typename _DataType, size_t _DataTypeCapacity, typename _MemoryPoolType, size_t _ByteOffset>
    class MemoryPoolRange final {
    public:
      using value_type = _DataType;
      using size_type = size_t;
      using Type = MemoryPoolRange<_DataType,_DataTypeCapacity,_MemoryPoolType,_ByteOffset>;
      static_assert((_DataTypeCapacity > 0u), "MemoryPoolRange: _DataTypeCapacity must be > 0.");
      static_assert((_ByteOffset + _DataTypeCapacity*sizeof(_DataType) <= _MemoryPoolType::size()), "MemoryPoolRange: _ByteOffset + _DataTypeCapacity*(item size) must not exceed the pool capacity.");

      /// @brief Create range manager for a memory pool
      /// @warning Every range manager object MUST be destroyed before the target memory pool!
      MemoryPoolRange(_MemoryPoolType& pool)
        : _indexes(std::make_unique<bool[_DataTypeCapacity]>(_DataTypeCapacity)), _pool(&pool) {}

      MemoryPoolRange(const Type&) = delete;
      MemoryPoolRange(Type&&) = default;
      Type& operator=(const Type&) = delete;
      Type& operator=(Type&&) = default;

      ~MemoryPoolRange() noexcept {
        if (this->_indexes != nullptr && this->_pool != nullptr)
          clear();
      }

      // -- getters --

      static constexpr inline size_t size() noexcept { return _DataTypeCapacity; }   ///< Get full capacity of range (indexes)
      static constexpr inline size_t length() noexcept { return _DataTypeCapacity; } ///< Get full capacity of range (indexes)

      /// @brief Read object data at index
      /// @warning If not existing, nullptr is returned
      inline _DataType* at(size_t index) noexcept {
        assert(index < _DataTypeCapacity);
        if (index < _DataTypeCapacity && this->_indexes[index])
          return *(this->_pool).template at<_DataType,false>(_ByteOffset, index);
        return nullptr;
      }
      inline const _DataType* at(size_t index) const noexcept {
        assert(index < _DataTypeCapacity);
        if (index < _DataTypeCapacity && this->_indexes[index])
          return *(this->_pool).template at<_DataType,false>(_ByteOffset, index);
        return nullptr;
      }

      inline _DataType* operator[](size_t index) noexcept { return at(index); }
      inline const _DataType* operator[](size_t index) const noexcept { return at(index); }


      // -- operations --

      /// @brief Clear entire range
      inline void clear() noexcept {
        for (size_t index = 0; index < _DataTypeCapacity; ++index)
          _destroy(index);
      }
      /// @brief Clear a subset of the range
      inline void clear(size_t offset, size_t length) noexcept {
        length = (length + offset <= _DataTypeCapacity) ? length + offset : _DataTypeCapacity;
        for (size_t index = offset; index < length; ++index)
          _destroy(index);
      }

      /// @brief Fill entire range with a value
      template <typename T = _DataType>
      inline void fill(pandora::pattern::enable_if_copy_constructible<T, const T&> value) {
        for (size_t index = 0; index < _DataTypeCapacity; ++index) {
          _destroy(index);
          *(this->_pool).template createAt<_DataType,false>(_ByteOffset, index, value);
          this->_indexes[index] = true;
        }
      }
      /// @brief Fill a subset of the range with a value
      template <typename T = _DataType>
      inline void fill(pandora::pattern::enable_if_copy_constructible<T, const T&> value, size_t offset, size_t length) {
        length = (length + offset <= _DataTypeCapacity) ? length + offset : _DataTypeCapacity;
        for (size_t index = offset; index < length; ++index) {
          _destroy(index);
          *(this->_pool).template createAt<_DataType,false>(_ByteOffset, index, value);
          this->_indexes[index] = true;
        }
      }

      /// @brief Create object at index
      template <typename T = _DataType>
      inline bool createAt(size_t index, pandora::pattern::enable_if_copy_constructible<T, const T&> value) {
        assert(index < _DataTypeCapacity);
        if (index < _DataTypeCapacity) {
          _destroy(index);
          *(this->_pool).template createAt<_DataType,false>(_ByteOffset, index, value);
          this->_indexes[index] = true;
          return true;
        }
        return false;
      }
      template <typename T = _DataType>
      inline bool createAt(size_t index, pandora::pattern::enable_if_move_constructible<T, T&&> value) {
        assert(index < _DataTypeCapacity);
        if (index < _DataTypeCapacity) {
          _destroy(index);
          *(this->_pool).template createAt<_DataType,false>(_ByteOffset, index, std::move(value));
          this->_indexes[index] = true;
          return true;
        }
        return false;
      }
      /// @brief Create object in place at index
      template <typename... _Args>
      inline bool emplaceAt(size_t index, _Args... args) {
        assert(index < _DataTypeCapacity);
        if (index < _DataTypeCapacity) {
          _destroy(index);
          *(this->_pool).template emplaceAt<_Args...,false>(_ByteOffset, index, std::forward<_Args>(args)...);
          this->_indexes[index] = true;
          return true;
        }
        return false;
      }

      /// @brief Destroy object at index
      inline bool destroyAt(size_t index) noexcept {
        assert(index < _DataTypeCapacity);
        if (index < _DataTypeCapacity && this->_indexes[index]) {
          *(this->_pool).template destroyAt<_DataType,false>(_ByteOffset, index);
          this->_indexes[index] = false;
          return true;
        }
        return false;
      }

    private:
      inline void _destroy(size_t index) noexcept {
        if (this->_indexes[index]) {
          *(this->_pool).template destroyAt<_DataType, false>(_ByteOffset, index);
          this->_indexes[index] = false;
        }
      }

    private:
      std::unique_ptr<bool[_DataTypeCapacity]> _indexes = nullptr;
      _MemoryPoolType* _pool = nullptr;
    };
    
  }
}
