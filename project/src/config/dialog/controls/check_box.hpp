/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : check box / radio button toolset
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
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
            /// @class CheckBox
            /// @brief Check box / radio button toolset
            class CheckBox
            {
            public:
                /// @enum event_t
                /// @brief Check box event types
                enum class event_t : uint32_t
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    clicked = BN_CLICKED
                    #else
                    clicked = 1//...
                    #endif
                };


                /// @brief Set check box text
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Check box resource identifier
                /// @param[in] text        Text value
                static inline void setText(window_handle_t hWindow, const int32_t resourceId, const std::wstring text) noexcept
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    SetDlgItemText(reinterpret_cast<HWND>(hWindow), resourceId, (LPCWSTR)text.c_str());
                    #else
                    //...
                    #endif
                }

                /// @brief Set check box state
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Check box resource identifier
                /// @param[in] isChecked   Checked or not
                static inline void setChecked(window_handle_t hWindow, const int32_t resourceId, bool isChecked)
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    CheckDlgButton(reinterpret_cast<HWND>(hWindow), resourceId, (isChecked) ? BST_CHECKED : BST_UNCHECKED);
                    #else
                    //...
                    #endif
                }

                /// @brief Set check box state
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Check box resource identifier
                /// @returns Checked or not
                static inline bool isChecked(window_handle_t hWindow, const int32_t resourceId)
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    return (IsDlgButtonChecked(reinterpret_cast<HWND>(hWindow), resourceId) == BST_CHECKED);
                    #else
                    //...
                    #endif
                }
            };
        }
    }
}