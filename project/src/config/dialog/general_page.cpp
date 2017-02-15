/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab page - general settings
*******************************************************************************/
#include "../../globals.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <unordered_set>
#include "../../lang/config_lang.h"
#include "../../res/resource.h"
#include "../../events/utils/logger.h"
#include "controls/common.h"
#include "controls/text_field.hpp"
#include "controls/group_box.h"
#include "controls/combo_box.h"
#include "controls/check_box.hpp"
#include "controls/label.hpp"
#include "controls/button.hpp"
#include "controls/screen.h"
#include "controls/keyboard.h"
#include "controls/msg_box.h"
#include "config_dialog.h"
#include "controls/tab_page.h"
#include "general_page.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Create tab page - general settings
/// @param[in] instance       Current instance handle
/// @param[in] pParentDialog  Parent dialog reference
GeneralPage::GeneralPage(controls::library_instance_t instance, controls::Dialog* pParentDialog)
    : controls::TabPage(instance, pParentDialog, IDD_GENERAL_PAGE) 
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
DIALOG_EVENT_RETURN GeneralPage::onInit(PAGE_EVENT_HANDLER_ARGUMENTS)
{
    // set labels
    GeneralPage& parent = getEventTargetPageReference(GeneralPage);
    parent.onLanguageChange(false);

    // set list of screen resolutions
    std::vector<std::wstring> fullscreenResList;
    int32_t resIndex = Screen::listAvailableResolutions(fullscreenResList, Config::display.fullscreenRes.x, Config::display.fullscreenRes.y);
    ComboBox::initValues(getEventWindowHandle(), IDC_GEN_FULLRES_LIST, fullscreenResList, resIndex);
    std::vector<std::wstring> bitDepths { L"16-bit"s, L"32-bit"s };
    ComboBox::initValues(getEventWindowHandle(), IDC_GEN_COLOR_LIST, bitDepths, (Config::display.colorDepth == display::window_color_mode_t::rgb_32bit) ? 1 : 0);

    // apply other config settings
    CheckBox::setChecked(getEventWindowHandle(), (Config::display.windowMode == display::utils::window_mode_t::fullscreen) ? IDC_GEN_FULLRES : IDC_GEN_WINRES, true);
    if (Config::display.windowMode == display::utils::window_mode_t::resizable)
        CheckBox::setChecked(getEventWindowHandle(), IDC_GEN_WINSIZE_CHECK, true);
    //...read config -> set controls
    //...

    return DIALOG_EVENT_RETURN_VALID;
}


/// @brief Sub-control command event handler
DIALOG_EVENT_RETURN GeneralPage::onCommand(PAGE_EVENT_HANDLER_ARGUMENTS)
{
    if (eventActionEquals(Button::event_t::clicked))
    {
        try
        {
            controls::Dialog::event_handler_t handlerRef;
            GeneralPage& parent = getEventTargetPageReference(GeneralPage);
            switch (getEventTargetControlId())
            {
                //open sub-dialogs
                case IDC_GEN_BTN_KEYBINDING: // control key binding
                {
                    controls::Dialog keyBindDialog(parent.m_instance);
                    handlerRef.handler = onKeyBindDialogInit;
                    keyBindDialog.registerEvent(controls::dialog_event_t::init, handlerRef);
                    handlerRef.handler = onKeyBindDialogConfirm;
                    keyBindDialog.registerEvent(controls::dialog_event_t::confirm, handlerRef);
                    // show modal dialog
                    keyBindDialog.showDialog(IDD_KEYBINDING_DIALOG, getEventWindowHandle(), false);

                    Button::unHighlight(hWindow, IDC_GEN_BTN_KEYBINDING);
                    return DIALOG_EVENT_RETURN_VALID; break;
                }
                case IDC_GEN_BTN_ADVANCED: // advanced settings
                {
                    Dialog advancedDialog(parent.m_instance);
                    handlerRef.handler = onAdvancedDialogInit;
                    advancedDialog.registerEvent(controls::dialog_event_t::init, handlerRef);
                    handlerRef.handler = onAdvancedDialogConfirm;
                    advancedDialog.registerEvent(controls::dialog_event_t::confirm, handlerRef);
                    // show modal dialog
                    advancedDialog.showDialog(IDD_ADVANCED_DIALOG, getEventWindowHandle(), false);

                    Button::unHighlight(hWindow, IDC_GEN_BTN_ADVANCED);
                    return DIALOG_EVENT_RETURN_VALID; break;
                }
            }
        }
        catch (...)
        {
            MsgBox::showMessage(L"Dialog error"s, L"Could not open sub-dialog."s, getEventWindowHandle(), MsgBox::button_set_t::ok, MsgBox::message_icon_t::error);
            return DIALOG_EVENT_RETURN_ERROR;
        }
    }
    return DIALOG_EVENT_RETURN_ERROR;
}



// -- specialized handlers -- --------------------------------------

