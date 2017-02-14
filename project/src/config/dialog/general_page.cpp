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
#include <stdexcept>
#include <functional>
#include "../../lang/config_lang.h"
#include "../../res/resource.h"
#include "../../events/utils/logger.h"
#include "controls/common.h"
#include "controls/text_field.hpp"
#include "controls/combo_box.h"
#include "controls/check_box.hpp"
#include "controls/group_box.h"
#include "controls/label.hpp"
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
    // set combo-boxes and labels
    GeneralPage& parent = getEventTargetPageReference(GeneralPage);
    parent.onLanguageChange(false);
    // set list of screen resolutions
    //...load list of fullscreen resolutions
    //...

    // apply config settings
    //...read config -> set controls
    //...

    return DIALOG_EVENT_RETURN_VALID;
}


/// @brief Sub-control command event handler
DIALOG_EVENT_RETURN GeneralPage::onCommand(PAGE_EVENT_HANDLER_ARGUMENTS)
{
    if (eventActionDiffers(ComboBox::event_t::selectionChanged))
    {
        try
        {
            GeneralPage& parent = getEventTargetPageReference(GeneralPage);
            switch (getEventTargetControlId())
            {
                //open popup dialogs
                case IDC_GEN_BTN_KEYBINDING: // control key binding
                {
                    Dialog keyBindDialog(parent.m_instance);
                    //...register init + confirm
                    //...
                    keyBindDialog.showDialog(IDD_KEYBINDING_DIALOG, getEventWindowHandle(), false);
                    //Button_SetState(GetDlgItem(hWindow,ID_DU_BOUTON), FALSE); // remove focus after button was clicked
                    //...
                    return DIALOG_EVENT_RETURN_VALID; break;
                }
                case IDC_GEN_BTN_ADVANCED: // advanced settings
                {
                    Dialog advancedDialog(parent.m_instance);
                    //...register init + confirm
                    //...
                    advancedDialog.showDialog(IDD_ADVANCED_DIALOG, getEventWindowHandle(), false);
                    //Button_SetState(GetDlgItem(hWindow,ID_DU_BOUTON), FALSE); // remove focus after button was clicked
                    //...
                    return DIALOG_EVENT_RETURN_VALID; break;
                }
            }
        }
        catch (const std::exception& exc)
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
