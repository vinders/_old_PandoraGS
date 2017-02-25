/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab page - general settings
*******************************************************************************/
#include "../../globals.h"
#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <functional>
#include <unordered_set>
#include "../../lang/config_lang.h"
#include "../../res/resource.h"
#include "../../events/utils/logger.h"
#include "controls/common.h"
#include "controls/text_field.hpp"
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

std::vector<controls::resolution_t> GeneralPage::m_resolutions; ///< Available resolutions


/// @brief Create tab page - general settings
/// @param[in] instance       Current instance handle
/// @param[in] pParentDialog  Parent dialog reference
GeneralPage::GeneralPage(controls::library_instance_t instance, controls::Dialog* pParentDialog)
    : controls::TabPage(instance, pParentDialog, IDD_GENERAL_PAGE) 
{
    // set event handlers
    TabPage::event_handler_t eventHandler;
    eventHandler.handler = onInit;
    TabPage::registerEvent(dialog_event_t::init, eventHandler);
    eventHandler.handler = onCommand;
    TabPage::registerEvent(dialog_event_t::command, eventHandler);
}

/// @brief Close tab page control - overridable method
void GeneralPage::overridableClose()
{
    TabPage::unregisterEvent(dialog_event_t::command);
    TabPage::unregisterEvent(dialog_event_t::init);
    getResolutions().clear();
}



// -- event handlers -- --------------------------------------------

/// @brief Initialization event handler
EVENT_RETURN GeneralPage::onInit(TabPage::event_args_t args)
{
    GeneralPage& parent = args.getParent<GeneralPage>();

    // translate controls/labels
    parent.m_hPage = args.window;
    parent.onLanguageChange(false);

    // set list of screen resolutions
    int32_t resIndex = Screen::listAvailableResolutions(GeneralPage::getResolutions(), Config::display.fullscreenRes.x, Config::display.fullscreenRes.y);
    std::vector<std::wstring> formattedResolutions;
    formattedResolutions.reserve(GeneralPage::getResolutions().size());
    formattedResolutions.push_back(parent.getParentDialog<ConfigDialog>()->getLanguageResource().generalSettings.desktopRes);
    for (uint32_t i = 1; i < GeneralPage::getResolutions().size(); ++i)
        formattedResolutions.push_back(Screen::formatResolutionString(GeneralPage::getResolutions().at(i).x, GeneralPage::getResolutions().at(i).y));
    ComboBox::initValues(args.window, IDC_GEN_FULLRES_LIST, formattedResolutions, resIndex);

    // apply other config settings
    std::vector<std::wstring> bitDepths { L"16-bit"s, L"32-bit"s };
    ComboBox::initValues(args.window, IDC_GEN_COLOR_LIST, bitDepths, (Config::display.colorDepth == display::window_color_mode_t::rgb_32bit) ? 1 : 0);
    CheckBox::setChecked(args.window, (Config::display.windowMode == display::utils::window_mode_t::fullscreen) ? IDC_GEN_FULLRES : IDC_GEN_WINRES, true);
    TextField::setValue(args.window, IDC_GEN_WINRESX_EDIT, std::to_wstring(Config::display.windowRes.x));
    TextField::setValue(args.window, IDC_GEN_WINRESY_EDIT, std::to_wstring(Config::display.windowRes.y));
    if (Config::display.windowMode == display::utils::window_mode_t::resizable)
        CheckBox::setChecked(args.window, IDC_GEN_WINSIZE_CHECK, true);
    if (Config::display.subprecisionMode != config::subprecision_settings_t::disabled)
        CheckBox::setChecked(args.window, IDC_GEN_GTEACC_CHECK, true);
    if (Config::events.isNoScreenSaver)
        CheckBox::setChecked(args.window, IDC_GEN_SCNSAVER_CHECK, true);
    if (Config::timer.frameLimitMode != config::framelimit_settings_t::disabled)
    {
        CheckBox::setChecked(args.window, IDC_GEN_FRAMELIMIT_CHECK, true);
        if (Config::timer.frameLimitMode == config::framelimit_settings_t::limitSkip)
            CheckBox::setChecked(args.window, IDC_GEN_FRAMESKIP_CHECK, true);
    }
    if (Config::timer.frameRateLimit == 0.0f)
    {
        CheckBox::setChecked(args.window, IDC_GEN_FPSAUTO_RADIO, true);
        TextField::setValue(args.window, IDC_GEN_FRAMELIMIT_EDIT, L"59.94"s);
    }
    else
    {
        CheckBox::setChecked(args.window, IDC_GEN_FPSFIXED_RADIO, true);
        std::wstringstream frameRateLimitStr;
        frameRateLimitStr << Config::timer.frameRateLimit;
        TextField::setValue(args.window, IDC_GEN_FRAMELIMIT_EDIT, frameRateLimitStr.str());
    }
    return EVENT_RETURN_VALID;
}


