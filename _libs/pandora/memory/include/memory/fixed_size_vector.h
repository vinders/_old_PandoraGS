/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <array>
#include <initializer_list>
#include <type_traits>
#include <stdexcept>
#include <pattern/optional.h>
#include <pattern/iterator.h>
#include <pattern/class_traits.h>
#if !defined(_CPP_REVISION) || _CPP_REVISION != 14
# define __if_constexpr if constexpr
#else
# define __if_constexpr if
#endif

namespace pandora {
  namespace memory {
    /// @class FixedSizeVector
    /// @brief A container that behaves like a vector with a max length, without involving any dynamic allocation.
    ///        Useful for real-time apps or for any vector with a fixed max size. The vector is STL-compliant.
    template <typename _DataType,  // Data type for vector items.
              size_t _Capacity,    // Max number of items that may be contained in the fixed-size vector.
              bool _ThrowWhenFull = false> // If vector is already full, throw an exception (true) or ignore additional data (false)
    class FixedSizeVector final : public pandora::pattern::ConstructorSelector<_DataType>, public pandora::pattern::AssignmentSelector<_DataType> {
    public:
      using value_type = _DataType;
      using item_type = typename std::conditional<std::is_class<_DataType>::value,  pandora::pattern::Optional<_DataType>, _DataType >::type;
      using size_type = size_t;
      using reference = _DataType&;
      using const_reference = const _DataType&;
      using pointer = _DataType*;
      using const_pointer = const _DataType*;
      using Type = FixedSizeVector<_DataType,_Capacity,_ThrowWhenFull>;
      static_assert((_Capacity > 0u), "FixedSizeVector: _Capacity can't be 0.");

      /// @brief Create empty vector
      FixedSizeVector() = default;
      /// @brief Destroy vector and all child instances
      ~FixedSizeVector() { clear(); }

      FixedSizeVector(const Type& rhs) : pandora::pattern::ConstructorSelector<_DataType>(rhs)       { _assign<false,_Capacity,_DataType>(rhs); }
      FixedSizeVector(Type&& rhs) : pandora::pattern::ConstructorSelector<_DataType>(std::move(rhs)) { _assign<_DataType>(std::move(rhs)); }
      Type& operator=(const Type& rhs) { pandora::pattern::AssignmentSelector<_DataType>::operator=(rhs);
        clear(); 
        _assign<false,_Capacity,_DataType>(rhs); 
        return *this;
      }
      Type& operator=(Type&& rhs) { pandora::pattern::AssignmentSelector<_DataType>::operator=(std::move(rhs));
        clear();
        _assign<_DataType>(std::move(rhs)); 
        return *this; 
      }

      /// @brief Copy a vector with different capacity
      /// @warning If (vector.size() > _Capacity): throws if _ThrowWhenFull, or uses max size
      template <size_t _RhsCapacity, typename T = _DataType>
      FixedSizeVector(pandora::pattern::enable_if_copy_constructible<T, const FixedSizeVector<T,_RhsCapacity>&> rhs) { _assign<true,_RhsCapacity,_DataType>(rhs); }
      template <size_t _RhsCapacity, typename T = _DataType>
      Type& operator=(pandora::pattern::enable_if_copy_constructible<T, const FixedSizeVector<T,_RhsCapacity>&> rhs) { 
        clear(); 
        _assign<true,_RhsCapacity,_DataType>(rhs);
        return *this; 
      }

      template <typename T = _DataType>
      void swap(pandora::pattern::enable_if_move_constructible<T, FixedSizeVector<T,_Capacity>&> rhs) {
        Type buffer(std::move(*this)); 
        *this = std::move(rhs);
        rhs = std::move(buffer);
      }
      template <typename T = _DataType>
      void swap(pandora::pattern::enable_if_copy_not_move_constructible<T, FixedSizeVector<T,_Capacity>&> rhs) {
        Type buffer(*this); 
        *this = rhs;
        rhs = buffer;
      }

      // -- special constructors --

