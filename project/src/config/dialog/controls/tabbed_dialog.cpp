/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : dialog control with tabs
*******************************************************************************/
#include "../../../globals.h"
#include <string>
#include <stdexcept>
#include <stack>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#endif
#include "../../../res/resource.h"
#include "dialog.h"
#include "tabbed_dialog.h"
using namespace config::dialog;

/// @brief Display modal dialog box
/// @param[in] dialogId  Dialog description identifier
/// @param[in] customEventHandler  Main event handler
/// @returns Dialog result
/// @throws runtime_error  Dialog creation/display error
#if _DIALOGAPI == DIALOGAPI_WIN32
dialog_result_t controls::TabbedDialog::showCustomDialog(const int32_t dialogId, INT_PTR(*customEventHandler)(HWND,UINT,WPARAM,LPARAM))
{
	if (customEventHandler == nullptr)
		customEventHandler = dialogEventHandler; // default handler

	// visual style init
	HANDLE hActCtx = INVALID_HANDLE_VALUE;
	ULONG_PTR cookie;
	if (s_stackedConfirmHandlers.empty()) // only initialize once (if multiple dialogs)
	{
		ACTCTX actCtx;
		ZeroMemory(&actCtx, sizeof(actCtx));
		actCtx.cbSize = sizeof(actCtx);
		actCtx.hModule = m_instance;
		actCtx.lpResourceName = MAKEINTRESOURCE(IDS_ACTIVATION_CONTEXT);
		actCtx.dwFlags = ACTCTX_FLAG_HMODULE_VALID | ACTCTX_FLAG_RESOURCE_NAME_VALID;

		hActCtx = CreateActCtx(&actCtx);
		if (hActCtx != INVALID_HANDLE_VALUE)
			ActivateActCtx(hActCtx, &cookie);
		InitCommonControls();
	}

	// open modal dialog box
	s_stackedInitHandlers.push(nullptr);
	s_stackedConfirmHandlers.push(nullptr);
	bool isSuccess = (DialogBox(static_cast<HINSTANCE>(m_instance), MAKEINTRESOURCE(dialogId),
								static_cast<HWND>(m_baseWindow), (DLGPROC)customEventHandler) >= 0);
	dialog_result_t result = s_latestResult;
	s_stackedInitHandlers.pop();
	s_stackedConfirmHandlers.pop();

	// disable visual style
	if (hActCtx != INVALID_HANDLE_VALUE)
	{
		DeactivateActCtx(0, cookie);
		ReleaseActCtx(hActCtx);
	}

	if (isSuccess == false) // error -> report
		throw std::runtime_error("Dialog opening/init failure");
	return result;
}
#else
dialog_result_t controls::TabbedDialog::showCustomDialog(const int32_t dialogId, void*(*customEventHandler)(void*))
{
	stackedInitHandlers.push(confirmHandler);
	stackedConfirmHandlers.push(confirmHandler);
	//...
	stackedInitHandlers.pop();
	stackedConfirmHandlers.pop();
	return dialog_result_t::cancel;
}
#endif