/// @brief Sub-control command event handler
EVENT_RETURN GeneralPage::onCommand(TabPage::event_args_t args)
{
    if (args.isEventAction(Button::event_t::clicked))
    {
        try
        {
            controls::Dialog::event_handler_t handlerRef;
            GeneralPage& parent = args.getParent<GeneralPage>();
            switch (args.controlId())
            {
                //open sub-dialogs
                case IDC_GEN_BTN_KEYBINDING: // control key binding
                {
                    controls::Dialog keyBindDialog(parent.m_instance);
                    handlerRef.handler = onKeyBindDialogInit;
                    keyBindDialog.registerEvent(controls::dialog_event_t::init, handlerRef);
                    handlerRef.handler = onKeyBindDialogConfirm;
                    keyBindDialog.registerEvent(controls::dialog_event_t::confirm, handlerRef);
                    keyBindDialog.setParent(parent.getParentDialog<Dialog>());
                    // show modal dialog
                    keyBindDialog.showDialog(IDD_KEYBINDING_DIALOG, args.window, 
                                             parent.getParentDialog<ConfigDialog>()->getLanguageResource().generalSettings.btnKeyBinding, false);

                    Button::unHighlight(args.window, IDC_GEN_BTN_KEYBINDING);
                    return EVENT_RETURN_VALID; break;
                }
                case IDC_GEN_BTN_ADVANCED: // advanced settings
                {
                    Dialog advancedDialog(parent.m_instance);
                    handlerRef.handler = onAdvancedDialogInit;
                    advancedDialog.registerEvent(controls::dialog_event_t::init, handlerRef);
                    handlerRef.handler = onAdvancedDialogConfirm;
                    advancedDialog.registerEvent(controls::dialog_event_t::confirm, handlerRef);
                    advancedDialog.setParent(parent.getParentDialog<Dialog>());
                    // show modal dialog
                    advancedDialog.showDialog(IDD_ADVANCED_DIALOG, args.window, 
                                              parent.getParentDialog<ConfigDialog>()->getLanguageResource().generalSettings.btnAdvanced, false);

                    Button::unHighlight(args.window, IDC_GEN_BTN_ADVANCED);
                    return EVENT_RETURN_VALID; break;
                }
            }
        }
        catch (...)
        {
            MsgBox::showMessage(L"Dialog error"s, L"Could not open sub-dialog."s, args.window, MsgBox::button_set_t::ok, MsgBox::message_icon_t::error);
            return EVENT_RETURN_ERROR;
        }
    }
    return EVENT_RETURN_IGNORE;
}



// -- specialized handlers -- --------------------------------------

