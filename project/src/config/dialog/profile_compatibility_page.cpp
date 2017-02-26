/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab sub-page - compatibility settings
*******************************************************************************/
#include "../../globals.h"
#include <string>
#include <stdexcept>
#include <functional>
#include "../../lang/config_lang.h"
#include "../../res/resource.h"
#include "../../events/utils/logger.h"
#include "controls/common.h"
#include "controls/tab_page.h"
#include "controls/combo_box.h"
#include "controls/check_box.hpp"
#include "controls/label.hpp"
#include "controls/button.hpp"
#include "controls/msg_box.h"
#include "config_dialog.h"
#include "profile_compatibility_page.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Create tab page - profile compatibility settings
/// @param[in] instance       Current instance handle
/// @param[in] pParentDialog  Parent dialog reference
ProfileCompatibilityPage::ProfileCompatibilityPage(controls::library_instance_t instance, controls::Dialog* pParentDialog)
    : controls::TabPage(instance, pParentDialog, IDD_PROFILE_COMPAT_TAB)
{
    // set event handlers
    TabPage::event_handler_t eventHandler;
    eventHandler.handler = onInit;
    TabPage::registerEvent(dialog_event_t::init, eventHandler);
    eventHandler.handler = onCommand;
    TabPage::registerEvent(dialog_event_t::command, eventHandler);
}



// -- event handlers -- --------------------------------------------

/// @brief Initialization event handler
EVENT_RETURN ProfileCompatibilityPage::onInit(TabPage::event_args_t args)
{
    ProfileCompatibilityPage& parent = args.getParent<ProfileCompatibilityPage>();
    parent.m_hPage = args.window;

    // translate controls/labels
    parent.onLanguageChange(false);
    // set config values
    parent.onProfileChange(false);
    return EVENT_RETURN_IGNORE;
}


/// @brief Sub-control command event handler
EVENT_RETURN ProfileCompatibilityPage::onCommand(TabPage::event_args_t args)
{
    if (args.isEventAction(Button::event_t::clicked)) // buttons
    {
        try
        {
            // open sub-dialog
            if (args.controlId() == IDC_PROCPT_BTN_FIXES) // custom game fixes
            {
                controls::Dialog::event_handler_t handlerRef;
                ProfileCompatibilityPage& parent = args.getParent<ProfileCompatibilityPage>();

                controls::Dialog fixesDialog(parent.m_instance);
                /*handlerRef.handler = onFixesDialogInit;
                fixesDialog.registerEvent(controls::dialog_event_t::init, handlerRef);
                handlerRef.handler = onFixesDialogConfirm;
                fixesDialog.registerEvent(controls::dialog_event_t::confirm, handlerRef);*/
                fixesDialog.setParent(parent.getParentDialog<Dialog>());
                // show modal dialog
                fixesDialog.showDialog(IDD_FIXES_DIALOG, args.window,
                            parent.getParentDialog<ConfigDialog>()->getLanguageResource().compatibilitySettings.btnGameFixes, false);

                Button::unHighlight(args.window, IDC_PROCPT_BTN_FIXES);
                return EVENT_RETURN_VALID;
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
bool ProfileCompatibilityPage::onDialogConfirm(controls::Dialog::event_args_t& args)
{
    onProfileSave();
    return true;
}


/// @brief Language change event
/// @param[in] isUpdate  Set to false to initialize controls
void ProfileCompatibilityPage::onLanguageChange(const bool isUpdate)
{
    //...
}


/// @brief Profile save event
/// @returns Validity
bool ProfileCompatibilityPage::onProfileSave()
{
    //...
    return true;
}


/// @brief Profile change event
/// @param[in] isUpdate  Set to false to initialize controls
void ProfileCompatibilityPage::onProfileChange(const bool isUpdate)
{
    //...
}