      /// @brief Create a vector initialized with a value repeated N times (N = 'count')
      /// @warning If (count > _Capacity): throws if _ThrowWhenFull, or uses max size
      template <typename T = _DataType>
      explicit FixedSizeVector(size_t count, pandora::pattern::enable_if_copy_constructible<T, const T&> value) { assign<_DataType>(count, value); }
      /// @brief Create a vector initialized with a range of iterators ('end' is excluded)
      /// @warning If (size > _Capacity): throws if _ThrowWhenFull, or uses max size
      template <typename _Iterator, typename T = _DataType, pandora::pattern::enable_if_copy_constructible<T,int> = 0>
      FixedSizeVector(pandora::pattern::enable_if_iterator<_Iterator, _Iterator> first, _Iterator end) { assign<_Iterator,_DataType>(first, end); }

      /// @brief Create a vector with an initializer list
      /// @warning If (init.size() > _Capacity): throws if _ThrowWhenFull, or uses max size
      template <typename T = _DataType>
      FixedSizeVector(pandora::pattern::enable_if_move_constructible<T, std::initializer_list<T> > init) {
        auto endIt = _verifyFixEndIterator(init.begin(), init.end(), init.size(), _Capacity);
        for (auto it = init.begin(); it != endIt; ++it)
          _emplace<false>(std::move(*it));
      }
      template <typename T = _DataType>
      FixedSizeVector(pandora::pattern::enable_if_copy_not_move_constructible<T, std::initializer_list<T> > init) {
        auto endIt = _verifyFixEndIterator(init.begin(), init.end(), init.size(), _Capacity);
        for (auto it = init.begin(); it != endIt; ++it)
          _emplace<false>(*it);
      }


      // -- getters --

      inline const _DataType* data() const noexcept { return &(_at(0)); } ///< Get pointer to raw vector data

      inline bool empty() const noexcept { return (this->_size == 0u); } ///< Check if the vector is empty
      inline size_t size() const noexcept { return this->_size; }        ///< Get current size of the vector
      constexpr inline bool full() const noexcept  { return (this->_size == _Capacity); } ///< Check if the max capacity of the vector has been reached

      inline size_t max_size() const noexcept { return _Capacity; } ///< Get fixed max capacity of the vector
      inline size_t capacity() const noexcept { return _Capacity; } ///< Get fixed max capacity of the vector

      inline _DataType& operator[](size_t index) noexcept { assert(index < this->_size); return _at(index); }             ///< Read value at a custom index (not verified!)
      inline const _DataType& operator[](size_t index) const noexcept { assert(index < this->_size); return _at(index); } ///< Read value at a custom index (not verified!) - const
      inline _DataType& at(size_t index) { ///< Read value at a custom index, with index verification
        if (index < this->_size)
          return _at(index);
        _throwOutOfRange();
      }
      inline const _DataType& at(size_t index) const { ///< Read value at a custom index, with index verification - const
        if (index < this->_size)
          return _at(index);
        _throwOutOfRange();
      }

      inline _DataType& front() noexcept { assert(!empty()); return _at(0); }             ///< Read first value of the vector (not verified -> check empty() !)
      inline const _DataType& front() const noexcept { assert(!empty()); return _at(0); } ///< Read first value of the vector (not verified -> check empty() !) - const
      inline _DataType& back() noexcept { assert(!empty()); return _at(this->_size - 1u); }             ///< Read last value of the vector (not verified -> check empty() !)
      inline const _DataType& back() const noexcept { assert(!empty()); return _at(this->_size - 1u); } ///< Read last value of the vector (not verified -> check empty() !)

      _P_RANDOMACCESS_ITERATOR_MEMBERS(Type, _DataType, _first(), _last());

      // -- comparisons --

      template <typename T = _DataType>
      inline bool operator==(pandora::pattern::enable_if_operator_equals<T, const FixedSizeVector<T,_Capacity,_ThrowWhenFull>&> rhs) const noexcept {
        if (this->_size == rhs._size) {
          for (const _DataType* it = this->_value[this->_size - 1u], rhsIt = rhs._value[rhs._size - 1u]; it >= &(this->_value[0]); --it, --rhsIt)
            if (_at(it) != _at(rhsIt))
              return false;
          return true;
        }
        return false;
      }
      template <typename T = _DataType>
      inline bool operator!=(pandora::pattern::enable_if_operator_equals<T, const FixedSizeVector<T,_Capacity,_ThrowWhenFull>&> rhs) const noexcept { 
        return !(this->operator==(rhs)); 
      }


      // -- change size --

