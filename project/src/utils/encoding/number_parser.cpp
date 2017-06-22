/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : number parser
*******************************************************************************/
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include "number_parser.h"

using namespace utils::encoding;

// -- Base N character to base 10 integer

#define CHAR_TO_INT_TABLE_LENGTH 80
#define charToIndex(VAL) (int)((VAL) - '+')

// hexa-decimal
#define HEX_CHAR_TO_INT { -1,-1,-1,-1,-1, \
                          0,1,2,3,4,5,6,7,8,9, -1,-1,-1,-1,-1,-1,-1, \
                          10,11,12,13,14,15, -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1, \
                          10,11,12,13,14,15, -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 } \
// base32
#define B32_CHAR_TO_INT { -1,-1,-1,-1,-1, \
                          -1,-1,26,27,28,29,30,31,-1,-1, -1,-1,-1,-1,-1,-1,-1, \
                          0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25, -1,-1,-1,-1,-1,-1, \
                          -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 } \
// base64
#define B64_CHAR_TO_INT { 63,-1,-1,-1,62, \
                          52,53,54,55,56,57,58,59,60,61, -1,-1,-1,-1,-1,-1,-1, \
                          0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25, -1,-1,-1,-1,-1,-1, \
                          26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51} \
                         

// -- Tools - prefix / sign / white-spaces --

/// @brief Check if white-space character
/// @param[in] val  Character
/// @returns Trimmed or not
static inline bool _isTrimmedChar(const char val)
{
    return (val == ' ' || (val >= 0x08 && val <= 0x0D));
}
/// @brief Left trim white-space characters - no copy
/// @param[in|out] out  Trimmed string (NOT A COPY : pointer to first non-trimmed character inside SAME string)
/// @returns Number of characters trimmed
static inline size_t _trimLeft(const char*& val)
{
    size_t trimmedChars = 0;
    while (_isTrimmedChar(*val)) 
    {
        ++val;
        ++trimmedChars;
    }
    return trimmedChars;
}
/// @brief Left trim white-space characters - no copy
/// @param[in|out] out  Trimmed string (NOT A COPY : pointer to first non-trimmed character inside SAME string)
/// @param[in] len      Max trimmed length
/// @returns Number of characters trimmed
static inline size_t _trimLeft(const char*& val, const size_t len)
{
    size_t trimmedChars = 0;
    while (trimmedChars < len && _isTrimmedChar(*val)) 
    {
        ++val;
        ++trimmedChars;
    }
    return trimmedChars;
}


/// @brief Count characters to trim in parsed string and check sign character
/// @param[in] val          String to parse
/// @param[out] isPositive  Sign found
/// @returns Number of characters read
template <bool isSigned>
static inline size_t _parseSignCharacter(const char* val, bool& isPositive)
{
    size_t signChars = 0;
    if (isSigned) 
    {
        if (*val == '-' || *val == '+')
        {
            isPositive = (*val == '+');
            ++val;
            ++signChars;
            
            signChars += _trimLeft(val); // remove white spaces after sign
        }
        else
            isPositive = true;
    }
    else // remove "plus" sign if unsigned value
    {
        if (*val == '+')
        {
            ++signChars;
            signChars += _trimLeft(val); // remove white spaces after sign
        }
        isPositive = true;
    }
    return signChars;
}
/// @brief Count characters to trim in parsed string and check sign character
/// @param[in] val          String to parse
/// @param[in] len          Max length
/// @param[out] isPositive  Sign found
/// @returns Number of characters read
template <bool isSigned>
static inline size_t _parseSignCharacter(const char* val, const size_t len, bool& isPositive)
{
    size_t signChars = 0;
    if (len > 0)
    {
        if (isSigned) 
        {
            if (*val == '-' || *val == '+')
            {
                isPositive = (*val == '+');
                ++val;
                ++signChars;
                
                signChars += _trimLeft(val, len - 1); // remove white spaces after sign
            }
            else
                isPositive = true;
        }
        else // remove "plus" sign if unsigned value
        {
            if (*val == '+')
            {
                ++signChars;
                signChars += _trimLeft(val, len - 1); // remove white spaces after sign
            }
            isPositive = true;
        }
    }
    return signChars;
}


