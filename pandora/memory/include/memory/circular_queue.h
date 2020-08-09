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
#include <pattern/optional.h>
#include <pattern/iterator.h>
#include <pattern/class_traits.h>

namespace pandora {
  namespace memory {
    /// @class CircularQueue
    /// @brief Circular/wrap-around fixed-size queue collection (FIFO, stack allocation)
    template <typename _DataType, size_t _Capacity>
    class CircularQueue final : public pandora::pattern::ConstructorSelector<_DataType>, public pandora::pattern::AssignmentSelector<_DataType> {
    public:
      using value_type = _DataType;
      using item_type = typename std::conditional<std::is_class<_DataType>::value,  pandora::pattern::Optional<_DataType>, _DataType >::type;
      using size_type = size_t;
      using reference = _DataType&;
      using const_reference = const _DataType&;
      using Type = CircularQueue<_DataType,_Capacity>;
      static_assert((_Capacity > 0u), "CircularQueue: _Capacity can't be 0.");
      
      /// @brief Create empty queue
      CircularQueue() noexcept : _head(0), _tail(0) {}
      /// @brief Destroy queue and all child instances
      ~CircularQueue() = default;
      
      CircularQueue(const Type& rhs) : pandora::pattern::ConstructorSelector<_DataType>(rhs), _head(0), _tail(0)       { _copyEachItem<_DataType>(rhs); }
      CircularQueue(Type&& rhs) : pandora::pattern::ConstructorSelector<_DataType>(std::move(rhs)), _head(0), _tail(0) { _moveEachItem<_DataType>(rhs); }
      Type& operator=(const Type& rhs) { pandora::pattern::AssignmentSelector<_DataType>::operator=(rhs);
        clear(); 
        _copyEachItem<_DataType>(rhs); 
        return *this; 
      }
      Type& operator=(Type&& rhs) { pandora::pattern::AssignmentSelector<_DataType>::operator=(std::move(rhs));
        clear(); 
        _moveEachItem<_DataType>(rhs); 
        return *this; 
      }
      
      void swap(Type& rhs) {
        std::swap(this->_queue, rhs._queue);
        std::swap(this->_head, rhs._head);
        std::swap(this->_tail, rhs._tail);
      }
      
      // -- getters --
      
      /// @brief Get number of items currently stored in queue ([0; _Capacity])
      constexpr inline size_t size() const noexcept { 
        return (this->_tail < this->_head) ? (this->_head - this->_tail) : (_Capacity + this->_head - this->_tail); 
      }
      constexpr inline bool capacity() const noexcept { return _Capacity; } ///< Get maximum capacity of the queue
      constexpr inline bool empty() const noexcept { return (this->_head == this->_tail); } ///< Check if any item is present in the queue
      constexpr inline bool full() const noexcept  { return (this->_head == -1); } ///< Check if the queue is full
      
      /// @brief Get first item in the queue (check not empty() before!)
      inline _DataType& front() { assert(!empty()); return _at(this->_tail); }
      /// @brief Get first item in the queue (check not empty() before!)
      inline const _DataType& front() const { assert(!empty()); return _at(this->_tail); }
      
      /// @brief Get most recent item in the queue (check not empty() before!)
      inline _DataType& back() { assert(!empty()); return _at(_lastIndex()); }
      /// @brief Get most recent item in the queue (check not empty() before!)
      inline const _DataType& back() const { assert(!empty()); return _at(_lastIndex()); }
      
      // -- operations --
      
      /// @brief Remove all items from the queue
      inline void clear() { 
        this->_queue = std::array<_DataType, _Capacity>{};
        this->_head = this->_tail = 0u;
      }
      
      /// @brief Insert an item at the end of the queue (copy)
      template <typename T = _DataType>
      inline bool push(pandora::pattern::enable_if_copy_constructible<T, const T&> value) {
        if (!full()) {
          Type::_copyIn(this->_queue[this->_head], value);
          this->_head = _nextIndex(this->_head);
          if (this->_head == this->_tail)
            this->_head = -1;
          return true;
        }
        return false;
      }
      template <typename T = _DataType>
      inline bool push_back(pandora::pattern::enable_if_copy_constructible<T, const T&> value) { return push(value); }
      
      /// @brief Insert an item at the end of the queue (move)
      template <typename T = _DataType>
      inline bool push(pandora::pattern::enable_if_move_constructible<T, T&&> value) {
        if (!full()) {
          Type::_moveIn(this->_queue[this->_head], std::move(value));
          this->_head = _nextIndex(this->_head);
          if (this->_head == this->_tail)
            this->_head = -1;
          return true;
        }
        return false;
      }
      template <typename T = _DataType>
      inline bool push_back(pandora::pattern::enable_if_move_constructible<T, T&&> value) { return push(std::move(value)); }
      
      /// @brief Create an item at the end of the queue
      template <typename ... _Args>
      bool emplace(_Args... args) { 
        if (!full()) {
          Type::_emplaceIn(this->_queue[this->_head], std::forward<_Args>(args)...);
          this->_head = _nextIndex(this->_head);
          if (this->_head == this->_tail)
            this->_head = -1;
          return true;
        }
        return false;
      }
      template <typename ... _Args>
      bool emplace_back(_Args... args) { return emplace(std::forward<_Args>(args)...); }
      