      /// @brief Remove all vector items
      inline void clear() noexcept {
        while (_removeLast<true>());
      }
      /// @brief Change size of vector - add default values to reach new size or remove last items
      /// @warning If (length > _Capacity): throws if _ThrowWhenFull, or uses max size
      template <typename T = _DataType>
      inline void resize(size_t length, pandora::pattern::enable_if_copy_constructible<T, const T&> defaultValue) {
        _verifyFixLength(length, _Capacity);
        while (length < this->_size)
          _removeLast<false>();
        while (length > this->_size) {
          _emplace<false>(defaultValue);
          --length;
        }
      }

      // -- assignment --

      /// @brief Clear vector and assign a value repeated N times (N = 'count')
      /// @warning If (count > _Capacity): throws if _ThrowWhenFull, or uses max size
      /// @returns New size of vector
      template <typename T = _DataType>
      inline size_t assign(size_t count, pandora::pattern::enable_if_copy_constructible<T, const T&> value) {
        _verifyFixLength(count, _Capacity);
        clear();
        while (count > 0u) {
          _emplace<false>(value);
          --count;
        }
        return this->_size;
      }

      /// @brief Clear vector and assign a range of iterators ('end' is excluded)
      /// @warning If (size > _Capacity): throws if _ThrowWhenFull, or uses max size
      /// @returns New size of vector
      template <typename _Iterator, typename T = _DataType, pandora::pattern::enable_if_copy_constructible<T,int> = 0>
      inline size_t assign(pandora::pattern::enable_if_iterator<_Iterator, _Iterator> first, _Iterator end) {
        end = _verifyFixEndIterator(first, end, std::distance(first, end), _Capacity);
        clear();
        while (first < end) {
          _emplace<false>(*first);
          ++first;
        }
        return this->_size;
      }

      // -- append --

      /// @brief Add a value at the end of the vector
      /// @warning If vector already full: returns false
      template <typename T = _DataType>
      inline bool push_back(pandora::pattern::enable_if_copy_constructible<T, const T&> value) { return _emplace<true>(value); }
      template <typename T = _DataType>
      inline bool push_back(pandora::pattern::enable_if_move_constructible<T, _DataType&&> value) { return _emplace<true>(std::move(value)); }
      /// @brief Create a value in place at the end of the vector
      /// @warning If vector already full: returns false
      template<class... Args>
      inline bool emplace_back(Args&&... args) { return _emplace<true>(std::forward<Args>(args)...); }

      // -- insert --

      /// @brief Insert value at the position identified by an iterator
      /// @warning If vector already full: throws if _ThrowWhenFull, or returns end()
      template <bool _ThrowIfOutOfRange = true, typename T = _DataType>
      inline iterator insert(const_iterator position, pandora::pattern::enable_if_copy_constructible<T, const T&> value) {
        if (_emplaceInsert<true,true,_ThrowIfOutOfRange,true>(position._index, value) || full())
          return iterator(*this, &(_at(position._index)), position._index);
        return end();
      }
      template <bool _ThrowIfOutOfRange = true, typename T = _DataType>
      inline iterator insert(const_iterator position, pandora::pattern::enable_if_move_constructible<T, T&&> value) {
        if (_emplaceInsert<true,true, _ThrowIfOutOfRange, true>(position._index, std::move(value) || full()))
          return iterator(*this, &(_at(position._index)), position._index);
        return end();
      }
      /// @brief Create value in place at the position identified by an iterator
      /// @warning If vector already full: throws if _ThrowWhenFull, or returns end()
      template <bool _ThrowIfOutOfRange = true, typename... Args>
      inline iterator emplace(const_iterator position, Args&&... args) {
        if (_emplaceInsert<true,true,_ThrowIfOutOfRange,true>(std::forward<Args>(args)...) || full())
          return iterator(*this, &(_at(position._index)), position._index);
        return end();
      }