/// @brief Language change event
/// @returns Validity
bool GeneralPage::onDialogConfirm(Dialog::event_args_t& args)
{
    // update config
    int32_t indexBuffer = 0;

    // display settings
    if (CheckBox::isChecked(args.window, IDC_GEN_WINRES))
    {
        if (CheckBox::isChecked(args.window, IDC_GEN_WINSIZE_CHECK))
            Config::display.windowMode = display::utils::window_mode_t::resizable;
        else
            Config::display.windowMode = display::utils::window_mode_t::fixed;
    }
    else
    {
        Config::display.windowMode = display::utils::window_mode_t::fullscreen;
    }
    // fullscreen
    if (ComboBox::getSelectedIndex(args.window, IDC_GEN_FULLRES_LIST, indexBuffer))
    {
        if (indexBuffer <= 0 || indexBuffer >= static_cast<int32_t>(GeneralPage::getResolutions().size()))
        {
            Config::display.fullscreenRes.x = RESOLUTION_AUTODETECT;
            Config::display.fullscreenRes.y = RESOLUTION_AUTODETECT;
        }
        else
        {
            Config::display.fullscreenRes.x = GeneralPage::getResolutions().at(indexBuffer).x;
            Config::display.fullscreenRes.y = GeneralPage::getResolutions().at(indexBuffer).y;
        }
    }
    // window
    Config::display.windowRes.x = Screen::parseDimension(TextField::getValue(args.window, IDC_GEN_WINRESX_EDIT), 800u);
    Config::display.windowRes.y = Screen::parseDimension(TextField::getValue(args.window, IDC_GEN_WINRESX_EDIT), 600u);
    // other display settings
    if (ComboBox::getSelectedIndex(args.window, IDC_GEN_COLOR_LIST, indexBuffer))
    {
        Config::display.colorDepth = (indexBuffer != 0) ? (display::window_color_mode_t::rgb_32bit) : (display::window_color_mode_t::rgb_16bit);
    }
    Config::display.subprecisionMode = (CheckBox::isChecked(args.window, IDC_GEN_GTEACC_CHECK)) ? (config::subprecision_settings_t::standard) : (config::subprecision_settings_t::disabled);
    Config::events.isNoScreenSaver = CheckBox::isChecked(args.window, IDC_GEN_SCNSAVER_CHECK);
    
    // timer settings
    if (CheckBox::isChecked(args.window, IDC_GEN_FRAMELIMIT_CHECK))
    {
        if (CheckBox::isChecked(args.window, IDC_GEN_FRAMESKIP_CHECK))
            Config::timer.frameLimitMode = config::framelimit_settings_t::limitSkip;
        else
            Config::timer.frameLimitMode = config::framelimit_settings_t::limit;
    }
    else
        Config::timer.frameLimitMode = config::framelimit_settings_t::disabled;
    // frame rate limit
    if (CheckBox::isChecked(args.window, IDC_GEN_FPSFIXED_RADIO))
    {
        std::wstring buffer = TextField::getValue(args.window, IDC_GEN_FRAMELIMIT_EDIT);
        if (buffer.empty() == false)
        {
            try
            {
                size_t pos = buffer.find_first_not_of(L' '); // remove leading spaces
                if (pos > 0 && pos != std::wstring::npos)
                    buffer = buffer.substr(pos);
                pos = buffer.find_first_of(L','); // convert comma to floating point
                if (pos > 0 && pos != std::wstring::npos)
                    buffer.at(pos) = L'.';
                Config::timer.frameRateLimit = std::stof(buffer); // string to float
                if (Config::timer.frameRateLimit < 10.0f) // min
                    Config::timer.frameRateLimit = 10.0f;
                else if (Config::timer.frameRateLimit > 200.0f) // max
                    Config::timer.frameRateLimit = 200.0f;
            }
            catch (...) {}
        }
    }
    else
        Config::timer.frameRateLimit = 0.0f;

    return true;
}


/// @brief Language change event
/// @param[in] isUpdate  Set to false to initialize controls
void GeneralPage::onLanguageChange(const bool isUpdate)
{
    window_handle_t hPage = getPageHandle();
    lang::ConfigLang& langRes = getParentDialog<ConfigDialog>()->getLanguageResource();

    if (GeneralPage::getResolutions().empty() == false)
    {
        std::vector<std::wstring> formattedResolutions;
        formattedResolutions.reserve(GeneralPage::getResolutions().size());
        formattedResolutions.push_back(langRes.generalSettings.desktopRes);
        for (uint32_t i = 1; i < GeneralPage::getResolutions().size(); ++i)
            formattedResolutions.push_back(Screen::formatResolutionString(GeneralPage::getResolutions().at(i).x, GeneralPage::getResolutions().at(i).y));
        ComboBox::setValues(hPage, IDC_GEN_FULLRES_LIST, formattedResolutions);
    }
    Label::setText(hPage, IDS_GEN_GROUP1, langRes.generalSettings.groupDisplay);
    Label::setText(hPage, IDS_GEN_RESOLUTION, langRes.generalSettings.resolution);
    CheckBox::setText(hPage, IDC_GEN_FULLRES, langRes.generalSettings.fullscreenRes);
    CheckBox::setText(hPage, IDC_GEN_WINRES, langRes.generalSettings.windowRes);
    CheckBox::setText(hPage, IDC_GEN_WINSIZE_CHECK, langRes.generalSettings.resizable);
    Label::setText(hPage, IDS_GEN_COLOR, langRes.generalSettings.colorDepth);
    CheckBox::setText(hPage, IDC_GEN_GTEACC_CHECK, langRes.generalSettings.subprecision);
    CheckBox::setText(hPage, IDC_GEN_SCNSAVER_CHECK, langRes.generalSettings.noScreenSaver);
    Label::setText(hPage, IDS_GEN_GROUP2, langRes.generalSettings.groupTimer);
    CheckBox::setText(hPage, IDC_GEN_FRAMELIMIT_CHECK, langRes.generalSettings.timerLimit);
    CheckBox::setText(hPage, IDC_GEN_FPSAUTO_RADIO, langRes.generalSettings.timerLimitAuto);
    CheckBox::setText(hPage, IDC_GEN_FPSFIXED_RADIO, langRes.generalSettings.timerLimitFixed);
    CheckBox::setText(hPage, IDC_GEN_FRAMESKIP_CHECK, langRes.generalSettings.timerSkipping);
    Button::setText(hPage, IDC_GEN_BTN_KEYBINDING, langRes.generalSettings.btnKeyBinding);
    Button::setText(hPage, IDC_GEN_BTN_ADVANCED, langRes.generalSettings.btnAdvanced);
}


