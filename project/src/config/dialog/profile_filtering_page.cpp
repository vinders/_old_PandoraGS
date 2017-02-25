/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab sub-page - filtering settings
*******************************************************************************/
#include "../../globals.h"
#include <string>
#include <stdexcept>
#include <functional>
#include "../../lang/config_lang.h"
#include "../../res/resource.h"
#include "../../events/utils/logger.h"
#include "controls/common.h"
#include "controls/scrollable_tab_page.h"
#include "controls/combo_box.h"
#include "controls/check_box.hpp"
#include "controls/track_bar.h"
#include "controls/label.hpp"
#include "controls/button.hpp"
#include "controls/msg_box.h"
#include "profile_filtering_page.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Create tab page - profile compatibility settings
/// @param[in] instance       Current instance handle
/// @param[in] pParentDialog  Parent dialog reference
ProfileFilteringPage::ProfileFilteringPage(controls::library_instance_t instance, controls::Dialog* pParentDialog)
    : controls::ScrollableTabPage(instance, pParentDialog, IDD_PROFILE_FILTERS_TAB, 570, false)
{
    // set event handlers
    TabPage::event_handler_t eventHandler;
    eventHandler.handler = onCommand;
    TabPage::registerEvent(dialog_event_t::command, eventHandler);
}



// -- event handlers -- --------------------------------------------

/// @brief Initialization extendable event handler
EVENT_RETURN ProfileFilteringPage::onInitOverridable(controls::TabPage::event_args_t& args)
{
    // initialize sliders
    TrackBar::initControl(args.window, IDC_PRO_FXSPEC_SLIDER, 3, 1, 1);
    TrackBar::initControl(args.window, IDC_PRO_FXBUMP_SLIDER, 3, 1, 1);
    TrackBar::initControl(args.window, IDC_PRO_FXBLOOM_SLIDER, 3, 1, 1);
    TrackBar::initControl(args.window, IDC_PRO_FXLUMA_SLIDER, 3, 1, 1);

    // translate controls/labels
    onLanguageChange(false);
    // set config values
    onProfileChange();
    return EVENT_RETURN_VALID; 
}


/// @brief Sub-control command event handler
EVENT_RETURN ProfileFilteringPage::onCommand(TabPage::event_args_t args)
{
    return EVENT_RETURN_IGNORE;
}



// -- specialized handlers -- --------------------------------------

/// @brief Language change event
/// @returns Validity
bool ProfileFilteringPage::onDialogConfirm(controls::Dialog::event_args_t& args)
{
    return true;
}


/// @brief Language change event
/// @param[in] isUpdate  Set to false to initialize controls
void ProfileFilteringPage::onLanguageChange(const bool isUpdate)
{
    if (isUpdate)
    {
        //ComboBox::initValues();
    }
    else
    {
        //ComboBox::setValues();
    }
}


/// @brief Profile save event
/// @returns Validity
bool ProfileFilteringPage::onProfileSave() 
{
    return true; 
}


/// @brief Profile change event
void ProfileFilteringPage::onProfileChange() 
{
}
