/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : dialog control
*******************************************************************************/
#include "../../../globals.h"
#include <string>
#include <stdexcept>
#include <stack>
#include "../../../res/resource.h"
#include "dialog.h"
using namespace config::dialog;

dialog_result_t controls::Dialog::s_latestResult = dialog_result_t::cancel;                              ///< Latest dialog result
std::stack<dialog_event_handler_t> controls::Dialog::s_stackedInitHandlers;    ///< Dialog initialization handlers
std::stack<dialog_event_handler_t> controls::Dialog::s_stackedConfirmHandlers; ///< Dialog confirmation handlers


/// @brief Create dialog box
/// @param[in] instance  Current instance handle
/// @throws runtime_error     Dialog creation error
/// @throws invalid_argument  Invalid instance
controls::Dialog::Dialog(library_instance_t instance) : m_instance(instance)
{
	#if _DIALOGAPI == DIALOGAPI_WIN32
	m_baseWindow = GetActiveWindow();
	if (m_baseWindow == (HWND)INVALID_HANDLE_VALUE)
		throw std::runtime_error("Could not obtain main window handle");
	if (instance == (HINSTANCE)INVALID_HANDLE_VALUE && (m_instance = GetModuleHandle(NULL)) == (HINSTANCE)INVALID_HANDLE_VALUE)
		throw std::invalid_argument("Invalid library instance handle");
	#endif
}


/// @brief Display modal dialog box
/// @param[in] dialogId  Dialog description identifier
/// @param[in] confirmHandler  Confirmation handler (optional)
/// @param[in] initHandler  Initialization handler (optional)
/// @returns Dialog result
/// @throws runtime_error  Dialog creation/display error
dialog_result_t controls::Dialog::showDialog(const int32_t dialogId, dialog_event_handler_t confirmHandler, dialog_event_handler_t initHandler)
{
	s_stackedInitHandlers.push(confirmHandler);
	s_stackedConfirmHandlers.push(confirmHandler);

	// open modal dialog box
	#if _DIALOGAPI == DIALOGAPI_WIN32
	bool isSuccess = (DialogBox(static_cast<HINSTANCE>(m_instance), MAKEINTRESOURCE(dialogId),
								static_cast<HWND>(m_baseWindow), (DLGPROC)dialogEventHandler) >= 0);
	#else
	bool isSuccess = /*...*/true;
	#endif
	dialog_result_t result = s_latestResult;

	s_stackedInitHandlers.pop();
	s_stackedConfirmHandlers.pop();

	if (isSuccess == false) // error -> report
		throw std::runtime_error("Dialog opening/init failure");
	return result;
}


#if _DIALOGAPI == DIALOGAPI_WIN32
/// @brief Dialog event handler
/// @param[in] hWindow  Window handle
/// @param[in] msg      Event message
/// @param[in] wParam   Command
/// @param[in] lParam   Informations
/// @returns Event result
INT_PTR controls::Dialog::dialogEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
    {
        case WM_INITDIALOG: // dialog init
        {
			dialog_event_handler_t initFunction = s_stackedInitHandlers.top();
			if (initFunction != nullptr)
				return initFunction(hWindow, msg, wParam, lParam); // call init handler
            return (INT_PTR)TRUE;
        }
        case WM_COMMAND: // dialog inner control
        {
            switch (LOWORD(wParam)) 
            {
				case IDOK: 
				{
					dialog_event_handler_t confirmFunction = s_stackedConfirmHandlers.top();
					if (confirmFunction != nullptr)
					{
						if (confirmFunction(hWindow, msg, wParam, lParam) == (INT_PTR)FALSE) // call confirmation handler
							return (INT_PTR)FALSE; // don't close dialog if handler refuses it
					}
                    EndDialog(hWindow, TRUE);
					s_latestResult = dialog_result_t::confirm;
                    return (INT_PTR)TRUE;
				}
                case IDCANCEL:  
                    EndDialog(hWindow, TRUE);
					s_latestResult = dialog_result_t::cancel;
                    return (INT_PTR)TRUE;
            } 
            break;
        }
        case WM_CLOSE: // close button/shortcut
            EndDialog(hWindow, TRUE);
			s_latestResult = dialog_result_t::cancel;
            return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}
#endif

