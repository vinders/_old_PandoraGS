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

            };
        }
    }
}