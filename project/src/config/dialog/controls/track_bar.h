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
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Track-bar resource identifier
                /// @returns Current value (or -1)
                static uint32_t getValue(window_handle_t hWindow, const int32_t resourceId);
            };
        }
    }
}