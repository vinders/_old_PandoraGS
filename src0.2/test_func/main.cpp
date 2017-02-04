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
using namespace std;
#include "windowManager.h"
#include "primitive.h"
#include "demo.h"
#include "main.h"

#include "psemu.h" // plugin PSEmu interface
#define OUTPUT_CONSOLE_HEIGHT 50

FILE* openTestConsole(LPCWSTR title, short bufferHeight);
void closeTestConsole(FILE* hfOut);
void listPrimitives();
bool renderPrimitive(int id, bool isFlipped);


///<summary>Test app to check plugin execution</summary>
///<returns>Exit code</returns>
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                       _In_ LPTSTR    lpCmdLine, _In_ int       nCmdShow)
{
    // open and manage new window
    return CreateBaseWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

///<summary>Plugin dialog testing</summary>
///<param name="type">Dialog type (0 = config ; 1 = about box)</param>
void DialogTest(int type)
{
    if (type == 0)
        GPUconfigure();
    else
        GPUabout();
}

///<summary>Plugin flow test function (use with step by step debugging)</summary>
///<param name="hWindow">Main window handle</param>
void ProcessTest(HWND hWindow)
{
    // initialize plugin
    if (GPUinit())
        return;

    // start renderer
    GPUsetExeName("UNITTEST.001");
    if (GPUopen(hWindow) == 0)
    {
        GPUsetfix(4096);
        GPUupdateLace();

        // ...
        // add PSEmu function(s) you need to test here
        // ...

        // demo animation
        renderDemoAnimation();

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
    FILE* hfOut = openTestConsole(L"Unit testing - console", 500); // output console

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
    GPUsetExeName("UNITTEST.001");
    if (GPUopen(hWindow) == 0)
    {
        GPUupdateLace();

        // primitive testing
        FILE* hfOut = openTestConsole(L"Primitives testing - console", 0); // test console
        int primitiveId = 0;
        listPrimitives();
        do
        {
            // choose primitive type
            primitiveId = 0;
            char inputBuffer = 0;
            bool isFlipped = false;
            printf("Primitive: ");
            fflush(stdin);
            while ((inputBuffer = getchar()) != EOF && inputBuffer != '\n')
            {
                if (inputBuffer >= '0' && inputBuffer <= '9')
                    primitiveId = (primitiveId << 4) + (inputBuffer - '0');
                else if (inputBuffer >= 'A' && inputBuffer <= 'F')
                    primitiveId = (primitiveId << 4) + ((inputBuffer - 'A') + 10);
                else if (inputBuffer == '-')
                    isFlipped = true;
            }

            // display primitive
            if (primitiveId != 0)
            {
                if (renderPrimitive(primitiveId, isFlipped))
                    listPrimitives();
                else
                    printf("Unknown primitive type: %d\n", primitiveId);
            }
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
///<param name="title">Console window title</param>
///<param name="bufferHeight">Output buffer height (lines)</param>
///<returns>Output stream descriptor</returns>
FILE* openTestConsole(LPCWSTR title, short bufferHeight)
{
    // output console
    AllocConsole();
    SetConsoleTitle(title);

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

    // resize
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = 80;
    coord.Y = (bufferHeight > 0) ? bufferHeight : OUTPUT_CONSOLE_HEIGHT;
    BOOL isResizeAbs = SetConsoleScreenBufferSize(hConsole, coord);
    SMALL_RECT winPos;
    winPos.Left = 0;
    winPos.Top = 0;
    winPos.Right = coord.X - 1;
    winPos.Bottom = OUTPUT_CONSOLE_HEIGHT - 1;
    SetConsoleWindowInfo(hConsole, isResizeAbs, &winPos);

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

///<summary>Ask the plugin to render a primitive</summary>
///<param name="id">Primitive ID (type)</param>
///<param name="isFlipped">Flip indicator (only for rectangles)</param>
///<returns>Valid primitive</returns>
bool renderPrimitive(int id, bool isFlipped)
{
    // create primitive description
    unsigned long* pPrim = NULL;
    int primLen = createPrimitive(id, &pPrim);
    if (primLen == -1)
        return false;
    if (isFlipped && !isPrimitiveFlippable(id))
        isFlipped = false;

    // render primitive
    GPUtestPrimitive(pPrim, primLen, isFlipped);
    return true;
}
