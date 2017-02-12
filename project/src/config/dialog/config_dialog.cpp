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
#include "../../lang/config_lang.h"
#include "../../res/resource.h"
#include "../../events/utils/logger.h"
#include "controls/common.h"
#include "controls/dialog.h"
#include "controls/file_dialog.h"
#include "controls/tab_control.h"
#include "controls/combo_box.h"
#include "controls/label.hpp"
#include "controls/msg_box.h"
#include "general_page.h"
#include "manager_page.h"
#include "profile_page.h"
#include "config_dialog.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Create dialog box
/// @param[in] instance  Current instance handle
/// @throws runtime_error     Dialog creation error
/// @throws invalid_argument  Invalid instance
ConfigDialog::ConfigDialog(library_instance_t instance) : Dialog(instance)
{
    // load config values
    config::Config::init();
    try
    {
        m_languageResource.setLanguage(config::Config::langCode, config::Config::langFilePath);
    }
    catch (...)
    {
        config::Config::langCode = lang::langcode_t::english;
    }

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
Dialog::result_t ConfigDialog::showDialog()
{
    return Dialog::showDialog(IDD_CONFIG_DIALOG, true);
}



// -- event handlers -- --------------------------------------------

/// @brief Initialization event handler
DIALOG_EVENT_RETURN ConfigDialog::onInit(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    ConfigDialog& parent = getEventTargetDialogReference(ConfigDialog);

    // set list of profiles
    ComboBox::initValues(getEventWindowHandle(), IDC_PROFILE_LIST, Config::getAllProfileNames(), 0u);
    Label::setVisible(hWindow, IDC_PROFILE_LIST, false); // hide (only visible when profile tab is active)
    Label::setVisible(hWindow, IDS_PROFILE, false);
    // translate main dialog buttons
    Label::setText(hWindow, IDS_PROFILE, parent.m_languageResource.profile.profileList);
    Label::setText(hWindow, IDOK, parent.m_languageResource.dialog.confirm);
    Label::setText(hWindow, IDCANCEL, parent.m_languageResource.dialog.cancel);

    // set language selector
    std::vector<std::wstring> langNames;
    uint32_t langNumber = static_cast<uint32_t>(LANGCODE_LAST_INTERNAL) + 1; // built-in languages + external
    for (uint32_t i = 0u; i <= langNumber; ++i)
        langNames.push_back(lang::langcodeNames[i]);
    ComboBox::initValues(getEventWindowHandle(), IDC_LANG_LIST, langNames, static_cast<int32_t>(Config::langCode));

    // add tab control and pages to dialog
    if (parent.m_tabs.create(getEventWindowHandle(), LOGO_HEIGHT + 2, LOGO_WIDTH) == false)
        return DIALOG_EVENT_RETURN_ERROR;
    return DIALOG_EVENT_RETURN_VALID;
}


/// @brief Paint event handler - draw dialog
DIALOG_EVENT_RETURN ConfigDialog::onPaint(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    ConfigDialog& parent = getEventTargetDialogReference(ConfigDialog);

    //...

    return DIALOG_EVENT_RETURN_VALID;
}


/// @brief Sub-control drawing event handler - draw component
DIALOG_EVENT_RETURN ConfigDialog::onDrawItem(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    ConfigDialog& parent = getEventTargetDialogReference(ConfigDialog);

    //...

    return DIALOG_EVENT_RETURN_VALID;
}


/// @brief Sub-control command event handler
DIALOG_EVENT_RETURN ConfigDialog::onCommand(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    // language change
    if (getEventTargetControlId() == IDC_LANG_LIST && eventActionEquals(ComboBox::event_t::selectionChanged))
    {
        int32_t buffer;
        if (ComboBox::getSelectedIndex(getEventWindowHandle(), IDC_LANG_LIST, buffer))
        {
            ConfigDialog& parent = getEventTargetDialogReference(ConfigDialog);
            if (parent.onLanguageChange(DIALOG_EVENT_HANDLER_ARGUMENTS_VALUES, buffer))
            {
                // translate window components
                Label::setText(hWindow, IDS_PROFILE, parent.m_languageResource.profile.profileList);
                Label::setText(hWindow, IDOK, parent.m_languageResource.dialog.confirm);
                Label::setText(hWindow, IDCANCEL, parent.m_languageResource.dialog.cancel);
                //...force to repaint tab buttons
                //...
                //...force to translate tab pages
                //...
            }
            return DIALOG_EVENT_RETURN_ERROR;
        }
        else
        {
            MsgBox::showMessage(L"An error occurred during selection. Please try again."s, L"Language selection error"s, 
                                getEventWindowHandle(), MsgBox::button_set_t::ok, MsgBox::message_icon_t::error);
            return DIALOG_EVENT_RETURN_ERROR;
        }
    }
    return DIALOG_EVENT_RETURN_ERROR;
}


/// @brief Dialog confirm event handler - check validity
DIALOG_EVENT_RETURN ConfigDialog::onConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    ConfigDialog& parent = getEventTargetDialogReference(ConfigDialog);

    //...check values
    //...
    //...save values
    //...

    return DIALOG_EVENT_RETURN_VALID;
}



// -- specialized handlers -- --------------------------------------

/// @brief Language change event
/// @param[in] value  Selected value
/// @returns Language validity / file validity
bool ConfigDialog::onLanguageChange(DIALOG_EVENT_HANDLER_ARGUMENTS, const int32_t value)
{
    // select custom language file
    if (value > static_cast<int32_t>(LANGCODE_LAST_INTERNAL))
    {
        try
        {
            // open file path choice dialog
            FileDialog fileDialog(static_cast<config::dialog::controls::library_instance_t>(m_instance), FileDialog::file_mode_t::load);
            if (fileDialog.showDialog(IDD_IMPORT_DIALOG, IDC_FILE_PATH_EDIT, IDC_FILE_BTN_PATH, Config::langFilePath) == Dialog::result_t::confirm) // file choice success
            {
                Config::langCode = lang::langcode_t::customFile;
                Config::langFilePath = fileDialog.getFilePath();
            }

            // if cancelled or error, restore previous language selection
            if (Config::langCode != lang::langcode_t::customFile)
            {
                ComboBox::setSelectedIndex(getEventWindowHandle(), IDC_LANG_LIST, static_cast<int32_t>(Config::langCode));
                return true;
            }
        }
        catch (const std::exception& exc)
        {
            events::utils::Logger::getInstance()->writeErrorEntry("ConfigDialog.onCommand"s, exc.what());
            return false;
        }
    }
    else // built-in language
    {
        Config::langCode = static_cast<lang::langcode_t>(value);
    }

    try
    {
        // update language resource
        m_languageResource.setLanguage(Config::langCode, Config::langFilePath);
    }
    catch (const std::exception& exc) // corrupted file
    {
        Config::langCode = lang::langcode_t::english;
        ComboBox::setSelectedIndex(hWindow, IDC_LANG_LIST, static_cast<int32_t>(Config::langCode));
        MsgBox::showMessage(L"Invalid language file"s, L"Invalid / corrupted language file"s,
            hWindow, MsgBox::button_set_t::ok, MsgBox::message_icon_t::error);
        return false;
    }
    return true;
}
