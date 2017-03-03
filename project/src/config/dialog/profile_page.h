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
#include "controls/scrollable_tab_page.h"

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

        private:
            /// @brief Close tab page control - overridable method
            virtual void overridableClose() override;


            // -- event handlers -- --------------------------------------------

            /// @brief Initialization event handler
            static EVENT_RETURN onInit(controls::TabPage::event_args_t args);
            /// @brief Notification event handler
            static EVENT_RETURN onNotify(controls::TabPage::event_args_t args);


        public:
            // -- specialized handlers -- --------------------------------------

            /// @brief Language change event
            /// @returns Validity
            virtual bool onDialogConfirm(controls::Dialog::event_args_t& args) override;
            /// @brief Language change event
            /// @param[in] isRecursive  Also translate controls in child pages or not
            virtual void onLanguageChange(const bool IsRecursive) override;
            /// @brief Profile change event
            void onProfileChange();
            /// @brief Profile save event
            /// @returns Validity
            virtual bool onProfileSave() override;

            /// @brief Transmit vertical mouse wheel event
            inline void onMouseWheel(controls::Dialog::event_args_t& args)
            {
                if (m_activeSubPage == 0 && m_subPages.size() > 0 && m_subPages.at(0).get() != nullptr)
                {
                    controls::ScrollableTabPage::onMouseWheel(controls::TabPage::event_args_t(m_subPages.at(0).get(), 
                                                              m_subPages.at(0)->getPageHandle(), args.eventData, 0));
                }
            }
        };
    }
}