/*******************************************************************************
PANDORAGS project - PS1 GPU driver test app
------------------------------------------------------------------------
File name :   main.cpp
Description : test app - flow/unit tests -- entry point and test function
*******************************************************************************/
#include <Windows.h>
#include <tchar.h>
#include <string>
#include "windowManager.h"
#include "main.h"

#include "gpu_main.h" // plugin interface


///<summary>Test app to check plugin execution flow</summary>
///<returns>Exit code</returns>
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                       _In_ LPTSTR    lpCmdLine, _In_ int       nCmdShow)
{
    // open and manage new window
    return CreateBaseWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

///<summary>Plugin test function (use with step by step debugging)</summary>
///<param name="hWindow">Main window handle</param>
void ProcessTest(HWND hWindow)
{
    // USE BREAKPOINT HERE (for init/open)

    // initialize plugin
    if (GPUinit())
        return;

    // start renderer
    GPUsetExeName("SCEE_TEST.001");
    if (GPUopen(hWindow) == 0)
    {
        // USE BREAKPOINT HERE
        // add function(s) you need to test here
        // ...

        // close renderer
        GPUclose();
    }

    // close plugin
    GPUshutdown();
}
