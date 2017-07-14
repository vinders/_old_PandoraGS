/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : character management toolset
*******************************************************************************/
#pragma once

#include <cstdint>
#include <unordered_map>
#include <type_traits>

#define _STATIC_ASSERT_IS_CHARACTER_TYPE(val) \
        static_assert((std::is_same<decltype(val),char>::value     || std::is_same<decltype(val),wchar_t>::value        || std::is_same<decltype(val),char16_t>::value \
                    || std::is_same<decltype(val),char32_t>::value || std::is_same<decltype(val),unicode_char_t>::value || std::is_same<decltype(val),unsigned int>::value \
                    || std::is_same<decltype(val),uint32_t>::value || std::is_same<decltype(val),uint16_t>::value       || std::is_same<decltype(val),uint8_t>::value>::value), \
                    "utils.Char: character type required : char, wchar_t, char16_t, char32_t, unicode_char_t, uin8_t, uint16_t, uint32_t")
#define _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(val) \
        static_assert((std::is_same<decltype(val),wchar_t>::value        || std::is_same<decltype(val),char16_t>::value || std::is_same<decltype(val),char32_t>::value \
                    || std::is_same<decltype(val),unicode_char_t>::value || std::is_same<decltype(val),uint32_t>::value \
                    || std::is_same<decltype(val),uint16_t>::value       || std::is_same<decltype(val),unsigned int>::value), \
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
        static utils::Char::char_traits_t getCharTraits(const utils::unicode_char_t ucode) noexcept 
        { 
            if (ucode <= 0x00FFu)
            {
                // Latin basic (ASCII)
                if (ucode <= 0x007Au && ucode >= 0x0041u)
                {
                    if (ucode <= 0x005Au) // upper-case
                        return char_traits_t { char_family_t::latin, true, ucode + 0x0020u }; 
                    else if (ucode >= 0x0061u) // lower-case
                        return char_traits_t { char_family_t::latin, false, ucode - 0x0020u }; 
                    else if (ucode >= 0x00A2u) // special alphabetic characters
                    {
                        if (ucode <= 0x00A5u || ucode == 0x00A9u || ucode == 0x00AE || ucode == 0x00B5)
                            return char_traits_t { char_family_t::latin, false, ucode }; 
                    }
                }
                
                // Latin-1 supplement
                else if (ucode >= 0x00C0u)
                {
                    if (ucode <= 0x00DEu)
                    {
                        if (ucode != 0x00D7u)
                            return char_traits_t { char_family_t::latin, true, ucode + 0x0020u }; 
                    }
                    else
                    {
                        if (ucode == 0x00DFu)
                            return char_traits_t { char_family_t::latin, false, ucode }; 
                        else if (ucode == 0x00FFu)
                            return char_traits_t { char_family_t::latin, false, 0x0178u }; 
                        else if (ucode != 0x00F7u)
                            return char_traits_t { char_family_t::latin, false, ucode - 0x0020u }; 
                    }
                }
            }
            else if (ucode <= 0x024Fu)
            {
                // Latin extended-A
                if (ucode <= 0x017Fu && ucode >= 0x0100u)
                {
                    if (ucode <= 0x0177u && (ucode <= 0x0137u || ucode >= 0x014Au))
                    {
                        return ((ucode & 0x0001u) == 0u) ? char_traits_t { char_family_t::latin, true, ucode + 0x0001u }
                                                         : char_traits_t { char_family_t::latin, false, ucode - 0x0001u }; 
                    }
                    else if (ucode >= 0x0179u || (ucode <= 0x0148u && ucode >= 0x0139u))
                    {
                        return ((ucode & 0x0001u) != 0u) ? char_traits_t { char_family_t::latin, true, ucode + 0x0001u }
                                                         : char_traits_t { char_family_t::latin, false, ucode - 0x0001u }; 
                    }
                    else if (ucode == 0x0178u)
                        return char_traits_t { char_family_t::latin, true, 0x00FFu };
                    else
                        return char_traits_t { char_family_t::latin, false, ucode };
                }
                
                // Latin extended-B
                else if (ucode >= 0x01CDu && ucode <= 0x0233u)
                {
                    if (ucode <= 0x01DCu)
                    {
                        return ((ucode & 0x0001u) != 0u) ? char_traits_t { char_family_t::latin, true, ucode + 0x0001u }
                                                         : char_traits_t { char_family_t::latin, false, ucode - 0x0001u }; 
                    }
                    else if (ucode <= 0x01EFu || ucode >= 0x01F4u)
                    {
                        if (ucode != 0x01DDu)
                        {
                            return ((ucode & 0x0001u) == 0u) ? char_traits_t { char_family_t::latin, true, ucode + 0x0001u }
                                                             : char_traits_t { char_family_t::latin, false, ucode - 0x0001u }; 
                        }
                        else
                            return char_traits_t { char_family_t::latin, false, ucode };
                    }
                    else if (ucode == 0x01F0u)
                        return char_traits_t { char_family_t::latin, false, ucode };
                    else
                        return char_traits_t { char_family_t::latin, true, ucode };
                }
                else if (ucode >= 0x0246u)
                {
                    return ((ucode & 0x0001u) == 0u) ? char_traits_t { char_family_t::latin, true, ucode + 0x0001u }
                                                             : char_traits_t { char_family_t::latin, false, ucode - 0x0001u }; 
                }
                else if (ucode <= 0x01BFu || ucode >= 0x00C4u)
                    return char_traits_t { char_family_t::latin, true, ucode };
            }
            // Latin extended additional
            else if (ucode <= 0x1EFF)
            {
                if (ucode <= 0x1E95u)
                {
                    if (ucode >= 0x1E00u)
                        return ((ucode & 0x0001u) == 0u) ? char_traits_t { char_family_t::latin, true, ucode + 0x0001u }
                                                             : char_traits_t { char_family_t::latin, false, ucode - 0x0001u }; 
                }
                else if (ucode >= 0x1E9Eu)
                {
                    return ((ucode & 0x0001u) == 0u) ? char_traits_t { char_family_t::latin, true, ucode + 0x0001u }
                                                             : char_traits_t { char_family_t::latin, false, ucode - 0x0001u }; 
                }
                else
                    return char_traits_t { char_family_t::latin, false, ucode };
            }

            // Greek
            else if ()
            {
            }
            // Cyrillic
            else if ()
            {
            }
            return char_traits_t { char_family_t::not_alpha, false, Ucode }; 
        }
        
        
        // -- Character traits - special characters --
        
        template <typename T>
        static inline bool isWhiteSpace(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (code == 0x20u || (code <= 0x0Du && code >= 0x08u));
        }
        
        template <typename T>
        static inline bool isControlChar(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (code <= 0x1Fu);
        }
        template <> static inline bool isControlChar<char>(const char code) noexcept
        {
            return (code <= 0x1F && code >= 0);
        }
        
        template <typename T>
        static inline bool isNumber(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (code <= 0x39u && code >= 0x30u);
        }
        
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
        
        template <typename T>
        static inline bool isAlphaAscii(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (code <= 0x7Au && code >= 0x41u && (code <= 0x5Au || code >= 0x61u));
        }
        template <typename T>
        static inline bool isAlphaNumAscii(const T code) noexcept
        {
            _STATIC_ASSERT_IS_CHARACTER_TYPE(code);
            return (code <= 0x7Au && code >= 0x30u && (code >= 0x61u || code <= 0x39u || (code <= 0x5Au && code >= 0x41u)));
        }
        
        template <typename T>
        static inline bool isAlphaLatin(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            return (getCharTraits(ucode).family == utils::Char::char_family_t::latin);
        }
        template <typename T>
        static inline bool isAlphaNumLatin(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            return (getCharTraits(ucode).family == utils::Char::char_family_t::latin || isNumberExtended(ucode));
        }
        
        template <typename T>
        static inline bool isAlphaUnicode(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            return (getCharTraits(ucode).family != utils::Char::char_family_t::not_alpha);
        }
        template <typename T>
        static inline bool isAlphaNumUnicode(const T ucode) noexcept
        {
            _STATIC_ASSERT_IS_WIDE_CHARACTER_TYPE(ucode);
            return (getCharTraits(ucode).family != utils::Char::char_family_t::not_alpha || isNumberExtended(ucode));
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
        
        
        // -- Comparisons --
        
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
