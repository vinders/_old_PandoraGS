/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration dialog
*******************************************************************************/
#include "../../globals.h"
#include <string>
#include <stdexcept>
#include <stack>
#include <memory>
#include "../../res/resource.h"
#include "controls/dialog.h"
#include "config_dialog.h"
using namespace config::dialog;

std::stack<ConfigDialog*> ConfigDialog::s_stackedSelfReferences; ///< Static references to non-static dialogs


/// @brief Create dialog box
/// @param[in] instance  Current instance handle
/// @throws runtime_error     Dialog creation error
/// @throws invalid_argument  Invalid instance
ConfigDialog::ConfigDialog(library_instance_t instance) : TabbedDialog(instance)
{
	// load config values
	config::Config::init();
	m_languageResource.setLanguage(config::Config::langCode, config::Config::langFilePath);

	// set dialog pages
	//...init pages and tabs
		//créer les tab_association_t et les insérer dans m_tabs avec paramètres appropriés pour boutons
}

/// @brief Destroy dialog box
ConfigDialog::~ConfigDialog()
{
	//...delete pages and tabs
	config::Config::close();
}


/// @brief Display modal dialog box
/// @returns Dialog result
/// @throws runtime_error  Dialog creation/display error
dialog_result_t ConfigDialog::showDialog()
{
	dialog_result_t result = dialog_result_t::cancel;
	try
	{
		s_stackedSelfReferences.push(this);
		result = showCustomDialog(IDD_CONFIG_DIALOG, configDialogEventHandler);
		s_stackedSelfReferences.pop();
	}
	catch (...)
	{
		s_stackedSelfReferences.pop();
		throw;
	}
	return result;
}


#if _DIALOGAPI == DIALOGAPI_WIN32
/// @brief Dialog event handler
/// @param[in] hWindow  Window handle
/// @param[in] msg      Event message
/// @param[in] wParam   Command
/// @param[in] lParam   Informations
/// @returns Event result
INT_PTR ConfigDialog::configDialogEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// dialog drawing
		case WM_INITDIALOG: /*...*/ break;
		case WM_PAINT:      /*...*/ break;
		case WM_DRAWITEM:   /*...*/ break;

		// dialog controls
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				// save and close button
				case IDOK:
					if (/*...validation...*/true)
					{
						EndDialog(hWindow, TRUE);
						s_latestResult = dialog_result_t::confirm;
						return (INT_PTR)TRUE;
					}
					break;

				// cancel and close button
				case IDCANCEL:
					EndDialog(hWindow, TRUE);
					s_latestResult = dialog_result_t::cancel;
					return (INT_PTR)TRUE;

				// language change
				case IDC_LANG_LIST:
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						//return onLanguageChange(hWindow);
					}
					break;
			}
			break;
		}
		case WM_CLOSE: // close button/shortcut
			EndDialog(hWindow, TRUE);
			s_latestResult = dialog_result_t::cancel;
			return (INT_PTR)TRUE; break;
	}
	return (INT_PTR)FALSE;
}
#endif


