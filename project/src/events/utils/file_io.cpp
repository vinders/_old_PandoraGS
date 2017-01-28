/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : file IO toolbox
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cerrno>
#include <io.h>
#include <fcntl.h>
#ifdef _WINDOWS
#include <Windows.h>
#include <shlobj.h>
#define _CRT_SECURE_NO_WARNINGS
#define access(dir, rights) _access(dir, rights)
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif
#include "file_io.h"
using namespace events::utils;


/// @brief Get file path with write access (same as plugin or home path)
/// @return File path
std::string FileIO::getWritableFilePath()
{
    std::string filePath;

    // default file path (same as plugin)
    if (access("./", 06) == 0)
    {
        filePath = "";
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
                filePath += std::string("\\");
        }
        #else
        char* buffer = getenv("HOME");
        if (buffer != NULL)
            filePath = buffer;
        else
        {
            char* homedir = getpwuid(getuid())->pw_dir;
            filePath = homedir;
        }
        filePath += std::string("/");
        #endif
        errno = 0;
    }
    return filePath;
}
