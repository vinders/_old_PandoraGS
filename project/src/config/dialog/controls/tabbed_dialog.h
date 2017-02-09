/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : dialog control with tabs
*******************************************************************************/
#pragma once

#include "dialog.h"
#include "tab_control.h"

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
            /// @class TabbedDialog
            /// @brief Dialog control with tabs
            /// @see Dialog
            class TabbedDialog : public Dialog
            {
            protected:
                TabControl m_tabs;
                
            public:
            };
        }
    }
}