/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   system_tools.cpp
Description : system management toolbox
*******************************************************************************/
#ifdef _WINDOWS
#include <Windows.h>
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
#endif
