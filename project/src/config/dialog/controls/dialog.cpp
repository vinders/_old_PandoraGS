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
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#endif
#include "../../../res/resource.h"
#include "common.h"
#include "dialog.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;

Dialog* Dialog::s_dialogRefBuffer = nullptr; ///< Dialog reference buffer (pass non-static data to static handler)


/// @brief Create dialog box
/// @param[in] instance  Current instance handle
/// @throws invalid_argument  Invalid instance
Dialog::Dialog(library_instance_t instance)
{
	m_dialogData.dialogResult = dialog_result_t::cancel;
	m_instance = instance;
	#if _DIALOGAPI == DIALOGAPI_WIN32
	if ((HINSTANCE)instance == (HINSTANCE)INVALID_HANDLE_VALUE 
	 && (HINSTANCE)(m_instance = (library_instance_t)GetModuleHandle(NULL)) == (HINSTANCE)INVALID_HANDLE_VALUE)
		throw std::invalid_argument("Invalid library instance handle");
	#endif
}


/// @brief Display modal dialog box
/// @param[in] dialogId        Dialog description identifier
/// @param[in] isStyleEnabled  Enable enhanced visual style
/// @returns Dialog result
/// @throws runtime_error  Dialog creation error or runtime error
dialog_result_t Dialog::showDialog(const int32_t resourceId, const bool isStyleEnabled)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
	HANDLE hActCtx = INVALID_HANDLE_VALUE;
	ULONG_PTR cookie;

	// get base window handle
	HWND hWindow = GetActiveWindow();
	if (hWindow == (HWND)INVALID_HANDLE_VALUE)
		throw std::runtime_error("Could not obtain main window handle");

	// visual style init
	static bool isVisualStyleLoaded = false;
	if (isStyleEnabled && isVisualStyleLoaded == false) // only initialized once
	{
		ACTCTX actCtx;
		ZeroMemory(&actCtx, sizeof(actCtx));
		actCtx.cbSize = sizeof(actCtx);
		actCtx.hModule = (HINSTANCE)m_instance;
		actCtx.lpResourceName = MAKEINTRESOURCE(IDS_ACTIVATION_CONTEXT);
		actCtx.dwFlags = ACTCTX_FLAG_HMODULE_VALID | ACTCTX_FLAG_RESOURCE_NAME_VALID;

		hActCtx = CreateActCtx(&actCtx);
		if (hActCtx != INVALID_HANDLE_VALUE)
			ActivateActCtx(hActCtx, &cookie);
		InitCommonControls();
		isVisualStyleLoaded = true;
	}

	// open modal dialog box
	s_dialogRefBuffer = this;
	m_dialogData.dialogResult = dialog_result_t::error;
	bool isSuccess = (DialogBox(static_cast<HINSTANCE>(m_instance), MAKEINTRESOURCE(resourceId), static_cast<HWND>(hWindow), (DLGPROC)dialogEventHandler) > 0);
	s_dialogRefBuffer = nullptr;

	// disable visual style (if enabled here)
	if (hActCtx != INVALID_HANDLE_VALUE)
	{
		DeactivateActCtx(0, cookie);
		ReleaseActCtx(hActCtx);
		isVisualStyleLoaded = false;
	}

	if (isSuccess == false) // error -> report
	{
		DWORD code = GetLastError();
		throw std::runtime_error("Dialog opening/init failure - code "s + std::to_string(static_cast<uint32_t>(code)));
	}
	return m_dialogData.dialogResult;
}
#else
{
	//...
	return dialog_result_t::cancel;
}
#endif


