/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration dialog
*******************************************************************************/
#pragma once

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

            /// @brief Get language resource (translations)
            /// @returns Language object reference
            inline lang::ConfigLang& getLanguageResource() noexcept
            {
                return m_languageResource;
            }
            /// @brief Get tab control
            /// @returns Tab control reference
            inline controls::TabControl& getTabControl() noexcept
            {
                return m_tabs;
            }


        private:
            // -- event handlers -- --------------------------------------------

            /// @brief Initialization event handler
            static DIALOG_EVENT_RETURN onInit(Dialog::event_args_t args);
            /// @brief Paint event handler - draw dialog
            static DIALOG_EVENT_RETURN onPaint(Dialog::event_args_t args);
            /// @brief Sub-control command event handler
            static DIALOG_EVENT_RETURN onCommand(Dialog::event_args_t args);
            /// @brief Dialog confirm event handler - check validity
            static DIALOG_EVENT_RETURN onConfirm(Dialog::event_args_t args);
            /// @brief Dialog close event handler
            static DIALOG_EVENT_RETURN onClose(Dialog::event_args_t args);


            // -- specialized handlers -- --------------------------------------

            /// @brief Language selection event
            /// @param[in] value  Selected value
            /// @returns Language validity / file validity
            bool onLanguageSelection(Dialog::event_args_t& args, const int32_t value);
            /// @brief Language change event
            /// @param[in] isRecursive  Also translate controls in child pages or not
            void onLanguageChange(Dialog::event_args_t& args, const bool isRecursive);
        };
    }
}