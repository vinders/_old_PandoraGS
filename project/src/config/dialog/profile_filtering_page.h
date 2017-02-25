/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab sub-page - filtering settings
*******************************************************************************/
#pragma once

#include <cstdint>
#include <string>
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
            ProfileFilteringPage(controls::library_instance_t instance, controls::Dialog* pParentDialog);


            /// @brief Close tab page control - overridable method
            virtual void overridableClose() {}


            // -- event handlers -- --------------------------------------------

            /// @brief Initialization extendable event handler
            virtual EVENT_RETURN onInitOverridable(controls::TabPage::event_args_t& args);
            /// @brief Sub-control command event handler
            static EVENT_RETURN onCommand(TabPage::event_args_t args);


            // -- specialized handlers -- --------------------------------------

            /// @brief Language change event
            /// @returns Validity
            virtual bool onDialogConfirm(controls::Dialog::event_args_t& args);
            /// @brief Language change event
            /// @param[in] isUpdate  Set to false to initialize controls
            virtual void onLanguageChange(const bool isUpdate);
            /// @brief Upscaling factor change event
            /// @param[in] listResourceId  Combo-box resource identifier
            /// @param[in] factor          Selected factor
            /// @param[in] selectedScaler  Index to select in list of upscalers
            /// @param[in] nativeLabel     Label for native entry
            void onUpscalingFactorChange(const int32_t listResourceId, const uint32_t factor, const config::upscaling_mode_t selectedScaler, const std::wstring& nativeLabel);

            /// @brief Profile save event
            /// @returns Validity
            bool onProfileSave();
            /// @brief Profile change event
            /// @param[in] isUpdate  Set to false to initialize controls
            void onProfileChange(const bool isUpdate);
        };
    }
}