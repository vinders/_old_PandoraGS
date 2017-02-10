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
#include "../config.h"
#include "controls/tabbed_dialog.h"

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
        class ConfigDialog : private controls::TabbedDialog
        {
        private:
			static std::stack<ConfigDialog*> s_stackedSelfReferences; ///< Static references to non-static dialogs


        public:
			/// @brief Create dialog box
			/// @param[in] instance  Current instance handle
			/// @throws runtime_error     Dialog creation error
			/// @throws invalid_argument  Invalid instance
			ConfigDialog(library_instance_t instance);
			/// @brief Destroy dialog box
			~ConfigDialog();

			/// @brief Display modal dialog box
			/// @returns Dialog result
			/// @throws runtime_error  Dialog creation/display error
			dialog_result_t showDialog();

		private:
			#if _DIALOGAPI == DIALOGAPI_WIN32
			/// @brief Dialog event handler
			/// @param[in] hWindow  Window handle
			/// @param[in] msg      Event message
			/// @param[in] wParam   Command
			/// @param[in] lParam   Informations
			/// @returns Event result
			static INT_PTR configDialogEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
			#endif
        };
    }
}