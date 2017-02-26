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
#include <vector>
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
#include "config_dialog.h"
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
    parent.onProfileChange(false);
    return EVENT_RETURN_IGNORE;
}


/// @brief Special drawing event handler
EVENT_RETURN ProfileScreenPage::onDrawItem(TabPage::event_args_t args)
{
    if (args.eventData == IDC_PROSTR_PICTUREBOX)
    {
        // read parameters
        int32_t stretchVal, cropVal;
        bool isMirrored = CheckBox::isChecked(args.window, IDC_PROSTR_MIRROR);
        if (TrackBar::getValue(args.window, IDC_PROSTR_STRETCH_SLIDER, stretchVal) && TrackBar::getValue(args.window, IDC_PROSTR_CUT_SLIDER, cropVal))
        {
            // draw preview
            //...draw preview box
        }
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
            if (ComboBox::getSelectedIndex(args.window, IDC_PROSTR_PRESET_LIST, selection) && selection > 0)
            {
                // set track-bars
                switch (selection)
                {
                    case 1:
                        TrackBar::setValue(args.window, IDC_PROSTR_STRETCH_SLIDER, SCREEN_RATIO_STRETCH_FullWindow);
                        TrackBar::setValue(args.window, IDC_PROSTR_CUT_SLIDER, SCREEN_RATIO_CROP_FullWindow);
                        break;
                    case 2:
                        TrackBar::setValue(args.window, IDC_PROSTR_STRETCH_SLIDER, SCREEN_RATIO_STRETCH_Keep);
                        TrackBar::setValue(args.window, IDC_PROSTR_CUT_SLIDER, SCREEN_RATIO_CROP_Keep);
                        break;
                    case 3:
                        TrackBar::setValue(args.window, IDC_PROSTR_STRETCH_SLIDER, SCREEN_RATIO_STRETCH_KeepFill);
                        TrackBar::setValue(args.window, IDC_PROSTR_CUT_SLIDER, SCREEN_RATIO_CROP_KeepFill);
                        break;
                    case 4:
                        TrackBar::setValue(args.window, IDC_PROSTR_STRETCH_SLIDER, SCREEN_RATIO_STRETCH_Semi);
                        TrackBar::setValue(args.window, IDC_PROSTR_CUT_SLIDER, SCREEN_RATIO_CROP_Semi);
                        break;
                }

                // invalidate preview
                //...
                return EVENT_RETURN_VALID;
            }
        }
    }
    else if (args.isEventAction(CheckBox::event_t::clicked)) // check-boxes
    {
        // mirror screen
        if (args.controlId() == IDC_PROSTR_MIRROR)
        {
            // invalidate preview
            //...
        }
    }
    return EVENT_RETURN_IGNORE;
}


/// @brief Track-bar event handler
EVENT_RETURN ProfileScreenPage::onTrackBarChange(TabPage::event_args_t args)
{
    if (args.isEventSubAction(TrackBar::event_t::endMove))
    {
        int32_t controlId = TrackBar::getControlId(args);
        // screen stretching
        if (controlId == IDC_PROSTR_STRETCH_SLIDER || controlId == IDC_PROSTR_CUT_SLIDER)
        {
            // check if corresponding preset exists
            int32_t stretchVal, cropVal;
            if (TrackBar::getValue(args.window, IDC_PROSTR_STRETCH_SLIDER, stretchVal) && TrackBar::getValue(args.window, IDC_PROSTR_CUT_SLIDER, cropVal))
            {
                uint32_t presetIndex = 0u;
                if (stretchVal == SCREEN_RATIO_STRETCH_FullWindow && cropVal == SCREEN_RATIO_CROP_FullWindow)
                    presetIndex = 1u;
                else if (stretchVal == SCREEN_RATIO_STRETCH_Keep && cropVal == SCREEN_RATIO_CROP_Keep)
                    presetIndex = 2u;
                else if (stretchVal == SCREEN_RATIO_STRETCH_KeepFill && cropVal == SCREEN_RATIO_CROP_KeepFill)
                    presetIndex = 3u;
                else if (stretchVal == SCREEN_RATIO_STRETCH_Semi && cropVal == SCREEN_RATIO_CROP_Semi)
                    presetIndex = 4u;
                ComboBox::setSelectedIndex(args.window, IDC_PROSTR_PRESET_LIST, presetIndex);
            }

            // invalidate preview
            //...
            return EVENT_RETURN_VALID;
        }
    }
    return EVENT_RETURN_IGNORE;
}



// -- specialized handlers -- --------------------------------------

/// @brief Language change event
/// @returns Validity
bool ProfileScreenPage::onDialogConfirm(controls::Dialog::event_args_t& args)
{
    onProfileSave();
    return true;
}


