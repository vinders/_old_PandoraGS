/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : character management toolset
*******************************************************************************/
#pragma once

#include <cstdint>
#include <type_traits>

#define _STATIC_ASSERT_IS_CHARACTER_TYPE(val) \
        static_assert((std::is_same<decltype(val),char>::value         || std::is_same<decltype(val),wchar_t>::value        || std::is_same<decltype(val),char16_t>::value \
                    || std::is_same<decltype(val),char32_t>::value     || std::is_same<decltype(val),utils::unicode_char_t>::value \
                    || std::is_same<decltype(val),unsigned int>::value || std::is_same<decltype(val),int>::value \
                    || std::is_same<decltype(val),uint32_t>::value || std::is_same<decltype(val),uint16_t>::value           || std::is_same<decltype(val),uint8_t>::value>::value), \
                    "utils.Char: character type required : char, wchar_t, char16_t, char32_t, unicode_char_t, uin8_t, uint16_t, uint32_t")
#define _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(val) \
        static_assert((std::is_same<decltype(val),wchar_t>::value               || std::is_same<decltype(val),char16_t>::value     || std::is_same<decltype(val),char32_t>::value \
                    || std::is_same<decltype(val),utils::unicode_char_t>::value || std::is_same<decltype(val),uint32_t>::value \
                    || std::is_same<decltype(val),uint16_t>::value              || std::is_same<decltype(val),unsigned int>::value || std::is_same<decltype(val),int>::value), \
                    "utils.Char: wide character type required : wchar_t, char16_t, char32_t, unicode_char_t, uint16_t, uint32_t")

/// @namespace utils
/// General utilities
namespace utils
{
    using unicode_char_t = char32_t; ///< Unicode (UCS) character
    
    /// @class Char
    /// @brief Character management toolset
    class Char
    {
    public:
        // -- Character traits --
    
        /// @enum char_family_t
        /// @brief Character set family
        enum class char_family_t : uint32_t
        {
            not_alpha = 0, ///< Non-alphabetic character
            latin = 1,     ///< Latin ASCII or extended
            greek = 2,     ///< Greek and coptic
            cyrillic = 3   ///< Cyrillic
        }
        
        /// @struct char_traits_t
        /// @brief Unicode character traits
        struct char_traits_t
        {
            char_family_t family; ///< Character family
            bool isUpper;         ///< If alphabetic, upper-case or not
            unicode_char_t equiv; ///< If alphabetic, upper/lower-case equivalent character
        }
        
        /// @brief Get unicode character traits
        /// @param[in] ucode  Unicode character code
        /// @returns Character traits for specified unicode character
        /// @source  en.wikipedia.org/wiki/List_of_Unicode_characters
        template <typename T>
        static utils::Char::char_traits_t getCharTraits(const T ucode) noexcept;
        /// @brief Get unicode character alphabetic family
        /// @param[in] ucode  Unicode character code
        /// @returns Alphabetic character family
        /// @source  en.wikipedia.org/wiki/List_of_Unicode_characters
        template <typename T>
        static utils::Char::char_family_t getCharFamily(const T ucode) noexcept;
        
        
        
        // -- Character traits - special characters --
        
        /// @brief Check if character is a white space character (' ', '\n', '\t', '\r', ...)
        /// @param[in] code  Character code
        /// @returns White space character (true) or not
        template <typename T>
        static inline bool isWhiteSpace(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (code == 0x20u || (code <= 0x0Du && code >= 0x08u));
        }
        /// @brief Check if character is a control character
        /// @param[in] code  Character code
        /// @returns Control character (true) or not
        template <typename T>
        static inline bool isControlChar(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            if (std::is_signed<T>::value)
                return (code <= 0x1F && code >= 0);
            else
                return (code <= 0x1Fu);
        }
        
