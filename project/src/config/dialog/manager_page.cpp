/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab page - profile manager
*******************************************************************************/
#include "../../globals.h"
#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <functional>
#include "../../lang/config_lang.h"
#include "../../res/resource.h"
#include "../../events/utils/logger.h"
#include "controls/common.h"
#include "controls/text_field.hpp"
#include "controls/combo_box.h"
#include "controls/check_box.hpp"
#include "controls/label.hpp"
#include "config_dialog.h"
#include "controls/tab_page.h"
#include "controls/file_dialog.h"
#include "manager_page.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Create tab page - profile manager
/// @param[in] instance       Current instance handle
/// @param[in] pParentDialog  Parent dialog reference
ManagerPage::ManagerPage(controls::library_instance_t instance, controls::Dialog* pParentDialog)
    : controls::TabPage(instance, pParentDialog, IDD_MANAGER_PAGE)
{
    // set event handlers
    TabPage::event_handler_t eventHandler;
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(PAGE_EVENT_HANDLER_ARGUMENTS)>(onInit);
    TabPage::registerEvent(dialog_event_t::init, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(PAGE_EVENT_HANDLER_ARGUMENTS)>(onCommand);
    TabPage::registerEvent(dialog_event_t::command, eventHandler);
}



// -- event handlers -- --------------------------------------------

/// @brief Initialization event handler
DIALOG_EVENT_RETURN ManagerPage::onInit(PAGE_EVENT_HANDLER_ARGUMENTS)
{
    ManagerPage& parent = getEventTargetPageReference(ManagerPage);

    // add icons to buttons
    parent.m_buttonIcons.push_back(std::make_shared<controls::ButtonIcon>(parent.m_instance, IDI_CONFIG_BUTTON_ADD, 24, 24));
    parent.m_buttonIcons.push_back(std::make_shared<controls::ButtonIcon>(parent.m_instance, IDI_CONFIG_BUTTON_EDIT, 24, 24));
    parent.m_buttonIcons.push_back(std::make_shared<controls::ButtonIcon>(parent.m_instance, IDI_CONFIG_BUTTON_DEL, 24, 24));
    parent.m_buttonIcons.push_back(std::make_shared<controls::ButtonIcon>(parent.m_instance, IDI_CONFIG_BUTTON_IN, 24, 24));
    parent.m_buttonIcons.push_back(std::make_shared<controls::ButtonIcon>(parent.m_instance, IDI_CONFIG_BUTTON_OUT, 24, 24));
    parent.m_buttonIcons.at(0)->addIconToButton(getEventWindowHandle(), IDC_MNG_BTN_ADD, L"Add"s);
    parent.m_buttonIcons.at(1)->addIconToButton(getEventWindowHandle(), IDC_MNG_BTN_EDIT, L"Edit"s);
    parent.m_buttonIcons.at(2)->addIconToButton(getEventWindowHandle(), IDC_MNG_BTN_REMOVE, L"Del."s);
    parent.m_buttonIcons.at(3)->addIconToButton(getEventWindowHandle(), IDC_MNG_BTN_IMPORT, L"In."s);
    parent.m_buttonIcons.at(4)->addIconToButton(getEventWindowHandle(), IDC_MNG_BTN_EXPORT, L"Ex."s);

    // translate controls/labels
    parent.onLanguageChange(false);

    // apply config settings
    //...

    return DIALOG_EVENT_RETURN_VALID;
}

/// @brief Sub-control command event handler
DIALOG_EVENT_RETURN ManagerPage::onCommand(PAGE_EVENT_HANDLER_ARGUMENTS)
{
    ManagerPage& parent = getEventTargetPageReference(ManagerPage);

    return DIALOG_EVENT_RETURN_ERROR;
}



// -- specialized handlers -- --------------------------------------

/// @brief Language change event
/// @returns Validity
bool ManagerPage::onDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    // update config
    //...copy config settings
    //...
    return true;
}

/// @brief Language change event
/// @param[in] isRecursive    Also translate controls in child pages or not
void ManagerPage::onLanguageChange(const bool isRecursive)
{
    window_handle_t hPage = getPageHandle();
    lang::ConfigLang& langRes = getParentDialog<ConfigDialog>()->getLanguageResource();

    Label::setText(hPage, IDS_MNG_GROUP1, langRes.profileManager.groupList);
    //...
}


/*{
    FileDialog fileDialog(...);
    fileDialog.registerEvent(...onInit);
    fileDialog.registerEvent(...onCommand);
    fileDialog.registerEvent(...onConfirm);
}

onInit
{
    FileDialog::onInit(...); // call original handler
    //...
    // add other stuff here
    //...
}*/

