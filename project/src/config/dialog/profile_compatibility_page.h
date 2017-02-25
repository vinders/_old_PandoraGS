/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab sub-page - compatibility settings
*******************************************************************************/
#pragma once

#include "controls/common.h"
#include "controls/tab_page.h"

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @class ProfileCompatibilityPage
        /// @brief Tab sub-page - compatibility settings
        /// @see TabPage
        class ProfileCompatibilityPage : public controls::TabPage
        {
        private:
        
        public:
            /// @brief Create tab page - profile compatibility settings
            /// @param[in] instance       Current instance handle
            /// @param[in] pParentDialog  Parent dialog reference
            ProfileCompatibilityPage(controls::library_instance_t instance, controls::Dialog* pParentDialog)
                : controls::TabPage(instance, pParentDialog, IDD_PROFILE_COMPAT_TAB) {}


            /// @brief Close tab page control - overridable method
            virtual void overridableClose() {}


            // -- specialized handlers -- --------------------------------------

            /// @brief Language change event
            /// @returns Validity
            virtual bool onDialogConfirm(controls::Dialog::event_args_t& args) { return true; };
            /// @brief Language change event
            /// @param[in] isRecursive    Also translate controls in child pages or not
            virtual void onLanguageChange(const bool isRecursive) {};
        };
    }
}