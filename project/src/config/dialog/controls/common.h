/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : dialog control
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include <functional>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#endif

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @namespace config.dialog.controls
        /// Dialog controls
        namespace controls
        {
            /// @enum dialog_event_t
            /// @brief Events managed by dialog control
            enum class dialog_event_t : uint32_t
            {
                close = 0u,
                confirm = 1u,
                init = 2u,
                paint = 3u,
                drawItem = 4u,
                command = 5u,
                scrollX = 6u,
                scrollY = 7u
            };

            #if _DIALOGAPI == DIALOGAPI_WIN32
            typedef HINSTANCE library_instance_t; ///< Library instance
            typedef HWND window_handle_t; ///< Window handle
            typedef COLORREF color_ref_t; ///< Color reference
            #else
            typedef int32_t library_instance_t; ///< Library instance
            typedef int32_t window_handle_t; ///< Window handle
            typedef uint32_t color_ref_t; ///< Color reference
            #endif
        }
    }
}