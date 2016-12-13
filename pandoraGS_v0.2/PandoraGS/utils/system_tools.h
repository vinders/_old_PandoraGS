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

namespace SystemTools
{
#ifdef _WINDOWS
    /// <summary>Enable or disable screen-saver</summary>
    /// <param name="isEnabled">Enabled/disabled</param>
    void setScreensaver(bool isEnabled);
#endif
}

#endif
