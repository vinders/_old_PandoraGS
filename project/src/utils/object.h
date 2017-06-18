/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : object toolset
*******************************************************************************/
#pragma once

#include <cstdio>
#include <cstddef>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @class Object
    /// @brief Object toolset
    class Object
    {
    public:
        /// @brief Check whether an object pointer is null or not
        /// @param[in] pObject  Object pointer
        /// @returns Null or not
        template <typename T>
        static inline bool isNull(const T* pObject) { return (pObject == nullptr); }
        
        /// @brief Check whether an object is an instance of a specific child class
        /// @param[in] pObject  Object pointer
        /// @returns Instance or not
        template <typename ClassType, typename T>
        static inline bool isInstanceOf(const T* pObject) { return (dynamic_cast<ClassType*>(pObject) != nullptr); }
        
        /// @brief Get size of an object (bytes)
        /// @param[in] pObject  Object pointer
        /// @returns Size (bytes)
        template <typename T>
        static inline size_t getSize(const T* pObject) { return (pObject != nullptr) ? sizeof(*pObject) : 0; }
    };
}
