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

/// @namespace utils
/// General utilities
namespace utils
{
    /// @class Char
    /// @brief Character management toolset
    class Char
    {
    public:
        template <typename T>
        static inline bool isWhiteSpace(const T val) noexcept
        {
            static_assert((std::is_same<decltype(val),char>::value || std::is_same<decltype(val),wchar_t>::value || std::is_same<decltype(val),char16_t>::value || std::is_same<decltype(val),char32_t>::value 
                        || std::is_same<decltype(val),uint32_t>::value || std::is_same<decltype(val),uint16_t>::value || std::is_same<decltype(val),uint8_t>::value>::value || std::is_same<decltype(val),unsigned int>::value), 
                        "character type required : char, wchar_t, char16_t, char32_t, uin8_t, uint16_t, uint32_t");
            return (val == 0x20u || (val <= 0x0Du && val >= 0x08u));
        }
        
        template <typename T>
        static inline bool isControlChar(const T val) noexcept
        {
            static_assert((std::is_same<decltype(val),char>::value || std::is_same<decltype(val),wchar_t>::value || std::is_same<decltype(val),char16_t>::value || std::is_same<decltype(val),char32_t>::value 
                        || std::is_same<decltype(val),uint32_t>::value || std::is_same<decltype(val),uint16_t>::value || std::is_same<decltype(val),uint8_t>::value>::value || std::is_same<decltype(val),unsigned int>::value), 
                        "character type required : char, wchar_t, char16_t, char32_t, uin8_t, uint16_t, uint32_t");
            return (val <= 0x1Fu);
        }
        template <> static inline bool isControlChar<char>(const char val) noexcept
        {
            return (val <= 0x1F && val >= 0);
        }
        
        template <typename T>
        static inline bool isNum(const T val) noexcept
        {
            static_assert((std::is_same<decltype(val),char>::value || std::is_same<decltype(val),wchar_t>::value || std::is_same<decltype(val),char16_t>::value || std::is_same<decltype(val),char32_t>::value 
                        || std::is_same<decltype(val),uint32_t>::value || std::is_same<decltype(val),uint16_t>::value || std::is_same<decltype(val),uint8_t>::value>::value || std::is_same<decltype(val),unsigned int>::value), 
                        "character type required : char, wchar_t, char16_t, char32_t, uin8_t, uint16_t, uint32_t");
            return (val <= 0x39u && val >= 0x30u);
        }
        
        
        template <typename T>
        static inline bool isAlphaAscii(const T val) noexcept
        {
            static_assert((std::is_same<decltype(val),char>::value || std::is_same<decltype(val),wchar_t>::value || std::is_same<decltype(val),char16_t>::value || std::is_same<decltype(val),char32_t>::value 
                        || std::is_same<decltype(val),uint32_t>::value || std::is_same<decltype(val),uint16_t>::value || std::is_same<decltype(val),uint8_t>::value>::value || std::is_same<decltype(val),unsigned int>::value), 
                        "character type required : char, wchar_t, char16_t, char32_t, uin8_t, uint16_t, uint32_t");
            return (val <= 0x7Au && val >= 0x41u && (val <= 0x5Au || val >= 0x61u));
        }
        template <typename T>
        static inline bool isAlphaNumAscii(const T val) noexcept
        {
            static_assert((std::is_same<decltype(val),char>::value || std::is_same<decltype(val),wchar_t>::value || std::is_same<decltype(val),char16_t>::value || std::is_same<decltype(val),char32_t>::value 
                        || std::is_same<decltype(val),uint32_t>::value || std::is_same<decltype(val),uint16_t>::value || std::is_same<decltype(val),uint8_t>::value>::value || std::is_same<decltype(val),unsigned int>::value), 
                        "character type required : char, wchar_t, char16_t, char32_t, uin8_t, uint16_t, uint32_t");
            return (val <= 0x7Au && val >= 0x30u && (val >= 0x61u || val <= 0x39u || (val <= 0x5Au && val >= 0x41u)));
        }
        
