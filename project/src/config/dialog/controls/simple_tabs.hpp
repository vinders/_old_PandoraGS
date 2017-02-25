/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : simple tabs toolset - standard system tabs
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include <string>
#include <vector>
#include "common.h"
using namespace std::literals::string_literals;

#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#define isTabEventAction(NOTIFIERDATA, EVENTTYPE) static_cast<int32_t>(((LPNMHDR)NOTIFIERDATA)->code) == static_cast<int32_t>(EVENTTYPE)
#define tabControlId(NOTIFIERDATA) (((LPNMHDR)NOTIFIERDATA)->hwndFrom)
#else
#define isTabEventAction(DATA, EVENTTYPE) ((DATA>>8)&0x0FF)==EVENTTYPE
#define tabControlId(DATA) (DATA&0x0FF)
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
            /// @class SimpleTabs
            /// @brief Simple tabs toolset - standard system tabs
            class SimpleTabs
            {
            public:
                /// @enum event_t
                /// @brief Simple tabs event types
                enum class event_t : uint32_t
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    tabChanged = TCN_SELCHANGE
                    #else
                    tabChanged = 1//...
                    #endif
                };

                /// @brief Initialize list of tabs
                /// @param[in] hWindow        Parent window handle
                /// @param[in] resourceId     Simple tab control resource identifier
                /// @param[in] tabLabels      Names of tabs
                /// @param[in] selectedIndex  Default index to select
                /// @returns Success
                static inline bool insertTabs(window_handle_t hWindow, const int32_t resourceId, const std::vector<std::wstring>& tabLabels, int32_t selectedIndex)
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    HWND hTabControl = GetDlgItem(hWindow, resourceId);
                    if (!hTabControl || hTabControl == (HWND)INVALID_HANDLE_VALUE)
                        return false;

                    // insert tabs
                    TCITEM ti;
                    ti.mask = TCIF_TEXT;
                    for (uint32_t i = 0; i < tabLabels.size(); ++i)
                    {
                        ti.pszText = (LPWSTR)tabLabels.at(i).c_str();
                        TabCtrl_InsertItem(hTabControl, i, &ti);
                    }

                    // select active tab
                    if (selectedIndex < 0 || selectedIndex > static_cast<int32_t>(tabLabels.size()))
                        selectedIndex = 0;
                    TabCtrl_SetCurSel(hTabControl, selectedIndex);
                    #else
                    //...
                    #endif
                    return true;
                }

                /// @brief Rename tabs
                /// @param[in] hWindow        Parent window handle
                /// @param[in] resourceId     Simple tab control resource identifier
                /// @param[in] tabLabels      Names of tabs
                static inline void setTabTitles(window_handle_t hWindow, const int32_t resourceId, const std::vector<std::wstring>& tabLabels)
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    HWND hTabControl = GetDlgItem(hWindow, resourceId);
                    if (!hTabControl || hTabControl == (HWND)INVALID_HANDLE_VALUE)
                        return;

                    // rename tabs
                    TCITEM ti;
                    ti.mask = TCIF_TEXT;
                    for (uint32_t i = 0; i < tabLabels.size(); ++i)
                    {
                        ti.pszText = (LPWSTR)tabLabels.at(i).c_str();
                        TabCtrl_SetItem(hTabControl, i, &ti);
                    }
                    #else
                    //...
                    #endif
                }

                /// @brief Get index of selected tab
                /// @param[in] hTabControl  Simple tab control handle
                /// @returns Selected index
                #if _DIALOGAPI == DIALOGAPI_WIN32
                static inline int32_t getActiveTabIndex(HWND hTabControl)
                {
                    int32_t curTab = TabCtrl_GetCurSel(hTabControl); // set new tab
                    if (curTab == -1)
                    {
                        curTab = 0;
                        TabCtrl_SetCurSel(hTabControl, 0);
                    }
                    return curTab;
                }
                #else
                static int32_t getActiveTabIndex(uint32_t hTabControl)
                {
                    //...
                    return 0;
                }
                #endif
            };
        }
    }
}