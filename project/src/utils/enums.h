/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : advanced enumeration with external tools
*******************************************************************************/
#pragma once

#include <cstdint>
#include <string>
#include "preprocessor.h"

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.enums
    /// Enumeration toolset
    namespace enums
    {
        /// @brief Get size of enum (declared with TYPED_ENUM)
        /// @returns Number of values in enum
        template <typename T>
        const size_t size();
        /// @brief Get max value of enum (declared with TYPED_ENUM)
        /// @returns Max value (last if uninitialized, biggest if initialized)
        template <typename T>
        T max();
        /// @brief Convert enum value to inner type (declared with TYPED_ENUM)
        /// @param[in] val  Enum value
        /// @returns Base value
        template <typename T>
        U toValue(const T val);
        /// @brief Convert inner type to enum value, by only checking max value (declared with TYPED_ENUM)
        /// @param[in] val  Base value
        /// @returns Enum value
        template <typename T, typename U>
        bool toEnumUnsafe(const U val, T& out);
        
        /// @brief Convert base type to enum value, by checking each possible enum value (declared with ENUM_SERIALIZER)
        /// @param[in] val  Base value
        /// @returns Enum value
        template <typename T, typename U>
        bool toEnum(const U val, T& out);
        
        /// @brief Serialize/deserialize enum (for which ENUM_SERIALIZER is defined)
        template <typename T>
        struct serializer
        {
            /// @brief Serialize enum value
            /// @returns String representation
            template <T> static std::string toString();
            /// @brief Parse serialized enum value (string)
            /// @param[in] enumName  Enumeration type name
            /// @returns Value successfully converted (or invalid)
            static bool parse(const std::string val, T& out);
        };
    }
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
        template <> inline const size_t ::utils::enums::size<##enumName##>() noexcept { return _COUNT_VA_ARGS(__VA_ARGS__); } \
        template <> inline enumName ::utils::enums::max<##enumName##>() noexcept { return enumName##::##maxValue; } \
        \
        template <> inline dataType ::utils::enums::toValue<##enumName##>(const enumName val) noexcept { return static_cast<##dataType##>(val); } \
        \
        template <> inline bool ::utils::enums::toEnumUnsafe<##enumName##,uint64_t>(const uint64_t val, enumName##& out) noexcept { if (static_cast<##dataType##>(val) <= static_cast<##dataType##>(::utils::enums::max<##enumName##>())) { out = static_cast<##enumName##>(val); return true; } return false; } \
        template <> inline bool ::utils::enums::toEnumUnsafe<##enumName##,int64_t>(const int64_t val, enumName##& out) noexcept   { if (static_cast<##dataType##>(val) <= static_cast<##dataType##>(::utils::enums::max<##enumName##>())) { out = static_cast<##enumName##>(val); return true; } return false; } \
        template <> inline bool ::utils::enums::toEnumUnsafe<##enumName##,uint32_t>(const uint32_t val, enumName##& out) noexcept { if (static_cast<##dataType##>(val) <= static_cast<##dataType##>(::utils::enums::max<##enumName##>())) { out = static_cast<##enumName##>(val); return true; } return false; } \
        template <> inline bool ::utils::enums::toEnumUnsafe<##enumName##,int32_t>(const int32_t val, enumName##& out) noexcept   { if (static_cast<##dataType##>(val) <= static_cast<##dataType##>(::utils::enums::max<##enumName##>())) { out = static_cast<##enumName##>(val); return true; } return false; } \
        template <> inline bool ::utils::enums::toEnumUnsafe<##enumName##,uint16_t>(const uint16_t val, enumName##& out) noexcept { if (static_cast<##dataType##>(val) <= static_cast<##dataType##>(::utils::enums::max<##enumName##>())) { out = static_cast<##enumName##>(val); return true; } return false; } \
        template <> inline bool ::utils::enums::toEnumUnsafe<##enumName##,int16_t>(const int16_t val, enumName##& out) noexcept   { if (static_cast<##dataType##>(val) <= static_cast<##dataType##>(::utils::enums::max<##enumName##>())) { out = static_cast<##enumName##>(val); return true; } return false; } \
        template <> inline bool ::utils::enums::toEnumUnsafe<##enumName##,uint8_t>(const uint8_t val, enumName##& out) noexcept   { if (static_cast<##dataType##>(val) <= static_cast<##dataType##>(::utils::enums::max<##enumName##>())) { out = static_cast<##enumName##>(val); return true; } return false; } \
        template <> inline bool ::utils::enums::toEnumUnsafe<##enumName##,int8_t>(const int8_t val, enumName##& out) noexcept     { if (static_cast<##dataType##>(val) <= static_cast<##dataType##>(::utils::enums::max<##enumName##>())) { out = static_cast<##enumName##>(val); return true; } return false; } \
        template <> inline bool ::utils::enums::toEnumUnsafe<##enumName##,bool>(const bool val, enumName##& out) noexcept         { if (static_cast<##dataType##>(val) <= static_cast<##dataType##>(::utils::enums::max<##enumName##>())) { out = static_cast<##enumName##>(val); return true; } return false; }


// -- enumeration value serializer (!!! enum must already be declared with 'TYPED_ENUM' or with 'enum class') - enum to string - string to enum --

