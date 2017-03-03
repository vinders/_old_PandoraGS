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
#include "../../events/utils/file_io.h"
#include "manager_page.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;

ManagerPage* ManagerPage::s_pLastParentPage = nullptr; ///< Page reference buffer (for subdialogs)


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
EVENT_RETURN ManagerPage::onInit(TabPage::event_args_t args)
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
        return EVENT_RETURN_ERROR;
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
    return EVENT_RETURN_VALID;
}


/// @brief Sub-control command event handler
EVENT_RETURN ManagerPage::onCommand(TabPage::event_args_t args)
{
    if (args.isEventSignedAction(Button::event_t::clicked)) // buttons
    {
        ManagerPage& parent = args.getParent<ManagerPage>();
        switch (args.controlId())
        {
            // apply profile preset
            case IDC_MNG_BTN_PRESETS:
                parent.onProfilePresetChoice(args.window);
                Button::unHighlight(args.window, IDC_MNG_BTN_PRESETS);
                return EVENT_RETURN_VALID; break;

            // add new profile
            case IDC_MNG_BTN_ADD:
            {
                controls::Dialog::event_handler_t handlerRef;
                controls::Dialog addProfileDialog(parent.m_instance);
                handlerRef.handler = onAddProfileDialogInit;
                addProfileDialog.registerEvent(controls::dialog_event_t::init, handlerRef);
                handlerRef.handler = onAddProfileDialogConfirm;
                addProfileDialog.registerEvent(controls::dialog_event_t::confirm, handlerRef);
                addProfileDialog.setParent(parent.getParentDialog<Dialog>());
                // show modal dialog
                ManagerPage* pPrevBuffer = s_pLastParentPage;
                s_pLastParentPage = &parent;
                addProfileDialog.showDialog(IDD_ADDPROFILE_DIALOG, args.window,
                                 parent.getParentDialog<ConfigDialog>()->getLanguageResource().profileManager.btnAdd, false);
                s_pLastParentPage = pPrevBuffer;

                Button::unHighlight(args.window, IDC_MNG_BTN_ADD);
                return EVENT_RETURN_VALID; break;
            }

            // edit selected profile
            case IDC_MNG_BTN_EDIT:
                parent.onProfileEdit(args.window);
                Button::unHighlight(args.window, IDC_MNG_BTN_EDIT);
                return EVENT_RETURN_VALID; break;

            // remove selected profile(s)
            case IDC_MNG_BTN_REMOVE:
                parent.onProfileRemoval(args.window); 
                Button::unHighlight(args.window, IDC_MNG_BTN_REMOVE);
                return EVENT_RETURN_VALID; break;

            // import new/existing profile
            case IDC_MNG_BTN_IMPORT:
            {
                controls::Dialog::event_handler_t handlerRef;
                controls::FileDialog importProfileDialog(parent.m_instance, controls::FileDialog::file_mode_t::load);
                handlerRef.handler = onImportProfileDialogInit;
                importProfileDialog.registerEvent(controls::dialog_event_t::init, handlerRef);
                handlerRef.handler = onImportProfileDialogConfirm;
                importProfileDialog.registerEvent(controls::dialog_event_t::confirm, handlerRef);
                importProfileDialog.setParent(parent.getParentDialog<Dialog>());
                // show modal dialog
                ManagerPage* pPrevBuffer = s_pLastParentPage;
                s_pLastParentPage = &parent;
                importProfileDialog.showDialog(IDD_IMPORT_DIALOG, IDC_IMPORT_PATH_EDIT, args.window, IDC_IMPORT_BTN_PATH, L""s,
                                    parent.getParentDialog<ConfigDialog>()->getLanguageResource().profileManager.btnImport);
                s_pLastParentPage = pPrevBuffer;

                Button::unHighlight(args.window, IDC_MNG_BTN_IMPORT);
                return EVENT_RETURN_VALID; break;
            }

            // export selected profile(s)
            case IDC_MNG_BTN_EXPORT:
                parent.onProfileExport(args.window);
                Button::unHighlight(args.window, IDC_MNG_BTN_EXPORT);
                return EVENT_RETURN_VALID; break;
        }
    }
    return EVENT_RETURN_IGNORE;
}