/// @brief Count characters to trim in parsed string and check sign character
/// @param[in] val          String to parse
/// @param[out] isPositive  Sign found
/// @returns Number of characters read
template <bool isSigned, uint32_t Base>
static inline size_t _parsePrefixCharacters(const char* val, bool& isPositive)
{
    size_t prefixChars = _trimLeft(val); // remove leading white spaces
    
    // check for sign
    size_t signChars = _parseSignCharacter<isSigned>(val, isPositive);
    prefixChars += signChars;
    val += signChars;
    
    // remove hexa-decimal prefix
    if (Base == 16u)
    {
        if (*val == 'x' || *val == 'X')
        {
            ++prefixChars;
        }
        else if (*val == '0' || *val == '\\')
        {
            ++val;
            if (*val == 'x' || *val == 'X')
                prefixChars += 2u;
        }
    }
    // remove octal prefix
    else if (Base == 8u)
    {
        if (*val == '0' || *val == '\\')
            ++prefixChars;
    }
    return prefixChars;
}
/// @brief Count characters to trim in parsed string and check sign character
/// @param[in] val          String to parse
/// @param[in] len          Max length
/// @param[out] isPositive  Sign found
/// @returns Number of characters read
template <bool isSigned, uint32_t Base>
static inline size_t _parsePrefixCharacters(const char* val, const size_t len, bool& isPositive)
{
    size_t prefixChars = _trimLeft(val, len); // remove leading white spaces
    
    // check for sign
    size_t signChars = _parseSignCharacter<isSigned>(val, len - prefixChars, isPositive);
    prefixChars += signChars;
    val += signChars;
    
    // remove hexa-decimal prefix
    if (Base == 16u)
    {
        if (prefixChars + 1 < len)
        {
            if (*val == 'x' || *val == 'X')
            {
                ++prefixChars;
            }
            else if (*val == '0' || *val == '\\')
            {
                ++val;
                if (*val == 'x' || *val == 'X')
                    prefixChars += 2u;
            }
        }
    }
    // remove octal prefix
    else if (Base == 8u)
    {
        if (prefixChars < len)
        {
            if (*val == '0' || *val == '\\')
                ++prefixChars;
        }
    }
    return prefixChars;
}



// -- Parsers - integers --

/// @brief Parse integer digits
/// @param[in] val   String to parse
/// @param[out] out  Output value
/// @returns Number of characters read
template <typename T, uint32_t Base>
static inline size_t _parseIntegerValue(const char* val, T& out)
{
    size_t i = 0;
    if (Base <= 10u)
    {
        for (out = 0; *val >= '0' && *val <= '9'; ++val)
        {
            out *= Base;
            out += *val - '0';
            ++i;
        }
    }
    else if (Base <= 64u)
    {
        int32_t translation[CHAR_TO_INT_TABLE_LENGTH] = (Base <= 16u) ? HEX_CHAR_TO_INT : ((Base <= 32u) ? B32_CHAR_TO_INT : B64_CHAR_TO_INT);
        int32_t buffer;
        for (out = 0; *val >= '0' && *val <= 'z'); ++val)
        {
            if ((buffer = translation[charToIndex(*val)]) == -1)
                break;
            out *= Base;
            out += buffer;
            ++i;
        }
    }
    return i;
}
/// @brief Parse integer digits
/// @param[in] val   String to parse
/// @param[in] len   String length
/// @param[out] out  Output value
/// @returns Number of characters read
template <typename T, uint32_t Base>
static inline size_t _parseIntegerValue(const char* val, const size_t len, T& out)
{
    size_t i = 0;
    if (Base <= 10u)
    {
        for (out = 0; i < len && *val >= '0' && *val <= '9'; ++val)
        {
            out *= Base;
            out += *val - '0';
            ++i;
        }
    }
    else if (Base <= 64u)
    {
        int32_t translation[CHAR_TO_INT_TABLE_LENGTH] = (Base <= 16u) ? HEX_CHAR_TO_INT : ((Base <= 32u) ? B32_CHAR_TO_INT : B64_CHAR_TO_INT);
        int32_t buffer;
        for (out = 0; i < len && *val >= '0' && *val <= 'z'); ++val)
        {
            if ((buffer = translation[charToIndex(*val)]) == -1)
                break;
            out *= Base;
            out += buffer;
            ++i;
        }
    }
    return i;
}

