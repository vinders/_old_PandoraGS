/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab page - profile settings
*******************************************************************************/
#pragma once

#include <cstdint>
#include <vector>
#include <memory>
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
            int32_t m_activeSubPage;
            std::vector<std::shared_ptr<controls::TabPage>> m_subPages;
        
        public:
            /// @brief Create tab page - profile settings
            /// @param[in] instance       Current instance handle
            /// @param[in] pParentDialog  Parent dialog reference
            ProfilePage(controls::library_instance_t instance, controls::Dialog* pParentDialog);
            /// @brief Destroy tab page - profile settings
            ~ProfilePage() {}

            /// @brief Close tab page control - overridable method
            virtual void overridableClose() override;


            // -- event handlers -- --------------------------------------------

            /// @brief Initialization event handler
            static DIALOG_EVENT_RETURN onInit(PAGE_EVENT_HANDLER_ARGUMENTS);
            /// @brief Notification event handler
            static DIALOG_EVENT_RETURN onNotify(PAGE_EVENT_HANDLER_ARGUMENTS);


            // -- specialized handlers -- --------------------------------------

            /// @brief Language change event
            /// @returns Validity
            virtual bool onDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS) override;
            /// @brief Language change event
            /// @param[in] isRecursive  Also translate controls in child pages or not
            virtual void onLanguageChange(const bool IsRecursive) override;
        };
    }
}