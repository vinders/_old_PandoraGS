/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : terminal color codes (for text display)
*******************************************************************************/
#pragma once

#ifdef _WINDOWS

#   include <Windows.h>
#   define INIT_ANSI_COLOR_SUPPORT()  HANDLE _hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE)
#   define SET_ANSI_COLOR(color)      SetConsoleTextAttribute(_hConsoleOutput, color)

#   define ANSI_COLOR_RESET   FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_GREEN
#   define ANSI_COLOR_RED     FOREGROUND_RED|FOREGROUND_INTENSITY 
#   define ANSI_COLOR_GREEN   FOREGROUND_GREEN|FOREGROUND_INTENSITY
#   define ANSI_COLOR_YELLOW  FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY
#   define ANSI_COLOR_BLUE    FOREGROUND_BLUE|FOREGROUND_INTENSITY
#   define ANSI_COLOR_MAGENTA FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_INTENSITY
#   define ANSI_COLOR_CYAN    FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY

#else

#   define INIT_ANSI_COLOR_SUPPORT() 
#   define SET_ANSI_COLOR(color)   printf(color);
    
#   define ANSI_COLOR_RESET   "\x1b[0m"
#   define ANSI_COLOR_RED     "\x1b[31m"
#   define ANSI_COLOR_GREEN   "\x1b[32m"
#   define ANSI_COLOR_YELLOW  "\x1b[33m"
#   define ANSI_COLOR_BLUE    "\x1b[34m"
#   define ANSI_COLOR_MAGENTA "\x1b[35m"
#   define ANSI_COLOR_CYAN    "\x1b[36m"

#endif