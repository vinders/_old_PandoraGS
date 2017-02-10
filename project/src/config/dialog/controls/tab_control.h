/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : advanced tab control
*******************************************************************************/
#pragma once

#include <vector>
#include <memory>
#include "tab_button.h"
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
            /// @struct tab_association_t
            /// @brief Tab button/page association
            struct tab_association_t
            {
                TabButton button;
                std::shared_ptr<TabPage> page;
            };
            
            
            /// @class TabControl
            /// @brief Advanced tab control
            class TabControl
            {
            protected:
                std::vector<tab_association_t> m_pages;
                
            public:
				TabControl() {}
                
                //void clear();
                //void addTab(tab_association_t& tabData);
                //void removeTab(uint32_t pos);
            };
        }
    }
}