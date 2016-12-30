/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   system_tools.h
Description : system management toolbox
*******************************************************************************/
#ifndef _SYSTEM_TOOLS_H
#define _SYSTEM_TOOLS_H
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string>
#ifdef _WINDOWS
#include <Windows.h>
#include <tchar.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

// data types
typedef int32_t buffer_t;
typedef uint32_t ubuffer_t;
enum change_t : int32_t // pending change
{
    Change_none = 0,
    Change_disable = 1,
    Change_enable = 2
};


// System management toolbox
namespace SystemTools
{
    /// <summary>Create a new output console window</summary>
    void createOutputWindow();
    /// <summary>Close current output console window</summary>
    void closeOutputWindow();
    /// <summary>Set cursor position in output console window</summary>
    /// <param name="line">Line number</param>
    void setConsoleCursorPos(int line);

#ifdef _WINDOWS
    /// <summary>Create a new output console window</summary>
    /// <param name="hWindow">Main window handle</param>
    /// <param name="isFullscreen">Fullscreen or not</param>
    /// <param name="isResizable">Resizable window (if window mode) or not</param>
    void createDisplayWindow(HWND hWindow, bool isFullscreen, bool isResizable);
    /// <summary>Close current output console window</summary>
    /// <param name="hWindow">Main window handle</param>
    void closeDisplayWindow(HWND hWindow);
    /// <summary>Enable or disable screen-saver</summary>
    /// <param name="isEnabled">Enabled/disabled</param>
    void setScreensaver(bool isEnabled);
#else
    /// <summary>Create a new output console window</summary>
    void createDisplayWindow();
    /// <summary>Close current output console window</summary>
    void closeDisplayWindow();
#endif

    /// <summary>Get file path with write access (same as plugin or home path)</summary>
    /// <returns>File path</returns>
    std::string getWritableFilePath();
}

#endif
