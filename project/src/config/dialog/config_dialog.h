/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration dialog
*******************************************************************************/
#pragma once

#include "controls/tabbed_dialog.h"

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @class ConfigDialog
        /// @brief Configuration dialog
        /// @see TabbedDialog
        class ConfigDialog : public controls::TabbedDialog
        {
        private:
        public:
            //créer les tab_association_t et les insérer dans m_tabs avec paramètres appropriés pour boutons
        };
    }
}