/// @brief Notification event handler
EVENT_RETURN ManagerPage::onNotify(TabPage::event_args_t args)
{
    if (args.notifierData)
    {
        ManagerPage& parent = args.getParent<ManagerPage>();
        try
        {
            parent.getDataTable().notifyEvent(args.notifierData);
        }
        catch (...) // already reported in onInit()
        { 
            return EVENT_RETURN_ERROR;
        }
    }
    return EVENT_RETURN_IGNORE;
}



// -- specialized handlers -- --------------------------------------

/// @brief Profile edit event - open dialog
void ManagerPage::onProfileEdit(window_handle_t hWindow)
{
    lang::ConfigLang& langRes = getParentDialog<ConfigDialog>()->getLanguageResource();

    // get selected index(es)
    int32_t selectedIndex = getDataTable().getFirstSelectedIndex();
    if (selectedIndex < 0)
    {
        MsgBox::showMessage(langRes.profileManager.msgBoxEmptyTitle, langRes.profileManager.msgBoxEmpty,
                            hWindow, MsgBox::button_set_t::ok, MsgBox::message_icon_t::warning);
        return;
    }
    // default profile cannot be edited
    if (selectedIndex == 0)
    {
        MsgBox::showMessage(langRes.profileManager.msgBoxEditDefaultTitle, langRes.profileManager.msgBoxEditDefault,
            hWindow, MsgBox::button_set_t::ok, MsgBox::message_icon_t::warning);
        return;
    }

    controls::Dialog::event_handler_t handlerRef;
    controls::Dialog editProfileDialog(m_instance);
    handlerRef.handler = onEditProfileDialogInit;
    editProfileDialog.registerEvent(controls::dialog_event_t::init, handlerRef);
    handlerRef.handler = onEditProfileDialogConfirm;
    editProfileDialog.registerEvent(controls::dialog_event_t::confirm, handlerRef);
    editProfileDialog.setParent(getParentDialog<Dialog>());
    // show modal dialog
    ManagerPage* pPrevBuffer = s_pLastParentPage;
    s_pLastParentPage = this;
    editProfileDialog.showDialog(IDD_EDITPROFILE_DIALOG, hWindow,
                      getParentDialog<ConfigDialog>()->getLanguageResource().profileManager.btnEdit, false);
    s_pLastParentPage = pPrevBuffer;
}