/// @brief Initialize key binding sub-dialog
EVENT_RETURN GeneralPage::onKeyBindDialogInit(Dialog::event_args_t args)
{
    // fill lists of keys
    Keyboard keys;
    ComboBox::initValues(args.window, IDS_KEY_0_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[0]));
    ComboBox::initValues(args.window, IDS_KEY_1_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[1]));
    ComboBox::initValues(args.window, IDS_KEY_2_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[2]));
    ComboBox::initValues(args.window, IDS_KEY_3_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[3]));
    ComboBox::initValues(args.window, IDS_KEY_4_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[4]));
    ComboBox::initValues(args.window, IDS_KEY_5_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[5]));
    ComboBox::initValues(args.window, IDS_KEY_6_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[6]));
    ComboBox::initValues(args.window, IDS_KEY_7_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[7]));
    ComboBox::initValues(args.window, IDS_KEY_8_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[8]));
    ComboBox::initValues(args.window, IDS_KEY_9_LIST, keys.getKeyNames(), keys.keyCodeToIndex(Config::events.pTriggerKeys[9]));
    CheckBox::setChecked(args.window, IDC_KEY_WINDOWMODE_CHECK, Config::events.isWindowModeChangeable);

    // translate controls/labels
    controls::Dialog& parent = args.getParent<controls::Dialog>();
    if (parent.getParent<ConfigDialog>() != nullptr)
    {
        lang::ConfigLang& langRes = parent.getParent<ConfigDialog>()->getLanguageResource();

        Button::setText(args.window, IDS_KEY_0, langRes.keyBindingSettings.keyTitle0);
        Button::setText(args.window, IDS_KEY_0_DESC, langRes.keyBindingSettings.keyDescription0);
        Button::setText(args.window, IDS_KEY_1, langRes.keyBindingSettings.keyTitle1);
        Button::setText(args.window, IDS_KEY_1_DESC, langRes.keyBindingSettings.keyDescription1);
        Button::setText(args.window, IDS_KEY_2, langRes.keyBindingSettings.keyTitle2);
        Button::setText(args.window, IDS_KEY_2_DESC, langRes.keyBindingSettings.keyDescription2);
        Button::setText(args.window, IDS_KEY_3, langRes.keyBindingSettings.keyTitle3);
        Button::setText(args.window, IDS_KEY_3_DESC, langRes.keyBindingSettings.keyDescription3);
        Button::setText(args.window, IDS_KEY_4, langRes.keyBindingSettings.keyTitle4);
        Button::setText(args.window, IDS_KEY_4_DESC, langRes.keyBindingSettings.keyDescription4);
        Button::setText(args.window, IDS_KEY_5, langRes.keyBindingSettings.keyTitle5);
        Button::setText(args.window, IDS_KEY_5_DESC, langRes.keyBindingSettings.keyDescription5);
        Button::setText(args.window, IDS_KEY_6, langRes.keyBindingSettings.keyTitle6);
        Button::setText(args.window, IDS_KEY_6_DESC, langRes.keyBindingSettings.keyDescription6);
        Button::setText(args.window, IDS_KEY_7, langRes.keyBindingSettings.keyTitle7);
        Button::setText(args.window, IDS_KEY_7_DESC, langRes.keyBindingSettings.keyDescription7);
        Button::setText(args.window, IDS_KEY_8, langRes.keyBindingSettings.keyTitle8);
        Button::setText(args.window, IDS_KEY_8_DESC, langRes.keyBindingSettings.keyDescription8);
        Button::setText(args.window, IDS_KEY_9, langRes.keyBindingSettings.keyTitle9);
        Button::setText(args.window, IDS_KEY_9_DESC, langRes.keyBindingSettings.keyDescription9);
        Button::setText(args.window, IDC_KEY_WINDOWMODE_CHECK, langRes.keyBindingSettings.backspace);

        Button::setText(args.window, IDOK, langRes.dialog.confirm);
        Button::setText(args.window, IDCANCEL, langRes.dialog.cancel);
    }
    return EVENT_RETURN_VALID;
}

