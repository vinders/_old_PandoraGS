/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : string encoding conversion
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <codecvt>
#include <locale>
#include <vector>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.io
    /// Input/output utilities
    namespace io
    {
        /// @class StringEncoder
        /// @brief String encoding conversion
        class StringEncoder
        {
        public:
            // -- Character byte conversion --
            
            /// @brief Split wide character into 2 bytes
            /// @param[in] val       Wide character
            /// @param[out] outHigh  Byte with higher bits
            /// @param[out] outLow   Byte with lower bits
            static inline void wideCharToBytes(const wchar_t val, char& outHigh, char& outLow)
            {
                wchar_t highBits = val >> 8;
                wchar_t lowBits = (val & 0x0FF);
                outHigh = static_cast<char>(highBits);
                outLow = static_cast<char>(lowBits);
            }
            /// @brief Convert 2 bytes to wide character
            /// @param[in] valHigh  Byte with higher bits
            /// @param[in] valLow   Byte with lower bits
            /// @param[out] out     Wide character
            static inline void bytesToWideChar(const char valHigh, const char valLow, wchar_t& out)
            {
                out = ((static_cast<wchar_t>(valHigh) << 8) | static_cast<wchar_t>(valLow));
            }
            
            /// @brief Convert wide character string to little-endian byte string
            /// @param[in] wstr  Wide character string
            /// @returns Little-endian byte string
            static std::vector<char> wideStringToLittleEndianBytes(const wchar_t* wstr)
            /// @brief Convert little-endian byte string to wide character string
            /// @param[in] str  Byte string
            /// @returns Wide character string
            static std::wstring littleEndianBytesToWideString(const std::vector<char> str)
            
            /// @brief Convert wide character string to big-endian byte string
            /// @param[in] wstr  Wide character string
            /// @returns Big-endian byte string
            static std::vector<char> wideStringToBigEndianBytes(const wchar_t* wstr)
            /// @brief Convert big-endian byte string to wide character string
            /// @param[in] str  Byte string
            /// @returns Wide character string
            static std::wstring bigEndianBytesToWideString(const std::vector<char> str)
            
            
            
            // -- Character encoding conversion --
            
            /// @brief Convert code-page string to UTF-8 string
            /// @param[in] str       Source string
            /// @param[in] codePage  Code-page identifier
            /// @returns UTF-8 string
            static inline std::string codePageToUtf8(const char* str, const std::locale codePage)
            {
                if (str == nullptr || str[0] == '\0')
                    return std::string("");
                
                // code page to unicode
                std::vector<wchar_t> codePageVec(unicodeStr.size());
                std::use_facet<std::ctype<wchar_t>>(codePage).widen(str, str + strlen(str), codePageVec.data());
                // unicode to UTF-8
                std::string unicodeStr(codePageVec.data(), codePageVec.size());
                return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(unicodeStr);
            }
            /// @brief Convert ANSI (Windows-1252) string to UTF-8 string
            /// @param[in] str       Source ANSI string
            /// @returns UTF-8 string
            static inline std::string ansi1252ToUtf8(const char* str)
            {
                return codePageToUtf8(str, std::locale(".1252"));
            }
            
            /// @brief Convert UTF-8 string to code-page string
            /// @param[in] utf8      Source UTF-8 string
            /// @param[in] codePage  Code-page identifier
            /// @returns Code-page string
            static inline std::string utf8ToCodePage(const char* utf8, const std::locale codePage)
            {
                if (utf8 == nullptr || utf8[0] == '\0')
                    return std::string("");
                
                // UTF-8 to unicode
                wstring unicodeStr = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(utf8str);
                // unicode to code page
                std::vector<char> codePageVec(unicodeStr.size());
                std::use_facet<std::ctype<wchar_t>>(codePage).narrow(unicodeStr.data(), unicodeStr.data() + unicodeStr.size(), '?', codePageVec.data());
                return std::string(codePageVec.data(), codePageVec.size());
            }
            /// @brief Convert UTF-8 string to ANSI (Windows-1252) string
            /// @param[in] utf8      Source UTF-8 string
            /// @returns Code-page string
            static inline std::string utf8ToAnsi1252(const char* utf8)
            {
                return utf8ToCodePage(utf8, std::locale(".1252"));
            }
            

            /// @brief Convert code-page string to UTF-16 string
            /// @param[in] str       Source string
            /// @param[in] codePage  Code-page identifier
            /// @returns UTF-16 string
            static inline std::wstring codePageToUtf16(const char* str, const std::locale codePage);
            /// @brief Convert ANSI (Windows-1252) string to UTF-16 string
            /// @param[in] str       Source ANSI string
            /// @returns UTF-16 string
            static inline std::wstring ansi1252ToUtf16(const char* str)
            {
                return codePageToUtf16(str, std::locale(".1252"));
            }
            
            /// @brief Convert UTF-16 string to code-page string
            /// @param[in] utf16     Source UTF-16 string
            /// @param[in] codePage  Code-page identifier
            /// @returns Code-page string
            static std::string utf16ToCodePage(const wchar_t* utf16, const std::locale codePage);
            /// @brief Convert UTF-16 string to ANSI (Windows-1252) string
            /// @param[in] utf16     Source UTF-16 string
            /// @returns Code-page string
            static inline std::string utf16ToAnsi1252(const wchar_t* utf16)
            {
                return utf16ToCodePage(utf16, std::locale(".1252"));
            }


            /// @brief Convert UTF-8 string to UTF-16 string
            /// @param[in] utf8  Source UTF-8 string
            /// @returns UTF-16 string
            static inline std::wstring utf8ToUtf16(const char* utf8)
            {
                return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conv.from_bytes(utf8);
            }
            /// @brief Convert UTF-16 string to UTF-8 string
            /// @param[in] utf16  Source UTF-16 string
            /// @returns UTF-8 string
            static inline std::string utf16ToUtf8(const wchar_t* utf16)
            {
                return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conv.to_bytes(utf16);
            }
            
            
            /// @brief Convert ANSI (Windows 1252) string to UCS-2 string
            /// @param[in] str  Source ANSI string
            /// @returns UCS-2 string
            static inline std::wstring ansi1252ToUcs2(const char* str)
            {
                return utf16ToUcs2(ansi1252ToUtf16(utf8).c_str());
            }
            /// @brief Convert UCS-2 string to ANSI (Windows 1252) string
            /// @param[in] ucs2  Source UCS-2 string
            /// @returns ANSI string
            static inline std::string ucs2ToAnsi1252(const wchar_t* ucs2)
            {
                return utf16ToAnsi1252(ucs2ToUtf16(ucs2).c_str());
            }
            
            /// @brief Convert UTF-8 string to UCS-2 string
            /// @param[in] utf8  Source UTF-8 string
            /// @returns UCS-2 string
            static inline std::wstring utf8ToUcs2(const char* utf8)
            {
                return utf16ToUcs2(utf8ToUtf16(utf8).c_str());
            }
            /// @brief Convert UCS-2 string to UTF-8 string
            /// @param[in] ucs2  Source UCS-2 string
            /// @returns UTF-8 string
            static inline std::string ucs2ToUtf8(const wchar_t* ucs2)
            {
                return utf16ToUtf8(ucs2ToUtf16(ucs2).c_str());
            }
            
            /// @brief Convert UTF-16 string to UCS-2 string
            /// @param[in] utf8  Source UTF-16 string
            /// @returns UCS-2 string
            static std::wstring utf16ToUcs2(const wchar_t* utf16);
            /// @brief Convert UCS-2 string to UTF-16 string
            /// @param[in] ucs2  Source UCS-2 string
            /// @returns UTF-16 string
            static inline std::wstring ucs2ToUtf16(const wchar_t* ucs2)
            {
                return std::wstring(ucs2);
            }
        };
    }
}