/// @brief Profile removal event
void ManagerPage::onProfileRemoval(window_handle_t hWindow)
{
    lang::ConfigLang& langRes = getParentDialog<ConfigDialog>()->getLanguageResource();

    // get selected index(es)
    std::vector<uint32_t> selectedIndexes;
    getDataTable().getSelectedIndexes(selectedIndexes);
    if (selectedIndexes.size() == 0)
    {
        MsgBox::showMessage(langRes.profileManager.msgBoxEmptyTitle, langRes.profileManager.msgBoxEmpty,
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
        // sort by reversed index order
        for (int32_t j = 1; j < static_cast<int32_t>(selectedIndexes.size()); ++j)
        {
            int32_t i = j - 1;
            uint32_t curIndex = selectedIndexes.at(j);
            while (i >= 0 && selectedIndexes.at(i) < curIndex)
            {
                selectedIndexes.at(i + 1) = selectedIndexes.at(i);
                --i;
            }
            selectedIndexes.at(i + 1) = curIndex;
        }
        // remove rows from table + config
        for (uint32_t i = 0; i < selectedIndexes.size(); ++i)
        {
            // update config
            Config::removeProfile(selectedIndexes.at(i));

            // refill data table
            getDataTable().clearRows();
            std::vector<std::wstring>& profileNames = Config::getAllProfileNames();
            std::vector<std::vector<std::wstring>> rowData;
            for (uint32_t pr = 0; pr < profileNames.size(); ++pr)
            {
                rowData.push_back({ std::to_wstring(pr), profileNames.at(pr) });
            }
            getDataTable().insertRows(rowData);
        }
        // refresh profile list + profile pages
        getParentDialog<ConfigDialog>()->onProfileDataUpdate(true, selectedIndexes);
    }
}


/// @brief Profile export event - open dialog
void ManagerPage::onProfileExport(window_handle_t hWindow)
{
    lang::ConfigLang& langRes = getParentDialog<ConfigDialog>()->getLanguageResource();

    // get selected index(es)
    std::vector<uint32_t> selectedIndexes;
    getDataTable().getSelectedIndexes(selectedIndexes);
    if (selectedIndexes.size() == 0)
    {
        MsgBox::showMessage(langRes.profileManager.msgBoxEmptyTitle, langRes.profileManager.msgBoxEmpty,
            hWindow, MsgBox::button_set_t::ok, MsgBox::message_icon_t::warning);
        return;
    }
    // get writable file path
    std::wstring defaultPath = events::utils::FileIO::getWritableFileWidePath();

    controls::Dialog::event_handler_t handlerRef;
    controls::FileDialog exportProfileDialog(m_instance, controls::FileDialog::file_mode_t::savePath);
    handlerRef.handler = onExportProfileDialogInit;
    exportProfileDialog.registerEvent(controls::dialog_event_t::init, handlerRef);
    handlerRef.handler = onExportProfileDialogConfirm;
    exportProfileDialog.registerEvent(controls::dialog_event_t::confirm, handlerRef);
    exportProfileDialog.setParent(getParentDialog<Dialog>());
    // show modal dialog
    ManagerPage* pPrevBuffer = s_pLastParentPage;
    s_pLastParentPage = this;
    exportProfileDialog.showDialog(IDD_EXPORT_DIALOG, IDC_EXPORT_PATH_EDIT, hWindow, IDC_EXPORT_BTN_PATH, defaultPath,
                        getParentDialog<ConfigDialog>()->getLanguageResource().profileManager.btnExport);
    s_pLastParentPage = pPrevBuffer;
}


/// @brief Profile preset choice event
void ManagerPage::onProfilePresetChoice(window_handle_t hWindow)
{
    lang::ConfigLang& langRes = getParentDialog<ConfigDialog>()->getLanguageResource();

    // get selected index(es)
    std::vector<uint32_t> selectedIndexes;
    getDataTable().getSelectedIndexes(selectedIndexes);
    if (selectedIndexes.size() == 0)
    {
        MsgBox::showMessage(langRes.profileManager.msgBoxEmptyTitle, langRes.profileManager.msgBoxEmpty,
                            hWindow, MsgBox::button_set_t::ok, MsgBox::message_icon_t::warning);
        return;
    }

    // confirm preset selection
    std::wstring message = langRes.profileManager.msgBoxPresetConfirm;
    if (selectedIndexes.size() > 1)
        message += L" ("s + std::to_wstring(selectedIndexes.size()) + L")"s;

    if (MsgBox::showMessage(langRes.profileManager.msgBoxPresetConfirmTitle, message, hWindow,
            MsgBox::button_set_t::okCancel, MsgBox::message_icon_t::question) == MsgBox::result_t::ok)
    {
        // get preset choice
        int32_t profilePresetIndex;
        if (ComboBox::getSelectedIndex(hWindow, IDC_MNG_PRESETS_LIST, profilePresetIndex) == false 
         || profilePresetIndex < 0 || profilePresetIndex >= CONFIG_PRESET_LENGTH)
        {
            MsgBox::showMessage(L"Failed to read selection"s, L"Could not read selected preset. No change applied."s,
                                hWindow, MsgBox::button_set_t::ok, MsgBox::message_icon_t::error);
            return;
        }
        // apply preset
        for (uint32_t i = 0; i < selectedIndexes.size(); ++i)
        {
            ConfigProfile* pProfile = Config::getProfile(selectedIndexes.at(i));
            if (pProfile != nullptr)
            {
                pProfile->setPresetValues(static_cast<config::config_preset_t>(profilePresetIndex));
                pProfile->setUpdateFlag();
            }
        }
        // refresh profile pages
        getParentDialog<ConfigDialog>()->onProfileDataUpdate(false, selectedIndexes);
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



// -- sub-dialogs handlers -- --------------------------------------

/// @brief Add profile dialog - Initialization event handler
EVENT_RETURN ManagerPage::onAddProfileDialogInit(controls::Dialog::event_args_t args)
{
    if (s_pLastParentPage == nullptr)
        return EVENT_RETURN_ERROR;

    // translate controls
    lang::ConfigLang& langRes = args.getParent<Dialog>().getParent<ConfigDialog>()->getLanguageResource();
    Label::setText(args.window, IDS_ADD_NAME, langRes.profileManager.profileName);
    CheckBox::setText(args.window, IDC_ADD_PRESET_RADIO, langRes.profileManager.addUsePreset);
    CheckBox::setText(args.window, IDC_ADD_CLONE_RADIO, langRes.profileManager.addCloneProfile);
    Label::setText(args.window, IDS_ADD_POS, langRes.profileManager.choosePosition);
    Label::setText(args.window, IDOK, langRes.dialog.confirm);
    Label::setText(args.window, IDCANCEL, langRes.dialog.cancel);

    // position selector
    std::vector<std::wstring> positions = { langRes.profileManager.endPosition };
    for (uint32_t i = 1; i < Config::countProfiles(); ++i)
        positions.push_back(L"# "s + std::to_wstring(i));
    ComboBox::initValues(args.window, IDC_ADD_POS_LIST, positions, 0);

    // profile preset selector
    CheckBox::setChecked(args.window, IDC_ADD_PRESET_RADIO, true);
    ComboBox::initValues(args.window, IDC_ADD_PRESET_LIST, langRes.profileManager.presets, 1);
    // profile clone selector
    ComboBox::initValues(args.window, IDC_ADD_CLONE_LIST, Config::getAllProfileNames(), 0);
    return EVENT_RETURN_VALID;
}

/// @brief Add profile dialog - Confirm event handler
EVENT_RETURN ManagerPage::onAddProfileDialogConfirm(controls::Dialog::event_args_t args)
{
    if (s_pLastParentPage == nullptr)
    {
        MsgBox::showMessage(L"No parent page reference..."s, L"Could not apply changes."s, args.window, MsgBox::button_set_t::ok, MsgBox::message_icon_t::error);
        return EVENT_RETURN_ERROR;
    }
    ManagerPage& parent = *ManagerPage::s_pLastParentPage;

    // create profile
    ConfigProfile* pProfile = new ConfigProfile(NEW_CONFIG_PROFILE_ID, TextField::getValue(args.window, IDC_ADD_NAME_EDIT));
    if (pProfile == nullptr)
        return EVENT_RETURN_ERROR;
    pProfile->setUpdateFlag();

    if (CheckBox::isChecked(args.window, IDC_ADD_CLONE_RADIO)) // clone existing profile
    {
        int32_t selectedProfile;
        if (ComboBox::getSelectedIndex(args.window, IDC_ADD_CLONE_LIST, selectedProfile) == false 
         || selectedProfile < 0 || static_cast<uint32_t>(selectedProfile) >= Config::countProfiles())
            selectedProfile = 0;
        ConfigProfile* pDataToCopy = Config::getProfile(selectedProfile);
        if (pDataToCopy != nullptr)
            pProfile->clone(*pDataToCopy);
        else
            pProfile->setPresetValues(config::config_preset_t::standard);
    }
    else // use preset
    {
        int32_t selectedPreset;
        if (ComboBox::getSelectedIndex(args.window, IDC_ADD_PRESET_LIST, selectedPreset) == false 
         || selectedPreset < 0 || selectedPreset >= CONFIG_PRESET_LENGTH)
            selectedPreset = 0;
        pProfile->setPresetValues(static_cast<config::config_preset_t>(selectedPreset));
    }

    // update config
    int32_t pos;
    if (ComboBox::getSelectedIndex(args.window, IDC_ADD_POS_LIST, pos) == false 
     || pos < 0 || static_cast<uint32_t>(pos) >= Config::countProfiles())
        pos = PROFILE_INDEX_APPEND;
    Config::insertProfile(pos, pProfile);

    // update data table
    std::vector<std::wstring> row;
    if (pos == PROFILE_INDEX_APPEND) // simply append
    {
        pos = Config::countProfiles() - 1;
        row = { std::to_wstring(pos), pProfile->getProfileName() };
        parent.getDataTable().insertRow(row, -1);
    }
    else // insert at specified index and add offset to next profiles
    {
        row = { std::to_wstring(pos), pProfile->getProfileName() };
        parent.getDataTable().insertRow(row, pos);
        for (uint32_t upos = static_cast<uint32_t>(pos) + 1u; upos < Config::countProfiles(); ++upos)
        {
            row = { std::to_wstring(upos) };
            parent.getDataTable().updateRow(upos, row);
        }
    }

    // refresh profile list
    ConfigDialog& parentDialog = *(parent.getParentDialog<ConfigDialog>());
    parentDialog.onProfileListUpdate(Config::countProfiles() - 1, pos);
    return EVENT_RETURN_VALID;
}


/// @brief Edit profile dialog - Initialization event handler
EVENT_RETURN ManagerPage::onEditProfileDialogInit(controls::Dialog::event_args_t args)
{
    if (s_pLastParentPage == nullptr)
        return EVENT_RETURN_ERROR;

    // translate controls
    lang::ConfigLang& langRes = args.getParent<Dialog>().getParent<ConfigDialog>()->getLanguageResource();
    Label::setText(args.window, IDS_EDT_NAME, langRes.profileManager.profileName);
    Label::setText(args.window, IDS_EDT_POS, langRes.profileManager.choosePosition);
    Label::setText(args.window, IDOK, langRes.dialog.confirm);
    Label::setText(args.window, IDCANCEL, langRes.dialog.cancel);

    // current name
    int32_t index = s_pLastParentPage->getDataTable().getFirstSelectedIndex();
    if (index >= 0 && index < Config::countProfiles() && Config::getProfileName(index) != nullptr)
        TextField::setValue(args.window, IDC_EDT_NAME_EDIT, *(Config::getProfileName(index)));

    // position selector
    std::vector<std::wstring> positions = { langRes.profileManager.endPosition };
    for (uint32_t i = 1; i < Config::countProfiles(); ++i)
        positions.push_back(L"# "s + std::to_wstring(i));
    ComboBox::initValues(args.window, IDC_EDT_POS_LIST, positions, 0);
    return EVENT_RETURN_VALID;
}

/// @brief Edit profile dialog - Confirm event handler
EVENT_RETURN ManagerPage::onEditProfileDialogConfirm(controls::Dialog::event_args_t args)
{
    if (s_pLastParentPage == nullptr)
    {
        MsgBox::showMessage(L"No parent page reference..."s, L"Could not apply changes."s, args.window, MsgBox::button_set_t::ok, MsgBox::message_icon_t::error);
        return EVENT_RETURN_ERROR;
    }
    ManagerPage& parent = *ManagerPage::s_pLastParentPage;

    // get old position + set name
    int32_t oldIndex = s_pLastParentPage->getDataTable().getFirstSelectedIndex();
    if (oldIndex < 1)
        return EVENT_RETURN_ERROR;
    ConfigProfile* pProfile = Config::getProfile(oldIndex);
    if (pProfile == nullptr)
        return EVENT_RETURN_ERROR;
    pProfile->setProfileName(TextField::getValue(args.window, IDC_EDT_NAME_EDIT));
    pProfile->setUpdateFlag();

    // get new position
    int32_t pos;
    if (ComboBox::getSelectedIndex(args.window, IDC_EDT_POS_LIST, pos) == false
        || pos < 0 || static_cast<uint32_t>(pos) >= Config::countProfiles())
        pos = PROFILE_INDEX_APPEND;
    if (pos == PROFILE_INDEX_APPEND) // move at the end
    {
        pos = Config::countProfiles() - 1;
    }

    // update config
    Config::moveProfile(oldIndex, pos);

    // refill data table
    parent.getDataTable().clearRows();
    std::vector<std::wstring>& profileNames = Config::getAllProfileNames();
    std::vector<std::vector<std::wstring>> rowData;
    for (uint32_t pr = 0; pr < profileNames.size(); ++pr)
    {
        rowData.push_back({ std::to_wstring(pr), profileNames.at(pr) });
    }
    parent.getDataTable().insertRows(rowData);

    // refresh profile list
    ConfigDialog& parentDialog = *(parent.getParentDialog<ConfigDialog>());
    parentDialog.onProfileListUpdate(oldIndex, pos);
    return EVENT_RETURN_VALID;
}


/// @brief Import profile dialog - Initialization event handler
EVENT_RETURN ManagerPage::onImportProfileDialogInit(controls::Dialog::event_args_t args)
{
    if (FileDialog::onInit(args) != EVENT_RETURN_VALID || s_pLastParentPage == nullptr)
        return EVENT_RETURN_ERROR;

    //...choix indice placement (combo avec nb 1 à N)
    //...
    //...radio pour insérer nouveau ou remplacer existant (si remplace <default>, le nom de profil du fichier n'est pas pris en compte)
        //-> choix insérer nouveau à position voulue, OU remplacer existant (même position) -> radios et 2 listes (positions et noms profils)
    return EVENT_RETURN_VALID;
}

/// @brief Import profile dialog - Confirm event handler
EVENT_RETURN ManagerPage::onImportProfileDialogConfirm(controls::Dialog::event_args_t args)
{
    if (FileDialog::onConfirm(args) != EVENT_RETURN_VALID)
        return EVENT_RETURN_ERROR;
    if (s_pLastParentPage == nullptr)
    {
        MsgBox::showMessage(L"No parent page reference..."s, L"Could not apply changes."s, args.window, MsgBox::button_set_t::ok, MsgBox::message_icon_t::error);
        return EVENT_RETURN_ERROR;
    }

    // update name and order
    // call ConfigDialog->onProfileListChange(oldIndex, newIndex)

    return EVENT_RETURN_VALID;
}


/// @brief Export profile dialog - Initialization event handler
EVENT_RETURN ManagerPage::onExportProfileDialogInit(controls::Dialog::event_args_t args)
{
    if (FileDialog::onInit(args) != EVENT_RETURN_VALID || s_pLastParentPage == nullptr)
        return EVENT_RETURN_ERROR;

    //...seulement choix de path, pas du nom des fichiers (potentiellement plusieurs)
    return EVENT_RETURN_VALID;
}

/// @brief Export profile dialog - Confirm event handler
EVENT_RETURN ManagerPage::onExportProfileDialogConfirm(controls::Dialog::event_args_t args)
{
    if (FileDialog::onConfirm(args) != EVENT_RETURN_VALID)
        return EVENT_RETURN_ERROR;
    if (s_pLastParentPage == nullptr)
    {
        MsgBox::showMessage(L"No parent page reference..."s, L"Could not obtain selection from list."s, args.window, MsgBox::button_set_t::ok, MsgBox::message_icon_t::error);
        return EVENT_RETURN_ERROR;
    }


    return EVENT_RETURN_VALID;
}
