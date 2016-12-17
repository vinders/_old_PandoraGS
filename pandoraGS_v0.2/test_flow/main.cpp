/*******************************************************************************
PANDORAGS project - PS1 GPU driver test app
------------------------------------------------------------------------
File name :   main.cpp
Description : test app - flow/unit tests -- entry point and test functions
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
#include <tchar.h>
#include <string>
#include "windowManager.h"
#include "main.h"

#include "psemu.h" // plugin PSEmu interface


///<summary>Test app to check plugin execution flow</summary>
///<returns>Exit code</returns>
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                       _In_ LPTSTR    lpCmdLine, _In_ int       nCmdShow)
{
    // open and manage new window
    return CreateBaseWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

///<summary>Plugin flow test function (use with step by step debugging)</summary>
///<param name="hWindow">Main window handle</param>
void ProcessTest(HWND hWindow)
{
    // initialize plugin
    if (GPUinit())
        return;

    // start renderer
    GPUsetExeName("SCEE_TEST.001");
    if (GPUopen(hWindow) == 0)
    {
        // flow tests
        // USE BREAKPOINT HERE
        // ...
        // add PSEmu function(s) you need to test here
        // ...

        // close renderer
        GPUclose();
    }
    // close plugin
    GPUshutdown();
}

///<summary>Complete plugin unit testing</summary>
///<param name="hWindow">Main window handle</param>
void UnitTest(HWND hWindow)
{
    // output console
    AllocConsole();

    // set output stream
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrtOut = _open_osfhandle((long)hOutput, _O_TEXT);
    FILE* hfOut = _fdopen(hCrtOut, "w");
    setvbuf(hfOut, NULL, _IONBF, 1);
    *stdout = *hfOut;
    // set input stream
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    int hCrtIn = _open_osfhandle((long)hInput, _O_TEXT);
    FILE* hfIn = _fdopen(hCrtIn, "r");
    setvbuf(hfIn, NULL, _IONBF, 128);
    *stdin = *hfIn;

    // unit tests
    if (GPUtestUnits((void*)&hWindow))
        printf("\nSUCCESS: all the tests were successful.");
    else
        printf("\nWARNING: some tests have failed...");

    system("pause");
    fclose(hfOut);
    FreeConsole(); // close console
}
