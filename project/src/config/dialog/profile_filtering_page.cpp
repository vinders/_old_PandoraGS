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
    onProfileChange();
    return EVENT_RETURN_VALID; 
}


/// @brief Sub-control command event handler
EVENT_RETURN ProfileFilteringPage::onCommand(TabPage::event_args_t args)
{
    if (args.isEventAction(ComboBox::event_t::selectionChanged)) // combo-boxes
    {
        switch (args.controlId())
        {
            // texture upscaling
            case IDC_PRO_TXUPSCALE_FACTOR:
            {
                int32_t selection;
                if (ComboBox::getSelectedIndex(args.window, IDC_PRO_TXUPSCALE_FACTOR, selection))
                {
                    //...
                }
                break;
            }

            // 2D/sprite upscaling
            case IDC_PRO_2DUPSCALE_FACTOR: 
            {
                int32_t selection;
                if (ComboBox::getSelectedIndex(args.window, IDC_PRO_2DUPSCALE_FACTOR, selection))
                {
                    //...
                }
                break;
            }

            // screen upscaling
            case IDC_PRO_SCRUPSCALE_FACTOR: 
            {
                int32_t selection;
                if (ComboBox::getSelectedIndex(args.window, IDC_PRO_SCRUPSCALE_FACTOR, selection))
                {
                    //...
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
