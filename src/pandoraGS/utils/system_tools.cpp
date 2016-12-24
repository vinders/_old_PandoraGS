/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   system_tools.cpp
Description : system management toolbox
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <io.h>
#include <fcntl.h>
#include <errno.h>
#ifdef _WINDOWS
#include <Windows.h>
#include <tchar.h>
#include <shlobj.h>
#define _CRT_SECURE_NO_WARNINGS
#define access(dir, rights) _access(dir, rights)
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif
using namespace std;
#include "system_tools.h"

#ifdef _WINDOWS
/// <summary>Enable or disable screen saver</summary>
/// <param name="isEnabled">Enabled/disabled</param>
void SystemTools::setScreensaver(bool isEnabled)
{
    // dynamic thread state system call
    EXECUTION_STATE(WINAPI *D_SetThreadExecutionState)(EXECUTION_STATE esFlags);
    HINSTANCE hKernel32 = NULL;

    // load the kernel32.dll library
    hKernel32 = LoadLibrary(L"kernel32.dll");
    if (hKernel32 != NULL)
    {
        D_SetThreadExecutionState = (EXECUTION_STATE(WINAPI *)(EXECUTION_STATE))GetProcAddress(hKernel32, "SetThreadExecutionState");
        if (D_SetThreadExecutionState != NULL)
        {
            if (isEnabled)
                D_SetThreadExecutionState(ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED); // enabled
            else
                D_SetThreadExecutionState(ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED | ES_CONTINUOUS); // disabled
        }
        FreeLibrary(hKernel32);
    }
}

/// <summary>Create a new output console window</summary>
/// <returns>Output stream descriptor</returns>
FILE* SystemTools::createOutputWindow()
{
    AllocConsole(); // open output console

    // redirect output stream
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrtOut = _open_osfhandle((long)hOutput, _O_TEXT);
    FILE* hfOut = _fdopen(hCrtOut, "w");
    setvbuf(hfOut, NULL, _IONBF, 1);
    *stdout = *hfOut;
    return hfOut;
}

/// <summary>Close current output console window</summary>
/// <param name="hfOut">Output stream descriptor</param>
void SystemTools::closeOutputWindow(FILE* hfOut)
{
    fflush(stdout);
    fclose(hfOut); // close output stream
    FreeConsole(); // close console
}

#else
/// <summary>Create a new output console window</summary>
void SystemTools::createOutputWindow()
{
}

/// <summary>Close current output console window</summary>
void SystemTools::closeOutputWindow()
{
}
#endif

/// <summary>Get file path with write access (same as plugin or home path)</summary>
/// <returns>File path</returns>
std::string SystemTools::getWritableFilePath()
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
