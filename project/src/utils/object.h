/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : object toolset
*******************************************************************************/
#pragma once

#include <cstdio>
#include <cstddef>
#include <string>
#include <type_traits>
#include <typeinfo>

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
        /// @brief Get size of an object (bytes)
        /// @param[in] obj  Object reference
        /// @returns Size (bytes)
        template <typename T>
        static inline size_t getSize(const T& obj) { return sizeof(obj); }
        
        /// @brief Get type name of an object
        /// @param[in] pObject  Object pointer
        /// @returns Type name
        template <typename T>
        static inline std::string getType(const T* pObject) { return (pObject != nullptr) ? std::string(typeid(*pObject).name()) : std::string("null"); }
        /// @brief Check whether an object is an instance of a specific child class
        /// @param[in] pObject  Object pointer
        /// @returns Instance or not
        template <typename ClassType, typename T>
        static inline bool isInstanceOf(const T* pObject) { return (dynamic_cast<ClassType*>(pObject) != nullptr); }
        
        
        /// @brief Check whether an object is a number
        /// @param[in] obj  Object reference
        /// @returns Numeric or not
        template <typename T>
        static inline bool isNumeric(const T& obj) { return (std::is_numeric<T>::value); }
        /// @brief Check whether an object is an integer number
        /// @param[in] obj  Object reference
        /// @returns Integer or not
        template <typename T>
        static inline bool isInteger(const T& obj) { return (std::is_integral<T>::value); }
        /// @brief Check whether an object is a floating-point number
        /// @param[in] obj  Object reference
        /// @returns Float or not
        template <typename T>
        static inline bool isFloat(const T& obj) { return (std::is_floating_point<T>::value); }
        /// @brief Check whether an object is a signed number
        /// @param[in] obj  Object reference
        /// @returns Signed or not
        template <typename T>
        static inline bool isSigned(const T& obj) { return (std::is_signed<T>::value); }

        /// @brief Check whether an object is an enumeration item
        /// @param[in] obj  Object reference
        /// @returns Enum or not
        template <typename T>
        static inline bool isEnum(const T& obj) { return (std::is_enum<T>::value); }
        /// @brief Check whether an object is a pointer
        /// @param[in] obj  Object reference
        /// @returns Pointer or not
        template <typename T>
        static inline bool isPointer(const T& obj) { return (std::is_pointer<T>::value); }
        /// @brief Check whether an object reference is an abstract super-class
        /// @param[in] obj  Object reference
        /// @returns Abstract or not
        template <typename T>
        static inline bool isAbstract(const T& obj) { return (std::is_abstract<T>::value); }
        
    };
}
