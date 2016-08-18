/*******************************************************************************
PANDORAGS project - PS1 GPU driver test app
------------------------------------------------------------------------
File name :   UiLauncher.cpp
Description : test app - plugin dialogs/GUI
*******************************************************************************/
#include <Windows.h>
#include <tchar.h>

#include "gui_main.h" // plugin interface

///<summary>Test plugin dialogs</summary>
///<returns>Exit code</returns>
int _tmain(int argc, _TCHAR* argv[])
{
    GPUconfigure();
    GPUabout();
    return 0;
}

