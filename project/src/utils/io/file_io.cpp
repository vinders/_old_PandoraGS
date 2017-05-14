/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : file input/output - base
*******************************************************************************/
#include "../../globals.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <locale>
#include <vector>
#include <cerrno>
#include <codecvt>
#include <io.h>
#include <fcntl.h>
#ifdef _WINDOWS
#   include <Windows.h>
#   include <tchar.h>
#   include <shlobj.h>
#   define access(dir,rights) _access(dir,rights)
#   ifndef #define _WIN32_IE
#       define _WIN32_IE 0x0600
#   endif
#else
#   include <unistd.h>
#   include <sys/types.h>
#   include <pwd.h>
#endif
#include "../assert.h"
#include "file_io.h"
using namespace std::literals::string_literals;
using namespace utils::io;


/// @brief Get system home directory
/// @returns Home directory path
std::wstring FileIO::getHomeDirectoryPath() noexcept
{
    std::wstring filePath;
    try
    {
        #ifdef _WINDOWS
        wchar_t pathBuffer[MAX_PATH];
        if (SHGetFolderPath(0, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, pathBuffer) == S_OK) // %Homepath%
        {
            filePath = std::wstring(pathBuffer) + L"\\"s;
        }
        else
        {
            ASSERT(false);
            filePath = L"%Homepath%\\"s;
        }
        #else // LINUX/UNIX
        char* pathBuffer = std::getenv("HOME"); // $HOME
        if (pathBuffer == nullptr)
            pathBuffer = getpwuid(getuid())->pw_dir;
        if (pathBuffer != nullptr)
        {
            std::string pathStrBuffer(pathBuffer);
            filePath = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(pathStrBuffer);
        }
        else
        {
            ASSERT(false);
            filePath = L"$HOME/"s;
        }
        #endif
    }
    catch (...)
    {
        ASSERT(false);
        #ifdef _WINDOWS
        filePath = L"%Homepath%\\"s;
        #else
        filePath = L"$HOME/"s;
        #endif
    }
    errno = 0;
    return filePath;
}

/// @brief Get working directory
/// @returns Working directory path
std::wstring FileIO::getWorkingDirectoryPath() noexcept
{
    std::wstring filePath;
    // current directory, if allowed
    if (access("./", 06) == 0)
    {
        filePath = L""s;
    }
    else // access denied -> use common working directory
    {
        #ifdef _WINDOWS
        try
        {
            wchar_t pathBuffer[MAX_PATH];
            if (SHGetFolderPath(0, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, pathBuffer) == S_OK) // %AppData%
            {
                filePath = std::wstring(pathBuffer) + L"\\"s;
            }
            else
            {
                ASSERT(false);
                filePath = L"%AppData%\\"s;
            }
        }
        catch (...)
        {
            ASSERT(false);
            filePath = L"%AppData%\\"s;
        }
        #else // Linux/UNIX
        filePath = getHomeDirectoryPath(); // $HOME
        #endif
    }
    return filePath;
}


// -- Character encoding conversion --

/// @brief Convert code-page string to UTF-16 string
/// @param[in] str       Source string
/// @param[in] codePage  Code-page identifier
/// @returns UTF-16 string
std::wstring FileIO::codePageToUtf16(const char* str, const std::locale codePage)
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
std::string FileIO::utf16ToCodePage(const wchar_t* utf16, const std::locale codePage)
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
std::wstring FileIO::utf16ToUcs2(const wchar_t* utf16)
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
