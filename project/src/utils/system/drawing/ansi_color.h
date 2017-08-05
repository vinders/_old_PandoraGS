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
        /// @namespace utils.system.drawing
        /// System drawing utilities
        namespace drawing
        {
            /// @enum ansi_color_t
            /// @brief ANSI color codes
            enum class ansi_color_t : uint16_t
            {
#               ifdef _WINDOWS
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
                
#               else // -- linux/UNIX/POSIX --
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
#               endif  
            };
            
            
            /// @brief Change terminal output color
            template <ansi_color_t ForeColor, ansi_color_t BackColor> 
            inline void setTextAnsiColor()
            {
#               ifdef _WINDOWS
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<uint16_t>(ForeColor) | (static_cast<uint16_t>(BackColor) << 4));
                
#               else // -- linux/UNIX/POSIX --
#               define _UTILS__ANSI_COLOR_CODE_BUILDER(foreColorCode, backColorEnum) \
                        switch (backColorEnum) \
                        { \
                            case black:   printf("\x1B["#foreColorCode";40m"); break; \
                            case red:     printf("\x1B["#foreColorCode";41m"); break; \
                            case green:   printf("\x1B["#foreColorCode";42m"); break; \
                            case yellow:  printf("\x1B["#foreColorCode";43m"); break; \
                            case blue:    printf("\x1B["#foreColorCode";44m"); break; \
                            case magenta: printf("\x1B["#foreColorCode";45m"); break; \
                            case cyan:    printf("\x1B["#foreColorCode";46m"); break; \
                            case white:   printf("\x1B["#foreColorCode";47m"); break; \
                            case light_black:   printf("\x1B["#foreColorCode";100m"); break; \
                            case light_red:     printf("\x1B["#foreColorCode";101m"); break; \
                            case light_green:   printf("\x1B["#foreColorCode";102m"); break; \
                            case light_yellow:  printf("\x1B["#foreColorCode";103m"); break; \
                            case light_blue:    printf("\x1B["#foreColorCode";104m"); break; \
                            case light_magenta: printf("\x1B["#foreColorCode";105m"); break; \
                            case light_cyan:    printf("\x1B["#foreColorCode";106m"); break; \
                            case light_white:   printf("\x1B["#foreColorCode";107m"); break; \
                        }

                switch (ForeColor)
                {
                    case black:   { _UTILS__ANSI_COLOR_CODE_BUILDER(30, BackColor); break; }
                    case red:     { _UTILS__ANSI_COLOR_CODE_BUILDER(31, BackColor); break; }
                    case green:   { _UTILS__ANSI_COLOR_CODE_BUILDER(32, BackColor); break; }
                    case yellow:  { _UTILS__ANSI_COLOR_CODE_BUILDER(33, BackColor); break; }
                    case blue:    { _UTILS__ANSI_COLOR_CODE_BUILDER(34, BackColor); break; }
                    case magenta: { _UTILS__ANSI_COLOR_CODE_BUILDER(35, BackColor); break; }
                    case cyan:    { _UTILS__ANSI_COLOR_CODE_BUILDER(36, BackColor); break; }
                    case white:   { _UTILS__ANSI_COLOR_CODE_BUILDER(37, BackColor); break; }
                    case light_black:   { _UTILS__ANSI_COLOR_CODE_BUILDER(90, BackColor); break; }
                    case light_red:     { _UTILS__ANSI_COLOR_CODE_BUILDER(91, BackColor); break; }
                    case light_green:   { _UTILS__ANSI_COLOR_CODE_BUILDER(92, BackColor); break; }
                    case light_yellow:  { _UTILS__ANSI_COLOR_CODE_BUILDER(93, BackColor); break; }
                    case light_blue:    { _UTILS__ANSI_COLOR_CODE_BUILDER(94, BackColor); break; }
                    case light_magenta: { _UTILS__ANSI_COLOR_CODE_BUILDER(95, BackColor); break; }
                    case light_cyan:    { _UTILS__ANSI_COLOR_CODE_BUILDER(96, BackColor); break; }
                    case light_white:   { _UTILS__ANSI_COLOR_CODE_BUILDER(97, BackColor); break; }
                }
#               undef _UTILS__ANSI_COLOR_CODE_BUILDER
#               endif  
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
}
