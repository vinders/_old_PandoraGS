/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration dialog
*******************************************************************************/
#include "../../globals.h"
#include <string>
#include <stdexcept>
#include <functional>
#include <stack>
#include <memory>
#include "../../res/resource.h"
#include "controls/common.h"
#include "controls/dialog.h"
#include "config_dialog.h"
using namespace config::dialog;
using namespace config::dialog::controls;


/// @brief Create dialog box
/// @param[in] instance  Current instance handle
/// @throws runtime_error     Dialog creation error
/// @throws invalid_argument  Invalid instance
ConfigDialog::ConfigDialog(library_instance_t instance) : Dialog(instance)
{
    // load config values
    config::Config::init();
    m_languageResource.setLanguage(config::Config::langCode, config::Config::langFilePath);

    // set dialog pages
    //...init pages and tabs
        //créer les tab_association_t et les insérer dans m_tabs avec paramètres appropriés pour boutons

    // set event handlers
    dialog_event_handler_t eventHandler;
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(DIALOG_EVENT_HANDLER_ARGUMENTS)>(onInit);
    Dialog::registerEvent(dialog_event_t::init, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(DIALOG_EVENT_HANDLER_ARGUMENTS)>(onPaint);
    Dialog::registerEvent(dialog_event_t::paint, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(DIALOG_EVENT_HANDLER_ARGUMENTS)>(onDrawItem);
    Dialog::registerEvent(dialog_event_t::drawItem, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(DIALOG_EVENT_HANDLER_ARGUMENTS)>(onCommand);
    Dialog::registerEvent(dialog_event_t::command, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(DIALOG_EVENT_HANDLER_ARGUMENTS)>(onConfirm);
    Dialog::registerEvent(dialog_event_t::confirm, eventHandler);
}

/// @brief Destroy dialog box
ConfigDialog::~ConfigDialog()
{
    //...delete pages and tabs
    config::Config::close();
}

/// @brief Display modal dialog box
/// @returns Dialog result
/// @throws runtime_error  Dialog creation/display error
dialog_result_t ConfigDialog::showDialog()
{
    return Dialog::showDialog(IDD_CONFIG_DIALOG, true);
}


/// @brief Initialization event handler
DIALOG_EVENT_RETURN ConfigDialog::onInit(DIALOG_EVENT_HANDLER_ARGUMENTS)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    return (INT_PTR)TRUE;
}
#else
{
    return 0;//...
}
#endif


/// @brief Paint event handler - draw dialog
DIALOG_EVENT_RETURN ConfigDialog::onPaint(DIALOG_EVENT_HANDLER_ARGUMENTS)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    return (INT_PTR)TRUE;
}
#else
{
    return 0;//...
}
#endif


/// @brief Sub-control drawing event handler - draw component
DIALOG_EVENT_RETURN ConfigDialog::onDrawItem(DIALOG_EVENT_HANDLER_ARGUMENTS)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    return (INT_PTR)TRUE;
}
#else
{
    return 0;//...
}
#endif


/// @brief Sub-control command event handler
DIALOG_EVENT_RETURN ConfigDialog::onCommand(DIALOG_EVENT_HANDLER_ARGUMENTS)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    switch (0)//...
    {
        //...
    }
    return (INT_PTR)FALSE;
}
#else
{
    return 0;//...
}
#endif


/// @brief Dialog confirm event handler - check validity
DIALOG_EVENT_RETURN ConfigDialog::onConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    return (INT_PTR)TRUE;
}
#else
{
    return 0;//...
}
#endif

