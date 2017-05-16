/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : file input/output - base
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <string>
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
#include "string_encoder.h"
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
            filePath = StringEncoder::utf8ToUtf16(pathBuffer);
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