/// @brief Language change event
/// @param[in] isUpdate  Set to false to initialize controls
void ProfileScreenPage::onLanguageChange(const bool isUpdate)
{
    lang::ConfigLang& langRes = getParentDialog<ConfigDialog>()->getLanguageResource();
    if (isUpdate == false) // initialize
    {
        ConfigProfile& profile = *(Config::getCurrentProfile());

        // internal resolution
        ComboBox::initValues(getPageHandle(), IDC_PROSTR_INTRESX_LIST, langRes.screenSettings.internalResX, internalResXFactorToIndex(profile.display.internalRes.x));
        ComboBox::initValues(getPageHandle(), IDC_PROSTR_INTRESY_LIST, langRes.screenSettings.internalResY, internalResYFactorToIndex(profile.display.internalRes.y));
        // presets
        ComboBox::initValues(getPageHandle(), IDC_PROSTR_PRESET_LIST, langRes.screenSettings.stretchingPresets, 0u);
    }
    else
    {
        // internal resolution
        ComboBox::setValues(getPageHandle(), IDC_PROSTR_INTRESX_LIST, langRes.screenSettings.internalResX);
        ComboBox::setValues(getPageHandle(), IDC_PROSTR_INTRESY_LIST, langRes.screenSettings.internalResY);
        // presets
        ComboBox::setValues(getPageHandle(), IDC_PROSTR_PRESET_LIST, langRes.screenSettings.stretchingPresets);
    }

    // labels
    Label::setText(getPageHandle(), IDS_PROSTR_INTRES, langRes.screenSettings.internalRes);
    Label::setText(getPageHandle(), IDS_PROSTR_STRETCH_MIN, langRes.screenSettings.unstretched);
    Label::setText(getPageHandle(), IDS_PROSTR_STRETCH_MAX, langRes.screenSettings.stretched);
    Label::setText(getPageHandle(), IDS_PROSTR_CUT_MIN, langRes.screenSettings.uncropped);
    Label::setText(getPageHandle(), IDS_PROSTR_CUT_MAX, langRes.screenSettings.cropped);
    Label::setText(getPageHandle(), IDC_PROSTR_PXRATIO_CHECK, langRes.screenSettings.pixelRatio);
    Label::setText(getPageHandle(), IDC_PROSTR_MIRROR, langRes.screenSettings.mirror);
    Label::setText(getPageHandle(), IDC_PROSTR_BLACKBORDERS_CHECK, langRes.screenSettings.blackBorders);
    Label::setText(getPageHandle(), IDS_PROSTR_BLACKBORDERSX, langRes.screenSettings.blackBordersX);
    Label::setText(getPageHandle(), IDS_PROSTR_BLACKBORDERSY, langRes.screenSettings.blackBordersY);

    //...
}


/// @brief Profile save event
/// @returns Validity
bool ProfileScreenPage::onProfileSave()
{
    //...
    return true;
}


/// @brief Profile change event
/// @param[in] isUpdate  Set to false to initialize controls
void ProfileScreenPage::onProfileChange(const bool isUpdate)
{
    ConfigProfile& profile = *(Config::getCurrentProfile());

    if (isUpdate)
    {
        // internal resolution
        ComboBox::setSelectedIndex(getPageHandle(), IDC_PROSTR_INTRESX_LIST, internalResXFactorToIndex(profile.display.internalRes.x));
        ComboBox::setSelectedIndex(getPageHandle(), IDC_PROSTR_INTRESY_LIST, internalResYFactorToIndex(profile.display.internalRes.y));
    }

    // stretching
    TrackBar::setValue(getPageHandle(), IDC_PROSTR_STRETCH_SLIDER, profile.display.ratioStretch);
    TrackBar::setValue(getPageHandle(), IDC_PROSTR_CUT_SLIDER, profile.display.ratioCrop);
    CheckBox::setChecked(getPageHandle(), IDC_PROSTR_PXRATIO_CHECK, (profile.display.pixelRatio == config::pixel_ratio_mode_t::nonSquare));
    CheckBox::setChecked(getPageHandle(), IDC_PROSTR_MIRROR, profile.display.isMirrored);
    // stretching presets
    uint32_t presetIndex = 0u;
    if (profile.display.ratioStretch == SCREEN_RATIO_STRETCH_FullWindow && profile.display.ratioCrop == SCREEN_RATIO_CROP_FullWindow)
        presetIndex = 1u;
    else if (profile.display.ratioStretch == SCREEN_RATIO_STRETCH_Keep && profile.display.ratioCrop == SCREEN_RATIO_CROP_Keep)
        presetIndex = 2u;
    else if (profile.display.ratioStretch == SCREEN_RATIO_STRETCH_KeepFill && profile.display.ratioCrop == SCREEN_RATIO_CROP_KeepFill)
        presetIndex = 3u;
    else if (profile.display.ratioStretch == SCREEN_RATIO_STRETCH_Semi && profile.display.ratioCrop == SCREEN_RATIO_CROP_Semi)
        presetIndex = 4u;
    ComboBox::setSelectedIndex(getPageHandle(), IDC_PROSTR_PRESET_LIST, presetIndex);
    // invalidate preview
    //...

    // misc
    CheckBox::setChecked(getPageHandle(), IDC_PROSTR_BLACKBORDERS_CHECK, (profile.display.blackBorders.x != 0u || profile.display.blackBorders.y != 0u));
    TextField::setValue(getPageHandle(), IDC_PROSTR_BLACKBORDERSX_EDIT, std::to_wstring(profile.display.blackBorders.x));
    TextField::setValue(getPageHandle(), IDC_PROSTR_BLACKBORDERSY_EDIT, std::to_wstring(profile.display.blackBorders.y));
    //...
}
