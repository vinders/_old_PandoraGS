/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab page - profile manager
*******************************************************************************/
#include "../../globals.h"
#include <cstdint>
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
#include "controls/button.hpp"
#include "config_dialog.h"
#include "controls/tab_page.h"
#include "controls/file_dialog.h"
#include "controls/tooltip.hpp"
#include "controls/data_table.h"
#include "controls/msg_box.h"
#include "manager_page.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Create tab page - profile manager
/// @param[in] instance       Current instance handle
/// @param[in] pParentDialog  Parent dialog reference
ManagerPage::ManagerPage(controls::library_instance_t instance, controls::Dialog* pParentDialog)
    : controls::TabPage(instance, pParentDialog, IDD_MANAGER_PAGE), m_pDataTable(nullptr)
{
    // set event handlers
    TabPage::event_handler_t eventHandler;
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(PAGE_EVENT_HANDLER_ARGUMENTS)>(onInit);
    TabPage::registerEvent(dialog_event_t::init, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(PAGE_EVENT_HANDLER_ARGUMENTS)>(onCommand);
    TabPage::registerEvent(dialog_event_t::command, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(PAGE_EVENT_HANDLER_ARGUMENTS)>(onNotify);
    TabPage::registerEvent(dialog_event_t::notify, eventHandler);
}

/// @brief Close tab page control - overridable method
void ManagerPage::overridableClose()
{
    TabPage::unregisterEvent(dialog_event_t::notify);
    TabPage::unregisterEvent(dialog_event_t::command);
    TabPage::unregisterEvent(dialog_event_t::init);
    m_buttonIcons.clear();
    m_tooltips.clear();
    if (m_pDataTable != nullptr)
        delete m_pDataTable;
    m_pDataTable = nullptr;
}



// -- event handlers -- --------------------------------------------

/// @brief Initialization event handler
DIALOG_EVENT_RETURN ManagerPage::onInit(PAGE_EVENT_HANDLER_ARGUMENTS)
{
    ManagerPage& parent = getEventTargetPageReference(ManagerPage);
    parent.m_hPage = getEventWindowHandle();

    // add icons to buttons
    parent.m_buttonIcons.push_back(std::make_shared<controls::ButtonIcon>(parent.m_instance, IDI_CONFIG_BUTTON_ADD, 24, 24));
    parent.m_buttonIcons.at(0)->addIconToButton(getEventWindowHandle(), IDC_MNG_BTN_ADD, L"Add"s);
    parent.m_buttonIcons.push_back(std::make_shared<controls::ButtonIcon>(parent.m_instance, IDI_CONFIG_BUTTON_EDIT, 24, 24));
    parent.m_buttonIcons.at(1)->addIconToButton(getEventWindowHandle(), IDC_MNG_BTN_EDIT, L"Edit"s);
    parent.m_buttonIcons.push_back(std::make_shared<controls::ButtonIcon>(parent.m_instance, IDI_CONFIG_BUTTON_DEL, 24, 24));
    parent.m_buttonIcons.at(2)->addIconToButton(getEventWindowHandle(), IDC_MNG_BTN_REMOVE, L"Del."s);
    parent.m_buttonIcons.push_back(std::make_shared<controls::ButtonIcon>(parent.m_instance, IDI_CONFIG_BUTTON_IN, 24, 24));
    parent.m_buttonIcons.at(3)->addIconToButton(getEventWindowHandle(), IDC_MNG_BTN_IMPORT, L"In."s);
    parent.m_buttonIcons.push_back(std::make_shared<controls::ButtonIcon>(parent.m_instance, IDI_CONFIG_BUTTON_OUT, 24, 24));
    parent.m_buttonIcons.at(4)->addIconToButton(getEventWindowHandle(), IDC_MNG_BTN_EXPORT, L"Exp."s);

    // create data table
    try
    {
        lang::ConfigLang& langRes = parent.getParentDialog<ConfigDialog>()->getLanguageResource();
        parent.m_pDataTable = new DataTable(parent.m_instance, getEventWindowHandle(), IDC_MNG_LISTVIEW, true, 30, 40, 389, 309);
        parent.m_pDataTable->addColumn(langRes.profileManager.tableNumber, 40);
        parent.m_pDataTable->addColumn(langRes.profileManager.tableProfile, 0);
    }
    catch (...)
    {
        MsgBox::showMessage(L"Profile manager initialization failure"s, L"Failed to create data table..."s, 
                            getEventWindowHandle(), MsgBox::button_set_t::ok, MsgBox::message_icon_t::error);
        return DIALOG_EVENT_RETURN_ERROR;
    }
    // fill data table
    std::vector<std::wstring>& profileNames = Config::getAllProfileNames();
    std::vector<std::vector<std::wstring>> rowData;
    for (uint32_t pr = 0; pr < profileNames.size(); ++pr)
    {
        rowData.push_back({ std::to_wstring(pr), profileNames.at(pr) });
    }
    parent.m_pDataTable->insertRows(rowData);

    // translate controls/labels
    parent.onLanguageChange(false);
    return DIALOG_EVENT_RETURN_VALID;
}


/// @brief Sub-control command event handler
DIALOG_EVENT_RETURN ManagerPage::onCommand(PAGE_EVENT_HANDLER_ARGUMENTS)
{
    ManagerPage& parent = getEventTargetPageReference(ManagerPage);
    if (getEventActionType() != CBN_SELCHANGE) // ignore preset combo box events
    {
        switch (getEventTargetControlId())
        {
            case IDC_MNG_BTN_PRESETS:
            {
                break;
            }
            case IDC_MNG_BTN_ADD:
            {
                break;
            }
            case IDC_MNG_BTN_EDIT:
            {
                break;
            }
            case IDC_MNG_BTN_REMOVE:
            {
                break;
            }
            case IDC_MNG_BTN_IMPORT:
            {
                break;
            }
            case IDC_MNG_BTN_EXPORT:
            {
                break;
            }
        }
    }
    return DIALOG_EVENT_RETURN_ERROR;
}


/// @brief Notification event handler
DIALOG_EVENT_RETURN ManagerPage::onNotify(PAGE_EVENT_HANDLER_ARGUMENTS)
{
    if (getEventArgs())
    {
        ManagerPage& parent = getEventTargetPageReference(ManagerPage);
        try
        {
            parent.getDataTable().notifyEvent(getEventArgs());
        }
        catch (...) { /* already reported in onInit */ }
    }
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
/// @param[in] IsUpdate  Set to false to initialize controls
void ManagerPage::onLanguageChange(const bool IsUpdate)
{
    window_handle_t hPage = getPageHandle();
    lang::ConfigLang& langRes = getParentDialog<ConfigDialog>()->getLanguageResource();

    if (IsUpdate == false) // initialize
    {
        ComboBox::initValues(hPage, IDC_MNG_PRESETS_LIST, langRes.profileManager.presets, 1);
    }
    else
    {
        // table headers
        if (m_pDataTable != nullptr)
            m_pDataTable->updateColumn(2u);
        // presets
        ComboBox::setValues(hPage, IDC_MNG_PRESETS_LIST, langRes.profileManager.presets);
    }

    Label::setText(hPage, IDS_MNG_GROUP1, langRes.profileManager.groupList);
    Label::setText(hPage, IDS_MNG_PRESETS, langRes.profileManager.profilePresets);
    Button::setText(hPage, IDC_MNG_BTN_PRESETS, langRes.profileManager.btnPresets);

    // tooltips
    if (m_tooltips.size() < 5u)
    {
        for (uint32_t i = m_tooltips.size(); i < 5u; ++i)
            m_tooltips.push_back(std::make_shared<controls::Tooltip>(m_instance, hPage));
    }
    m_tooltips[0]->setText(IDC_MNG_BTN_ADD, langRes.profileManager.btnAdd);
    m_tooltips[1]->setText(IDC_MNG_BTN_EDIT, langRes.profileManager.btnEdit);
    m_tooltips[2]->setText(IDC_MNG_BTN_REMOVE, langRes.profileManager.btnRemove);
    m_tooltips[3]->setText(IDC_MNG_BTN_IMPORT, langRes.profileManager.btnImport);
    m_tooltips[4]->setText(IDC_MNG_BTN_EXPORT, langRes.profileManager.btnExport);
}
