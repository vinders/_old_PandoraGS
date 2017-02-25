/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab sub-page - screen settings
*******************************************************************************/
#include "../../globals.h"
#include <string>
#include <stdexcept>
#include <functional>
#include "../config.h"
#include "../../lang/config_lang.h"
#include "../../res/resource.h"
#include "../../events/utils/logger.h"
#include "controls/common.h"
#include "controls/tab_page.h"
#include "controls/label.hpp"
#include "controls/check_box.hpp"
#include "controls/combo_box.h"
#include "controls/text_field.hpp"
#include "controls/track_bar.h"
#include "controls/preview_box.h"
#include "profile_screen_page.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Create tab page - profile compatibility settings
/// @param[in] instance       Current instance handle
/// @param[in] pParentDialog  Parent dialog reference
ProfileScreenPage::ProfileScreenPage(controls::library_instance_t instance, controls::Dialog* pParentDialog)
    : controls::TabPage(instance, pParentDialog, IDD_PROFILE_STRETCHING_TAB)
{
    // set event handlers
    TabPage::event_handler_t eventHandler;
    eventHandler.handler = onInit;
    TabPage::registerEvent(dialog_event_t::init, eventHandler);
    eventHandler.handler = onDrawItem;
    TabPage::registerEvent(dialog_event_t::drawItem, eventHandler);
    eventHandler.handler = onCommand;
    TabPage::registerEvent(dialog_event_t::command, eventHandler);
    eventHandler.handler = onTrackBarChange;
    TabPage::registerEvent(dialog_event_t::scrollX, eventHandler);
}



// -- event handlers -- --------------------------------------------

/// @brief Initialization event handler
EVENT_RETURN ProfileScreenPage::onInit(TabPage::event_args_t args)
{
    ProfileScreenPage& parent = args.getParent<ProfileScreenPage>();
    parent.m_hPage = args.window;

    // initialize sliders
    TrackBar::initControl(args.window, IDC_PROSTR_STRETCH_SLIDER, SCREEN_RATIO_MAX_VAL, 1, SCREEN_RATIO_MAX_VAL / 2);
    TrackBar::initControl(args.window, IDC_PROSTR_CUT_SLIDER, SCREEN_RATIO_MAX_VAL, 1, SCREEN_RATIO_MAX_VAL / 2);

    // translate controls/labels
    parent.onLanguageChange(false);
    // set config values
    parent.onProfileChange();
    return EVENT_RETURN_IGNORE;
}


/// @brief Special drawing event handler
EVENT_RETURN ProfileScreenPage::onDrawItem(TabPage::event_args_t args)
{
    if (args.eventData == IDC_PROSTR_PICTUREBOX)
    {
        // read parameters
        uint32_t stretchVal = TrackBar::getValue(args.window, IDC_PROSTR_STRETCH_SLIDER);
        uint32_t cropVal = TrackBar::getValue(args.window, IDC_PROSTR_CUT_SLIDER);
        bool isMirrored = CheckBox::isChecked(args.window, IDC_PROSTR_MIRROR);

        // draw preview
        //...draw preview box
    }
    return EVENT_RETURN_IGNORE;
}


/// @brief Sub-control command event handler
EVENT_RETURN ProfileScreenPage::onCommand(TabPage::event_args_t args)
{
    if (args.isEventAction(ComboBox::event_t::selectionChanged)) // combo-boxes
    {
        // screen stretching preset
        if (args.controlId() == IDC_PROSTR_PRESET_LIST)
        {
            int32_t selection;
            if (ComboBox::getSelectedIndex(args.window, IDC_PROSTR_PRESET_LIST, selection))
            {
                //...
            }
        }
    }
    else if (args.isEventAction(CheckBox::event_t::clicked)) // check-boxes
    {
        // mirror screen
        if (args.controlId() == IDC_PROSTR_MIRROR)
        {
            //...
        }
    }
    return EVENT_RETURN_IGNORE;
}


/// @brief Track-bar event handler
EVENT_RETURN ProfileScreenPage::onTrackBarChange(TabPage::event_args_t args)
{
    return EVENT_RETURN_IGNORE;
}



// -- specialized handlers -- --------------------------------------

/// @brief Language change event
/// @returns Validity
bool ProfileScreenPage::onDialogConfirm(controls::Dialog::event_args_t& args)
{
    return true;
}


/// @brief Language change event
/// @param[in] isUpdate  Set to false to initialize controls
void ProfileScreenPage::onLanguageChange(const bool isUpdate)
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
bool ProfileScreenPage::onProfileSave()
{
    return true;
}


/// @brief Profile change event
void ProfileScreenPage::onProfileChange()
{
}
