/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : dialog control
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <stack>
#include <functional>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#endif

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
		#if _DIALOGAPI == DIALOGAPI_WIN32
		typedef HWND window_handle_t; ///< Window handle
		typedef HINSTANCE library_instance_t; ///< Library instance
		typedef std::function<INT_PTR(HWND,UINT,WPARAM,LPARAM)> dialog_event_handler_t; ///< Event handling sub-function
		#else
		typedef int32_t window_handle_t; ///< Window handle
		typedef int32_t library_instance_t; ///< Library instance
		typedef std::function<int(void*)> dialog_event_handler_t; ///< Event handling sub-function
		#endif

		/// @enum dialog_result_t
		/// @brief Action performed when closing a dialog
		enum class dialog_result_t : int32_t
		{
			cancel = 0,
			confirm = 1
		};


        /// @namespace config.dialog.controls
        /// Dialog controls
        namespace controls
        {
            /// @class Dialog
            /// @brief Dialog control
            class Dialog
            {
			protected:
				static dialog_result_t s_latestResult;                                 ///< Latest dialog result
				static std::stack<dialog_event_handler_t> s_stackedInitHandlers;    ///< Dialog initialization handlers
				static std::stack<dialog_event_handler_t> s_stackedConfirmHandlers; ///< Dialog confirmation handlers
				library_instance_t m_instance; ///< Library instance handle
				window_handle_t m_baseWindow;  ///< Base window handle

			public:
				/// @brief Create dialog box
				/// @param[in] instance  Current instance handle
				/// @throws runtime_error     Dialog creation error
				/// @throws invalid_argument  Invalid instance
				Dialog(library_instance_t instance);
				/// @brief Destroy dialog box
				virtual ~Dialog() {}

				/// @brief Display modal dialog box
				/// @param[in] dialogId  Dialog description identifier
				/// @param[in] confirmHandler  Confirmation handler (optional)
				/// @param[in] initHandler  Initialization handler (optional)
				/// @returns Dialog result
				/// @throws runtime_error  Dialog creation/display error
				dialog_result_t showDialog(const int32_t dialogId, dialog_event_handler_t confirmHandler = nullptr,
										   dialog_event_handler_t initHandler = nullptr);

			protected:
				#if _DIALOGAPI == DIALOGAPI_WIN32
				/// @brief Dialog event handler
				/// @param[in] hWindow  Window handle
				/// @param[in] msg      Event message
				/// @param[in] wParam   Command
				/// @param[in] lParam   Informations
				/// @returns Event result
				static INT_PTR dialogEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
				#endif
            };
        }
    }
}