/*
// example
ENUM_SERIALIZER(my_enum_t, NoValue,TinyValue,SmallValue,MediumValue,BigValue);
*/

/// @brief Create enum/string parser/formatter (declare this at the same place as the enum)
/// @param[in] enumName  Enumeration type name
/// @param[in] ...       All possible enumeration values
#define ENUM_SERIALIZER(enumName,...) \
        template <> struct ::utils::enums::serializer<##enumName##> \
        { \
            template <##enumName##> static std::string toString(); \
            PP_FOREACH_WITH_PARAM(__UTILS_ENUM_SERIALIZER_SERIALIZER,enumName,(__VA_ARGS__)) \
            \
            static bool parse(const std::string val, enumName out) \
            { \
                static enumName values[] = { PP_FOREACH(PP_EXPAND_ARG_WITH_COMMA,__PP_SKIP_FIRST_VARIADIC_ARG(__VA_ARGS__)) PP_EXPAND_ARG(__PP_GET_FIRST_VARIADIC_ARG(__VA_ARGS__)) }; \
                static std::string serialized[] = { PP_FOREACH(PP_STRINGIFY_ARG_WITH_COMMA,__PP_SKIP_FIRST_VARIADIC_ARG(__VA_ARGS__)) PP_STRINGIFY_ARG(__PP_GET_FIRST_VARIADIC_ARG(__VA_ARGS__)) }; \
                std::string* it = serialized; \
                for (int i = 0; i < PP_COUNT_VARIADIC_ARGS(__VA_ARGS__); ++i) \
                { \
                    if (*it == val) \
                    { \
                        out = values[i]; \
                        return true; \
                    } \
                    ++it; \
                } \
                return false; \
            } \
        } \
        \
        template <> inline bool ::utils::enums::toEnum<##enumName##,uint64_t>(const uint64_t val, enumName##& out) noexcept { switch (static_cast<##enumName##>(val)) { PP_FOREACH_WITH_PARAM(__UTILS_ENUM_SERIALIZER_CONVERTER,enumName,(__VA_ARGS__)) } return false; } } \
        template <> inline bool ::utils::enums::toEnum<##enumName##,int64_t>(const int64_t val, enumName##& out) noexcept   { switch (static_cast<##enumName##>(val)) { PP_FOREACH_WITH_PARAM(__UTILS_ENUM_SERIALIZER_CONVERTER,enumName,(__VA_ARGS__)) } return false; } } \
        template <> inline bool ::utils::enums::toEnum<##enumName##,uint32_t>(const uint32_t val, enumName##& out) noexcept { switch (static_cast<##enumName##>(val)) { PP_FOREACH_WITH_PARAM(__UTILS_ENUM_SERIALIZER_CONVERTER,enumName,(__VA_ARGS__)) } return false; } } \
        template <> inline bool ::utils::enums::toEnum<##enumName##,int32_t>(const int32_t val, enumName##& out) noexcept   { switch (static_cast<##enumName##>(val)) { PP_FOREACH_WITH_PARAM(__UTILS_ENUM_SERIALIZER_CONVERTER,enumName,(__VA_ARGS__)) } return false; } \
        template <> inline bool ::utils::enums::toEnum<##enumName##,uint16_t>(const uint16_t val, enumName##& out) noexcept { switch (static_cast<##enumName##>(val)) { PP_FOREACH_WITH_PARAM(__UTILS_ENUM_SERIALIZER_CONVERTER,enumName,(__VA_ARGS__)) } return false; } \
        template <> inline bool ::utils::enums::toEnum<##enumName##,int16_t>(const int16_t val, enumName##& out) noexcept   { switch (static_cast<##enumName##>(val)) { PP_FOREACH_WITH_PARAM(__UTILS_ENUM_SERIALIZER_CONVERTER,enumName,(__VA_ARGS__)) } return false; } \
        template <> inline bool ::utils::enums::toEnum<##enumName##,uint8_t>(const uint8_t val, enumName##& out) noexcept   { switch (static_cast<##enumName##>(val)) { PP_FOREACH_WITH_PARAM(__UTILS_ENUM_SERIALIZER_CONVERTER,enumName,(__VA_ARGS__)) } return false; } \
        template <> inline bool ::utils::enums::toEnum<##enumName##,int8_t>(const int8_t val, enumName##& out) noexcept     { switch (static_cast<##enumName##>(val)) { PP_FOREACH_WITH_PARAM(__UTILS_ENUM_SERIALIZER_CONVERTER,enumName,(__VA_ARGS__)) } return false; } \
        template <> inline bool ::utils::enums::toEnum<##enumName##,bool>(const bool val, enumName##& out) noexcept         { switch (static_cast<##enumName##>(val)) { PP_FOREACH_WITH_PARAM(__UTILS_ENUM_SERIALIZER_CONVERTER,enumName,(__VA_ARGS__)) } return false; }

#define __UTILS_ENUM_SERIALIZER_SERIALIZER(enumName,enumVal)  template<> static inline std::string toString<##enumName##::##enumVal##>() { return #enumVal; }
#define __UTILS_ENUM_SERIALIZER_CONVERTER(enumName,enumVal)   case ##enumName##::##enumVal##: { out = enumName##::##enumVal##; return true; break; }
