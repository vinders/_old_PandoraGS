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
    eventHandler.handler = onInit;
    TabPage::registerEvent(dialog_event_t::init, eventHandler);
    eventHandler.handler = onCommand;
    TabPage::registerEvent(dialog_event_t::command, eventHandler);
    eventHandler.handler = onNotify;
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
DIALOG_EVENT_RETURN ManagerPage::onInit(TabPage::event_args_t args)
{
    ManagerPage& parent = args.getParent<ManagerPage>();
    parent.m_hPage = args.window;

    // add icons to buttons
    parent.m_buttonIcons.push_back(std::make_shared<controls::ButtonIcon>(parent.m_instance, IDI_CONFIG_BUTTON_ADD, 24, 24));
    parent.m_buttonIcons.at(0)->addIconToButton(args.window, IDC_MNG_BTN_ADD, L"Add"s);
    parent.m_buttonIcons.push_back(std::make_shared<controls::ButtonIcon>(parent.m_instance, IDI_CONFIG_BUTTON_EDIT, 24, 24));
    parent.m_buttonIcons.at(1)->addIconToButton(args.window, IDC_MNG_BTN_EDIT, L"Edit"s);
    parent.m_buttonIcons.push_back(std::make_shared<controls::ButtonIcon>(parent.m_instance, IDI_CONFIG_BUTTON_DEL, 24, 24));
    parent.m_buttonIcons.at(2)->addIconToButton(args.window, IDC_MNG_BTN_REMOVE, L"Del."s);
    parent.m_buttonIcons.push_back(std::make_shared<controls::ButtonIcon>(parent.m_instance, IDI_CONFIG_BUTTON_IN, 24, 24));
    parent.m_buttonIcons.at(3)->addIconToButton(args.window, IDC_MNG_BTN_IMPORT, L"In."s);
    parent.m_buttonIcons.push_back(std::make_shared<controls::ButtonIcon>(parent.m_instance, IDI_CONFIG_BUTTON_OUT, 24, 24));
    parent.m_buttonIcons.at(4)->addIconToButton(args.window, IDC_MNG_BTN_EXPORT, L"Exp."s);

    // create data table
    try
    {
        lang::ConfigLang& langRes = parent.getParentDialog<ConfigDialog>()->getLanguageResource();
        parent.m_pDataTable = new DataTable(parent.m_instance, args.window, IDC_MNG_LISTVIEW, true, 30, 40, 389, 309);
        parent.m_pDataTable->addColumn(langRes.profileManager.tableNumber, 40);
        parent.m_pDataTable->addColumn(langRes.profileManager.tableProfile, 0);
    }
    catch (...)
    {
        MsgBox::showMessage(L"Profile manager initialization failure"s, L"Failed to create data table..."s, 
                            args.window, MsgBox::button_set_t::ok, MsgBox::message_icon_t::error);
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
DIALOG_EVENT_RETURN ManagerPage::onCommand(TabPage::event_args_t args)
{
    ManagerPage& parent = args.getParent<ManagerPage>();
    if (args.isEventSignedAction(ComboBox::event_t::selectionChanged) == false) // ignore preset combo box events
    {
        switch (args.controlId())
        {
            // apply profile preset
            case IDC_MNG_BTN_PRESETS:
                //...confirm + warning if none
                Button::unHighlight(args.window, IDC_MNG_BTN_PRESETS);
                break;

            // add new profile
            case IDC_MNG_BTN_ADD:
                //...dialog
                Button::unHighlight(args.window, IDC_MNG_BTN_ADD);
                break;

            // edit selected profile
            case IDC_MNG_BTN_EDIT:
                //...dialog + warning if none
                Button::unHighlight(args.window, IDC_MNG_BTN_EDIT);
                break;

            // remove selected profile(s)
            case IDC_MNG_BTN_REMOVE:
                parent.onProfileRemoval(args.window); 
                Button::unHighlight(args.window, IDC_MNG_BTN_REMOVE);
                break;

            // import new/existing profile
            case IDC_MNG_BTN_IMPORT:
                //...file dialog
                Button::unHighlight(args.window, IDC_MNG_BTN_IMPORT);
                break;

            // export selected profile(s)
            case IDC_MNG_BTN_EXPORT:
                //...file dialog + warning if none
                Button::unHighlight(args.window, IDC_MNG_BTN_EXPORT);
                break;
        }
    }
    return DIALOG_EVENT_RETURN_ERROR;
}


/// @brief Notification event handler
DIALOG_EVENT_RETURN ManagerPage::onNotify(TabPage::event_args_t args)
{
    if (args.notifierData)
    {
        ManagerPage& parent = args.getParent<ManagerPage>();
        try
        {
            parent.getDataTable().notifyEvent(args.notifierData);
        }
        catch (...) { /* already reported in onInit() */ }
    }
    return DIALOG_EVENT_RETURN_ERROR;
}



// -- specialized handlers -- --------------------------------------

/// @brief Profile removal event
void ManagerPage::onProfileRemoval(window_handle_t hWindow)
{
    lang::ConfigLang& langRes = getParentDialog<ConfigDialog>()->getLanguageResource();

    // get selected index(es)
    std::vector<uint32_t> selectedIndexes;
    getDataTable().getSelectedIndexes(selectedIndexes);
    if (selectedIndexes.size() == 0)
    {
        MsgBox::showMessage(langRes.profileManager.msgBoxRemoveEmptyTitle, langRes.profileManager.msgBoxRemoveEmpty,
                            hWindow, MsgBox::button_set_t::ok, MsgBox::message_icon_t::warning);
        return;
    }

    // show warning if default profile included (can't be removed)
    for (uint32_t i = 0; i < selectedIndexes.size(); ++i)
    {
        if (selectedIndexes.at(i) == 0)
        {
            MsgBox::showMessage(langRes.profileManager.msgBoxRemoveDefaultTitle, langRes.profileManager.msgBoxRemoveDefault,
                                hWindow, MsgBox::button_set_t::ok, MsgBox::message_icon_t::warning);
            return;
        }
    }
    
    // confirm removal
    std::wstring message = langRes.profileManager.msgBoxRemoveConfirm;
    if (selectedIndexes.size() > 1)
        message += L" ("s + std::to_wstring(selectedIndexes.size()) + L")"s;

    if (MsgBox::showMessage(langRes.profileManager.msgBoxRemoveConfirmTitle, message, hWindow,
        MsgBox::button_set_t::okCancel, MsgBox::message_icon_t::question) == MsgBox::result_t::ok)
    {
        for (uint32_t i = 0; i < selectedIndexes.size(); ++i)
        {
            // remove row from table
            getDataTable().removeRow(selectedIndexes.at(i));

            // remove profile from config
            //...remove from config
            //...
        }
    }
}

/// @brief Language change event
/// @returns Validity
bool ManagerPage::onDialogConfirm(Dialog::event_args_t& args)
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
