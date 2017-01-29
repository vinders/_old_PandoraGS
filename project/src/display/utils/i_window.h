/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : window management - interface
*******************************************************************************/
#pragma once

#include <cstdint>
#ifdef _WINDOWS
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

/// @namespace display
/// Display management
namespace display
{
    // data types
#ifdef _WINDOWS
    typedef HDC device_handle_t;  ///< Device context handle (Windows)
    typedef HWND window_handle_t; ///< Window handle (Windows)
    typedef HMENU window_menu_t;  ///< Window menu handle (Windows)
    typedef DWORD window_style_t; ///< Window visual style (Windows)
    typedef RECT rect_t;          ///< Rectangle coordinates
#else
    typedef int32_t device_handle_t; ///< Device context handle (Linux/UNIX)
    typedef int32_t window_handle_t; ///< Window handle (Linux/UNIX)
    typedef int32_t window_menu_t;  ///< Window menu handle (Linux/UNIX)
    typedef uint32_t window_style_t; ///< Window visual style (Linux/UNIX)
    struct rect_t                    ///< Rectangle coordinates (Linux/UNIX)
    {
        int32_t left;
        int32_t top;
        int32_t right;
        int32_t bottom;
    };
#endif

    /// @struct coord_t
    /// @brief Coordinates
    struct coord_t
    {
        uint32_t x; ///< Horizontal coordinate
        uint32_t y; ///< Vertical coordinate
    };

    /// @enum window_color_mode_t
    /// @brief Window color mode
    enum class window_color_mode_t : bool
    {
        rgb_16bit = false, ///< 16-bit RGB color mode
        rgb_32bit = true   ///< 32-bit RGB color mode
    };
        
        
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
            /// @param width Window width
            /// @param height Window height
            /// @param flag Special flag
            virtual void show(uint32_t width, uint32_t height, int32_t flag) = 0;

            /// @brief Hide existing window
            virtual void hide() = 0;
        };
    }
}
