/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : string management toolset
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstring>
#include <string>
#include "char.h"

/// @namespace utils
/// General utilities
namespace utils
{
    using ustring = std::basic_string<utils::unicode_char_t>; ///< Unicode (UCS) string
    
    /// @class String
    /// @brief String management toolset
    class String
    {
    public:
        // String.isEmpty ("")
        static bool isEmpty(const char* data) { return (data == nullptr || data[0] == '\0'); }
        static bool isEmpty(const std::string data) { return (data.empty()); }
        static bool isEmpty(const std::wstring data) { return (data.empty()); }
        template <typename T>
        static bool isEmpty(const std::basic_string<T> data) { return (data.empty()); }
        
        // String.isEqual (==)
        static bool equals(const char* first, const char* second) { return (first == second || strcmp(first, second) == 0); }
        static bool equals(const std::string first, const std::string second) { return (first.compare(second) == 0); }
        static bool equals(const std::wstring first, const std::wstring second) { return (first.compare(second) == 0); }
        template <typename T>
        static bool equals(const std::basic_string<T> first, const std::basic_string<T> second) { return (data.empty()); }
        
        // String.startsWith (Abc[...])
        static bool startsWith(const std::string data, const std::string pattern) { return (data.find(pattern) == 0u); }
        static bool startsWith(const std::wstring data, const std::wstring pattern) { return (data.find(pattern) == 0u); }
        // String.endsWith ([...]abc)
        static bool endsWith(const std::string data, const std::string pattern) 
        { 
            size_t pos = data.find(pattern); 
            return (pos != std::string::npos && pos + pattern.size() == data.size()); 
        }
        static bool endsWith(const std::wstring data, const std::wstring pattern) 
        { 
            size_t pos = data.find(pattern); 
            return (pos != std::wstring::npos && pos + pattern.size() == data.size()); 
        }
        // String.contains ([...]abc[...])
        static bool contains(const std::string data, const std::string pattern) { return (data.find(pattern) != std::string::npos); }
        static bool contains(const std::wstring data, const std::wstring pattern) { return (data.find(pattern) != std::wstring::npos); }
        
        
        

        
        // isEmpty
        // equals
        // iequalsAscii
        // iequalsUnicode
        
        // find
        // findLast
        // findAll
        // count
        // startsWith
        // contains
        // endsWith
        
        // toUpperAscii
        // toUpperUnicode
        // toLowerAscii
        // toLowerUnicode
        
        // split à position fournie
        // split à chaque occurrence d'un caractère
        
        
        template <typename T>
        static inline std::basic_string<T> ltrim(const std::basic_string<T>& data)
        {
            if (data.empty() == false)
            {
                // find first non-white-space character
                size_t pos = 0u;
                const T* it = data.c_str();
                while (utils::Char::isWhiteSpace(*it))
                {
                    ++it;
                    ++pos;
                }
                // trim
                return (pos == 0u) ? data : data.substr(pos);
            }
        }
        
        template <typename T>
        static inline std::basic_string<T> rtrim(const std::basic_string<T>& data)
        {
            if (data.empty() == false)
            {
                // find last non-white-space character
                size_t pos = data.size() - 1u;
                const T* it = data.c_str();
                it += pos;
                while (pos >= 0u && utils::Char::isWhiteSpace(*it))
                {
                    --it;
                    --pos;
                }
                // trim
                return (pos == data.size() - 1u) ? data : data.substr(0, pos + 1u);
            }
        }
        
        template <typename T>
        static std::basic_string<T> trim(const std::basic_string<T>& data)
        {
            if (data.empty() == false)
            {
                // find first non-white-space character
                size_t posFirst = 0u;
                const T* it = data.c_str();
                while (utils::Char::isWhiteSpace(*it))
                {
                    ++it;
                    ++posFirst;
                }

                // find last non-white-space character
                size_t posLast = data.size() - 1u;
                const T* it = data.c_str();
                it += posLast;
                while (posLast >= 0u && utils::Char::isWhiteSpace(*it))
                {
                    --it;
                    --posLast;
                }
                // trim
                if (posFirst == 0u)
                {
                    return (posLast == data.size() - 1u) ? data : data.substr(0, posLast + 1u);
                }
                else if (posLast == data.size() - 1u)
                    return data.substr(posFirst);
                else
                    return (posFirst < posLast) ? data.substr(posFirst, (posLast + 1u) - posFirst) : std::basic_string<T> {};
            }
        }
    };
}
