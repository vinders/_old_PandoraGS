/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : terminal ANSI color codes
*******************************************************************************/
#pragma once

#include <cstdio.h>
#include <cstdarg.h>
#include "system_info.h"
#ifdef _WINDOWS
#   include "../vendor/windows_api.h"
#endif

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.system
    /// System management utilities
    namespace system
    {
        /// @enum ansi_color_t
        /// @brief ANSI color codes
        enum class ansi_color_t : uint16_t
        {
#           ifdef _WINDOWS
            black   = 0,
            red     = FOREGROUND_RED,
            green   = FOREGROUND_GREEN,
            yellow  = FOREGROUND_RED|FOREGROUND_GREEN,
            blue    = FOREGROUND_BLUE,
            magenta = FOREGROUND_RED|FOREGROUND_BLUE,
            cyan    = FOREGROUND_GREEN|FOREGROUND_BLUE,
            white   = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE,
            light_black   = FOREGROUND_INTENSITY,
            light_red     = FOREGROUND_INTENSITY|FOREGROUND_RED,
            light_green   = FOREGROUND_INTENSITY|FOREGROUND_GREEN,
            light_yellow  = FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN,
            light_blue    = FOREGROUND_INTENSITY|FOREGROUND_BLUE,
            light_magenta = FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE,
            light_cyan    = FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE,
            light_white   = FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE
            
#           else // -- linux/UNIX/POSIX --
            black   = 30,
            red     = 31,
            green   = 32,
            yellow  = 33,
            blue    = 34,
            magenta = 35,
            cyan    = 36,
            white   = 37,
            light_black   = 90,
            light_red     = 91,
            light_green   = 92,
            light_yellow  = 93,
            light_blue    = 94,
            light_magenta = 95,
            light_cyan    = 96,
            light_white   = 97
#           endif  
        };
        
        /// @brief Change terminal output color
        template <ansi_color_t ForeColor, ansi_color_t BackColor> 
        inline void setTextAnsiColor()
        {
#           ifdef _WINDOWS
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<uint16_t>(ForeColor) | (static_cast<uint16_t>(BackColor) << 4));
#           else // -- linux/UNIX/POSIX --
            char buffer[] = "\x1B[30;97m";
            buffer[2] = (BackColor / 10) + '1';
            buffer[3] = (BackColor % 10) + '0';
            buffer[5] = (ForeColor / 10) + '1';
            buffer[6] = (ForeColor % 10) + '0';
            printf(buffer);
#           endif  
        }
        
        /// @brief Change terminal text output color and print some text
        /// @param[in] format     Format string (printf compatible)
        /// @param[in] ...        Additional parameters (used by format string)
        template <ansi_color_t ForeColor, ansi_color_t BackColor> 
        inline void printfColoredText(const char* format, ...)
        {
            setTextAnsiColor<ForeColor, BackColor>();
            if (format != nullptr)
            {
                va_list args;
                va_start(args, format);
                vprintf(format, args);
                va_end(args);
            }
        }
    }
}