      /// @brief Remove first item from the queue
      bool pop(size_t index) { 
        if (!empty()) {
          this->_queue[this->_tail].reset();
          if (full())
            this->_head = this->_tail;
          this->_tail = _nextIndex(this->_tail);
          return true;
        }
        return false;
      }
      inline void pop_front(size_t index) { pop(index); }
      
      
      // -- iterators --

      _P_FORWARD_ITERATOR_MEMBERS(Type, _DataType, _first())
      _DataType* next(_DataType* current, uint32_t currentIndex, size_t offset = 1u) { return _next(current, currentIndex, offset); }
      const _DataType* next(const _DataType* current, uint32_t currentIndex, size_t offset = 1u) const { return _next(current, currentIndex, offset); }
      
    private:
      static constexpr inline int32_t _previousIndex(int32_t index) {
        return (index > 0) ? (index - 1) : (_Capacity - 1);
      }
      static constexpr inline int32_t _nextIndex(int32_t index) {
        return ((index + 1) % static_cast<uint32_t>(_Capacity));
      }
      constexpr inline int32_t _lastIndex() {
        return (this->_head > -1) ? _previousIndex(this->_head) : _previousIndex(this->_tail);
      }

      static inline _DataType& _getValue(pandora::pattern::Optional<_DataType>& embedder) noexcept { return embedder.value(); }
      static inline _DataType& _getValue(_DataType& value) noexcept { return value; }
      inline _DataType& _at(int32_t index) noexcept { return Type::_getValue(this->_queue[index]); }


      // -- private - operations --

      template <typename T, typename _EnabledType = void>
      using enable_if_copyable_class = typename std::enable_if<std::is_class<T>::value && std::is_copy_constructible<T>::value, _EnabledType>::type;
      template <typename T, typename _EnabledType = void>
      using enable_if_movable_class = typename std::enable_if<std::is_class<T>::value && std::is_move_constructible<T>::value, _EnabledType>::type;
      template <typename T, typename _EnabledType = void>
      using enable_if_base_type = typename std::enable_if<!std::is_class<T>::value, _EnabledType>::type;

      template <typename T = _DataType>
      void _copyEachItem(enable_if_copyable_class<T, const CircularQueue<T,_Capacity>&> rhs) {
        for (uint32_t i = 0; i < static_cast<uint32_t>(rhs._queue.size()); ++i)
          this->_queue[i].assign(rhs._queue[i].value());
      }
      template <typename T = _DataType>
      void _copyEachItem(enable_if_base_type<T, const CircularQueue<T,_Capacity>&> rhs) {
        memcpy((void*)this->_queue, (void*)rhs._queue, sizeof(_DataType)*_Capacity);
      }

      template <typename T = _DataType>
      void _moveEachItem(enable_if_movable_class<T, CircularQueue<T,_Capacity>&> rhs) {
        for (uint32_t i = 0; i < static_cast<uint32_t>(rhs._queue.size()); ++i)
          this->_queue[i].assign(std::move(rhs._queue[i].value()));
      }
      template <typename T = _DataType>
      void _moveEachItem(enable_if_base_type<T, CircularQueue<T,_Capacity>&> rhs) {
        memcpy((void*)this->_queue, (void*)rhs._queue, sizeof(_DataType)*_Capacity);
      }

      template <typename T = _DataType>
      static inline void _copyIn(pandora::pattern::Optional<_DataType>& destination, pandora::pattern::enable_if_copy_constructible<T, const T&> value) { 
        destination.assign(value);
      }
      template <typename T = _DataType>
      static inline void _copyIn(_DataType& destination, pandora::pattern::enable_if_copy_constructible<T, const T&> value) { destination = value; }

      template <typename T = _DataType>
      static inline void _moveIn(pandora::pattern::Optional<_DataType>& destination, pandora::pattern::enable_if_move_constructible<T, T&&> value) {
        destination.assign(std::move(value));
      }
      template <typename T = _DataType>
      static inline void _moveIn(_DataType& destination, pandora::pattern::enable_if_move_constructible<T, T&&> value) { destination = std::move(value); }

      template <typename ... _Args>
      static inline void _emplaceIn(pandora::pattern::Optional<_DataType>& destination, _Args... args) noexcept { destination.emplace(std::forward<_Args>(args)...); }
      template <typename ... _Args>
      static inline void _emplaceIn(_DataType& destination, _Args... args) noexcept { destination = _DataType(std::forward<_Args>(args)...); }

      // -- private - iterators --

      inline _DataType* _first() { return (!empty()) ? &front() : nullptr; }

      _DataType* _next(const _DataType* current, uint32_t currentIndex, size_t offset = 1u) const {
        if (current != nullptr && !empty()) {
          if (offset > 0u) {
            int32_t nextIndex = _nextIndex(static_cast<int32_t>(currentIndex) + static_cast<int32_t>(offset) - 1);
            int32_t lastIndex = _lastIndex();
            if (nextIndex <= lastIndex) {
              if (this->_tail <= nextIndex || lastIndex < this->_tail)
                return &(_at(nextIndex));
            }
            else {
              if (this->_tail <= nextIndex && lastIndex < this->_tail)
                return &(_at(nextIndex));
            }
          }
          else
            return current;
        }
        return nullptr;
      }
      
    private:
      std::array<item_type, _Capacity> _queue;
      int32_t _head{ 0 };
      int32_t _tail{ 0 };
    };
    
  }
}
