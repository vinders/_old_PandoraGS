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
#include <vector>
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
#include "config_dialog.h"
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
    onProfileChange(false);
    return EVENT_RETURN_VALID; 
}


/// @brief Sub-control command event handler
EVENT_RETURN ProfileFilteringPage::onCommand(TabPage::event_args_t args)
{
    if (args.isEventAction(ComboBox::event_t::selectionChanged)) // combo-boxes
    {
        ProfileFilteringPage& parent = args.getParent<ProfileFilteringPage>();
        switch (args.controlId())
        {
            // texture upscaling
            case IDC_PRO_TXUPSCALE_FACTOR:
            {
                int32_t selection;
                if (ComboBox::getSelectedIndex(args.window, IDC_PRO_TXUPSCALE_FACTOR, selection))
                {
                    selection = (selection >= UPSCALING_MODE_TEXTURE_MAX_FACTOR) ? UPSCALING_MODE_TEXTURE_MAX_FACTOR : selection + 1;
                    parent.onUpscalingFactorChange(IDC_PRO_TXUPSCALE_LIST, selection, config::upscaling_mode_t::native,
                                                   parent.getParentDialog<ConfigDialog>()->getLanguageResource().filteringSettings.nativeScale);
                }
                break;
            }

            // 2D/sprite upscaling
            case IDC_PRO_2DUPSCALE_FACTOR: 
            {
                int32_t selection;
                if (ComboBox::getSelectedIndex(args.window, IDC_PRO_2DUPSCALE_FACTOR, selection))
                {
                    selection = config::upscalingIndexToFactor(selection);
                    parent.onUpscalingFactorChange(IDC_PRO_2DUPSCALE_LIST, selection, config::upscaling_mode_t::native,
                                                   parent.getParentDialog<ConfigDialog>()->getLanguageResource().filteringSettings.nativeScale);
                }
                break;
            }

            // screen upscaling
            case IDC_PRO_SCRUPSCALE_FACTOR: 
            {
                int32_t selection;
                if (ComboBox::getSelectedIndex(args.window, IDC_PRO_SCRUPSCALE_FACTOR, selection))
                {
                    selection = config::upscalingIndexToFactor(selection);
                    parent.onUpscalingFactorChange(IDC_PRO_SCRUPSCALE_LIST, selection, config::upscaling_mode_t::native,
                                                   parent.getParentDialog<ConfigDialog>()->getLanguageResource().filteringSettings.noScreenScale);
                }
                break;
            }
        }
    }
    else if (args.isEventAction(Button::event_t::clicked)) // buttons
    {
        try
        {
            controls::Dialog::event_handler_t handlerRef;
            ProfileFilteringPage& parent = args.getParent<ProfileFilteringPage>();
            switch (args.controlId())
            {
                //open sub-dialogs
                case IDC_PRO_BTN_SCANLINES: // scanlines settings
                {
                    controls::Dialog scanlinesDialog(parent.m_instance);
                    /*handlerRef.handler = onScanlinesDialogInit;
                    scanlinesDialog.registerEvent(controls::dialog_event_t::init, handlerRef);
                    handlerRef.handler = onScanlinesDialogDrawItem;
                    scanlinesDialog.registerEvent(controls::dialog_event_t::drawItem, handlerRef);
                    handlerRef.handler = onScanlinesDialogCommand;
                    scanlinesDialog.registerEvent(controls::dialog_event_t::command, handlerRef);
                    handlerRef.handler = onScanlinesDialogConfirm;
                    scanlinesDialog.registerEvent(controls::dialog_event_t::confirm, handlerRef);*/
                    scanlinesDialog.setParent(parent.getParentDialog<Dialog>());
                    // show modal dialog
                    scanlinesDialog.showDialog(IDD_SCANLINES_DIALOG, args.window,
                                    parent.getParentDialog<ConfigDialog>()->getLanguageResource().filteringSettings.btnScanlines, false);

                    Button::unHighlight(args.window, IDC_PRO_BTN_SCANLINES);
                    return EVENT_RETURN_VALID; break;
                }
                case IDC_PRO_BTN_CURSOR: // cursor settings
                {
                    Dialog cursorDialog(parent.m_instance);
                    /*handlerRef.handler = onCursorDialogInit;
                    cursorDialog.registerEvent(controls::dialog_event_t::init, handlerRef);
                    handlerRef.handler = onCursorDialogDrawItem;
                    cursorDialog.registerEvent(controls::dialog_event_t::drawItem, handlerRef);
                    handlerRef.handler = onCursorDialogCommand;
                    cursorDialog.registerEvent(controls::dialog_event_t::command, handlerRef);
                    handlerRef.handler = onCursorDialogConfirm;
                    cursorDialog.registerEvent(controls::dialog_event_t::confirm, handlerRef);*/
                    cursorDialog.setParent(parent.getParentDialog<Dialog>());
                    // show modal dialog
                    cursorDialog.showDialog(IDD_CURSOR_DIALOG, args.window,
                                 parent.getParentDialog<ConfigDialog>()->getLanguageResource().filteringSettings.btnCursor, false);

                    Button::unHighlight(args.window, IDC_PRO_BTN_CURSOR);
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
bool ProfileFilteringPage::onDialogConfirm(controls::Dialog::event_args_t& args)
{
    onProfileSave();
    return true;
}


/// @brief Language change event
/// @param[in] isUpdate  Set to false to initialize controls
void ProfileFilteringPage::onLanguageChange(const bool isUpdate)
{
    lang::ConfigLang& langRes = getParentDialog<ConfigDialog>()->getLanguageResource();

    if (isUpdate == false) // initialize
    {
        ConfigProfile& profile = *(Config::getCurrentProfile());
        ComboBox::initValues(getPageHandle(), IDC_PRO_TXFILTER_LIST, langRes.filteringSettings.interpolations, static_cast<uint32_t>(profile.scaling.textureSmoothing));
        ComboBox::initValues(getPageHandle(), IDC_PRO_2DFILTER_LIST, langRes.filteringSettings.interpolations, static_cast<uint32_t>(profile.scaling.spriteSmoothing));
        ComboBox::initValues(getPageHandle(), IDC_PRO_SCRFILTER_LIST, langRes.filteringSettings.screenSmoothing, static_cast<uint32_t>(profile.scaling.screenSmoothing));

        //...
    }
    else // update
    {
        // smoothing
        ComboBox::setValues(getPageHandle(), IDC_PRO_TXFILTER_LIST, langRes.filteringSettings.interpolations);
        ComboBox::setValues(getPageHandle(), IDC_PRO_2DFILTER_LIST, langRes.filteringSettings.interpolations);
        ComboBox::setValues(getPageHandle(), IDC_PRO_SCRFILTER_LIST, langRes.filteringSettings.screenSmoothing);
        // upscaling
        int32_t selection;
        if (ComboBox::getSelectedIndex(getPageHandle(), IDC_PRO_TXUPSCALE_FACTOR, selection) && selection == 0)
        {
            std::vector<std::wstring> nativeLabel = { langRes.filteringSettings.nativeScale };
            ComboBox::setValues(getPageHandle(), IDC_PRO_TXUPSCALE_LIST, nativeLabel);
        }
        if (ComboBox::getSelectedIndex(getPageHandle(), IDC_PRO_2DUPSCALE_FACTOR, selection) && selection == 0)
        {
            std::vector<std::wstring> nativeLabel = { langRes.filteringSettings.nativeScale };
            ComboBox::setValues(getPageHandle(), IDC_PRO_2DUPSCALE_LIST, nativeLabel);
        }
        if (ComboBox::getSelectedIndex(getPageHandle(), IDC_PRO_SCRUPSCALE_FACTOR, selection) && selection == 0)
        {
            std::vector<std::wstring> nativeLabel = { langRes.filteringSettings.noScreenScale };
            ComboBox::setValues(getPageHandle(), IDC_PRO_SCRUPSCALE_LIST, nativeLabel);
        }

        //...
    }

    //...
}



/// @brief Upscaling factor change event
/// @param[in] listResourceId  Combo-box resource identifier
/// @param[in] factor          Selected factor
/// @param[in] selectedScaler  Index to select in list of upscalers
/// @param[in] nativeLabel     Label for native entry
void ProfileFilteringPage::onUpscalingFactorChange(const int32_t listResourceId, const uint32_t factor, const config::upscaling_mode_t selectedScaler, const std::wstring& nativeLabel)
{
    std::vector<std::wstring> scalers;
    uint32_t selectedIndex = 0u;
    switch (factor)
    {
        case 2u:
        {
            scalers = { L"SaI"s, L"xBR"s, L"xBRZ"s, L"NNEDI3"s, L"super-xBR"s, L"super-xBR bilateral"s };
            switch (selectedScaler)
            {
                case config::upscaling_mode_t::sai:             selectedIndex = 0u; break;
                case config::upscaling_mode_t::xbr:             selectedIndex = 1u; break;
                case config::upscaling_mode_t::xbrz:            selectedIndex = 2u; break;
                case config::upscaling_mode_t::nnedi3:          selectedIndex = 3u; break;
                case config::upscaling_mode_t::superXbr:        selectedIndex = 4u; break;
                case config::upscaling_mode_t::superXbrFastBlt: selectedIndex = 5u; break;
                default: selectedIndex = 0u; break;
            }
            break;
        }
        case 3u:
        {
            scalers = { L"xBR"s, L"xBRZ"s, L"xBRZ depolarized"s };
            switch (selectedScaler)
            {
                case config::upscaling_mode_t::xbr:          selectedIndex = 0u; break;
                case config::upscaling_mode_t::xbrz:         selectedIndex = 1u; break;
                case config::upscaling_mode_t::xbrzEnhanced: selectedIndex = 2u; break;
                default: selectedIndex = 1u; break;
            }
            break;
        }
        case 4u:
        {
            scalers = { L"xBR"s, L"xBRZ"s, L"xBRZ depolarized"s, L"NNEDI3"s, L"super-xBR"s, L"super-xBR bilateral"s };
            switch (selectedScaler)
            {
                case config::upscaling_mode_t::xbr:             selectedIndex = 0u; break;
                case config::upscaling_mode_t::xbrz:            selectedIndex = 1u; break;
                case config::upscaling_mode_t::xbrzEnhanced:    selectedIndex = 2u; break;
                case config::upscaling_mode_t::nnedi3:          selectedIndex = 3u; break;
                case config::upscaling_mode_t::superXbr:        selectedIndex = 4u; break;
                case config::upscaling_mode_t::superXbrFastBlt: selectedIndex = 5u; break;
                default: selectedIndex = 4u; break;
            }
            break;
        }
        case 5u:
        {
            scalers = { L"xBRZ"s, L"xBRZ depolarized"s };
            switch (selectedScaler)
            {
                case config::upscaling_mode_t::xbrz:            selectedIndex = 0u; break;
                case config::upscaling_mode_t::xbrzEnhanced:    selectedIndex = 1u; break;
                default: selectedIndex = 0u; break;
            }
            break;
        }
        case 8u:
        {
            scalers = { L"NNEDI3"s, L"super-xBR"s, L"super-xBR bilateral"s };
            switch (selectedScaler)
            {
                case config::upscaling_mode_t::nnedi3:          selectedIndex = 0u; break;
                case config::upscaling_mode_t::superXbr:        selectedIndex = 1u; break;
                case config::upscaling_mode_t::superXbrFastBlt: selectedIndex = 2u; break;
                default: selectedIndex = 1u; break;
            }
            break;
        }
        default:
        {
            scalers = { nativeLabel };
            selectedIndex = 0u; 
            break;
        }
    }
    // fill list of scalers
    ComboBox::setValues(getPageHandle(), listResourceId, scalers, selectedIndex);
}



/// @brief Profile save event
/// @returns Validity
bool ProfileFilteringPage::onProfileSave() 
{
    //...
    return true; 
}


/// @brief Profile change event
/// @param[in] isUpdate  Set to false to initialize controls
void ProfileFilteringPage::onProfileChange(const bool isUpdate) 
{
    ConfigProfile& profile = *(Config::getCurrentProfile());
    lang::ConfigLang& langRes = getParentDialog<ConfigDialog>()->getLanguageResource();

    // screen upscaling factors + scalers
    std::vector<std::wstring> factors = { L"1x"s, L"2x"s, L"3x"s, L"4x"s, L"5x"s, L"8x"s };
    uint32_t selectedIndex = config::upscalingFactorToIndex(profile.scaling.screenScaling.factor);
    if (isUpdate == false)
        ComboBox::initValues(getPageHandle(), IDC_PRO_SCRUPSCALE_FACTOR, factors, selectedIndex);
    else
        ComboBox::setValues(getPageHandle(), IDC_PRO_SCRUPSCALE_FACTOR, factors, selectedIndex);
    onUpscalingFactorChange(IDC_PRO_SCRUPSCALE_LIST, profile.scaling.screenScaling.factor, profile.scaling.screenScaling.mode, langRes.filteringSettings.noScreenScale);

    // sprite upscaling factors + scalers
    selectedIndex = config::upscalingFactorToIndex(profile.scaling.spriteScaling.factor);
    if (isUpdate == false)
        ComboBox::initValues(getPageHandle(), IDC_PRO_2DUPSCALE_FACTOR, factors, selectedIndex);
    else
        ComboBox::setValues(getPageHandle(), IDC_PRO_2DUPSCALE_FACTOR, factors, selectedIndex);
    onUpscalingFactorChange(IDC_PRO_2DUPSCALE_LIST, profile.scaling.spriteScaling.factor, profile.scaling.spriteScaling.mode, langRes.filteringSettings.nativeScale);

    // texture upscaling factors + scalers
    while (factors.size() > UPSCALING_MODE_TEXTURE_MAX_FACTOR)
        factors.pop_back();
    selectedIndex = profile.scaling.textureScaling.factor - 1u;
    if (selectedIndex >= UPSCALING_MODE_TEXTURE_MAX_FACTOR)
        selectedIndex = 0u;
    if (isUpdate == false)
        ComboBox::initValues(getPageHandle(), IDC_PRO_TXUPSCALE_FACTOR, factors, selectedIndex);
    else
        ComboBox::setValues(getPageHandle(), IDC_PRO_TXUPSCALE_FACTOR, factors, selectedIndex);
    onUpscalingFactorChange(IDC_PRO_TXUPSCALE_LIST, profile.scaling.textureScaling.factor, profile.scaling.textureScaling.mode, langRes.filteringSettings.nativeScale);


    if (isUpdate)
    {
        // smoothing
        ComboBox::setSelectedIndex(getPageHandle(), IDC_PRO_TXFILTER_LIST, static_cast<uint32_t>(profile.scaling.textureSmoothing));
        ComboBox::setSelectedIndex(getPageHandle(), IDC_PRO_2DFILTER_LIST, static_cast<uint32_t>(profile.scaling.spriteSmoothing));
        ComboBox::setSelectedIndex(getPageHandle(), IDC_PRO_SCRFILTER_LIST, static_cast<uint32_t>(profile.scaling.screenSmoothing));
    }

    //...
}
