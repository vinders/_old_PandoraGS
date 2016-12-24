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
#ifdef _WINDOWS
    /// <summary>Enable or disable screen-saver</summary>
    /// <param name="isEnabled">Enabled/disabled</param>
    void setScreensaver(bool isEnabled);

    /// <summary>Create a new output console window</summary>
    /// <returns>Output stream descriptor</returns>
    FILE* createOutputWindow();
    /// <summary>Close current output console window</summary>
    /// <param name="hfOut">Output stream descriptor</param>
    void closeOutputWindow(FILE* hfOut);

#else
    /// <summary>Create a new output console window</summary>
    void createOutputWindow();
    /// <summary>Close current output console window</summary>
    void closeOutputWindow();
#endif

    /// <summary>Get file path with write access (same as plugin or home path)</summary>
    /// <returns>File path</returns>
    std::string getWritableFilePath();
}

#endif
