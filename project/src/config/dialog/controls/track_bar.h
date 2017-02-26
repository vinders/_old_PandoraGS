/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : track-bar toolset
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <commctrl.h>
#endif
#include "tab_page.h"
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
            /// @class TrackBar
            /// @brief Track-bar toolset
            class TrackBar
            {
            public:
                /// @enum event_t
                /// @biref Track-bar events
                enum class event_t : int32_t
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    endMove = TB_ENDTRACK
                    #else
                    endMove = 0
                    #endif
                };

                /// @brief Get track-bar control ID
                /// @param[in] notifierData  Event notification data
                /// @returns ID of track-bar associated with event
                static inline int32_t getControlId(TabPage::event_args_t args)
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    return GetDlgCtrlID((HWND)args.notifierData);
                    #else
                    //...
                    return 0;
                    #endif
                }

                /// @brief Initialize track-bar control
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Track-bar resource identifier
                /// @param[in] maxValue    Max value (range = 0 to max)
                /// @param[in] unitSize    Minimum step unit
                /// @param[in] pageSize    Page-step unit
                /// @returns Success
                static bool initControl(window_handle_t hWindow, const int32_t resourceId, const uint32_t maxValue, const uint32_t unitSize, const uint32_t pageSize);

                /// @brief Initialize track-bar control
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Track-bar resource identifier
                /// @param[in] value       Value to set
                static void setValue(window_handle_t hWindow, const int32_t resourceId, const uint32_t value);
                /// @brief Initialize track-bar control
                /// @param[in]  hWindow     Parent window handle
                /// @param[in]  resourceId  Track-bar resource identifier
                /// @param[out] outValue    Output value
                /// @returns Success
                static bool getValue(window_handle_t hWindow, const int32_t resourceId, int32_t& outValue);
            };
        }
    }
}