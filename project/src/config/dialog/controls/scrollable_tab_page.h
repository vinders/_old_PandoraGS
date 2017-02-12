/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : scrollable page for tab control
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
            /// @class ScrollableTabPage
            /// @brief Scrollable page for tab control
            class ScrollableTabPage : public TabPage
            {
                
            };
        }
    }
}