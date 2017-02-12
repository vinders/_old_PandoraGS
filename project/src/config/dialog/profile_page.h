/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab page - profile settings
*******************************************************************************/
#pragma once

#include <array>
#include "../config.h"
#include "controls/common.h"
#include "controls/tab_page.h"
#include "controls/tab_control.h"

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @class ProfilePage
        /// @brief Tab page - profile settings
        /// @see TabPage
        class ProfilePage : public controls::TabPage
        {
        private:
            std::array<ProfilePage,3> m_subPages;
        
        public:    
        };
    }
}