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
#include <cstdint>

typedef int32_t buffer_t;
typedef uint32_t ubuffer_t;
enum change_t : int32_t // pending change
{
    Change_none = 0,
    Change_disable = 1,
    Change_enable = 2
};

namespace SystemTools
{
#ifdef _WINDOWS
    /// <summary>Enable or disable screen-saver</summary>
    /// <param name="isEnabled">Enabled/disabled</param>
    void setScreensaver(bool isEnabled);
#endif
}

#endif
