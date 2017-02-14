/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : text field toolset
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
            /// @class TextField
            /// @brief Text field toolset
            class TextField
            {
            public:
                /// @brief Set text field value
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Label resource identifier
                /// @param[in] text        Text value
                static inline void setValue(window_handle_t hWindow, const int32_t resourceId, const std::wstring text) noexcept
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    SetDlgItemText(reinterpret_cast<HWND>(hWindow), resourceId, (LPCWSTR)text.c_str());
                    #else
                    //...
                    #endif
                }

                /// @brief Get text field value
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Check box resource identifier
                /// @returns Text value
                static inline std::wstring getValue(window_handle_t hWindow, const int32_t resourceId)
                {
                    wchar_t pBuffer[256];
                    memset(pBuffer, 0x0, sizeof(pBuffer));
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    GetDlgItemText(reinterpret_cast<HWND>(hWindow), resourceId, (LPWSTR)pBuffer, sizeof(pBuffer) - 1);
                    #else
                    //...
                    #endif
                    return std::wstring(pBuffer);
                }
            };
        }
    }
}