/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : advanced enumeration with external tools
*******************************************************************************/
#pragma once

#include <string>
#include "preprocessor.h"

/// @namespace utils
/// General utilities
namespace utils
{
    /// @brief Get size of enum (declared with TYPED_ENUM)
    /// @returns Number of values in enum
    template <typename T>
    const size_t enum_length();
    /// @brief Get max value of enum (declared with TYPED_ENUM)
    /// @returns Max value (last if uninitialized, biggest if initialized)
    template <typename T>
    T enum_max();
    
    /// @brief Serialize/deserialize enum (for which ENUM_SERIALIZER is defined)
    template <typename T>
    struct enum_serializer
    {
        /// @brief Get size of enum
        /// @returns Number of values in enum
        static const size_t size();
        /// @brief Serialize enum value
        /// @returns String representation
        template <T> static std::string toString();
        /// @brief Parse serialized enum value (string)
        /// @param[in] enumName  Enumeration type name
        /// @returns Value successfully converted (or invalid)
        static bool parse(const std::string val, T out);
    };
}
                                        
                                        
// -- declare typed enum with count utilities --

/*
// example
TYPED_ENUM(my_enum_t, uint32_t, BigValue,
    NoValue = 0,
    TinyValue,
    SmallValue,
    MediumValue,
    BigValue);
*/

/// @brief Create typed enum class with 
/// @param[in] enumName  Enumeration type name
/// @param[in] dataType  Standard type of data (int, short, bool, ...)
/// @param[in] maxValue  Biggest value name (if initialized) or last value (if uninitialized)
/// @param[in] ...       All possible enumeration values (with or without initializer)
#define TYPED_ENUM(enumName,dataType,maxValue,...) \
        enum class enumName : dataType \
        { \
            __VA_ARGS__ \
        }; \
        template <> \
        inline const size_t ::utils::enum_length<enumName>() noexcept { return _COUNT_VA_ARGS(__VA_ARGS__); } \
        inline enumName ::utils::enum_max<enumName>() noexcept { return enumName##::##maxValue; }


// -- enumeration value serializer - enum to string - string to enum --

/*
// example
ENUM_SERIALIZER(my_enum_t, NoValue,TinyValue,SmallValue,MediumValue,BigValue);
*/

/// @brief Create enum/string parser/formatter (declare this at the same place as the enum)
/// @param[in] enumName  Enumeration type name
/// @param[in] ...       All possible enumeration values
#define ENUM_SERIALIZER(enumName,...) \
    template <> struct ::utils::enum_serializer<##enumName##> \
    { \
        static inline const size_t size() noexcept { return PP_COUNT_VARIADIC_ARGS(__VA_ARGS__); } \
        \
        template <##enumName##> static std::string toString(); \
        PP_FOREACH_WITH_PARAM(__UTILS_ENUM_SERIALIZER_SERIALIZER,enumName,(__VA_ARGS__)) \
        \
        static bool parse(const std::string val, enumName out) \
        { \
            enumName values[] = { PP_FOREACH(PP_EXPAND_ARG_WITH_COMMA,__PP_SKIP_FIRST_VARIADIC_ARG(__VA_ARGS__)) PP_EXPAND_ARG(__PP_GET_FIRST_VARIADIC_ARG(__VA_ARGS__)) }; \
            static std::string serialized[] = { PP_FOREACH(PP_STRINGIFY_ARG_WITH_COMMA,__PP_SKIP_FIRST_VARIADIC_ARG(__VA_ARGS__)) PP_STRINGIFY_ARG(__PP_GET_FIRST_VARIADIC_ARG(__VA_ARGS__)) }; \
            for (int i = 0; i < PP_COUNT_VARIADIC_ARGS(__VA_ARGS__); ++i) \
            { \
                if (serialized[i] == val) \
                { \
                    out = values[i]; \
                    return true; \
                } \
            } \
            return false; \
        } \
    }

#define __UTILS_ENUM_SERIALIZER_SERIALIZER(enumName,val)   template<> static inline std::string toString<##enumName##::##val##>() { return #val; }
