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
    parent.onProfileChange();
    return EVENT_RETURN_IGNORE;
}


/// @brief Sub-control command event handler
EVENT_RETURN ProfileCompatibilityPage::onCommand(TabPage::event_args_t args)
{
    return EVENT_RETURN_IGNORE;
}



// -- specialized handlers -- --------------------------------------

/// @brief Language change event
/// @returns Validity
bool ProfileCompatibilityPage::onDialogConfirm(controls::Dialog::event_args_t& args)
{
    return true;
}


/// @brief Language change event
/// @param[in] isUpdate  Set to false to initialize controls
void ProfileCompatibilityPage::onLanguageChange(const bool isUpdate)
{
}


/// @brief Profile save event
/// @returns Validity
bool ProfileCompatibilityPage::onProfileSave()
{
    return true;
}


/// @brief Profile change event
void ProfileCompatibilityPage::onProfileChange()
{
}
