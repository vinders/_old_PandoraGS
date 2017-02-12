/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : message box toolset
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include <string>
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
            /// @class MsgBox
            /// @brief Track-bar toolset
            class MsgBox
            {
            public:
                /// @enum event_t
                /// @brief Combo box event types
                enum class message_icon_t : int32_t
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    none = 0,
                    info = MB_ICONINFORMATION,
                    question = MB_ICONQUESTION,
                    warning = MB_ICONWARNING,
                    error = MB_ICONERROR,
                    stop = MB_ICONSTOP
                    #else
                    none = 0,
                    info = 1,
                    question = 2,
                    warning = 3,
                    error = 4,
                    stop = 5
                    #endif
                };
                /// @enum event_t
                /// @brief Combo box event types
                enum class button_set_t : int32_t
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    ok = MB_OK,
                    okCancel = MB_OKCANCEL,
                    retryCancel = MB_RETRYCANCEL,
                    yesNo = MB_YESNO,
                    yesNoCancel = MB_YESNOCANCEL,
                    #else
                    ok = 0,
                    okCancel = 1,
                    retryCancel = 2,
                    yesNo = 3,
                    yesNoCancel = 4,
                    #endif
                };

                /// @brief Show message box
                /// @param[in] title    Message box title
                /// @param[in] message  Displayed message
                /// @param[in] hWindow  Parent window (use nullptr for non-modal message box)
                /// @param[in] buttons  Message box buttons (optional)
                /// @param[in] icon     Message box icon (optional)
                static void showMessage(const std::wstring& title, const std::wstring& message, window_handle_t hWindow,
                                        const button_set_t buttons = button_set_t::ok, const message_icon_t icon = message_icon_t::none);
            };
        }
    }
}