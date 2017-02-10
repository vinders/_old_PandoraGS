/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : dialog control with tabs
*******************************************************************************/
#pragma once

#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#endif
#include "dialog.h"
#include "tab_control.h"

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @namespace config.dialog.controls
        /// Dialog controls
        namespace controls
        {
            /// @class TabbedDialog
            /// @brief Dialog control with tabs - base class (no instanciation)
            /// @see Dialog
            class TabbedDialog : public Dialog
            {
            protected:
                TabControl m_tabs; ///< Managed tab control


			protected:
				/// @brief Create dialog box
				/// @param[in] instance  Current instance handle
				/// @throws runtime_error     Dialog creation error
				/// @throws invalid_argument  Invalid instance
				TabbedDialog(library_instance_t instance) : Dialog(instance) {}
				/// @brief Destroy dialog box
				virtual ~TabbedDialog() {}

				/// @brief Display modal dialog box
				/// @param[in] dialogId  Dialog description identifier
				/// @param[in] customEventHandler  Main event handler
				/// @returns Dialog result
				/// @throws runtime_error  Dialog creation/display error
				#if _DIALOGAPI == DIALOGAPI_WIN32
				dialog_result_t showCustomDialog(const int32_t dialogId, INT_PTR(*customEventHandler)(HWND,UINT,WPARAM,LPARAM));
				#else
				dialog_result_t showCustomDialog(const int32_t dialogId, void*(*customEventHandler)(void*));
				#endif
            };
        }
    }
}