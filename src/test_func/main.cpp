/*******************************************************************************
PANDORAGS project - PS1 GPU driver test app
------------------------------------------------------------------------
File name :   main.cpp
Description : test app - flow/unit tests -- entry point and test functions
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
#include <tchar.h>
#include <string>
#include "windowManager.h"
#include "main.h"

#include "psemu.h" // plugin PSEmu interface

FILE* openTestConsole();
void closeTestConsole(FILE* hfOut);
void listPrimitives();
void renderPrimitive(int id);


///<summary>Test app to check plugin execution</summary>
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
    FILE* hfOut = openTestConsole(); // output console

    // unit tests
    if (GPUtestUnits((void*)&hWindow))
        printf("\nSUCCESS: all the tests were successful.\n");
    else
        printf("\nWARNING: some tests have failed...\n");

    fflush(stdout);
    system("pause");
    closeTestConsole(hfOut);
}

///<summary>Custom drawing primitives testing</summary>
///<param name="hWindow">Main window handle</param>
void PrimitivesTest(HWND hWindow)
{
    // initialize plugin
    if (GPUinit())
        return;

    // start renderer
    GPUsetExeName("SCEE_TEST.001");
    if (GPUopen(hWindow) == 0)
    {
        // primitive testing
        FILE* hfOut = openTestConsole(); // test console
        int primitiveId = 0;
        do
        {
            listPrimitives();

            // choose primitive type
            primitiveId = 0;
            char inputBuffer = 0;
            printf("\nPrimitive: ");
            fflush(stdin);
            while ((inputBuffer = getchar()) != EOF && inputBuffer != '\n')
            {
                if (inputBuffer >= '0' && inputBuffer <= '9')
                    primitiveId = primitiveId * 10 + (inputBuffer - '0');
            }

            // display primitive
            renderPrimitive(primitiveId);
        }
        while (primitiveId != 0);
        closeTestConsole(hfOut);

        // close renderer
        GPUclose();
    }
    // close plugin
    GPUshutdown();
}


// ---

///<summary>Create test console</summary>
///<returns>Output stream descriptor</returns>
FILE* openTestConsole()
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

    return hfOut;
}

///<summary>Close test console</summary>
///<param name="hfOut">Output stream descriptor</param>
void closeTestConsole(FILE* hfOut)
{
    fclose(hfOut);
    FreeConsole(); // close console
}


// ---

///<summary>Show full list of available primitives</summary>
void listPrimitives()
{
    printf("01 - ???\n");
    //...
    printf("Enter 0 to exit.\n");
}

///<summary>Ask the plugin to render a primitive</summary>
///<param name="id">Primitive ID (type)</param>
void renderPrimitive(int id)
{
    // create primitive description
    int primLen = 0;
    uint16_t pPrim[12];
    switch (id)
    {
        case 1: // ???
            primLen = 0;
            break;
        //...
        default: printf("Unknown primitive type: %d", id); return;
    }

    // render primitive
    printf("Rendering primitive %d...\n\n", id);
    //...
}