        /// @brief Check if character is a simple number (0 to 9)
        /// @param[in] code  Character code
        /// @returns Number character (true) or not
        template <typename T>
        static inline bool isNumber(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (code <= 0x39u && code >= 0x30u);
        }
        /// @brief Check if character is an extended number (0 to 9, ², ³, ½, ...)
        /// @param[in] code  Character code
        /// @returns Extended number character (true) or not
        template <typename T>
        static inline bool isNumberExtended(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            if (code <= 0x39u && code >= 0x30u)
                return true;
            switch (code)
            {
                case 0xB2u: 
                case 0xB3u: 
                case 0xB9u: 
                case 0xBAu: 
                case 0xBCu: 
                case 0xBDu: 
                case 0xBEu: 
                    return true; break;
            } 
            return false;
        }
        
        
        // -- Character traits - alphabetic --
        
        /// @brief Check if character is an ASCII alphabetic character (A to Z, a to z)
        /// @param[in] code  Character code
        /// @returns ASCII alphabetic character (true) or not
        template <typename T>
        static inline bool isAlphaAscii(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (code <= 0x7Au && code >= 0x41u && (code <= 0x5Au || code >= 0x61u));
        }
        /// @brief Check if character an ASCII alphabetic character or a simple number (A to Z, a to z, 0 to 9)
        /// @param[in] code  Character code
        /// @returns ASCII alphabetic character or number (true) or other character (false)
        template <typename T>
        static inline bool isAlphaNumAscii(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (code <= 0x7Au && code >= 0x30u && (code >= 0x61u || code <= 0x39u || (code <= 0x5Au && code >= 0x41u)));
        }
        
        /// @brief Check if character is a latin alphabetic character
        /// @param[in] ucode  Unicode character code (UCS)
        /// @returns Latin alphabetic character (true) or not
        template <typename T>
        static inline bool isAlphaLatin(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            return (getCharFamily(ucode) == utils::Char::char_family_t::latin);
        }
        /// @brief Check if character a latin alphabetic character or an extended number
        /// @param[in] ucode  Unicode character code (UCS)
        /// @returns Latin alphabetic character or extended number (true) or other character (false)
        template <typename T>
        static inline bool isAlphaNumLatin(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            return (getCharFamily(ucode) == utils::Char::char_family_t::latin || isNumberExtended(ucode));
        }
        
        /// @brief Check if character is a unicode alphabetic character (latin / greek / cyrillic)
        /// @param[in] ucode  Unicode character code (UCS)
        /// @returns Alphabetic character (true) or not
        template <typename T>
        static inline bool isAlphaUnicode(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            return (getCharFamily(ucode) != utils::Char::char_family_t::not_alpha);
        }
        /// @brief Check if character a unicode alphabetic character (latin / greek / cyrillic) or an extended number
        /// @param[in] ucode  Unicode character code (UCS)
        /// @returns Alphabetic character or extended number (true) or other character (false)
        template <typename T>
        static inline bool isAlphaNumUnicode(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            return (getCharFamily(ucode) != utils::Char::char_family_t::not_alpha || isNumberExtended(ucode));
        }

        
        // -- Character traits - upper/lower-case --
        
        template <typename T>
        static inline bool isLowerAscii(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (code <= 0x7Au && code >= 0x61u);
        }
        template <typename T>
        static inline bool isUpperAscii(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (code <= 0x5Au && code >= 0x41u);
        }
        
        template <typename T>
        static inline bool isLowerLatin(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            utils::Char::char_traits_t info = getCharTraits(ucode);
            return (info.isUpper == false && info.family == utils::Char::char_family_t::latin);
        }
        template <typename T>
        static inline bool isUpperLatin(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            utils::Char::char_traits_t info = getCharTraits(ucode);
            return (info.isUpper && info.family == utils::Char::char_family_t::latin);
        }
        
        template <typename T>
        static inline bool isLowerUnicode(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            utils::Char::char_traits_t info = getCharTraits(ucode);
            return (info.isUpper == false && info.family != utils::Char::char_family_t::not_alpha);
        }
        template <typename T>
        static inline bool isUpperUnicode(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            return getCharTraits(ucode).isUpper;
        }
        
        
        // -- Character traits - upper/lower-case converters --
        
        template <typename T>
        static inline T toLowerAscii(const T val) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (isUpperAscii(code)) ? (code + 0x20u) : code;
        }
        template <typename T>
        static inline T toUpperAscii(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (isLowerAscii(code)) ? (code - 0x20u) : code;
        }
        
