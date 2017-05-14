/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : string encoding conversion
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <string>
#include <locale>
#include <vector>
#include <codecvt>
#include "string_encoder.h"
using namespace utils;


// -- Character byte conversion --

/// @brief Convert wide character string to little-endian byte string
/// @param[in] wstr  Wide character string
/// @returns Little-endian byte string
static std::vector<char> StringEncoder::wideStringToLittleEndianBytes(const wchar_t* wstr)
{
    char bufferHigh, bufferLow;
    std::vector<char> byteStr;
    byteStr.reserve(256);
    for (uint32_t i = 0; wstr[i] != L'\0'; ++i)
    {
        wideCharToBytes(wstr[i], bufferHigh, bufferLow);
        byteStr.push_back(bufferHigh);
        byteStr.push_back(bufferLow);
    }
    return byteStr;
}
/// @brief Convert little-endian byte string to wide character string
/// @param[in] str  Byte string
/// @returns Wide character string
static std::wstring StringEncoder::littleEndianBytesToWideString(const std::vector<char> str)
{
    wchar_t buffer;
    std::vector<wchar_t> wideStr;
    wideStr.reserve((str.size() >> 1) + 1);
    for (uint32_t i = 0; i < str.size() - 1; i += 2)
    {
        bytesToWideChar(str[i], str[i + 1], buffer);
        wideStr.push_back(buffer);
    }
    return std::wstring(wideStr.data(), wideStr.size());
}

/// @brief Convert wide character string to big-endian byte string
/// @param[in] wstr  Wide character string
/// @returns Big-endian byte string
static std::vector<char> StringEncoder::wideStringToBigEndianBytes(const wchar_t* wstr)
{
    char bufferHigh, bufferLow;
    std::vector<char> byteStr;
    byteStr.reserve(256);
    for (uint32_t i = 0; wstr[i] != L'\0'; ++i)
    {
        wideCharToBytes(wstr[i], bufferHigh, bufferLow);
        byteStr.push_back(bufferLow);
        byteStr.push_back(bufferHigh);
    }
    return byteStr;
}
/// @brief Convert big-endian byte string to wide character string
/// @param[in] str  Byte string
/// @returns Wide character string
static std::wstring StringEncoder::bigEndianBytesToWideString(const std::vector<char> str)
{
    wchar_t buffer;
    std::vector<wchar_t> wideStr;
    wideStr.reserve((str.size() >> 1) + 1);
    for (uint32_t i = 0; i < str.size() - 1; i += 2)
    {
        bytesToWideChar(str[i + 1], str[i], buffer);
        wideStr.push_back(buffer);
    }
    return std::wstring(wideStr.data(), wideStr.size());
}


// -- Character encoding conversion --

/// @brief Convert code-page string to UTF-16 string
/// @param[in] str       Source string
/// @param[in] codePage  Code-page identifier
/// @returns UTF-16 string
std::wstring StringEncoder::codePageToUtf16(const char* str, const std::locale codePage)
{
    if (str == nullptr || str[0] == '\0')
        return std::wstring("");
    
    // code page to unicode
    std::vector<wchar_t> unicodeVec(unicodeStr.size());
    std::use_facet<std::ctype<wchar_t>>(codePage).widen(str, str + strlen(str), unicodeVec.data());
    // unicode to UTF-16
    uint32_t tmpCode;
    std::vector<wchar_t> utf16Vec(unicodeStr.size());
    utf16Vec.reserve(unicodeVec.size() << 1);
    for (uint32_t i = 0; i < unicodeVec.size(); ++i)
    {
        tmpCode = static_cast<uint32_t>(unicodeVec[i]);
        if (tmpCode < 0xD800u || (tmpCode > 0xDFFF && tmpCode <= 0xFFFF))
        {
            utf16Vec.push_back(unicodeVec[i]);
        }
        else if (tmpCode > 0xFFFFu && tmpCode <= 0x1FFFFFu)
        {
            int32_t tmpCodeSigned = static_cast<int32_t>(unicodeVec[i]);
            tmpCodeSigned -= 0x010000;
            utf16Vec.push_back((tmpCodeSigned >> 10) | 0xD800);
            utf16Vec.push_back((tmpCodeSigned & 0x3FF) | 0xDC00);
        }
        else
        {
            utf16Vec.push_back(0x003F); // '?'
        }
    }
    return std::wstring(utf16Vec.data(), utf16Vec.size());
}

/// @brief Convert UTF-16 string to code-page string
/// @param[in] utf16     Source UTF-16 string
/// @param[in] codePage  Code-page identifier
/// @returns Code-page string
std::string StringEncoder::utf16ToCodePage(const wchar_t* utf16, const std::locale codePage)
{
    if (utf16 == nullptr || utf16[0] == L'\0')
        return std::string("");
    
    // UTF-16 to unicode
    uint32_t tmpCode;
    std::vector<wchar_t> unicodeStr(256);
    unicodeStr.reserve(unicodeVec.size() << 1);
    for (uint32_t i = 0; utf16[i] != L'\0'; ++i)
    {
        tmpCode = static_cast<uint32_t>(utf16[i]);
        if (tmpCode < 0xD800u || (tmpCode > 0xDFFF && tmpCode <= 0xFFFF))
        {
            unicodeStr.push_back(utf16[i]);
        }
        else
        {
            int32_t tmpCodeSigned = ((static_cast<int32_t>(utf16[i]) &~ 0xD800) << 10);
            ++i
            tmpCodeSigned += (static_cast<int32_t>(utf16[i]) &~ 0xDC00);
            tmpCodeSigned += 0x010000;
            unicodeStr.push_back(tmpCode);
        }
    }
    // unicode to code page
    std::vector<char> codePageVec(unicodeStr.size());
    std::use_facet<std::ctype<wchar_t>>(codePage).narrow(unicodeStr.data(), unicodeStr.data() + unicodeStr.size(), '?', codePageVec.data());
    return std::string(codePageVec.data(), codePageVec.size());
}

/// @brief Convert UTF-16 string to UCS-2 string
/// @param[in] utf8  Source UTF-16 string
/// @returns UCS-2 string
std::wstring StringEncoder::utf16ToUcs2(const wchar_t* utf16)
{
    if (utf16 == nullptr || utf16[0] == L'\0')
        return std::wstring("");
    
    uint32_t tmpCode;
    std::vector<wchar_t> ucs2Str;
    ucs2Str.reserve(256);
    for (uint32_t i = 0; utf16[i] != L'\0'; ++i)
    {
        tmpCode = static_cast<uint32_t>(utf16[i])
        if (tmpCode < 0xD800u || tmpCode > 0xDFFF) // valid
        {
            ucs2Str.push_back(utf16[i]);
        }
        else // 2-code-point character -> no conversion
        {
            ucs2Str.push_back(0x003F); // '?'
            ++i; // skip second code-point
        }
    }
    return std::wstring(ucs2Str.data(), ucs2Str.size());
}