      /// @brief Insert value repeated N times (N = 'count') at the position identified by an iterator
      /// @warning If vector already full: throws if _ThrowWhenFull, or returns end()
      template <bool _ThrowIfOutOfRange = true, typename T = _DataType>
      inline iterator insert(const_iterator position, size_t count, pandora::pattern::enable_if_copy_constructible<T, const T&> value) {
        _verifyFixLength(count, _Capacity - this->_size);
        if (position._index < this->_size) {
          if (count > 0u) {
            _rshiftItems(position._index, count);
            while (count > 0u) {
              _emplaceInsert<false,false,false,false>(position._index, value);
              --count;
            }
          }
          return iterator(*this, &(_at(position._index)), position._index);
        }
        if (_ThrowIfOutOfRange)
          _throwOutOfRange();
        return end();
      }
      /// @brief Insert a range of iterators at the position identified by an iterator ('end' is excluded)
      /// @warning If vector already full: throws if _ThrowWhenFull, or returns end()
      template <typename _Iterator, bool _ThrowIfOutOfRange = true, typename T = _DataType, pandora::pattern::enable_if_copy_constructible<T,int> = 0>
      inline iterator insert(const_iterator position, pandora::pattern::enable_if_iterator<_Iterator, _Iterator> first, _Iterator end) {
        size_t count = std::distance(first,end);
        end = _verifyFixEndIterator<_Iterator>(first, end, count, _Capacity - this->_size);
        if (position._index < this->_size && first < end) {
          _rshiftItems(position._index, count);
          while (first < end) {
            _emplaceInsert<false,false,false,false>(position._index, *first);
            ++first;
          }
          return iterator(*this, &(_at(position._index)), position._index);
        }
        if (_ThrowIfOutOfRange)
          _throwOutOfRange();
        return end();
      }

      /// @brief Insert initializer list of values at the position identified by an iterator
      /// @warning If vector already full: throws if _ThrowWhenFull, or returns end()
      template <bool _ThrowIfOutOfRange = true, typename T = _DataType>
      inline iterator insert(const_iterator position, pandora::pattern::enable_if_move_constructible<T, std::initializer_list<T> > init) {
        auto endIt = _verifyFixEndIterator(init.begin(), init.end(), init.size(), _Capacity - this->_size);
        if (position._index < this->_size) {
          _rshiftItems(position._index, init.size());
          for (auto it = init.begin(); it != endIt; ++it)
            _emplaceInsert<false,false,false,false>(position._index, std::move(*it));
          return iterator(*this, &(_at(position._index)), position._index);
        }
        if (_ThrowIfOutOfRange)
          _throwOutOfRange();
        return end();
      }
      template <bool _ThrowIfOutOfRange = true, typename T = _DataType>
      inline iterator insert(const_iterator position, pandora::pattern::enable_if_copy_not_move_constructible<T, std::initializer_list<T> > init) {
        auto endIt = _verifyFixEndIterator(init.begin(), init.end(), init.size(), _Capacity - this->_size);
        if (position._index < this->_size) {
          _rshiftItems(position._index, init.size());
          for (auto it = init.begin(); it != endIt; ++it)
            _emplaceInsert<false,false,false,false>(position._index, *it);
          return iterator(*this, &(_at(position._index)), position._index);
        }
        if (_ThrowIfOutOfRange)
          _throwOutOfRange();
        return end();
      }

      // -- erase --

      /// @brief Remove last vector item
      inline bool pop_back() noexcept { return _removeLast<true>(); }

      /// @brief Remove vector item at the position identified by an iterator
      template <bool _ThrowIfOutOfRange = false, typename T = _DataType, pandora::pattern::enable_if_copy_or_move_constructible<T,int> = 0>
      inline iterator erase(const_iterator position) {
        if (_remove<true,true>(position._index))
          return iterator(*this, (position._index < this->_size) ? &(_at(position._index)) : nullptr, position._index);
        if (_ThrowIfOutOfRange)
          _throwOutOfRange();
        return end();
      }
      /// @brief Remove vector items located in a range of iterators ('end' is excluded)
      template <bool _ThrowIfOutOfRange = false, typename T = _DataType, pandora::pattern::enable_if_copy_or_move_constructible<T,int> = 0>
      iterator erase(const_iterator first, const_iterator end) {
        if (first < end && (end - first) <= (_Capacity - this->_size)) {
          int32_t firstIndex = first._index;
          int32_t endIndex = first._index + (end - first);
          while (first < end) {
            _remove<false,false>(first._index);
            ++first;
          }
          _lshiftItems(endIndex, static_cast<size_t>(endIndex - firstIndex));
          return iterator(*this, (firstIndex < this->_size) ? &(_at(firstIndex)) : nullptr, firstIndex);
        }
        if (_ThrowIfOutOfRange)
          _throwOutOfRange();
        return end();
      }