/// @brief Language change event
/// @param[in] isRecursive    Also translate controls in child pages or not
void GeneralPage::onLanguageChange(const bool isRecursive)
{
    window_handle_t hPage = getPageHandle();
    lang::ConfigLang& langRes = getParentDialog<ConfigDialog>()->getLanguageResource();

    GroupBox::setText(hPage, IDS_GEN_GROUP1, langRes.generalSettings.groupDisplay);
    //...
    GroupBox::setText(hPage, IDS_GEN_GROUP2, langRes.generalSettings.groupTimer);
    //...
}


/// @brief Initialize key binding sub-dialog
DIALOG_EVENT_RETURN GeneralPage::onKeyBindDialogInit(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    // fill lists of keys
    Keyboard keys;
    ComboBox::initValues(getEventWindowHandle(), IDS_KEY_0_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[0]));
    ComboBox::initValues(getEventWindowHandle(), IDS_KEY_1_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[1]));
    ComboBox::initValues(getEventWindowHandle(), IDS_KEY_2_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[2]));
    ComboBox::initValues(getEventWindowHandle(), IDS_KEY_3_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[3]));
    ComboBox::initValues(getEventWindowHandle(), IDS_KEY_4_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[4]));
    ComboBox::initValues(getEventWindowHandle(), IDS_KEY_5_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[5]));
    ComboBox::initValues(getEventWindowHandle(), IDS_KEY_6_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[6]));
    ComboBox::initValues(getEventWindowHandle(), IDS_KEY_7_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[7]));
    ComboBox::initValues(getEventWindowHandle(), IDS_KEY_8_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[8]));
    ComboBox::initValues(getEventWindowHandle(), IDS_KEY_9_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[9]));

    //...translate controls
    //...
    return DIALOG_EVENT_RETURN_VALID;
}

/// @brief Confirm key binding sub-dialog settings
DIALOG_EVENT_RETURN GeneralPage::onKeyBindDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    // read key selections
    int32_t bufferArray[EVENT_KEYS_STRING_LENGTH - 1];
    if (ComboBox::getSelectedIndex(getEventWindowHandle(), IDS_KEY_0_LIST, bufferArray[0]) == false) bufferArray[0] = 0;
    if (ComboBox::getSelectedIndex(getEventWindowHandle(), IDS_KEY_1_LIST, bufferArray[1]) == false) bufferArray[1] = 0;
    if (ComboBox::getSelectedIndex(getEventWindowHandle(), IDS_KEY_2_LIST, bufferArray[2]) == false) bufferArray[2] = 0;
    if (ComboBox::getSelectedIndex(getEventWindowHandle(), IDS_KEY_3_LIST, bufferArray[3]) == false) bufferArray[3] = 0;
    if (ComboBox::getSelectedIndex(getEventWindowHandle(), IDS_KEY_4_LIST, bufferArray[4]) == false) bufferArray[4] = 0;
    if (ComboBox::getSelectedIndex(getEventWindowHandle(), IDS_KEY_5_LIST, bufferArray[5]) == false) bufferArray[5] = 0;
    if (ComboBox::getSelectedIndex(getEventWindowHandle(), IDS_KEY_6_LIST, bufferArray[6]) == false) bufferArray[6] = 0;
    if (ComboBox::getSelectedIndex(getEventWindowHandle(), IDS_KEY_7_LIST, bufferArray[7]) == false) bufferArray[7] = 0;
    if (ComboBox::getSelectedIndex(getEventWindowHandle(), IDS_KEY_8_LIST, bufferArray[8]) == false) bufferArray[8] = 0;
    if (ComboBox::getSelectedIndex(getEventWindowHandle(), IDS_KEY_9_LIST, bufferArray[9]) == false) bufferArray[9] = 0;

    // check if same key is not used more than once
    std::unordered_set<int32_t> checkerSet;
    for (int32_t i = 0; i < EVENT_KEYS_STRING_LENGTH - 1; ++i)
    {
        if (checkerSet.count(bufferArray[i]))
        {
            if (bufferArray[i] != 0) // only VK_NOKEY (<none>) can be repeated
            {
                MsgBox::showMessage(L"Invalid settings"s, L"Each key can only be used once (except \"<none>\")."s, 
                    getEventWindowHandle(), MsgBox::button_set_t::ok, MsgBox::message_icon_t::warning);
                return DIALOG_EVENT_RETURN_ERROR;
            }
        }
        else
            checkerSet.insert(bufferArray[i]);
    }

    // update config
    Keyboard keys(false);
    for (int32_t i = 0; i < EVENT_KEYS_STRING_LENGTH - 1; ++i)
        Config::events.pTriggerKeys[i] = static_cast<char>(keys.indexToKeyCode(bufferArray[i]));
    return DIALOG_EVENT_RETURN_VALID;
}


/// @brief Initialize advanced settings sub-dialog
DIALOG_EVENT_RETURN GeneralPage::onAdvancedDialogInit(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    //...use settings from Config
    //...

    //...translate controls
    //...
    return DIALOG_EVENT_RETURN_VALID;
}

/// @brief Confirm advanced settings sub-dialog settings
DIALOG_EVENT_RETURN GeneralPage::onAdvancedDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    //...copy settings in Config
    //...
    return DIALOG_EVENT_RETURN_VALID;
}
