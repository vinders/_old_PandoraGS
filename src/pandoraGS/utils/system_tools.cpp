/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   system_tools.cpp
Description : system management toolbox
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <io.h>
#include <fcntl.h>
#include <errno.h>
#ifdef _WINDOWS
#include <shlobj.h>
#define _CRT_SECURE_NO_WARNINGS
#define access(dir, rights) _access(dir, rights)
#endif
using namespace std;
#include "system_tools.h"

#ifdef _WINDOWS
HMENU g_hMenu = NULL;        // emulator menu handle
DWORD g_origStyle = 0uL;     // original window style
RECT g_origSize;     // original window size
FILE* g_hfConsoleOut = NULL; // console output stream descriptor

/// <summary>Create a new output console window</summary>
void SystemTools::createOutputWindow()
{
    // open output console
    if (g_hfConsoleOut != NULL)
        closeOutputWindow();
    AllocConsole();
    SetConsoleTitle(L"PandoraGS - Debug console");

    // resize
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = 80;
    coord.Y = 40;
    BOOL isResizeAbs = SetConsoleScreenBufferSize(hConsole, coord);
    SMALL_RECT winPos;
    winPos.Left = 0;
    winPos.Top = 0;
    winPos.Right = coord.X - 1;
    winPos.Bottom = coord.Y - 1;
    SetConsoleWindowInfo(hConsole, isResizeAbs, &winPos);

    // redirect output stream
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrtOut = _open_osfhandle((long)hOutput, _O_TEXT);
    FILE* hfOut = _fdopen(hCrtOut, "w");
    setvbuf(hfOut, NULL, _IONBF, 1);
    *stdout = *hfOut;
    g_hfConsoleOut = hfOut;
}

/// <summary>Close current output console window</summary>
void SystemTools::closeOutputWindow()
{
    if (g_hfConsoleOut != NULL)
    {
        fflush(stdout);
        fclose(g_hfConsoleOut); // close output stream
        FreeConsole(); // close console
        g_hfConsoleOut = NULL;
    }
}

/// <summary>Set cursor position in output console window</summary>
/// <param name="line">Line number</param>
void SystemTools::setConsoleCursorPos(int line)
{
    HANDLE hStream = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { 0, line };
    SetConsoleCursorPosition(hStream, pos);
}


/// <summary>Create a new display window</summary>
/// <param name="hWindow">Main window handle</param>
/// <param name="isFullscreen">Fullscreen or not</param>
/// <param name="isResizable">Resizable window (if window mode) or not</param>
/// <param name="winResX">Window width (if window mode)</param>
/// <param name="winResY">Window height (if window mode)</param>
void SystemTools::createDisplayWindow(HWND hWindow, bool isFullscreen, bool isResizable, uint32_t winResX, uint32_t winResY)
{
    if (g_hMenu != NULL)
        closeDisplayWindow(hWindow);

    // save style backup to restore it
    GetWindowRect(hWindow, &g_origSize);
    DWORD dwStyle = GetWindowLong(hWindow, GWL_STYLE);
    g_origStyle = dwStyle;

    // set new window style
    if (isFullscreen) // fullscreen mode
    {
        dwStyle = CS_OWNDC;
    }
    else // window mode
    {
        if (isResizable == false)
            dwStyle &= ~WS_THICKFRAME;
        dwStyle |= (WS_BORDER | WS_CAPTION | CS_OWNDC);
    }
    SetWindowLong(hWindow, GWL_STYLE, dwStyle);

    // hide emulator menu
    g_hMenu = GetMenu(hWindow);
    if (g_hMenu)
        SetMenu(hWindow, NULL);

    // display new window
    if (isFullscreen)
    {
        // enter fullscreen mode
        //...ChangeDisplaySettings ...
        ShowWindow(hWindow, SW_SHOWMAXIMIZED);
    }
    else // window mode
    {
        // get screen size
        int screenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
        if (screenWidth < 320)
            screenWidth = 800;
        int screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
        if (screenHeight < 240) 
            screenHeight = 600;

        // show centered window
        ShowWindow(hWindow, SW_SHOWNORMAL);
        MoveWindow(hWindow,
            screenWidth / 2 - winResX / 2,
            screenHeight / 2 - winResY / 2,
            winResX + GetSystemMetrics(SM_CXFIXEDFRAME) + 3,
            winResY + GetSystemMetrics(SM_CYFIXEDFRAME) + 3 + GetSystemMetrics(SM_CYCAPTION), 
            TRUE);
        UpdateWindow(hWindow);
    }
}

/// <summary>Close current display window</summary>
/// <param name="hWindow">Main window handle</param>
void SystemTools::closeDisplayWindow(HWND hWindow)
{
    // restore window style
    SetWindowLong(hWindow, GWL_STYLE, g_origStyle);
    MoveWindow(hWindow, g_origSize.left, g_origSize.top, g_origSize.right - g_origSize.left, g_origSize.bottom - g_origSize.top, TRUE);
    // restore emulator menu
    //...ChangeDisplaySettings(NULL, 0);
    if (g_hMenu)
        SetMenu(hWindow, g_hMenu);
    g_hMenu = NULL;
}

/// <summary>Fill display window</summary>
/// <param name="hDC">Main display context</param>
/// <param name="lx">Width</param>
/// <param name="ly">Height</param>
void SystemTools::fillDisplayWindow(HDC hDC, uint32_t lx, uint32_t ly)
{
    RECT screenRect;
    screenRect.left = screenRect.top = 0;
    screenRect.right = lx; screenRect.bottom = ly;
    FillRect(hDC, &screenRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

/// <summary>Get dimensions of display window</summary>
/// <param name="hWindow">Display window handle</param>
/// <param name="outX">Width destination</param>
/// <param name="outY">Height destination</param>
bool SystemTools::getDisplayWindowSize(HWND hWindow, long& outX, long& outY)
{
    RECT clientRect;
    if (GetClientRect(hWindow, &clientRect))
    {
        outX = clientRect.right;
        outY = clientRect.bottom;
        return true;
    }
    return false;
}

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

#else
/// <summary>Create a new output console window</summary>
void SystemTools::createOutputWindow()
{
}

/// <summary>Close current output console window</summary>
void SystemTools::closeOutputWindow()
{
}

/// <summary>Set cursor position in output console window</summary>
/// <param name="line">Line number</param>
void SystemTools::setConsoleCursorPos(int line)
{
    printf("%c[%d;%df", 0x1B, line, 0);
}


/// <summary>Create a new output console window</summary>
void createDisplayWindow()
{
}

/// <summary>Close current output console window</summary>
void closeDisplayWindow()
{
}
#endif

/// <summary>Get file path with write access (same as plugin or home path)</summary>
/// <returns>File path</returns>
std::string SystemTools::getWritableFilePath()
{
    std::string filePath;

    // default file path (same as plugin)
    if (access("./", 06) == 0)
    {
        filePath = "";
    }
    else // default access denied
    {
        // get home folder path
        #ifdef _WINDOWS
        char outPath[MAX_PATH];
        if (SHGetSpecialFolderPathA(NULL, outPath, CSIDL_APPDATA, 0)) 
        {
            filePath = outPath; // %AppData%/Roaming
            if (filePath.length() > 0)
                filePath += std::string("\\");
        }
        #else
        char* buffer = getenv("HOME");
        if (buffer != NULL) 
            filePath = buffer;
        else
        {
            char* homedir = getpwuid(getuid())->pw_dir;
            filePath = homedir;
        }
        filePath += std::string("/");
        #endif
        errno = 0;
    }
    return filePath;
}
