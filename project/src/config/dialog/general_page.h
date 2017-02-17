/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab page - general settings
*******************************************************************************/
#pragma once

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
        /// @class GeneralPage
        /// @brief Tab page - general settings
        /// @see TabPage
        class GeneralPage : public controls::TabPage
        {
        private:
        
        public:
            /// @brief Create tab page - general settings
            /// @param[in] instance       Current instance handle
            /// @param[in] pParentDialog  Parent dialog reference
            GeneralPage(controls::library_instance_t instance, controls::Dialog* pParentDialog);


            // -- event handlers -- --------------------------------------------

            /// @brief Initialization event handler
            static DIALOG_EVENT_RETURN onInit(PAGE_EVENT_HANDLER_ARGUMENTS);
            /// @brief Sub-control command event handler
            static DIALOG_EVENT_RETURN onCommand(PAGE_EVENT_HANDLER_ARGUMENTS);


            // -- specialized handlers -- --------------------------------------

            /// @brief Language change event
            /// @returns Validity
            virtual bool onDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS);
            /// @brief Language change event
            /// @param[in] isRecursive    Also translate controls in child pages or not
            virtual void onLanguageChange(const bool isRecursive);

        private:
            /// @brief Initialize key binding sub-dialog
            static DIALOG_EVENT_RETURN onKeyBindDialogInit(DIALOG_EVENT_HANDLER_ARGUMENTS);
            /// @brief Confirm key binding sub-dialog settings
            static DIALOG_EVENT_RETURN onKeyBindDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS);
            /// @brief Initialize advanced settings sub-dialog
            static DIALOG_EVENT_RETURN onAdvancedDialogInit(DIALOG_EVENT_HANDLER_ARGUMENTS);
            /// @brief Confirm advanced settings sub-dialog settings
            static DIALOG_EVENT_RETURN onAdvancedDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS);
        };
    }
}