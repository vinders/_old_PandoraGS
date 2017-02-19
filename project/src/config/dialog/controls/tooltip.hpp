/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : control tooltip
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include <string>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
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
            /// @class Tooltip
            /// @brief Control tooltip
            class Tooltip
            {
            private:
                window_handle_t m_hParent;  ///< Parent window handle
                window_handle_t m_hTooltip; ///< Tooltip handle
                bool m_isInitialized; ///< Status

            public:
                /// @brief Create tooltip
                /// @param[in] instance    Current instance handle
                /// @param[in] hWindow     Parent window handle
                Tooltip(library_instance_t instance, window_handle_t hWindow) : m_hParent(hWindow), m_isInitialized(false)
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    // tooltip creation
                    m_hTooltip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP,
                                                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
                                                reinterpret_cast<HWND>(hWindow), NULL, reinterpret_cast<HINSTANCE>(instance), NULL);
                    #else
                    //...
                    #endif
                }

                /// @brief Destroy tooltip
                ~Tooltip()
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    if (m_hTooltip)
                        DestroyWindow(m_hTooltip);
                    #endif
                }


                /// @brief Set tooltip text
                /// @param[in] text        Text value
                /// @param[in] resourceId  Associated resource identifier
                inline void setText(const int32_t resourceId, const std::wstring text)
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    if (m_hTooltip)
                    {
                        HWND hItem = GetDlgItem(m_hParent, resourceId);
                        if (hItem)
                        {
                            TOOLINFO ti = { 0 };
                            ti.cbSize = sizeof(ti);
                            ti.hwnd = reinterpret_cast<HWND>(m_hParent);
                            ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
                            ti.uId = (UINT_PTR)hItem;
                            ti.lpszText = (LPWSTR)text.c_str();
                            SendMessage(m_hTooltip, (m_isInitialized) ? TTM_SETTOOLINFO : TTM_ADDTOOL, 0, (LPARAM)&ti);
                            m_isInitialized = true;
                        }
                    }
                    #else
                    //...
                    #endif
                }
            };
        }
    }
}