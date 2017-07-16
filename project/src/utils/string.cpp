/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : string management toolset
*******************************************************************************/
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include "char.h"
#include "string.h"
using namespace utils;


// -- Compare --

/// @brief Compare two strings
/// @param[in] lhs  First string value
/// @param[in] rhs  Second string value
/// @returns Comparison result : lhs==rhs (0), lhs<rhs (-1), lhs>rhs (1)
template <typename T>
int32_t utils::String::compare(const T* lhs, const T* rhs) 
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

        
// -- Find first --

/// @brief Find first occurrence of a pattern in a string
/// @param[in] data     String value
/// @param[in] pattern  String to find
/// @returns Pattern position
template <typename T>
size_t utils::String::find(const T* data, const T* pattern)
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
                while (*needleDataIt != 0 && *needleDataIt == *needlePatternIt)
                {
                    ++needleDataIt;
                    ++needlePatternIt;
                }
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


// -- Find last --

/// @brief Find last occurrence of a pattern in a string
/// @param[in] data     String value
/// @param[in] pattern  String to find
/// @returns Pattern position
/// @warning Very inefficient. Prefer rfind(const T* data, const size_t len, const T* pattern).
template <typename T>
size_t utils::String::rfind(const T* data, const T* pattern)
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
                while (*needleDataIt != 0 && *needleDataIt == *needlePatternIt)
                {
                    ++needleDataIt;
                    ++needlePatternIt;
                }
                if (*needlePatternIt == 0)
                    needlePos = curPos;
            }
            ++curPos;
        }
    }
    return needlePos;
}

/// @brief Find last occurrence of a pattern in a string
/// @param[in] data     String value
/// @param[in] pattern  String to find
/// @returns Pattern position
template <typename T>
size_t utils::String::rfind(const T* data, const size_t len, const T* pattern)
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
                while (*needleDataIt != 0 && *needleDataIt == *needlePatternIt)
                {
                    ++needleDataIt;
                    ++needlePatternIt;
                }
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
        
        
// -- Find all --

/// @brief Find all occurrences of a pattern in a string (occurrences may overlap)
/// @param[in] data     String value
/// @param[in] pattern  String to find
/// @returns Vector with found positions
template <typename T>
std::vector<size_t> utils::String::findAll(const T* data, const T* pattern)
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
                while (*needleDataIt != 0 && *needleDataIt == *needlePatternIt)
                {
                    ++needleDataIt;
                    ++needlePatternIt;
                }
                if (*needlePatternIt == 0)
                    posArray.push_back(curPos);
            }
            ++curPos;
        }
    }
    return posArray;
}

/// @brief Find all occurrences of a pattern in a string (occurrences may overlap)
/// @param[in] data     String value
/// @param[in] pattern  String to find
/// @returns Vector with found positions
template <typename T>
std::vector<size_t> utils::String::findAll(const std::basic_string<T> data, const std::basic_string<T> pattern)
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
                while (*needleDataIt != 0 && *needleDataIt == *needlePatternIt)
                {
                    ++needleDataIt;
                    ++needlePatternIt;
                }
                if (*needlePatternIt == 0)
                    posArray.push_back(curPos);
            }
            ++curPos;
        }
    }
    return posArray;
}


// -- Count all --

/// @brief Count all occurrences of a pattern in a string (occurrences may overlap)
/// @param[in] data     String value
/// @param[in] pattern  String to find
/// @returns Number of occurrences
template <typename T>
uint32_t utils::String::countOccurrences(const T* data, const T* pattern)
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
                while (*needleDataIt != 0 && *needleDataIt == *needlePatternIt)
                {
                    ++needleDataIt;
                    ++needlePatternIt;
                }
                if (*needlePatternIt == 0)
                    ++count;
            }
            ++curPos;
        }
    }
    return count;
}

/// @brief Count all occurrences of a pattern in a string (occurrences may overlap)
/// @param[in] data     String value
/// @param[in] pattern  String to find
/// @returns Number of occurrences
template <typename T>
uint32_t utils::String::countOccurrences(const std::basic_string<T> data, const std::basic_string<T> pattern)
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
                while (*needleDataIt != 0 && *needleDataIt == *needlePatternIt)
                {
                    ++needleDataIt;
                    ++needlePatternIt;
                }
                if (*needlePatternIt == 0)
                    ++count;
            }
            ++curPos;
        }
    }
    return count;
}


// -- Split --

/// @brief Split a string every time an occurrence of a character is found
/// @param[in] data                 String value
/// @param[in] sep                  Separator (character)
/// @param[in] isKeepingEmptyParts  If two separators are contiguous, add an empty sub-string (default: false)
/// @returns Vector with sub-strings
template <typename T>
std::vector<std::basic_string<T>> utils::String::split(const std::basic_string<T>& data, const T sep, const bool isKeepingEmptyParts = false)
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

/// @brief Split a string every time an occurrence of a pattern is found (no overlap)
/// @param[in] data                 String value
/// @param[in] sepString            Separator (pattern)
/// @param[in] isKeepingEmptyParts  If two separators are contiguous, add an empty sub-string (default: false)
/// @returns Vector with sub-strings
template <typename T>
std::vector<std::basic_string<T>> utils::String::split(const std::basic_string<T>& data, const std::basic_string<T>& sepString, const bool isKeepingEmptyParts = false)
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

/// @brief Remove all white-space characters at the beginning and the end of a string
/// @param[in] data  String value
/// @returns Trimmed string
template <typename T>
std::basic_string<T> utils::String::trim(const std::basic_string<T>& data)
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