/// @brief Parse string with leading number
/// @param[in] val   String to parse
/// @param[out] out  Output value (0 if empty/null string)
/// @returns Number of characters read
template <typename T, bool isSigned, uint32_t Base>
size_t NumberParser::parseString(const char* val, T& out)
{
    if (val == nullptr || *val == '\0') // null/empty string
    {
        m_lastError = "NumberParser: the string is empty or null";
        out = static_cast<T>(0);
        return 0;
    }
    
    // trim and check sign
    bool isPositive = true;
    size_t prefixChars = _parsePrefixCharacters<isSigned, Base>(val, isPositive);
    val += prefixChars;
    
    // parse value
    T parsed;
    const size_t i = _parseIntegerValue<T, Base>(val, parsed);
    if (i == 0)
    {
        m_lastError = "NumberParser: the string does not contain a valid number.";
        out = static_cast<T>(0);
        return 0;
    }
    out = (isPositive) ? parsed : -parsed;
    return i + prefixChars;
}
/// @brief Parse sub-string with leading number
/// @param[in] val   String to parse
/// @param[in] len   String length
/// @param[out] out  Output value (0 if empty/null string)
/// @returns Number of characters read
template <typename T, bool isSigned, uint32_t Base>
size_t NumberParser::parseString(const char* val, const size_t len, T& out)
{
    if (len == 0u)
    {
        m_lastError = "NumberParser: specified string length (len) is 0";
        out = static_cast<T>(0);
        return 0;
    }
    if (val == nullptr || *val == '\0') // null/empty string
    {
        m_lastError = "NumberParser: the string is empty or null";
        out = static_cast<T>(0);
        return 0;
    }
    
    // trim and check sign
    bool isPositive = true;
    size_t prefixChars = _parsePrefixCharacters<isSigned, Base>(val, len, isPositive);
    val += prefixChars;
    
    // parse value
    T parsed;
    const size_t i = _parseIntegerValue<T, Base>(val, len - prefixChars, parsed);
    if (i == 0)
    {
        m_lastError = "NumberParser: the string does not contain a valid number.";
        out = static_cast<T>(0);
        return 0;
    }
    out = (isPositive) ? parsed : -parsed;
    return i + prefixChars;
}


// -- Parsers - floating-point numbers --

/// @brief Parse floating-point number digits
/// @param[in] val   String to parse
/// @param[out] out  Output value
/// @returns Number of characters read
static inline size_t _parseFloatValue(const char* val, double& out)
{
    // parse integer part
    uint64_t parsedPart;
    size_t i = _parseIntegerValue<T, 10>(val, parsedPart);
    out = static_cast<double>(parsedPart);
    
    // parse decimal part
    if (*val == '.' || *val == ',') 
    {
        ++val; // point
        uint32_t decRows = _parseIntegerValue<T, 10>(val, parsedPart);
        i += decRows + 1;
        
        uint64_t divider = 1;
        for (uint32_t row = 0; row < decRows; ++row)
            divider *= 10;
        out += static_cast<double>(parsedPart) / static_cast<double>(divider);
    }
    return i;
}
/// @brief Parse floating-point number digits
/// @param[in] val   String to parse
/// @param[in] len   String length
/// @param[out] out  Output value
/// @returns Number of characters read
static inline size_t _parseFloatValue(const char* val, const size_t len, double& out)
{
    // parse integer part
    uint64_t parsedPart;
    size_t i = _parseIntegerValue<T, 10>(val, len, parsedPart);
    out = static_cast<double>(parsedPart);
    
    // parse decimal part
    if (*val == '.' || *val == ',') 
    {
        ++val; // point
        ++i;
        uint32_t decRows = _parseIntegerValue<T, 10>(val, len - i, parsedPart);
        i += decRows;
        
        uint64_t divider = 1;
        for (uint32_t row = 0; row < decRows; ++row)
            divider *= 10;
        out += static_cast<double>(parsedPart) / static_cast<double>(divider);
    }
    return i;
}

