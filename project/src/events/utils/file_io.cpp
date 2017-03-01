/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : file IO toolbox
*******************************************************************************/
#include "../../globals.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>
#include <cerrno>
#include <locale>
#include <codecvt>
#include <io.h>
#include <fcntl.h>
#ifdef _WINDOWS
#include <Windows.h>
#include <tchar.h>
#include <shlobj.h>
//#define _CRT_SECURE_NO_WARNINGS
#define access(dir, rights) _access(dir, rights)
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif
using namespace std::literals::string_literals;
#include "file_io.h"
using namespace events::utils;


/// @brief Get file path with write access (same as plugin or home path)
/// @returns File path
std::string FileIO::getWritableFilePath() noexcept
{
    std::string filePath;
    try
    {
        // default file path (same as plugin)
        if (access("./", 06) == 0)
        {
            filePath = ""s;
        }
        else // default access denied
        {
            // get home folder path
            #ifdef _WINDOWS
            char outPath[MAX_PATH];
            if (SHGetSpecialFolderPathA(NULL, outPath, CSIDL_APPDATA, 0))
            {
                filePath = outPath; // %AppData%/Roaming
                if (filePath.length() > 0)
                    filePath += "\\"s;
            }
            #else
            char* buffer = getenv("HOME");
            if (buffer != nullptr)
                filePath = buffer;
            else
            {
                char* homedir = getpwuid(getuid())->pw_dir;
                filePath = homedir;
            }
            filePath += "/"s;
            #endif
            errno = 0;
        }
    }
    catch (...)
    {
        filePath = ""s;
    }
    return filePath;
}

/// @brief Get file path with write access (same as plugin or home path)
/// @returns File path as wide string
std::wstring FileIO::getWritableFileWidePath() noexcept
{
    std::string narrow = getWritableFilePath();
    return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(narrow);
}