      // -- iterators --

      _DataType* next(_DataType* current, uint32_t currentIndex, size_t offset = 1u) { return _next(current, currentIndex, offset); }
      const _DataType* next(const _DataType* current, uint32_t currentIndex, size_t offset = 1u) const { return _next(current, currentIndex, offset); }
      _DataType* previous(_DataType* current, uint32_t currentIndex, size_t offset = 1u) { return _previous(current, currentIndex, offset); }
      const _DataType* previous(const _DataType* current, uint32_t currentIndex, size_t offset = 1u) const { return _previous(current, currentIndex, offset); }


    private:
      static inline void _throwCapacityExceeded() { throw std::out_of_range("FixedSizeVector: total size exceeds max capacity."); }
      static inline void _throwOutOfRange() { throw std::out_of_range("FixedSizeVector: invalid index (out of range)."); }

      // -- private - conditional getters --

      inline _DataType& _at(size_t index) noexcept { return Type::_getValue(this->_value[index]); }
      inline const _DataType& _at(size_t index) const noexcept { return Type::_getValue(this->_value[index]); }

      static inline _DataType& _getValue(pandora::pattern::Optional<_DataType>& location) noexcept { return location->value(); }
      static inline const _DataType& _getValue(const pandora::pattern::Optional<_DataType>& location) noexcept { return location->value(); }
      static inline _DataType& _getValue(_DataType& location) noexcept { return location; }
      static inline const _DataType& _getValue(const _DataType& location) noexcept { return location; }

      // -- private - conditional setters --

      template <typename T = _DataType, typename... Args>
      inline void _emplaceAt(typename std::enable_if<std::is_class<T>::value, size_t>::type index, Args&&... args) {
        this->_value[index].emplace(std::forward<Args>(args)...);
      }
      template <typename T = _DataType, typename... Args>
      inline void _emplaceAt(typename std::enable_if<!std::is_class<T>::value, size_t>::type index, Args&&... args) {
        this->_value[index] = T(std::forward<Args>(args)...);
      }
      
      template <typename T = _DataType, typename std::enable_if<std::is_class<T>::value && std::is_move_constructible<T>::value,int>::type = 0>
      inline void _moveItem(size_t fromIndex, size_t toIndex) { this->_value[toIndex] = std::move(this->_value[fromIndex].value()); }
      template <typename T = _DataType, typename std::enable_if<!std::is_class<T>::value || !std::is_move_constructible<T>::value,int>::type = 0>
      inline void _moveItem(size_t fromIndex, size_t toIndex) { this->_value[toIndex] = this->_value[fromIndex]; }

      template <typename T = _DataType, typename std::enable_if<std::is_class<T>::value, int>::type = 0>
      inline void _clearAt(size_t index) { this->_value[index].reset(); }
      template <typename T = _DataType, typename std::enable_if<!std::is_class<T>::value, int>::type = 0>
      inline void _clearAt(size_t index) noexcept { this->_value[index] = (T)0; }

      // -- private - operations --

      inline void _lshiftItems(size_t firstIndex, size_t offset) {
        size_t lastIndex = this->_size - 1u;
        while (firstIndex <= lastIndex) {
          _moveItem(firstIndex, firstIndex - offset);
          ++firstIndex;
        }
      }
      inline void _rshiftItems(size_t firstIndex, size_t offset) {
        size_t lastIndex = this->_size - 1u;
        while (lastIndex >= firstIndex) {
          _moveItem(lastIndex, lastIndex + offset);
          --lastIndex;
        }
      }

      template <bool _DoSizeCheck, size_t _RhsCapacity, typename T = _DataType>
      inline void _assign(pandora::pattern::enable_if_copy_constructible<T, const FixedSizeVector<T,_RhsCapacity>&> rhs) {
        __if_constexpr (_DoSizeCheck) {
          size_t count = rhs.size();
          _verifyFixLength(count, _Capacity);
          for (size_t index = 0u; index < count; ++index)
            this->_value[index] = rhs._value[index];
          this->_size = count;
        }
        else {
          for (size_t index = 0u; index < rhs.size(); ++index)
            this->_value[index] = rhs._value[index];
          this->_size = rhs.size();
        }
      }
      template <typename T = _DataType>
      inline void _assign(pandora::pattern::enable_if_move_constructible<T, FixedSizeVector<T,_Capacity>&&> rhs) {
        for (size_t index = 0u; index < rhs._size; ++index)
          this->_value[index] = std::move(rhs._value[index]);
        this->_size = rhs._size;
        rhs._size = 0;
      }

