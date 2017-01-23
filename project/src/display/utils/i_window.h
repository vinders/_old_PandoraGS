/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : window management - interface
*******************************************************************************/
#pragma once

#ifdef _WINDOWS
#include <Windows.h>
#endif

/// @namespace display
/// Display management
namespace display
{
    // data types
#ifdef _WINDOWS
    typedef HWND window_handle_t; ///< window handle (Windows)
#else
    typedef int window_handle_t; ///< window handle (Linux/UNIX)
#endif
        
        
    /// @namespace display.utils
    /// Display management utilities
    namespace utils
    {
        /// @class IWindow
        /// @brief Window management - interface
        class IWindow
        {
        public:
            virtual ~IWindow() {}
            
            /// @brief Display window on screen
            virtual void show() = 0;
            /// @brief Hide existing window
            virtual void hide() = 0;
            /// @brief Clear window content
            virtual void clear() = 0;
        };
    }
}
