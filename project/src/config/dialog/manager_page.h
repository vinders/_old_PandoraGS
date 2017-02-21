/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab page - profile manager
*******************************************************************************/
#pragma once

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
            static DIALOG_EVENT_RETURN onInit(PAGE_EVENT_HANDLER_ARGUMENTS);
            /// @brief Sub-control command event handler
            static DIALOG_EVENT_RETURN onCommand(PAGE_EVENT_HANDLER_ARGUMENTS);
            /// @brief Notification event handler
            static DIALOG_EVENT_RETURN onNotify(PAGE_EVENT_HANDLER_ARGUMENTS);


            // -- specialized handlers -- --------------------------------------

            /// @brief Profile removal event
            void onProfileRemoval(controls::window_handle_t hWindow);

            /// @brief Language change event
            /// @returns Validity
            virtual bool onDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS) override;
            /// @brief Language change event
            /// @param[in] IsUpdate  Set to false to initialize controls
            virtual void onLanguageChange(const bool IsUpdate) override;
        };
    }
}