/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab sub-page - filtering settings
*******************************************************************************/
#pragma once

#include "controls/scrollable_tab_page.h"

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @class FilteringSubPage
        /// @brief Tab sub-page - filtering settings
        /// @see ScrollableTabPage
        class FilteringSubPage : public controls::ScrollableTabPage
        {
        private:
        
        public:
        };
    }
}