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

            /// @struct paint_event_args_t
            /// @brief Drawing event arguments
            struct paint_event_args_t
            {
                HWND window;
                HDC context;
                uint32_t paintedResourceId;
            };

            #else
            typedef int32_t library_instance_t; ///< Library instance
            typedef int32_t window_handle_t; ///< Window handle

            // @struct paint_event_args_t
            /// @brief Drawing event arguments
            struct paint_event_args_t
            {
                int32_t window;
                int32_t context;
                uint32_t paintedResourceId;
            };
            #endif

            /// @struct paint_control_event_args_t
            /// @brief Control drawing event arguments
            struct paint_control_event_args_t : public paint_event_args_t
            {
                bool isSelected;
                bool isHover;
            };
        }
    }
}