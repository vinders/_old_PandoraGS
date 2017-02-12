/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : page for tab control
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include "common.h"

#if _DIALOGAPI == DIALOGAPI_WIN32
#define PAGE_EVENT_HANDLER_ARGUMENTS TabPage* pPage, HWND hWindow, WPARAM wParam, LPARAM lParam
#define getEventTargetPageReference(TYPE) *(static_cast<TYPE*>(pPage))
#else
#define PAGE_EVENT_HANDLER_ARGUMENTS void* pPage
#define getEventTargetPageReference(TYPE) *((TYPE*)pPage)
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
            /// @class TabPage
            /// @brief Page for tab control
            class TabPage
            {
                
            };
        }
    }
}