/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab page - general settings
*******************************************************************************/
#pragma once

#include <vector>
#include "controls/common.h"
#include "controls/tab_page.h"
#include "controls/tab_control.h"
#include "controls/screen.h"

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @class GeneralPage
        /// @brief Tab page - general settings
        /// @see TabPage
        class GeneralPage : public controls::TabPage
        {
        private:
            static std::vector<controls::resolution_t> m_resolutions; ///< Available resolutions

        
        public:
            /// @brief Create tab page - general settings
            /// @param[in] instance       Current instance handle
            /// @param[in] pParentDialog  Parent dialog reference
            GeneralPage(config::dialog::controls::library_instance_t instance, controls::Dialog* pParentDialog);
            /// @brief Destroy tab page - general settings
            ~GeneralPage() {}

            /// @brief Get list of available resolutions
            /// @returns List of resolution
            static inline std::vector<config::dialog::controls::resolution_t>& getResolutions()
            {
                return m_resolutions;
            }

            /// @brief Close tab page control - overridable method
            virtual void overridableClose() override;


            // -- event handlers -- --------------------------------------------

            /// @brief Initialization event handler
            static EVENT_RETURN onInit(controls::TabPage::event_args_t args);
            /// @brief Sub-control command event handler
            static EVENT_RETURN onCommand(controls::TabPage::event_args_t args);


            // -- specialized handlers -- --------------------------------------

            /// @brief Language change event
            /// @returns Validity
            virtual bool onDialogConfirm(controls::Dialog::event_args_t& args) override;
            /// @brief Language change event
            /// @param[in] IsUpdate  Set to false to initialize controls
            virtual void onLanguageChange(const bool isRecursive) override;

        private:
            /// @brief Initialize key binding sub-dialog
            static EVENT_RETURN onKeyBindDialogInit(controls::Dialog::event_args_t args);
            /// @brief Confirm key binding sub-dialog settings
            static EVENT_RETURN onKeyBindDialogConfirm(controls::Dialog::event_args_t args);
            /// @brief Initialize advanced settings sub-dialog
            static EVENT_RETURN onAdvancedDialogInit(controls::Dialog::event_args_t args);
            /// @brief Confirm advanced settings sub-dialog settings
            static EVENT_RETURN onAdvancedDialogConfirm(controls::Dialog::event_args_t args);
        };
    }
}