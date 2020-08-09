/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <array>
#include <stdexcept>
#if !defined(_CPP_REVISION) || _CPP_REVISION != 14
# define __if_constexpr if constexpr
#else
# define __if_constexpr if
#endif
#include <type_traits>
#include "./class_traits.h"

namespace pandora {
  namespace pattern {
    /// @class Optional
    /// @brief Pointer-like buffer optionally containing an object with in-place instance creation (no dynamic allocation)
    template <typename _DataType>
    class Optional final : public ConstructorSelector<_DataType>, public AssignmentSelector<_DataType> {
    public:
      /// @brief Create empty buffer
      Optional() : _hasValue(false) {}
      Optional(std::nullptr_t) noexcept : _hasValue(false) {} ///< Create empty buffer
      /// @brief Destroy buffer and optional instance
      ~Optional() noexcept { reset(); }

      /// @brief Create buffer containing an instance (copy)
      template <typename T = _DataType>
      Optional(enable_if_copy_constructible<T, const T&> value) { _assign<true, _DataType>(value); }
      /// @brief Create buffer containing an instance (move)
      template <typename T = _DataType>
      Optional(enable_if_move_constructible<T, T&&> value) { _assign<true, _DataType>(std::move(value)); }

      Optional(const Optional<_DataType>& rhs) : ConstructorSelector<_DataType>(rhs) { _copy<true>(rhs); }
      Optional(Optional<_DataType>&& rhs) : ConstructorSelector<_DataType>(std::move(rhs)) { _move<true>(std::move(rhs)); }
      Optional<_DataType>& operator=(const Optional<_DataType>& rhs) { AssignmentSelector<_DataType>::operator=(rhs); _copy<false>(rhs); return *this; }
      Optional<_DataType>& operator=(Optional<_DataType>&& rhs) { AssignmentSelector<_DataType>::operator=(std::move(rhs)); _move<false>(std::move(rhs)); return *this; }
      
      // -- getters --
      
      inline _DataType* get() noexcept { return (hasValue()) ? _get() : nullptr; } ///< Get buffer address (if instance exists)
      inline const _DataType* get() const noexcept { return (hasValue()) ? _get() : nullptr; } ///< Get buffer address (if instance exists)
      
      inline bool hasValue() const noexcept { return this->_hasValue; } ///< Verify if an instance is currently in the buffer
      inline operator bool() const noexcept { return this->_hasValue; }

      inline _DataType& value() noexcept { assert(hasValue()); return *_get(); } ///< Get current instance value (warning: always verify hasValue() first!)
      inline const _DataType& value() const noexcept { assert(hasValue()); return *_get(); } ///< Get current instance value (warning: always verify hasValue() first!)
      
      inline _DataType& operator*() noexcept { return value(); }
      inline const _DataType& operator*() const noexcept  { return value(); }
      inline _DataType* operator->() noexcept { assert(hasValue()); return _get(); }
      inline const _DataType* operator->() const noexcept { assert(hasValue()); return _get(); }
      
      // -- comparisons --
      
      template <typename T = _DataType>
      inline bool operator==(enable_if_operator_equals<T, const Optional<T>& > rhs) const noexcept { 
        return (this->_hasValue == rhs._hasValue && (!this->_hasValue || this->value() == rhs.value())); 
      }
      template <typename T = _DataType>
      inline bool operator!=(enable_if_operator_equals<T, const Optional<T>& > rhs) const noexcept { return !(this->operator==(rhs)); }

      inline bool operator==(std::nullptr_t) const noexcept { return !(this->_hasValue); }
      inline bool operator!=(std::nullptr_t) const noexcept { return (this->_hasValue); }

      template <typename T = _DataType>
      inline bool operator<(enable_if_operator_less<T, const Optional<T>& > rhs) const noexcept {
        return (this->_hasValue) ? (rhs._hasValue && this->value() < rhs.value()) : rhs._hasValue;
      }
      template <typename T = _DataType>
      inline bool operator<=(enable_if_operator_less_eq<T, const Optional<T>& > rhs) const noexcept {
        return (!this->_hasValue || (rhs._hasValue && this->value() <= rhs.value()));
      }
      template <typename T = _DataType>
      inline bool operator>(enable_if_operator_greater<T, const Optional<T>& > rhs) const noexcept {
        return (this->_hasValue && (!rhs._hasValue || this->value() > rhs.value()));
      }
      template <typename T = _DataType>
      inline bool operator>=(enable_if_operator_greater_eq<T, const Optional<T>& > rhs) const noexcept {
        return (this->_hasValue) ? (!rhs._hasValue || this->value() >= rhs.value()) : !rhs._hasValue;
      }
      
      // -- operations --
      
      /// @brief Destroy current instance (if any)
      inline void reset() noexcept {
        if (this->_hasValue) {
          value().~_DataType();
          _hasValue = false;
        }
      }
      inline Optional<_DataType>& operator=(std::nullptr_t) noexcept { reset(); return *this; }

      /// @brief Create an instance (copy)
      template <typename T = _DataType>
      inline void assign(enable_if_copy_constructible<T, const T&> value) { _assign<false,T>(value); }
      /// @brief Create an instance (move)
      template <typename T = _DataType>
      inline void assign(enable_if_move_constructible<T, T&&> value) { _assign<false,T>(std::move(value)); }
      /// @brief Create an instance (emplace)
      template <typename ... _Args>
      inline void emplace(_Args... args) { _emplace<false, _Args...>(std::forward<_Args>(args)...); }

      template <typename T = _DataType>
      inline Optional<_DataType>& operator=(enable_if_copy_constructible<T, const T&> value) { _assign<false,T>(value); return *this; }
      template <typename T = _DataType>
      inline Optional<_DataType>& operator=(enable_if_move_constructible<T, T&&> value) { _assign<false,T>(std::move(value)); return *this; }
      
    private:
      inline _DataType* _get() noexcept { return reinterpret_cast<_DataType*>(this->_value.data()); }
      inline const _DataType* _get() const noexcept { return reinterpret_cast<const _DataType*>(this->_value.data()); }

      template <bool _IsConstruction, typename T>
      inline void _copy(const Optional<T>& rhs) {
        if (rhs._hasValue)
          _assign<_IsConstruction,T>(rhs.value());
        else
          this->_hasValue = false;
      }
      template <bool _IsConstruction, typename T>
      inline void _move(Optional<T>&& rhs) {
        if (rhs._hasValue) {
          _assign<_IsConstruction,T>(std::move(rhs.value()));
          rhs.reset();
        }
        else
          this->_hasValue = false;
      }

      template <bool _IsConstruction, typename T = _DataType>
      inline void _assign(enable_if_copy_constructible<T, const T&> value) {
        __if_constexpr(!_IsConstruction)
          reset();
        new(_get()) _DataType(value);
        this->_hasValue = true;
      }
      template <bool _IsConstruction, typename T = _DataType>
      inline void _assign(enable_if_move_constructible<T, T&&> value) {
        __if_constexpr(!_IsConstruction)
          reset();
        new(_get()) _DataType(std::move(value));
        this->_hasValue = true;
      }
      template <bool _IsConstruction, typename ... _Args>
      inline void _emplace(_Args... args) {
        __if_constexpr(!_IsConstruction)
          reset();
        new(_get()) _DataType(std::forward<_Args>(args)...);
        this->_hasValue = true;
      }
      
    private:
      bool _hasValue = false;
      std::array<uint8_t, sizeof(_DataType)> _value{};
    };
    
  }
}
#undef __if_constexpr
