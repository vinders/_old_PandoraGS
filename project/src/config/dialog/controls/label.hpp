/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : dialog label toolset
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include <string>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#endif
#include "common.h"

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
            /// @class Label
            /// @brief Dialog label toolset
            class Label
            {
            public:
                /// @brief Set label text
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Label resource identifier
                /// @param[in] text        Text value
                static inline void setText(window_handle_t hWindow, const int32_t resourceId, const std::wstring text) noexcept
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    SetDlgItemText(hWindow, resourceId, (LPCWSTR)text.c_str());
                    #else
                    //...
                    #endif
                }

                /// @brief Set label visibility
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Label resource identifier
                /// @param[in] isVisible   Visible or hidden
                static inline void setVisible(window_handle_t hWindow, const int32_t resourceId, bool isVisible)
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    HWND hControl = GetDlgItem(hWindow, resourceId);
                    if (hControl)
                        ShowWindow(hControl, (isVisible) ? SW_SHOW : SW_HIDE);
                    #else
                    //...
                    #endif
                }
            };
        }
    }
}