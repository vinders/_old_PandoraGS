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
#include <functional>
#include <stack>
#include <memory>
#include "../../res/resource.h"
#include "controls/common.h"
#include "controls/dialog.h"
#include "controls/tab_control.h"
#include "general_page.h"
#include "manager_page.h"
#include "profile_page.h"
#include "config_dialog.h"
using namespace config::dialog;
using namespace config::dialog::controls;


/// @brief Create dialog box
/// @param[in] instance  Current instance handle
/// @throws runtime_error     Dialog creation error
/// @throws invalid_argument  Invalid instance
ConfigDialog::ConfigDialog(library_instance_t instance) : Dialog(instance)
{
    // load config values
    config::Config::init();
    m_languageResource.setLanguage(config::Config::langCode, config::Config::langFilePath);

    // set tabs and pages
    tab_association_t tabGeneral, tabManager, tabProfile;
    tabGeneral.button = std::make_shared<TabButton>(instance, m_languageResource.dialog.generalSettings, IDC_GENERAL_TAB, IDB_CONFIG_ICONS, 1, 48);
    tabGeneral.page = std::make_shared<GeneralPage>();
    tabManager.button = std::make_shared<TabButton>(instance, m_languageResource.dialog.profileManagement, IDC_MANAGER_TAB, IDB_CONFIG_ICONS, 2, 48);
    tabManager.page = std::make_shared<ManagerPage>();
    tabProfile.button = std::make_shared<TabButton>(instance, m_languageResource.dialog.profileSettings, IDC_PROFILE_TAB, IDB_CONFIG_ICONS, 3, 48);
    tabProfile.page = std::make_shared<ProfilePage>();
    // insert pages
    m_tabs.addTab(tabGeneral);
    m_tabs.addTab(tabManager);
    m_tabs.addTab(tabProfile);

    // set event handlers
    dialog_event_handler_t eventHandler;
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(DIALOG_EVENT_HANDLER_ARGUMENTS)>(onInit);
    Dialog::registerEvent(dialog_event_t::init, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(DIALOG_EVENT_HANDLER_ARGUMENTS)>(onPaint);
    Dialog::registerEvent(dialog_event_t::paint, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(DIALOG_EVENT_HANDLER_ARGUMENTS)>(onDrawItem);
    Dialog::registerEvent(dialog_event_t::drawItem, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(DIALOG_EVENT_HANDLER_ARGUMENTS)>(onCommand);
    Dialog::registerEvent(dialog_event_t::command, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(DIALOG_EVENT_HANDLER_ARGUMENTS)>(onConfirm);
    Dialog::registerEvent(dialog_event_t::confirm, eventHandler);
}

/// @brief Destroy dialog box
ConfigDialog::~ConfigDialog()
{
    m_tabs.clear();
    config::Config::close();
}

/// @brief Display modal dialog box
/// @returns Dialog result
/// @throws runtime_error  Dialog creation/display error
dialog_result_t ConfigDialog::showDialog()
{
    return Dialog::showDialog(IDD_CONFIG_DIALOG, true);
}


/// @brief Initialization event handler
DIALOG_EVENT_RETURN ConfigDialog::onInit(DIALOG_EVENT_HANDLER_ARGUMENTS)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    ConfigDialog& parent = *(static_cast<ConfigDialog*>(pDialog));

    // translate main dialog buttons
    //...

    // set list of profiles
    //...

    // set language selector
    //...

    // add tab control and pages to dialog
    parent.m_tabs.create(static_cast<window_handle_t>(hWindow), LOGO_HEIGHT + 2, LOGO_WIDTH);

    return (INT_PTR)TRUE;
}
#else
{
    return 0;//...
}
#endif


/// @brief Paint event handler - draw dialog
DIALOG_EVENT_RETURN ConfigDialog::onPaint(DIALOG_EVENT_HANDLER_ARGUMENTS)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    return (INT_PTR)TRUE;
}
#else
{
    return 0;//...
}
#endif


/// @brief Sub-control drawing event handler - draw component
DIALOG_EVENT_RETURN ConfigDialog::onDrawItem(DIALOG_EVENT_HANDLER_ARGUMENTS)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    return (INT_PTR)TRUE;
}
#else
{
    return 0;//...
}
#endif


/// @brief Sub-control command event handler
DIALOG_EVENT_RETURN ConfigDialog::onCommand(DIALOG_EVENT_HANDLER_ARGUMENTS)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    switch (LOWORD(wParam))
    {
        case IDC_LANG_LIST:
        {
            // language change
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                //...


                return (INT_PTR)TRUE;
            }
            break;
        }
    }
    return (INT_PTR)FALSE;
}
#else
{
    return 0;//...
}
#endif


/// @brief Dialog confirm event handler - check validity
DIALOG_EVENT_RETURN ConfigDialog::onConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    return (INT_PTR)TRUE;
}
#else
{
    return 0;//...
}
#endif

