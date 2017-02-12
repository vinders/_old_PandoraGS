/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : file selection dialog control
*******************************************************************************/
#include "../../../globals.h"
#include <string>
#include <stdexcept>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#endif
#include "../../../res/resource.h"
#include "common.h"
#include "dialog.h"
#include "file_dialog.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Create dialog box
/// @param[in] instance  Current instance handle
/// @param[in] mode      File selection mode
/// @throws invalid_argument  Invalid instance
FileDialog::FileDialog(library_instance_t instance, file_mode_t mode) : Dialog(instance), m_mode(mode), m_fieldId(0)
{
    dialog_event_handler_t eventHandler;
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(DIALOG_EVENT_HANDLER_ARGUMENTS)>(onInit);
    Dialog::registerEvent(dialog_event_t::init, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(DIALOG_EVENT_HANDLER_ARGUMENTS)>(onCommand);
    Dialog::registerEvent(dialog_event_t::command, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(DIALOG_EVENT_HANDLER_ARGUMENTS)>(onConfirm);
    Dialog::registerEvent(dialog_event_t::confirm, eventHandler);
}

/// @brief Display modal dialog box
/// @param[in] dialogResourceId   Dialog description identifier
/// @param[in] fieldResourceId    File path field identifier
/// @param[in] browserResourceId  Browse button identifier
/// @param[in] defaultFile        Default file path
/// @returns Dialog result
/// @throws runtime_error  Dialog creation error or runtime error
Dialog::result_t FileDialog::showDialog(const int32_t dialogResourceId, const int32_t fieldResourceId, const int32_t browserResourceId, 
                                        const std::wstring& defaultFile)
{
    m_filePath = defaultFile;
    m_fieldId = fieldResourceId;
    m_browserId = browserResourceId;
    return Dialog::showDialog(dialogResourceId, false);
}


/// @brief Dialog initialization event handler
DIALOG_EVENT_RETURN FileDialog::onInit(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    FileDialog& parent = getEventTargetDialogReference(FileDialog);

    //...pre-fill field with m_filePath
    //...

    return DIALOG_EVENT_RETURN_VALID;
}


/// @brief Dialog initialization event handler
DIALOG_EVENT_RETURN FileDialog::onCommand(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    FileDialog& parent = getEventTargetDialogReference(FileDialog);

    //...browser
    //...

    return DIALOG_EVENT_RETURN_VALID;
}


/// @brief Dialog confirm event handler - check validity
DIALOG_EVENT_RETURN FileDialog::onConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    FileDialog& parent = getEventTargetDialogReference(FileDialog);

    //...check path
    //...

    return DIALOG_EVENT_RETURN_VALID;
}
