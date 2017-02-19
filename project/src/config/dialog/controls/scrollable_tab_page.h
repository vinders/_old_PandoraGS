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
#include "tab_page.h"

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
            public:
                /// @brief Create tab page - profile compatibility settings
                /// @param[in] instance       Current instance handle
                /// @param[in] pParentDialog  Parent dialog reference
                /// @param[in] resourceId     Tab page description identifier
                ScrollableTabPage(controls::library_instance_t instance, controls::Dialog* pParentDialog, const int32_t resourceId)
                    : controls::TabPage(instance, pParentDialog, resourceId) {}
            };
        }
    }
}