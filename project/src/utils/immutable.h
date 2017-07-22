/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : immutable object wrapper
*******************************************************************************/
#pragma once

#include <cstddef>
#include <memory>
#include <atomic>
#include <stdexcept>
#include "preprocessor.h"
#include "assert.h"

/// @namespace utils
/// General utilities
namespace utils
{
    template <typename T>
    using immutable<T> = utils::Immutable<T>; ///< Immutable object wrapper
    using const_immutable<T> = const T;       ///< Constant immutable object
    
    /// @class Immutable
    /// @brief Immutable object wrapper (make objects thread-safe, by disabling setters and using atomic operations)
    template <typename T>
    class Immutable
    {
    public:
        /// @brief Create immutable object with default object constructor
        Immutable() : m_pValue(nullptr) { m_pValue = std::make_shared<T>(); }
        /// @brief Create immutable object with constructor arguments
        /// @param[in] args  Arguments for inner object constructor
        template<typename... Args>
        Immutable(Args&&... args) : m_pValue(nullptr) { m_pValue = std::make_shared<T>(std::forward<Args>(args)...)); }
        
        /// @brief Create immutable object by copying a non-immutable object value
        /// @param[in] value  Object to copy
        explicit Immutable(const T& value) : m_pValue(nullptr) { m_pValue = std::make_shared<T>(value); }
        /// @brief Create immutable object by moving a non-immutable object value
        /// @param[in] value  Object to move
        explicit Immutable(T&& value) : m_pValue(nullptr) { m_pValue = std::make_shared<T>(std::move(value)); }
        
        /// @brief Create immutable object by copying another immutable object
        /// @param[in] other  Object to copy
        Immutable(const Immutable<T>& other) : m_pValue(nullptr) { m_pValue = std::make_shared<T>(*(std::atomic_load(other.m_pValue))); }
        /// @brief Create immutable object by moving another immutable object
        /// @param[in] other  Object to move
        Immutable(Immutable<T>&& other) : m_pValue(std::atomic_load(other.m_pValue)) { std::atomic_store(&other.m_pValue, nullptr); }
                

        /// @brief Assign immutable object by copying another immutable object
        /// @param[in] other  Object to copy
        inline const Immutable<T> const& operator=(const Immutable<T>& other) 
        {
            std::atomic_store(&m_pValue, std::make_shared<T>(*(std::atomic_load(other.m_pValue))));
            return *this;
        }
        /// @brief Assign immutable object by moving another immutable object
        /// @param[in] other  Object to move
        inline const Immutable<T> const& operator=(Immutable<T>&& other)
        {
            std::atomic_store(&m_pValue, std::atomic_load(other.m_pValue));
            std::atomic_store(&other.m_pValue, nullptr);
            return *this;
        }
        
        /// @brief Create copy of immutable object
        /// @returns Cloned object
        inline Immutable<T> clone() const
        {
            return Immutable<T>(*this);
        }
        
        
        // -- Getters --
        
        /// @brief Get inner object value (by reference)
        /// @returns Reference to object
        /// @throws std::logic_error  If inner value is not set (if the instance has been moved or if an assignment failed)
        inline const T const& data() const 
        { 
            ASSERT(m_pValue != nullptr); 
            if (isSet())
                return *m_pValue; 
            else
                throw std::logic_error(__FILE__ ":" PP_STRINGIFY_ARG(__LINE__) ":" __func__ ": "
                                       "inner value is not set (nullptr): either it was moved or its instanciation failed."));
        }
        /// @brief Get inner object value (by reference)
        /// @returns Reference to object
        /// @throws std::logic_error  If inner value is not set (if the instance has been moved or if an assignment failed)
        inline const T const& operator() const { return data(); }
        
        /// @brief Check if inner object is defined
        /// @returns Valid object (true) or nullptr (false)
        inline const bool isSet() const noexcept { return (m_pValue != nullptr); }
        
        
        // -- Compare --
        
        /// @brief Compare equality with another immutable object
        /// @returns Equal (true) or not
        inline const bool operator==(const Immutable<T>& rhs) const noexcept { ASSERT(m_pValue != nullptr && rhs.m_pValue != nullptr); return (isSet() && other.isSet() && *std::atomic_load(m_pValue) == *std::atomic_load(rhs.m_pValue)); }
        /// @brief Compare difference with another immutable object
        /// @returns Different (true) or not
        inline const bool operator!=(const Immutable<T>& rhs) const noexcept { ASSERT(m_pValue != nullptr && rhs.m_pValue != nullptr); return (isSet() && other.isSet() && *std::atomic_load(m_pValue) != *std::atomic_load(rhs.m_pValue)); }
        
        /// @brief Compare equality with a non-immutable object
        /// @returns Equal (true) or not
        inline const bool operator==(const T& rhs) const noexcept { ASSERT(isSet()); return (isSet() && *std::atomic_load(m_pValue) == rhs); }
        /// @brief Compare difference with a non-immutable object
        /// @returns Different (true) or not
        inline const bool operator!=(const T& rhs) const noexcept { ASSERT(isSet()); return (isSet() && *std::atomic_load(m_pValue) != rhs); }
    
    private:
        std::shared_ptr<T> m_pValue; ///< Inner object value
    };
}
