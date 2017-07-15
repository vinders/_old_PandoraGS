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
        
        /// @brief Check if ASCII character is alphabetic and lower-case
        /// @param[in] code  Character code
        /// @returns Lower-case ASCII alphabetic character (true) or not
        template <typename T>
        static inline bool isLowerAscii(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (code <= 0x7Au && code >= 0x61u);
        }
        /// @brief Check if ASCII character is alphabetic and upper-case
        /// @param[in] code  Character code
        /// @returns Upper-case ASCII alphabetic character (true) or not
        template <typename T>
        static inline bool isUpperAscii(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (code <= 0x5Au && code >= 0x41u);
        }
        
        /// @brief Check if latin unicode character is alphabetic and lower-case
        /// @param[in] ucode  Unicode character code (UCS)
        /// @returns Lower-case latin alphabetic character (true) or not
        template <typename T>
        static inline bool isLowerLatin(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            utils::Char::char_traits_t info = getCharTraits(ucode);
            return (info.isUpper == false && info.family == utils::Char::char_family_t::latin);
        }
        /// @brief Check if latin unicode character is alphabetic and upper-case
        /// @param[in] ucode  Unicode character code (UCS)
        /// @returns Upper-case latin alphabetic character (true) or not
        template <typename T>
        static inline bool isUpperLatin(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            utils::Char::char_traits_t info = getCharTraits(ucode);
            return (info.isUpper && info.family == utils::Char::char_family_t::latin);
        }
        
        /// @brief Check if unicode character is alphabetic and lower-case
        /// @param[in] ucode  Unicode character code (UCS)
        /// @returns Lower-case unicode alphabetic character (true) or not
        template <typename T>
        static inline bool isLowerUnicode(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            utils::Char::char_traits_t info = getCharTraits(ucode);
            return (info.isUpper == false && info.family != utils::Char::char_family_t::not_alpha);
        }
        /// @brief Check if unicode character is alphabetic and upper-case
        /// @param[in] ucode  Unicode character code (UCS)
        /// @returns Upper-case unicode alphabetic character (true) or not
        template <typename T>
        static inline bool isUpperUnicode(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            return getCharTraits(ucode).isUpper;
        }
        
        
        // -- Character traits - upper/lower-case converters --
        
        /// @brief Convert a character to lower-case
        /// @param[in] code  Character code
        /// @returns Lower-case value (if alphabetic, otherwise same value)
        template <typename T>
        static inline T toLowerAscii(const T val) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (isUpperAscii(code)) ? (code + 0x20u) : code;
        }
        /// @brief Convert a character to upper-case
        /// @param[in] code  Character code
        /// @returns Upper-case value (if alphabetic, otherwise same value)
        template <typename T>
        static inline T toUpperAscii(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (isLowerAscii(code)) ? (code - 0x20u) : code;
        }
        
        /// @brief Convert a unicode character to lower-case
        /// @param[in] ucode  Unicode character code (UCS)
        /// @returns Lower-case value (if alphabetic, otherwise same value)
        template <typename T>
        static inline T toLowerUnicode(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            utils::Char::char_traits_t info = getCharTraits(ucode);
            return (info.isUpper) ? info.equiv : ucode;
        }
        /// @brief Convert a unicode character to upper-case
        /// @param[in] ucode  Unicode character code (UCS)
        /// @returns Upper-case value (if alphabetic, otherwise same value)
        template <typename T>
        static inline T toUpperUnicode(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            utils::Char::char_traits_t info = getCharTraits(ucode);
            return (info.isUpper == false && info.family != utils::Char::char_family_t::not_alpha) ? info.equiv : ucode;
        }
        
        
        
        // -- Compare --
        
        /// @brief Compare two characters (equality)
        /// @param[in] lhs  First character code
        /// @param[in] rhs  Second character code
        /// @returns Equal (true) or not
        template <typename T>
        static inline bool equals(const T lhs, const T rhs) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(lhs);
            return (lhs == rhs);
        }
        
        /// @brief Compare two ASCII characters (case insensitive equality)
        /// @param[in] lhs  First character code
        /// @param[in] rhs  Second character code
        /// @returns Case insensitive equality (true) or not
        template <typename T>
        static inline bool iequalsAscii(const T lhs, const T rhs) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(lhs);
            if (lhs == rhs)
                return true;
            return (lhs > 0x005Au) ? (lhs - 0x20u == rhs) : (lhs + 0x20u == rhs);
        }
        /// @brief Compare two unicode characters (case insensitive equality)
        /// @param[in] lhs  First unicode character code (UCS)
        /// @param[in] rhs  Second unicode character code (UCS)
        /// @returns Case insensitive equality (true) or not
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
        
        /// @brief Check whether a character from a string is escaped or not
        /// @param[in] data        Entire string containing the character
        /// @param[in] pos         Position of the character inside the string
        /// @param[in] escapeChar  Escape indication character (default: backslash)
        /// @returns Escaped (true) or not
        template <typename T>
        static inline bool isEscaped(const std::basic_string<T> data, uint32_t pos, const T escapeChar = T { 0x5C })
        {
            return (pos < data.size() && isEscaped(data.c_str(), pos, escapeChar));
        }
        /// @brief Check whether a character from a string is escaped or not
        /// @param[in] data        Entire string containing the character
        /// @param[in] len         Length of the string 'data'
        /// @param[in] pos         Position of the character inside the string
        /// @param[in] escapeChar  Escape indication character (default: backslash)
        /// @returns Escaped (true) or not
        template <typename T>
        static inline bool isEscaped(const T* data, const size_t len, uint32_t pos, const T escapeChar = T { 0x5C })
        {
            return (pos < len && isEscaped(data, pos, escapeChar));
        }
        /// @brief Check whether a character from a string is escaped or not
        /// @param[in] data        Entire string containing the character
        /// @param[in] pos         Position of the character inside the string
        /// @param[in] escapeChar  Escape indication character (default: backslash)
        /// @returns Escaped (true) or not
        /// @warning Does not check if the position is out of range
        template <typename T>
        static inline bool isEscaped(const T* data, uint32_t pos, const T escapeChar = T { 0x5C })
        {
            bool isEsc = false;
            if (data != nullptr)
            {
                for (const T* it = &data[pos]; pos > 0 && *(--it) == escapeChar; --pos)
                    isEsc = !isEsc;
            }
            return isEsc;
        }
        
        
        /// @brief Check whether a character from a string is located after an non-escaped mark
        /// @param[in] data        Entire string containing the character
        /// @param[in] pos         Position of the character inside the string
        /// @param[in] mark        Mark indication character (default: quotation mark ('"'))
        /// @param[in] escapeChar  Escape indication character (default: backslash)
        /// @returns Escaped after mark (true) or not
        template <typename T>
        static inline bool isAfterMark(const std::basic_string<T> data, const uint32_t pos, const char mark = T { 0x22 }, const T escapeChar = T { 0x5C })
        {
            return (pos < data.size() && isAfterMark(data.c_str(), pos, mark, escapeChar));
        }
        /// @brief Check whether a character from a string is located after an non-escaped mark
        /// @param[in] data        Entire string containing the character
        /// @param[in] len         Length of the string 'data'
        /// @param[in] pos         Position of the character inside the string
        /// @param[in] mark        Mark indication character (default: double quotation mark ('"'))
        /// @param[in] escapeChar  Escape indication character (default: backslash)
        /// @returns Escaped after mark (true) or not
        template <typename T>
        static inline bool isAfterMark(const T* data, const size_t len, const uint32_t pos, const T mark = T { 0x22 }, const T escapeChar = T { 0x5C })
        {
            return (pos < len && isAfterMark(data, pos, mark, escapeChar));
        }
        /// @brief Check whether a character from a string is located after an non-escaped mark
        /// @param[in] data        Entire string containing the character
        /// @param[in] pos         Position of the character inside the string
        /// @param[in] mark        Mark indication character (default: quotation mark ('"'))
        /// @param[in] escapeChar  Escape indication character (default: backslash)
        /// @returns Escaped after mark (true) or not
        /// @warning Does not check if the position is out of range
        template <typename T>
        static bool isAfterMark(const T* data, const uint32_t pos, const T mark = T { 0x22 }, const T escapeChar = T { 0x5C });
        
        
        /// @brief Check whether a character from a string is located between non-escaped marks
        /// @param[in] data        Entire string containing the character
        /// @param[in] pos         Position of the character inside the string
        /// @param[in] mark        Mark indication character (default: quotation mark ('"'))
        /// @param[in] escapeChar  Escape indication character (default: backslash)
        /// @returns Escaped between marks (true) or not
        template <typename T>
        static inline bool isBetweenMarks(const std::basic_string<T> data, const uint32_t pos, const char mark = T { 0x22 }, const T escapeChar = T { 0x5C })
        {
            return isBetweenMarks(data.c_str(), data.size(), pos, mark, escapeChar);
        }
        /// @brief Check whether a character from a string is located between non-escaped marks
        /// @param[in] data        Entire string containing the character
        /// @param[in] len         Length of the string 'data'
        /// @param[in] pos         Position of the character inside the string
        /// @param[in] mark        Mark indication character (default: double quotation mark ('"'))
        /// @param[in] escapeChar  Escape indication character (default: backslash)
        /// @returns Escaped between marks (true) or not
        template <typename T>
        static bool isBetweenMarks(const T* data, const size_t len, const uint32_t pos, const T mark = T { 0x22 }, const T escapeChar = T { 0x5C })
        {
            bool isInsideMarks = false;
            if (data != nullptr && pos + 1 < len && isAfterMark(data, pos, mark, escapeChar))
            {
                ++pos;
                const T* it = &data[pos];
                while (pos < len)
                {
                    if (*it == mark && !isEscaped(it, pos, escapeChar))
                        isInsideMarks = !isInsideMarks;
                    ++pos;
                    ++it;
                }
            }
            return isInsideMarks;
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
