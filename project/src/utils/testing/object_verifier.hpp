/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : object verification toolset
*******************************************************************************/
#pragma once

#include <cstdio>
#include <cstddef>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.testing
    /// Testing utilities
    namespace testing
    {
        /// @class ObjectVerifier
        /// @brief Objects verifications
        class ObjectVerifier
        {
        public:
            // ObjectVerifier.isNull (nullptr)
            template <typename T>
            static bool isNull(const T* pObject) { return (pObject == nullptr); }
            // ObjectVerifier.isNotNull (!=nullptr)
            template <typename T>
            static bool isNotNull(const T* pObject) { return (pObject != nullptr); }
            
            // ObjectVerifier.isInstanceOf <className>
            template <typename ClassType, typename T>
            static bool isInstanceOf(const T* pObject) { return (dynamic_cast<ClassType*>(pObject) != nullptr); }
            // ObjectVerifier.isNotInstanceOf <className>
            template <typename ClassType, typename T>
            static bool isNotInstanceOf(const T* pObject) { return (dynamic_cast<ClassType*>(pObject) == nullptr); }
        };
    }
}