      template<bool _DoSizeCheck, typename... Args>
      inline bool _emplace(Args&&... args) {
        __if_constexpr (_DoSizeCheck) {
          if (this->_size >= _Capacity) {
            __if_constexpr (_ThrowWhenFull)
              _throwCapacityExceeded();
            return false;
          }
        }
        _emplaceAt<_DataType>(this->_size, std::forward<Args>(args)...);
        ++(this->_size);
        return true;
      }

      template <bool _DoSizeCheck, bool _DoIndexCheck, bool _ThrowIfOutOfRange, bool _DoShiftNext, typename... Args>
      inline bool _emplaceInsert(size_t index, const _DataType& value, Args&&... args) {
        __if_constexpr (_DoSizeCheck) {
          if (this->_size >= _Capacity) {
            __if_constexpr (_ThrowWhenFull)
              _throwCapacityExceeded();
            return false;
          }
        }
        __if_constexpr (_DoIndexCheck) {
          if (index >= this->_size) {
            __if_constexpr (_ThrowIfOutOfRange)
              _throwOutOfRange();
            return false;
          }
        }
        __if_constexpr (_DoShiftNext)
          _rshiftItems(index, 1u);
        _emplaceAt<_DataType>(index, std::forward<Args>(args)...);
        ++(this->_size);
        return true;
      }

      template <bool _DoSizeCheck>
      inline bool _removeLast() noexcept {
        __if_constexpr (_DoSizeCheck) {
          if (this->_size == 0u)
            return false;
        }
        --(this->_size);
        _clearAt<_DataType>(this->_size);
        return true;
      }

      template <bool _DoIndexCheck, bool _DoShiftNext, typename pandora::pattern::enable_if_copy_or_move_constructible<_DataType> = 0>
      inline bool _remove(size_t index) noexcept {
        __if_constexpr (_DoIndexCheck) {
          if (index >= this->_size)
            return false;
        }
        _clearAt<_DataType>(index);
        __if_constexpr (_DoShiftNext)
          _lshiftItems(index + 1u, 1u)
        --(this->_size);
        return true;
      }

      // -- private - length verifications --

      static inline void _verifyFixLength(size_t& length, size_t maxLength) {
        if (length > maxLength) {
          __if_constexpr (_ThrowWhenFull)
            _throwCapacityExceeded();
          length = maxLength;
        }
      }
      template <typename _Iterator>
      static inline _Iterator _verifyFixLastIterator(_Iterator first, _Iterator last, size_t expectedSize, size_t availableSize) {
        if (expectedSize <= availableSize)
          return last;
        else {
          __if_constexpr (_ThrowWhenFull)
            _throwCapacityExceeded();
          return first + availableSize - 1;
        }
      }
      template <typename _Iterator>
      static inline _Iterator _verifyFixEndIterator(_Iterator first, _Iterator end, size_t expectedSize, size_t availableSize) {
        if (expectedSize <= availableSize)
          return end;
        else {
          __if_constexpr (_ThrowWhenFull)
            _throwCapacityExceeded();
          return first + availableSize;
        }
      }

      // -- private - iterators --

      inline _DataType* _first() { return (!empty()) ? &front() : nullptr; }
      inline _DataType* _last() { return (!empty()) ? &back() : nullptr; }

      _DataType* _next(const _DataType* current, uint32_t currentIndex, size_t offset = 1u) const {
        if (current != nullptr && currentIndex + offset < this->_size)
          return &(_at(currentIndex + offset));
        return nullptr;
      }
      _DataType* _previous(const _DataType* current, uint32_t currentIndex, size_t offset = 1u) const {
        if (current != nullptr && currentIndex >= offset && currentIndex - offset < this->_size)
          return &(_at(currentIndex - offset));
        return nullptr;
      }

    private:
      std::array<item_type, _Capacity> _value;
      size_t _size = 0u;
    };

  }
}
#undef __if_constexpr
