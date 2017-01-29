/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : console output window management
*******************************************************************************/
#include "../../globals.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <io.h>
#include <fcntl.h>
#ifdef _WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <tchar.h>
#else
#include <unistd.h>
#include <sys/types.h>
#endif
#include "console_window.h"
using namespace display::utils;


/// @brief Display window on screen
/// @param bufferWidth Console buffer width
/// @param bufferHeight Console buffer height
/// @param height Window height (zero to keep buffer height)
void ConsoleWindow::show(uint32_t bufferWidth, uint32_t bufferHeight, int32_t height)
{
    if (m_isVisible)
        return;

    // open console window
    #ifdef _WINDOWS
    AllocConsole();
    SetConsoleTitle(L"PandoraGS - Debug console");
    #else
    // Linux/UNIX version
    //...
    #endif
    m_isVisible = true;

    // resize
    #ifdef _WINDOWS
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = bufferWidth;  // 80
    coord.Y = bufferHeight; // 40
    BOOL isResizeAbs = SetConsoleScreenBufferSize(hConsole, coord);
    SMALL_RECT winPos;
    winPos.Left = 0;
    winPos.Top = 0;
    winPos.Right = coord.X - 1;
    winPos.Bottom = (height > 0) ? (height - 1) : (coord.Y - 1);
    SetConsoleWindowInfo(hConsole, isResizeAbs, &winPos);
    #else
    // Linux/UNIX version
    //...
    #endif

    // redirect output stream
    if (m_hfOutput == NULL)
    {
        #ifdef _WINDOWS
        HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        int hCrtOut = _open_osfhandle((long)hOutput, _O_TEXT);
        m_hfOutput = _fdopen(hCrtOut, "w");
        setvbuf(m_hfOutput, NULL, _IONBF, 1);
        *stdout = *m_hfOutput;
        #else
        // Linux/UNIX version
        //...
        #endif
    }
}

/// @brief Hide existing window
void ConsoleWindow::hide()
{
    if (m_isVisible == false)
        return;

    // close output stream
    if (m_hfOutput != NULL)
    {
        fflush(stdout);
        fclose(m_hfOutput);
    }

    // close console window
    #ifdef _WINDOWS
    FreeConsole(); 
    #else
    // Linux/UNIX version
    //...
    #endif
    
    m_isVisible = false;
}

/// @brief Clear window content
void ConsoleWindow::clear()
{
    if (m_isVisible == false)
        return;
    
    // only reset cursor position without cleaning (to avoid flickering)
    #ifdef _WINDOWS
    HANDLE hStream = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(hStream, pos);
    #else
    // Linux/UNIX version
    //...
    #endif
}