/// @brief Confirm key binding sub-dialog settings
EVENT_RETURN GeneralPage::onKeyBindDialogConfirm(Dialog::event_args_t args)
{
    Keyboard keys(false);
    int32_t noKeyIndex = keys.getNoKeyIndex();

    // read key selections
    int32_t bufferArray[EVENT_KEYS_STRING_LENGTH - 1];
    if (ComboBox::getSelectedIndex(args.window, IDS_KEY_0_LIST, bufferArray[0]) == false) bufferArray[0] = noKeyIndex;
    if (ComboBox::getSelectedIndex(args.window, IDS_KEY_1_LIST, bufferArray[1]) == false) bufferArray[1] = noKeyIndex;
    if (ComboBox::getSelectedIndex(args.window, IDS_KEY_2_LIST, bufferArray[2]) == false) bufferArray[2] = noKeyIndex;
    if (ComboBox::getSelectedIndex(args.window, IDS_KEY_3_LIST, bufferArray[3]) == false) bufferArray[3] = noKeyIndex;
    if (ComboBox::getSelectedIndex(args.window, IDS_KEY_4_LIST, bufferArray[4]) == false) bufferArray[4] = noKeyIndex;
    if (ComboBox::getSelectedIndex(args.window, IDS_KEY_5_LIST, bufferArray[5]) == false) bufferArray[5] = noKeyIndex;
    if (ComboBox::getSelectedIndex(args.window, IDS_KEY_6_LIST, bufferArray[6]) == false) bufferArray[6] = noKeyIndex;
    if (ComboBox::getSelectedIndex(args.window, IDS_KEY_7_LIST, bufferArray[7]) == false) bufferArray[7] = noKeyIndex;
    if (ComboBox::getSelectedIndex(args.window, IDS_KEY_8_LIST, bufferArray[8]) == false) bufferArray[8] = noKeyIndex;
    if (ComboBox::getSelectedIndex(args.window, IDS_KEY_9_LIST, bufferArray[9]) == false) bufferArray[9] = noKeyIndex;
    Config::events.isWindowModeChangeable = CheckBox::isChecked(args.window, IDC_KEY_WINDOWMODE_CHECK);

    // check if same key is not used more than once
    std::unordered_set<int32_t> checkerSet;
    for (int32_t i = 0; i < EVENT_KEYS_STRING_LENGTH - 1; ++i)
    {
        if (checkerSet.count(bufferArray[i]))
        {
            if (bufferArray[i] != noKeyIndex) // only VK_NOKEY (<none>) can be repeated
            {
                MsgBox::showMessage(L"Invalid settings"s, L"Each key can only be used once (except \"<none>\")."s, 
                    args.window, MsgBox::button_set_t::ok, MsgBox::message_icon_t::warning);
                return EVENT_RETURN_ERROR;
            }
        }
        else
            checkerSet.insert(bufferArray[i]);
    }

    // update config
    for (int32_t i = 0; i < EVENT_KEYS_STRING_LENGTH - 1; ++i)
        Config::events.pTriggerKeys[i] = static_cast<char>(keys.indexToKeyCode(bufferArray[i]));
    return EVENT_RETURN_VALID;
}


/// @brief Initialize advanced settings sub-dialog
EVENT_RETURN GeneralPage::onAdvancedDialogInit(Dialog::event_args_t args)
{
    //...use settings from Config
    //...

    // translate controls/labels
    controls::Dialog& parent = args.getParent<controls::Dialog>();
    if (parent.getParent<ConfigDialog>() != nullptr)
    {
        lang::ConfigLang& langRes = parent.getParent<ConfigDialog>()->getLanguageResource();

        //...

        Button::setText(args.window, IDOK, langRes.dialog.confirm);
        Button::setText(args.window, IDCANCEL, langRes.dialog.cancel);
    }
    return EVENT_RETURN_VALID;
}

/// @brief Confirm advanced settings sub-dialog settings
EVENT_RETURN GeneralPage::onAdvancedDialogConfirm(Dialog::event_args_t args)
{
    //...copy settings in Config
    //...
    return EVENT_RETURN_VALID;
}
