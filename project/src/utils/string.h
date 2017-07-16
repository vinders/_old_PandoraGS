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
#include <vector>
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
        // -- Size --
    
        template <typename T>
        static inline bool isEmpty(const T* data) { return (data == nullptr || *data == 0); }
        template <typename T>
        static inline bool isEmpty(const std::basic_string<T>& data) { return data.empty(); }
        
        template <typename T>
        static inline size_t getLength(const T* data) { return (data != nullptr) ? strlen(data) : 0u; }
        template <typename T>
        static inline size_t getLength(const std::basic_string<T>& data) { return data.size(); }
        
        template <typename T>
        static inline void truncate(std::basic_string<T>& data, const size_t len) 
        { 
            if (len <= data.size())
                data = data.substr(0, len);
        }
        
        template <typename T>
        static inline void pad(std::basic_string<T>& data, const size_t len) 
        { 
            if (len > data.size())
            {
                data.reserve(len + 1);
                for (size_t padding = len - data.size(); padding > 0u; --padding)
                    data += ' ';
            }
        }
        
        template <typename T>
        static inline void truncateOrPad(std::basic_string<T>& data, const size_t len) 
        { 
            if (len <= data.size())
            {
                data = data.substr(0, len);
            }
            else
            {
                data.reserve(len + 1);
                for (size_t padding = len - data.size(); padding > 0u; --padding)
                    data += ' ';
            }
        }
        
        

        // -- Compare --
        
        template <typename T>
        static int32_t compare(const T* lhs, const T* rhs) 
        { 
            _STATIC_ASSERT_IS_CHARACTER_TYPE(lhs);
            if (lhs == rhs)
                return 0;
            if (lhs == nullptr)
                return (rhs == nullptr || *rhs == 0) ? 0 : -1;
            if (rhs == nullptr)
                return (*lhs == 0) ? 0 : 1;
            
            const T* lhsIt = lhs;
            const T* rhsIt = rhs;
            while (*lhsIt != 0 && *lhsIt == *rhsIt)
            {
                ++lhsIt;
                ++rhsIt;
            }
            if (*lhsIt == *rhsIt); // both '\0'
                return 0;
            else
                return (*lhsIt < *rhsIt) ? -1 : 1;
        }
        template <typename T>
        static inline int32_t compare(const std::basic_string<T>& lhs, const std::basic_string<T>& rhs) { return lhs.compare(rhs); }
        
        
        template <typename T>
        static bool equals(const T* lhs, const T* rhs) 
        { 
            if (lhs != nullptr && rhs != nullptr)
            {
                const T* lhsIt = lhs;
                const T* rhsIt = rhs;
                while (*lhsIt != 0 && *lhsIt == *rhsIt)
                {
                    ++lhsIt;
                    ++rhsIt;
                }
                return (*lhsIt == *rhsIt); // both '\0'
            }
            return false;
        }
        template <typename T>
        static inline bool equals(const std::basic_string<T>& lhs, const std::basic_string<T>& rhs) { return (lhs.compare(rhs) == 0); }

        
        template <typename T>
        static bool iequalsAscii(const T* lhs, const T* rhs) 
        { 
            if (lhs != nullptr && rhs != nullptr)
            {
                const T* lhsIt = lhs;
                const T* rhsIt = rhs;
                while (*lhsIt != 0 && utils::Char::iequalsAscii(*lhsIt, *rhsIt))
                {
                    ++lhsIt;
                    ++rhsIt;
                }
                return (*lhsIt == *rhsIt); // both '\0'
            }
            return false;
        }
        template <typename T>
        static inline bool iequalsAscii(const std::basic_string<T>& lhs, const std::basic_string<T>& rhs) 
        { 
            const T* lhsIt = lhs.c_str();
            const T* rhsIt = rhs.c_str();
            while (*lhsIt != 0 && utils::Char::iequalsAscii(*lhsIt, *rhsIt))
            {
                ++lhsIt;
                ++rhsIt;
            }
            return (*lhsIt == *rhsIt); // both '\0'
        }
        
        
        template <typename T>
        static bool iequalsUnicode(const T* lhs, const T* rhs) 
        { 
            if (lhs != nullptr && rhs != nullptr)
            {
                const T* lhsIt = lhs;
                const T* rhsIt = rhs;
                while (*lhsIt != 0 && utils::Char::iequalsUnicode(*lhsIt, *rhsIt))
                {
                    ++lhsIt;
                    ++rhsIt;
                }
                return (*lhsIt == *rhsIt); // both '\0'
            }
            return false;
        }
        template <typename T>
        static inline bool iequalsUnicode(const std::basic_string<T>& lhs, const std::basic_string<T>& rhs) 
        { 
            const T* lhsIt = lhs.c_str();
            const T* rhsIt = rhs.c_str();
            while (*lhsIt != 0 && utils::Char::iequalsUnicode(*lhsIt, *rhsIt))
            {
                ++lhsIt;
                ++rhsIt;
            }
            return (*lhsIt == *rhsIt); // both '\0'
        }
        
        
        
        // -- Upper/lower-case converters --
        
        template <typename T>
        static void toUpperAscii(T* data) 
        { 
            if (data != nullptr)
            {
                while (*data != 0)
                {
                    *data = utils::Char::toUpperAscii(*data);
                    ++data;
                }
            }
        }
        template <typename T>
        static void toUpperAscii(const T* data, T* out) 
        { 
            if (data != nullptr)
            {
                while (*data != 0)
                {
                    *out = utils::Char::toUpperAscii(*data);
                    ++data;
                    ++out;
                }
                *out = 0;
            }
        }
        template <typename T>
        static inline void toUpperAscii(std::basic_string<T>& data) 
        { 
            for (int32_t i = data.size() - 1; i >= 0; --i)
                data[i] = utils::Char::toUpperAscii(data[i]);
        }
        
        template <typename T>
        static void toUpperUnicode(T* data) 
        { 
            if (data != nullptr)
            {
                while (*data != 0)
                {
                    *data = utils::Char::toUpperUnicode(*data);
                    ++data;
                }
            }
        }
        template <typename T>
        static void toUpperUnicode(const T* data, T* out) 
        { 
            if (data != nullptr)
            {
                while (*data != 0)
                {
                    *out = utils::Char::toUpperUnicode(*data);
                    ++data;
                    ++out;
                }
                *out = 0;
            }
        }
        template <typename T>
        static inline void toUpperUnicode(std::basic_string<T>& data) 
        { 
            for (int32_t i = data.size() - 1; i >= 0; --i)
                data[i] = utils::Char::toUpperUnicode(data[i]);
        }
        
        
        template <typename T>
        static void toLowerAscii(T* data) 
        { 
            if (data != nullptr)
            {
                while (*data != 0)
                {
                    *data = utils::Char::toLowerAscii(*data);
                    ++data;
                }
            }
        }
        template <typename T>
        static void toLowerAscii(const T* data, T* out) 
        { 
            if (data != nullptr)
            {
                while (*data != 0)
                {
                    *out = utils::Char::toLowerAscii(*data);
                    ++data;
                    ++out;
                }
                *out = 0;
            }
        }
        template <typename T>
        static inline void toLowerAscii(std::basic_string<T>& data) 
        { 
            for (int32_t i = data.size() - 1; i >= 0; --i)
                data[i] = utils::Char::toLowerAscii(data[i]);
        }
        
        template <typename T>
        static void toLowerUnicode(T* data) 
        { 
            if (data != nullptr)
            {
                while (*data != 0)
                {
                    *data = utils::Char::toLowerUnicode(*data);
                    ++data;
                }
            }
        }
        template <typename T>
        static void toLowerUnicode(const T* data, T* out) 
        { 
            if (data != nullptr)
            {
                while (*data != 0)
                {
                    *out = utils::Char::toLowerUnicode(*data);
                    ++data;
                    ++out;
                }
                *out = 0;
            }
        }
        template <typename T>
        static inline void toLowerUnicode(std::basic_string<T>& data) 
        { 
            for (int32_t i = data.size() - 1; i >= 0; --i)
                data[i] = utils::Char::toLowerUnicode(data[i]);
        }
        

        
        // -- Find first --
        
        static constexpr size_t notFound = std::string::npos;
        
        template <typename T>
        static size_t find(const T* data, const T needleChar)
        {
            size_t needlePos = utils::String::notFound;
            size_t curPos = 0u;
            if (data != nullptr)
            {
                for (const T* it = data; *it != 0; ++it)
                {
                    if (*it == needleChar)
                    {
                        needlePos = curPos;
                        break;
                    }
                    ++curPos;
                }
            }
            return needlePos;
        }
        template <typename T>
        static inline size_t find(const std::basic_string<T> data, const T needleChar)
        {
            return data.find(needleChar);
        }
        
        template <typename T>
        static size_t find(const T* data, const T* pattern)
        {
            size_t needlePos = utils::String::notFound;
            size_t curPos = 0u;
            const T* needleDataIt;
            const T* needlePatternIt;
            if (data != nullptr)
            {
                for (const T* it = data; *it != 0; ++it)
                {
                    if (*it == *pattern)
                    {
                        needleDataIt = it + 1;
                        needlePatternIt = pattern + 1;
                        while (*needleDataIt != 0 && *needleDataIt == *needlePatternIt);
                        if (*needlePatternIt == 0)
                        { 
                            needlePos = curPos;
                            break;
                        }
                    }
                    ++curPos;
                }
            }
            return needlePos;
        }
        template <typename T>
        static inline size_t find(const std::basic_string<T> data, const std::basic_string<T> pattern)
        {
            return data.find(pattern);
        }
        
        
        // -- Find last --
        
        template <typename T>
        static size_t rfind(const T* data, const T needleChar)
        {
            size_t needlePos = utils::String::notFound;
            size_t curPos = 0u;
            if (data != nullptr)
            {
                for (const T* it = data; *it != 0; ++it)
                {
                    if (*it == needleChar)
                        needlePos = curPos;
                    ++curPos;
                }
            }
            return needlePos;
        }
        template <typename T>
        static size_t rfind(const T* data, const size_t len, const T needleChar)
        {
            size_t needlePos = utils::String::notFound;
            size_t curPos = len;
            if (data != nullptr)
            {
                for (const T* it = data[len - 1]; curPos > 0; --it)
                {
                    if (*it == needleChar)
                    {
                        needlePos = curPos - 1u;
                        break;
                    }
                    --curPos;
                }
            }
            return needlePos;
        }
        template <typename T>
        static size_t rfind(const std::basic_string<T> data, const T needleChar)
        {
            return data.rfind(needleChar);
        }
        
        template <typename T>
        static size_t rfind(const T* data, const T* pattern)
        {
            size_t needlePos = utils::String::notFound;
            size_t curPos = 0u;
            const T* needleDataIt;
            const T* needlePatternIt;
            if (data != nullptr)
            {
                for (const T* it = data; *it != 0; ++it)
                {
                    if (*it == *pattern)
                    {
                        needleDataIt = it + 1;
                        needlePatternIt = pattern + 1;
                        while (*needleDataIt != 0 && *needleDataIt == *needlePatternIt);
                        if (*needlePatternIt == 0)
                            needlePos = curPos;
                    }
                    ++curPos;
                }
            }
            return needlePos;
        }
        template <typename T>
        static size_t rfind(const T* data, const size_t len, const T* pattern)
        {
            size_t needlePos = utils::String::notFound;
            size_t curPos = len;
            const T* needleDataIt;
            const T* needlePatternIt;
            if (data != nullptr)
            {
                for (const T* it = data[len - 1]; curPos > 0; --it)
                {
                    if (*it == *pattern)
                    {
                        needleDataIt = it + 1;
                        needlePatternIt = pattern + 1;
                        while (*needleDataIt != 0 && *needleDataIt == *needlePatternIt);
                        if (*needlePatternIt == 0)
                        { 
                            needlePos = curPos - 1u;
                            break;
                        }
                    }
                    --curPos;
                }
            }
            return needlePos;
        }
        template <typename T>
        static size_t rfind(const std::basic_string<T> data, const std::basic_string<T> pattern)
        {
            return data.rfind(pattern);
        }
        
        
        // -- Find all --
        
        template <typename T>
        static std::vector<size_t> findAll(const T* data, const T needleChar)
        {
            std::vector<size_t> posArray;
            posArray.reserve(8);
            size_t curPos = 0u;
            if (data != nullptr)
            {
                for (const T* it = data; *it != 0; ++it)
                {
                    if (*it == needleChar)
                        posArray.push_back(curPos);
                    ++curPos;
                }
            }
            return posArray;
        }
        template <typename T>
        static inline std::vector<size_t> findAll(const std::basic_string<T> data, const T needleChar)
        {
            std::vector<size_t> posArray;
            posArray.reserve(8);
            size_t curPos = 0u;
            if (data.empty() == false)
            {
                for (const T* it = data.c_str(); *it != 0; ++it)
                {
                    if (*it == needleChar)
                        posArray.push_back(curPos);
                    ++curPos;
                }
            }
            return posArray;
        }
        
        template <typename T>
        static std::vector<size_t> findAll(const T* data, const T* pattern)
        {
            std::vector<size_t> posArray;
            posArray.reserve(8);
            size_t curPos = 0u;
            const T* needleDataIt;
            const T* needlePatternIt;
            if (data != nullptr)
            {
                for (const T* it = data; *it != 0; ++it)
                {
                    if (*it == *pattern)
                    {
                        needleDataIt = it + 1;
                        needlePatternIt = pattern + 1;
                        while (*needleDataIt != 0 && *needleDataIt == *needlePatternIt);
                        if (*needlePatternIt == 0)
                            posArray.push_back(curPos);
                    }
                    ++curPos;
                }
            }
            return posArray;
        }
        template <typename T>
        static inline std::vector<size_t> findAll(const std::basic_string<T> data, const std::basic_string<T> pattern)
        {
            std::vector<size_t> posArray;
            posArray.reserve(8);
            size_t curPos = 0u;
            const T* needleDataIt;
            const T* needlePatternIt;
            if (data.empty() == false)
            {
                for (const T* it = data.c_str(); *it != 0; ++it)
                {
                    if (*it == *pattern)
                    {
                        needleDataIt = it + 1;
                        needlePatternIt = pattern.c_str() + 1;
                        while (*needleDataIt != 0 && *needleDataIt == *needlePatternIt);
                        if (*needlePatternIt == 0)
                            posArray.push_back(curPos);
                    }
                    ++curPos;
                }
            }
            return posArray;
        }
        
        
        // -- Count all --

        template <typename T>
        static uint32_t countOccurrences(const T* data, const T needleChar)
        {
            uint32_t count = 0u;
            size_t curPos = 0u;
            if (data != nullptr)
            {
                for (const T* it = data; *it != 0; ++it)
                {
                    if (*it == needleChar)
                        ++count;
                    ++curPos;
                }
            }
            return count;
        }
        template <typename T>
        static inline uint32_t countOccurrences(const std::basic_string<T> data, const T needleChar)
        {
            uint32_t count = 0u;
            size_t curPos = 0u;
            if (data.empty() == false)
            {
                for (const T* it = data.c_str(); *it != 0; ++it)
                {
                    if (*it == needleChar)
                        ++count;
                    ++curPos;
                }
            }
            return count;
        }
        
        template <typename T>
        static uint32_t countOccurrences(const T* data, const T* pattern)
        {
            uint32_t count = 0u;
            size_t curPos = 0u;
            const T* needleDataIt;
            const T* needlePatternIt;
            if (data != nullptr)
            {
                for (const T* it = data; *it != 0; ++it)
                {
                    if (*it == *pattern)
                    {
                        needleDataIt = it + 1;
                        needlePatternIt = pattern + 1;
                        while (*needleDataIt != 0 && *needleDataIt == *needlePatternIt);
                        if (*needlePatternIt == 0)
                            ++count;
                    }
                    ++curPos;
                }
            }
            return count;
        }
        template <typename T>
        static inline uint32_t countOccurrences(const std::basic_string<T> data, const std::basic_string<T> pattern)
        {
            uint32_t count = 0u;
            size_t curPos = 0u;
            const T* needleDataIt;
            const T* needlePatternIt;
            if (data.empty() == false)
            {
                for (const T* it = data.c_str(); *it != 0; ++it)
                {
                    if (*it == *pattern)
                    {
                        needleDataIt = it + 1;
                        needlePatternIt = pattern.c_str() + 1;
                        while (*needleDataIt != 0 && *needleDataIt == *needlePatternIt);
                        if (*needlePatternIt == 0)
                            ++count;
                    }
                    ++curPos;
                }
            }
            return count;
        }
        
        
        // -- Other finding utilities --
        
        template <typename T>
        static inline bool contains(const T* data, const T needleChar) { return (find(data, needleChar) != utils::String::notFound); }
        template <typename T>
        static inline bool contains(const std::basic_string<T> data, const T needleChar) { return (data.find(needleChar) != std::basic_string<T>::npos); }
        template <typename T>
        static inline bool contains(const T* data, const T* pattern) { return (find(data, pattern) != utils::String::notFound); }
        template <typename T>
        static inline bool contains(const std::basic_string<T> data, const std::basic_string<T> pattern) { return (data.find(pattern) != std::basic_string<T>::npos); }
        
        template <typename T>
        static inline bool startsWith(const T* data, const T needleChar) { return (find(data, needleChar) == 0u); }
        template <typename T>
        static inline bool startsWith(const std::basic_string<T> data, const T needleChar) { return (data.find(needleChar) == 0u); }
        template <typename T>
        static inline bool startsWith(const T* data, const T* pattern) { return (find(data, pattern) == 0u); }
        template <typename T>
        static inline bool startsWith(const std::basic_string<T> data, const std::basic_string<T> pattern) { return (data.find(pattern) == 0u); }
        
        template <typename T>
        static inline bool endsWith(const T* data, const len, const T needleChar) { return (rfind(data, len, needleChar) == len - 1u); }
        template <typename T>
        static inline bool endsWith(const std::basic_string<T> data, const T needleChar) { return (data.rfind(needleChar) == data.size() - 1u); }
        template <typename T>
        static inline bool endsWith(const T* data, const len, const T* pattern) { return (rfind(data, len, pattern) == len - getLength(pattern)); }
        template <typename T>
        static inline bool endsWith(const std::basic_string<T> data, const std::basic_string<T> pattern) { return (data.rfind(pattern) == data.size() - pattern.size()); }
        
        
        
        // -- Split --

        template <typename T>
        static inline std::vector<std::basic_string<T>> splitAt(const std::basic_string<T>& data, const uint32_t pos)
        {
            if (data.size() > pos)
            {
                std::vector<std::basic_string<T>> parts { data.substr(0, pos), data.substr(pos) };
                return parts;
            }
            else
                return std::vector<std::basic_string<T>> { data };
        }
        template <typename T>
        static inline std::vector<std::basic_string<T>> splitAtExcluded(const std::basic_string<T>& data, const uint32_t pos)
        {
            if (data.size() > pos + 1)
            {
                std::vector<std::basic_string<T>> parts { data.substr(0, pos), data.substr(pos + 1u) };
                return parts;
            }
            else if (data.size() > pos)
                return std::vector<std::basic_string<T>> { data.substr(0, pos) };
            else
                return std::vector<std::basic_string<T>> { data };
        }
    
        template <typename T>
        static std::vector<std::basic_string<T>> split(const std::basic_string<T>& data, const T sep, const bool isKeepingEmptyParts = false)
        {
            std::vector<std::basic_string<T>> parts;
            parts.reserve(4);
            
            size_t partSize = 0u;
            const T* partBeginning = data.c_str();
            for (const T* it = data.c_str(); *it != 0; ++it)
            {
                if (*it != sep)
                {
                    ++partSize;
                }
                // separator found
                else if (partSize > 0u) // part before it is not empty -> extract
                {
                    parts.push_back(std::basic_string<T>(partBeginning, partSize));
                    partBeginning = it;
                    ++partBeginning;
                    partSize = 0u;
                }
                else if (isKeepingEmptyParts) // empty part
                {
                    parts.push_back(std::basic_string<T>{});
                }
            }
            // last part
            if (partSize > 0u) // not empty -> extract
                parts.push_back(std::basic_string<T>(partBeginning, partSize));
            else if (isKeepingEmptyParts) // empty part
                parts.push_back(std::basic_string<T>{});
            return parts;
        }
        
        template <typename T>
        static std::vector<std::basic_string<T>> split(const std::basic_string<T>& data, const std::basic_string<T>& sepString, const bool isKeepingEmptyParts = false)
        {
            std::vector<std::basic_string<T>> parts;
            parts.reserve(4);
            
            size_t partSize = 0u;
            bool isSepFound;
            const T* sepStringIt, dataSepIt;
            const T* partBeginning = data.c_str();
            for (const T* it = data.c_str(); *it != 0; ++it)
            {
                if (*it != *(sep.c_str()))
                {
                    ++partSize;
                }
                // beginning of separator found
                else 
                {
                    isSepFound = true;
                    dataSepIt = it + 1;
                    sepStringIt = sep.c_str();
                    while (*dataSepIt != 0)
                    {
                        if (*dataSepIt == *sepStringIt)
                        {
                            ++dataSepIt;
                            ++sepStringIt;
                        }
                        else // separator not complete -> ignore
                        {
                            isSepFound = false;
                            break;
                        }
                    }
                    
                    // complete separator found
                    if (isSepFound)
                    {
                        if (partSize > 0u) // part before it is not empty -> extract
                        {
                            parts.push_back(std::basic_string<T>(partBeginning, partSize));
                            it += sepString.size();
                            partBeginning = it;
                            --it; // for loop "++it" will still occur
                            partSize = 0u;
                        }
                        else if (isKeepingEmptyParts) // empty part
                        {
                            parts.push_back(std::basic_string<T>{});
                        }
                    }
                    else // not a real separator -> count character normally
                    {
                        ++partSize;
                    }
                }
            }
            // last part
            if (partSize > 0u) // not empty -> extract
                parts.push_back(std::basic_string<T>(partBeginning, partSize));
            else if (isKeepingEmptyParts) // empty part
                parts.push_back(std::basic_string<T>{});
            return parts;
        }
        
        
        
        // -- Trim --
        
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
