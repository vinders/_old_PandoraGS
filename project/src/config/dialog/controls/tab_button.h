/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab button for tab control
*******************************************************************************/
#pragma once

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
            /// @class TabButton
            /// @brief Tab button for tab control
            class TabButton
            {
            private:
                std::string& m_title;   ///< Button title text (reference)
                uint32_t m_iconIndex; ///< Button icon index

            public:
                /// @brief Create tab button
                /// @param[in] title  Tab name
                /// @param[in] icon   Icon index
                TabButton(std::string& title, const uint32_t icon) : m_title(title), m_iconIndex(icon) {}

                /// @brief Trigger control drawing
                /// @param[in] args  Event arguments
                void paint(paint_control_event_args_t& args);
            };
        }
    }
}