        template <typename T>
        static inline bool isLowerAscii(const T val) noexcept
        {
            static_assert((std::is_same<decltype(val),char>::value || std::is_same<decltype(val),wchar_t>::value || std::is_same<decltype(val),char16_t>::value || std::is_same<decltype(val),char32_t>::value 
                        || std::is_same<decltype(val),uint32_t>::value || std::is_same<decltype(val),uint16_t>::value || std::is_same<decltype(val),uint8_t>::value>::value || std::is_same<decltype(val),unsigned int>::value), 
                        "character type required : char, wchar_t, char16_t, char32_t, uin8_t, uint16_t, uint32_t");
            return (val <= 0x7Au && val >= 0x61u);
        }
        template <typename T>
        static inline bool isUpperAscii(const T val) noexcept
        {
            static_assert((std::is_same<decltype(val),char>::value || std::is_same<decltype(val),wchar_t>::value || std::is_same<decltype(val),char16_t>::value || std::is_same<decltype(val),char32_t>::value 
                        || std::is_same<decltype(val),uint32_t>::value || std::is_same<decltype(val),uint16_t>::value || std::is_same<decltype(val),uint8_t>::value>::value || std::is_same<decltype(val),unsigned int>::value), 
                        "character type required : char, wchar_t, char16_t, char32_t, uin8_t, uint16_t, uint32_t");
            return (val <= 0x5Au && val >= 0x41u);
        }
        
        
        template <typename T>
        static inline bool isAlphaUnicode(const T val) noexcept
        {
            static_assert((std::is_same<decltype(val),wchar_t>::value || std::is_same<decltype(val),char16_t>::value || std::is_same<decltype(val),char32_t>::value 
                        || std::is_same<decltype(val),uint32_t>::value || std::is_same<decltype(val),uint16_t>::value || std::is_same<decltype(val),unsigned int>::value), 
                        "wide character type required : wchar_t, char16_t, char32_t, uint16_t, uint32_t");
            return (isAlphaAscii(val) || m_specialAlphaChars.count(static_cast<uint32_t>(val)));
        }
        template <typename T>
        static inline bool isAlphaNumUnicode(const T val) noexcept
        {
            static_assert((std::is_same<decltype(val),wchar_t>::value || std::is_same<decltype(val),char16_t>::value || std::is_same<decltype(val),char32_t>::value 
                        || std::is_same<decltype(val),uint32_t>::value || std::is_same<decltype(val),uint16_t>::value || std::is_same<decltype(val),unsigned int>::value), 
                        "wide character type required : wchar_t, char16_t, char32_t, uint16_t, uint32_t");
            return (isAlphaNumAscii(val) || m_specialAlphaChars.count(static_cast<uint32_t>(val)));
        }
        
        template <typename T>
        static inline bool isLowerUnicode(const T val) noexcept
        {
            static_assert((std::is_same<decltype(val),wchar_t>::value || std::is_same<decltype(val),char16_t>::value || std::is_same<decltype(val),char32_t>::value 
                        || std::is_same<decltype(val),uint32_t>::value || std::is_same<decltype(val),uint16_t>::value || std::is_same<decltype(val),unsigned int>::value), 
                        "wide character type required : wchar_t, char16_t, char32_t, uint16_t, uint32_t");
            return (isLowerAscii(val) || (m_specialAlphaChars.count(static_cast<uint32_t>(val)) && m_specialAlphaChars[static_cast<uint32_t>(val)].isUpper == false));
        }
        template <typename T>
        static inline bool isUpperUnicode(const T val) noexcept
        {
            static_assert((std::is_same<decltype(val),wchar_t>::value || std::is_same<decltype(val),char16_t>::value || std::is_same<decltype(val),char32_t>::value 
                        || std::is_same<decltype(val),uint32_t>::value || std::is_same<decltype(val),uint16_t>::value || std::is_same<decltype(val),unsigned int>::value), 
                        "wide character type required : wchar_t, char16_t, char32_t, uint16_t, uint32_t");
            return (isUpperAscii(val) || (m_specialAlphaChars.count(static_cast<uint32_t>(val)) && m_specialAlphaChars[static_cast<uint32_t>(val)].isUpper));
        }
        
        
        static inline bool isEscaped(const char* val, const uint32_t pos, const char escapeChar = '\\')
        {
            
        }
        static inline bool isEscapedInQuotes(const char* val, const uint32_t pos, const char escapeChar = '\\')
        {
            
        }
        static inline bool isBetweenQuotes(const char* val, const uint32_t pos)
        {
            
        }
        
