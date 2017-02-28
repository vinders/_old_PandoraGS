/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab sub-page - screen settings
*******************************************************************************/
#pragma once

#include <cstdint>
#include <memory>
#include "controls/common.h"
#include "controls/preview_box.h"
#include "controls/tab_page.h"

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @class ProfileScreenPage
        /// @brief Tab sub-page - screen settings
        /// @see TabPage
        class ProfileScreenPage : public controls::TabPage
        {
        private:
            std::shared_ptr<controls::PreviewBox> m_screenPreview;
        
        public:
            /// @brief Create tab page - profile compatibility settings
            /// @param[in] instance       Current instance handle
            /// @param[in] pParentDialog  Parent dialog reference
            ProfileScreenPage(controls::library_instance_t instance, controls::Dialog* pParentDialog);


            /// @brief Close tab page control - overridable method
            virtual void overridableClose() {}


            // -- event handlers -- --------------------------------------------

            /// @brief Initialization event handler
            static EVENT_RETURN onInit(TabPage::event_args_t args);
            /// @brief Special drawing event handler
            static EVENT_RETURN onDrawItem(TabPage::event_args_t args);
            /// @brief Sub-control command event handler
            static EVENT_RETURN onCommand(TabPage::event_args_t args);
            /// @brief Track-bar event handler
            static EVENT_RETURN onTrackBarChange(TabPage::event_args_t args);


            // -- specialized handlers -- --------------------------------------

            /// @brief Language change event
            /// @returns Validity
            virtual bool onDialogConfirm(controls::Dialog::event_args_t& args);
            /// @brief Language change event
            /// @param[in] isUpdate  Set to false to initialize controls
            virtual void onLanguageChange(const bool isUpdate);

            /// @brief Profile save event
            /// @returns Validity
            bool onProfileSave();
            /// @brief Profile change event
            /// @param[in] isUpdate  Set to false to initialize controls
            virtual void onProfileChange(const bool isUpdate) override;
        };
    }
}