/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : dialog button toolset
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
            /// @class Button
            /// @brief Dialog button toolset
            class Button
            {
            public:
                /// @enum event_t
                /// @brief Combo box event types
                enum class event_t : uint32_t
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    clicked = BN_CLICKED
                    #else
                    clicked = 1//...
                    #endif
                };


                /// @brief Set button text
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Button resource identifier
                /// @param[in] text        Text value
                static inline void setText(window_handle_t hWindow, const int32_t resourceId, const std::wstring text) noexcept
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    SetDlgItemText(reinterpret_cast<HWND>(hWindow), resourceId, (LPCWSTR)text.c_str());
                    #else
                    //...
                    #endif
                }

                /// @brief Set button state (enable / disable)
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Button resource identifier
                /// @param[in] isVisible   Visible or hidden
                static inline void setState(window_handle_t hWindow, const int32_t resourceId, bool isEnabled)
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    HWND hControl = GetDlgItem(reinterpret_cast<HWND>(hWindow), resourceId);
                    if (hControl && hControl != reinterpret_cast<HWND>(INVALID_HANDLE_VALUE))
                        EnableWindow(hControl, (isEnabled) ? TRUE : FALSE);
                    #else
                    //...
                    #endif
                }

                /// @brief Remove active button highlight
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Button resource identifier
                static inline void unHighlight(window_handle_t hWindow, const int32_t resourceId)
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    HWND hControl = GetDlgItem(reinterpret_cast<HWND>(hWindow), resourceId);
                    if (hControl && hControl != reinterpret_cast<HWND>(INVALID_HANDLE_VALUE))
                        Button_SetStyle(hControl, BS_PUSHBUTTON, FALSE);
                    #else
                    //...
                    #endif
                }
            };
        }
    }
}