        static inline bool isEscaped(const char* val, const size_t len, const uint32_t pos, const char escapeChar = '\\')
        {
            
        }
        static inline bool isEscapedInQuotes(const char* val, const size_t len, const uint32_t pos, const char escapeChar = '\\')
        {
            
        }
        static inline bool isBetweenQuotes(const char* val, const size_t len, const uint32_t pos)
        {
            
        }
        
        
        template <typename T>
        static inline bool equals(const T lhs, const T rhs) noexcept
        {
            return (lhs == rhs);
        }
        template <typename T>
        static inline bool equalsIgnoreCaseAscii(const T lhs, const T rhs) noexcept
        {
            return (isUpperAscii(lhs)) ? (lhs == toUpperAscii(rhs)) : (lhs == toLowerAscii(rhs));
        }
        template <typename T>
        static inline bool equalsIgnoreCaseUnicode(const T lhs, const T rhs) noexcept
        {
            return (isUpperUnicode(lhs)) ? (lhs == toUpperUnicode(rhs)) : (lhs == toLowerUnicode(rhs));
        }
        
        
        template <typename T>
        static inline T toLowerAscii(const T val) noexcept
        {
            return (isUpperAscii(val)) ? (val - 0x20u) : val;
        }
        template <typename T>
        static inline T toUpperAscii(const T val) noexcept
        {
            return (isLowerAscii(val)) ? (val + 0x20u) : val;
        }
        
        template <typename T>
        static inline T toLowerUnicode(const T val) noexcept
        {
            static_assert((std::is_same<decltype(val),wchar_t>::value || std::is_same<decltype(val),char16_t>::value || std::is_same<decltype(val),char32_t>::value 
                        || std::is_same<decltype(val),uint32_t>::value || std::is_same<decltype(val),uint16_t>::value || std::is_same<decltype(val),unsigned int>::value), 
                        "wide character type required : wchar_t, char16_t, char32_t, uint16_t, uint32_t");
            if (isUpperAscii(val)) 
                return (val - 0x20u);
            else if (m_specialAlphaChars.count(static_cast<uint32_t>(val)) && m_specialAlphaChars[static_cast<uint32_t>(val)].isUpper)
                return static_cast<T>(m_specialAlphaChars[static_cast<uint32_t>(val)].character);
            return val;
        }
        template <typename T>
        static inline T toUpperUnicode(const T val) noexcept
        {
            static_assert((std::is_same<decltype(val),wchar_t>::value || std::is_same<decltype(val),char16_t>::value || std::is_same<decltype(val),char32_t>::value 
                        || std::is_same<decltype(val),uint32_t>::value || std::is_same<decltype(val),uint16_t>::value || std::is_same<decltype(val),unsigned int>::value), 
                        "wide character type required : wchar_t, char16_t, char32_t, uint16_t, uint32_t");
            if (isLowerAscii(val)) 
                return (val + 0x20u);
            else if (m_specialAlphaChars.count(static_cast<uint32_t>(val)) && m_specialAlphaChars[static_cast<uint32_t>(val)].isUpper == false)
                return static_cast<T>(m_specialAlphaChars[static_cast<uint32_t>(val)].character);
            return val;
        }
        
        
    private:
        struct char_equiv_t
        {
            bool isUpper;
            uint32_t character;
        }
    
        static std::unordered_map<uint32_t, char_equiv_t> m_specialAlphaChars;//init dans fichier CPP !!!! -> y mettre tous les caractères spéciaux MAJ et min, avec pour chacun l'équivalence (min/MAJ) et le flag qui dit si upper/lower
    };
}
