// main.cpp : Defines the entry point for the application.
//
#include<Windows.h>
#include<tchar.h>
#include<string>
#include "TestFlow.h"
#include "main.h"
#include "service_main.h"

// Global Variables
extern HINSTANCE hInst; // current instance


///<summary>Test app to check plugin execution flow (use with step by step debugging)</summary>
///<returns>Exit code</returns>
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                       _In_ LPTSTR    lpCmdLine, _In_ int       nCmdShow)
{
    // open new window
    HACCEL hAccelTable; 
    try
    {
        CreateBaseWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow, &hAccelTable);
    }
    catch (...)
    {
        return -1;
    }

    // main message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

///<summary>Plugin test function</summary>
///<param name="hWindow">Main window handle</param>
void ProcessTest(HWND hWindow)
{
    // initialize plugin
    if (GPUinit())
        return;
    // start renderer
    if (GPUopen(hWindow))
    {
        GPUshutdown();
        return;
    }

    // add whatever function you need to test here
    // ...

    // close renderer and plugin
    GPUclose();
    GPUshutdown();
}