/// @brief Parse string with leading floating-point number
/// @param[in] val   String to parse
/// @param[out] out  Output value
/// @returns Number of characters read
template <typename T>
size_t NumberParser::parseFloatString(const char* val, double& out)
{
    if (val == nullptr || *val == '\0') // null/empty string
    {
        m_lastError = "NumberParser: the string is empty or null";
        out = static_cast<T>(0.0);
        return 0;
    }
    
    // trim and check sign
    bool isPositive = true;
    size_t prefixChars = _parsePrefixCharacters<isSigned, 10>(val, isPositive);
    val += prefixChars;
    
    // parse integer value
    double parsed;
    size_t i = _parseFloatValue(val, parsed);
    if (i == 0)
    {
        m_lastError = "NumberParser: the string does not contain a valid number.";
        out = static_cast<T>(0.0);
        return 0;
    }
    out = (isPositive) ? static_cast<T>(parsed) : static_cast<T>(-parsed);
    return i + prefixChars;
}

/// @brief Parse sub-string with leading floating-point number
/// @param[in] val   String to parse
/// @param[in] len   String length
/// @param[out] out  Output value
/// @returns Number of characters read
template <typename T>
size_t NumberParser::parseFloatString(const char* val, const size_t len, T& out)
{
    if (len == 0u)
    {
        m_lastError = "NumberParser: specified string length (len) is 0";
        out = static_cast<T>(0.0);
        return 0;
    }
    if (val == nullptr || *val == '\0') // null/empty string
    {
        m_lastError = "NumberParser: the string is empty or null";
        out = static_cast<T>(0.0);
        return 0;
    }
    
    // trim and check sign
    bool isPositive = true;
    size_t prefixChars = _parsePrefixCharacters<isSigned, 10>(val, len, isPositive);
    val += prefixChars;
    
    // parse integer value
    double parsed;
    size_t i = _parseFloatValue(val, len - prefixChars, parsed);
    if (i == 0)
    {
        m_lastError = "NumberParser: the string does not contain a valid number.";
        out = static_cast<T>(0.0);
        return 0;
    }
    out = (isPositive) ? static_cast<T>(parsed) : static_cast<T>(-parsed);
    return i + prefixChars;
}


// -- Parsers - booleans --

/// @brief Parse string with leading boolean value (allowed values : true/false/TRUE/FALSE/0/1/00...00/00..01)
/// @param[in] val  String to parse
/// @param[out] out   Output value (false if empty or null string)
/// @returns Number of characters read
size_t NumberParser::parseBoolString(const char* val, bool& out)
{
    if (val == nullptr || *val == '\0') // null/empty string
    {
        m_lastError = "NumberParser: the string is empty or null";
        out = false;
        return 0;
    }
    
    switch (*val)
    {
        case 'f':
            if (*(++val) == 'a' && *(++val) == 'l' && *(++val) == 's' && *(++val) == 'e')
            {
                out = false;
                return 5;
            }
            break;
        case 'F':
            if (*(++val) == 'A' && *(++val) == 'L' && *(++val) == 'S' && *(++val) == 'E')
            {
                out = false;
                return 5;
            }
            break;
        case 't':
            if (*(++val) == 'r' && *(++val) == 'u' && *(++val) == 'e')
            {
                out = true;
                return 4;
            }
            break;
        case 'T':
            if (*(++val) == 'R' && *(++val) == 'U' && *(++val) == 'E')
            {
                out = true;
                return 4;
            }
            break;
        case '0':
        {
            size_t i = 1;
            while (*(++val) == '0')
                ++i;
            if (*val < '0' || *val > '9')
            {
                out = false;
                return i;
            }
            else if (*val == '1')
            {
                out = true;
                return i + 1;
            }
            break;
        }
        case '1':
            out = true;
            return 1;
            break;
    }
    m_lastError = "NumberParser: string does not contain a valid boolean";
    out = false;
    return 0;
}