#if _DIALOGAPI == DIALOGAPI_WIN32
/// @brief Get current dialog non-static reference
/// @param[in] hWindow  Window handle
Dialog* Dialog::getCurrentDialog(HWND hWindow)
{
	Dialog* pDialog = (Dialog*)GetWindowLongPtr(hWindow, GWLP_USERDATA); // read reference to dialog (if existing)
	if (!pDialog)
	{
		// store reference to non-static dialog
		pDialog = nullptr;
		if (s_dialogRefBuffer != nullptr)
		{
			pDialog = s_dialogRefBuffer;
			SetWindowLongPtr(hWindow, GWLP_USERDATA, (LONG_PTR)(pDialog));
		}
	}
	return pDialog;
}


/// @brief Main dialog event handler
/// @param[in] hWindow  Window handle
/// @param[in] msg      Event message
/// @param[in] wParam   Command
/// @param[in] lParam   Informations
/// @returns Event result
INT_PTR CALLBACK Dialog::dialogEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Dialog* pDialog = getCurrentDialog(hWindow);
	if (pDialog != nullptr)
	{
		switch (msg)
		{
			// dialog settings
			case WM_INITDIALOG:
			{
				if (pDialog->isRegisteredEvent(dialog_event_t::init)) // call handler
					return pDialog->getEventHandler(dialog_event_t::init).handler(pDialog, hWindow, wParam, lParam);
				return (INT_PTR)TRUE; break;
			}
			case WM_PAINT:
			{
				if (pDialog->isRegisteredEvent(dialog_event_t::paint)) // call handler
					return pDialog->getEventHandler(dialog_event_t::paint).handler(pDialog, hWindow, wParam, lParam);
				break;
			}
			case WM_DRAWITEM:
			{
				if (pDialog->isRegisteredEvent(dialog_event_t::drawItem)) // call handler
					return pDialog->getEventHandler(dialog_event_t::drawItem).handler(pDialog, hWindow, wParam, lParam);
				break;
			}

			// dialog controls
			case WM_COMMAND:
			{
				switch (LOWORD(wParam))
				{
					// save and close button
					case IDOK:
					{
						if (pDialog->isRegisteredEvent(dialog_event_t::confirm) == false // no handler (close directly) or handler returns true
						 || pDialog->getEventHandler(dialog_event_t::confirm).handler(pDialog, hWindow, wParam, lParam) == (INT_PTR)TRUE)
						{
							EndDialog(hWindow, TRUE);
							pDialog->setDialogResult(dialog_result_t::confirm);
							return (INT_PTR)TRUE;
						}
						else
							return (INT_PTR)FALSE;
						break;
					}
					// cancel and close button
					case IDCANCEL:
					{
						if (pDialog->isRegisteredEvent(dialog_event_t::cancel) == false // no handler (close directly) or handler returns true
						 || pDialog->getEventHandler(dialog_event_t::cancel).handler(pDialog, hWindow, wParam, lParam) == (INT_PTR)TRUE)
						{
							EndDialog(hWindow, TRUE);
							pDialog->setDialogResult(dialog_result_t::cancel);
							return (INT_PTR)TRUE;
						}
						else
							return (INT_PTR)FALSE;
						break;
					}
					// other command
					default:
					{
						if (pDialog->isRegisteredEvent(dialog_event_t::command) // call handler
						 && pDialog->getEventHandler(dialog_event_t::command).handler(pDialog, hWindow, wParam, lParam) == (INT_PTR)TRUE)
							return (INT_PTR)TRUE;
						break;
					}
				}
				break;
			}
			case WM_CLOSE: // close button/shortcut
			{
				if (pDialog->isRegisteredEvent(dialog_event_t::cancel) == false // no handler (close directly) or handler returns true
				 || pDialog->getEventHandler(dialog_event_t::cancel).handler(pDialog, hWindow, wParam, lParam) == (INT_PTR)TRUE)
				{
					EndDialog(hWindow, TRUE);
					pDialog->setDialogResult(dialog_result_t::cancel);
					return (INT_PTR)TRUE;
				}
				else
					return (INT_PTR)FALSE;
				break;
			}
		}
	}
	return DefWindowProc(hWindow, msg, wParam, lParam);
}
#endif
