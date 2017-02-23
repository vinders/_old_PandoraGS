/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab sub-page - filtering settings
*******************************************************************************/
#pragma once

#include "controls/common.h"
#include "controls/scrollable_tab_page.h"

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @class ProfileFilteringPage
        /// @brief Tab sub-page - filtering settings
        /// @see ScrollableTabPage
        class ProfileFilteringPage : public controls::ScrollableTabPage
        {
        private:
        
        public:
            /// @brief Create tab page - profile compatibility settings
            /// @param[in] instance       Current instance handle
            /// @param[in] pParentDialog  Parent dialog reference
            ProfileFilteringPage(controls::library_instance_t instance, controls::Dialog* pParentDialog)
                : controls::ScrollableTabPage(instance, pParentDialog, IDD_PROFILE_FILTERS_TAB, 600) {}


            /// @brief Close tab page control - overridable method
            virtual void overridableClose() {}


            // -- event handlers -- --------------------------------------------

            /// @brief Initialization extendable event handler
            virtual DIALOG_EVENT_RETURN onInitOverridable(PAGE_EVENT_HANDLER_ARGUMENTS) { return TRUE; }


            // -- specialized handlers -- --------------------------------------

            /// @brief Language change event
            /// @returns Validity
            virtual bool onDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS) { return true; };
            /// @brief Language change event
            /// @param[in] isRecursive    Also translate controls in child pages or not
            virtual void onLanguageChange(const bool isRecursive) {};
        };
    }
}