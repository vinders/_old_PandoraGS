/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab page - profile manager
*******************************************************************************/
#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <stdexcept>
#include "controls/common.h"
#include "controls/button_icon.h"
#include "controls/tab_page.h"
#include "controls/tab_control.h"
#include "controls/tooltip.hpp"
#include "controls/data_table.h"

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @class ManagerPage
        /// @brief Tab page - profile manager
        /// @see TabPage
        class ManagerPage : public controls::TabPage
        {
        private:
            std::vector<std::shared_ptr<controls::ButtonIcon>> m_buttonIcons;  ///< Icons for side buttons
            std::vector<std::shared_ptr<controls::Tooltip>> m_tooltips; ///< Tooltips for side buttons
            controls::DataTable* m_pDataTable; ///< Data table (profiles)
            static ManagerPage* s_pLastParentPage; ///< Page reference buffer (for subdialogs)

        
        public:
            /// @brief Create tab page - profile manager
            /// @param[in] instance       Current instance handle
            /// @param[in] pParentDialog  Parent dialog reference
            ManagerPage(controls::library_instance_t instance, controls::Dialog* pParentDialog);
            /// @brief Destroy tab page - profile manager
            ~ManagerPage() {}

            /// @brief Get reference to data table
            /// @returns Data table reference
            /// @throws invalid_argument  Uninitialized data table
            inline controls::DataTable& getDataTable()
            {
                if (m_pDataTable == nullptr)
                    throw std::invalid_argument("The data table is not initialized");
                return *m_pDataTable;
            }

            /// @brief Close tab page control - overridable method
            virtual void overridableClose() override;


            // -- event handlers -- --------------------------------------------

            /// @brief Initialization event handler
            static EVENT_RETURN onInit(controls::TabPage::event_args_t args);
            /// @brief Sub-control command event handler
            static EVENT_RETURN onCommand(controls::TabPage::event_args_t args);
            /// @brief Notification event handler
            static EVENT_RETURN onNotify(controls::TabPage::event_args_t args);


            // -- specialized handlers -- --------------------------------------

            /// @brief Profile edit event - open dialog
            void onProfileEdit(controls::window_handle_t hWindow);
            /// @brief Profile removal event
            void onProfileRemoval(controls::window_handle_t hWindow);
            /// @brief Profile export event - open dialog
            void onProfileExport(controls::window_handle_t hWindow);
            /// @brief Profile preset choice event
            void onProfilePresetChoice(controls::window_handle_t hWindow);

            /// @brief Language change event
            /// @returns Validity
            virtual bool onDialogConfirm(controls::Dialog::event_args_t& args) override;
            /// @brief Language change event
            /// @param[in] IsUpdate  Set to false to initialize controls
            virtual void onLanguageChange(const bool IsUpdate) override;


            // -- sub-dialogs handlers -- --------------------------------------

            /// @brief Add profile dialog - Initialization event handler
            static EVENT_RETURN onAddProfileDialogInit(controls::Dialog::event_args_t args);
            /// @brief Add profile dialog - Confirm event handler
            static EVENT_RETURN onAddProfileDialogConfirm(controls::Dialog::event_args_t args);

            /// @brief Edit profile dialog - Initialization event handler
            static EVENT_RETURN onEditProfileDialogInit(controls::Dialog::event_args_t args);
            /// @brief Edit profile dialog - Confirm event handler
            static EVENT_RETURN onEditProfileDialogConfirm(controls::Dialog::event_args_t args);

            /// @brief Import profile dialog - Initialization event handler
            static EVENT_RETURN onImportProfileDialogInit(controls::Dialog::event_args_t args);
            /// @brief Import profile dialog - Confirm event handler
            static EVENT_RETURN onImportProfileDialogConfirm(controls::Dialog::event_args_t args);

            /// @brief Export profile dialog - Initialization event handler
            static EVENT_RETURN onExportProfileDialogInit(controls::Dialog::event_args_t args);
            /// @brief Export profile dialog - Confirm event handler
            static EVENT_RETURN onExportProfileDialogConfirm(controls::Dialog::event_args_t args);
        };
    }
}