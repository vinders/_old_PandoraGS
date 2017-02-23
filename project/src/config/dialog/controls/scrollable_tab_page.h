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
            private:
                uint32_t m_maxRange;

            public:
                /// @brief Create tab page - profile compatibility settings
                /// @param[in] instance       Current instance handle
                /// @param[in] pParentDialog  Parent dialog reference
                /// @param[in] resourceId     Tab page description identifier
                /// @param[in] maxRange       Max scroll range
                ScrollableTabPage(controls::library_instance_t instance, controls::Dialog* pParentDialog, const int32_t resourceId, const uint32_t maxRange);
                // @brief Destroy tab page - profile compatibility settings
                virtual ~ScrollableTabPage() {}


                // -- event handlers -- --------------------------------------------

                /// @brief Initialization event handler
                static DIALOG_EVENT_RETURN onInit(PAGE_EVENT_HANDLER_ARGUMENTS);
                /// @brief Initialization extendable event handler
                virtual DIALOG_EVENT_RETURN onInitOverridable(PAGE_EVENT_HANDLER_ARGUMENTS) = 0;

                /// @brief Vertical scroll event handler
                static DIALOG_EVENT_RETURN onVerticalScroll(PAGE_EVENT_HANDLER_ARGUMENTS);
            };
        }
    }
}