        template <typename T>
        static inline T toLowerUnicode(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            utils::Char::char_traits_t info = getCharTraits(ucode);
            return (info.isUpper) ? info.equiv : ucode;
        }
        template <typename T>
        static inline T toUpperUnicode(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            utils::Char::char_traits_t info = getCharTraits(ucode);
            return (info.isUpper == false && info.family != utils::Char::char_family_t::not_alpha) ? info.equiv : ucode;
        }
        
        
        
        // -- Compare --
        
        template <typename T>
        static inline bool equals(const T lhs, const T rhs) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(lhs);
            return (lhs == rhs);
        }
        template <typename T>
        static inline bool iequalsAscii(const T lhs, const T rhs) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(lhs);
            if (lhs == rhs)
                return true;
            return (lhs > 0x005Au) ? (lhs - 0x20u == rhs) : (lhs + 0x20u == rhs);
        }
        template <typename T>
        static inline bool iequalsUnicode(const T lhs, const T rhs) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(lhs);
            if (lhs == rhs)
                return true;
            utils::Char::char_traits_t info = getCharTraits(lhs);
            return (info.equiv == rhs);
        }
        
        
        // -- Utilities --
        
        static inline bool isEscaped(const char* val, const uint32_t pos, const char escapeChar = '\\')
        {
            
        }
        static inline bool isEscapedBetweenMarkers(const char* val, const uint32_t pos, const char escapeChar = '\\', const char marker = '"')
        {
            
        }
        static inline bool isBetweenMarkers(const char* val, const uint32_t pos, const char marker = '"')
        {
            
        }
        
        static inline bool isEscaped(const char* val, const size_t len, const uint32_t pos, const char esc = '\\')
        {
            
        }
        static inline bool isEscapedBetweenMarkers(const char* val, const size_t len, const uint32_t pos, const char esc = '\\', const char marker = '"')
        {
            
        }
        static inline bool isBetweenMarkers(const char* val, const size_t len, const uint32_t pos, const char marker = '"')
        {
            
        }
    };
}

// -- Specialized templates --

template <> static utils::Char::char_traits_t utils::Char::getCharTraits<wchar_t>(const wchar_t ucode)
template <> static utils::Char::char_traits_t utils::Char::getCharTraits<char16_t>(const char16_t ucode)
template <> static utils::Char::char_traits_t utils::Char::getCharTraits<char32_t>(const char32_t ucode)
template <> static utils::Char::char_traits_t utils::Char::getCharTraits<utils::unicode_char_t>(const utils::unicode_char_t ucode)
template <> static utils::Char::char_traits_t utils::Char::getCharTraits<int>(const int ucode)
template <> static utils::Char::char_traits_t utils::Char::getCharTraits<unsigned int>(const unsigned int ucode)
template <> static utils::Char::char_traits_t utils::Char::getCharTraits<uint16_t>(const uint16_t ucode)
template <> static utils::Char::char_traits_t utils::Char::getCharTraits<uint32_t>(const uint32_t ucode)
template <> static utils::Char::char_traits_t utils::Char::getCharTraits<int32_t>(const int32_t ucode)

template <> static utils::Char::char_family_t utils::Char::getCharFamily<wchar_t>(const wchar_t ucode)
template <> static utils::Char::char_family_t utils::Char::getCharFamily<char16_t>(const char16_t ucode)
template <> static utils::Char::char_family_t utils::Char::getCharFamily<char32_t>(const char32_t ucode)
template <> static utils::Char::char_family_t utils::Char::getCharFamily<utils::unicode_char_t>(const utils::unicode_char_t ucode)
template <> static utils::Char::char_family_t utils::Char::getCharFamily<int>(const int ucode)
template <> static utils::Char::char_family_t utils::Char::getCharFamily<unsigned int>(const unsigned int ucode)
template <> static utils::Char::char_family_t utils::Char::getCharFamily<uint16_t>(const uint16_t ucode)
template <> static utils::Char::char_family_t utils::Char::getCharFamily<uint32_t>(const uint32_t ucode)
template <> static utils::Char::char_family_t utils::Char::getCharFamily<int32_t>(const int32_t ucode)
