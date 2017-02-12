/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration dialog
*******************************************************************************/
#pragma once

#include <stack>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#endif
#include "../config.h"
#include "controls/common.h"
#include "controls/dialog.h"
#include "controls/tab_control.h"
#include "../../lang/config_lang.h"

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @class ConfigDialog
        /// @brief Configuration dialog
        /// @see TabbedDialog
        class ConfigDialog : private controls::Dialog
        {
            private:
            lang::ConfigLang m_languageResource; ///< Language resources
            controls::TabControl m_tabs;         ///< Advanced tab control


        public:
            /// @brief Create dialog box
            /// @param[in] instance  Current instance handle
            /// @throws runtime_error     Dialog creation error
            /// @throws invalid_argument  Invalid instance
            ConfigDialog(controls::library_instance_t instance);
            /// @brief Destroy dialog box
            ~ConfigDialog();

            /// @brief Display modal dialog box
            /// @returns Dialog result
            /// @throws runtime_error  Dialog creation/display error
            controls::Dialog::result_t showDialog();


        private:
            // -- event handlers -- --------------------------------------------

            /// @brief Initialization event handler
            static DIALOG_EVENT_RETURN onInit(DIALOG_EVENT_HANDLER_ARGUMENTS);
            /// @brief Paint event handler - draw dialog
            static DIALOG_EVENT_RETURN onPaint(DIALOG_EVENT_HANDLER_ARGUMENTS);
            /// @brief Sub-control drawing event handler - draw component
            static DIALOG_EVENT_RETURN onDrawItem(DIALOG_EVENT_HANDLER_ARGUMENTS);
            /// @brief Sub-control command event handler
            static DIALOG_EVENT_RETURN onCommand(DIALOG_EVENT_HANDLER_ARGUMENTS);
            /// @brief Dialog confirm event handler - check validity
            static DIALOG_EVENT_RETURN onConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS);


            // -- specialized handlers -- --------------------------------------

            /// @brief Language change event
            /// @param[in] value  Selected value
            /// @returns Language validity / file validity
            bool onLanguageChange(DIALOG_EVENT_HANDLER_ARGUMENTS, const int32_t value);
        };
    }
}