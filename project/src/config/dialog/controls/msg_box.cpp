/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : message box toolset
*******************************************************************************/
#include "../../../globals.h"
#include <cstdint>
#include <string>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#endif
#include "msg_box.h"
using namespace config::dialog::controls;

/// @brief Show message box
/// @param[in] title    Message box title
/// @param[in] message  Displayed message
/// @param[in] hWindow  Parent window (use nullptr for non-modal message box)
/// @param[in] buttons  Message box buttons (optional)
/// @param[in] icon     Message box icon (optional)
/// @returns User action
MsgBox::result_t MsgBox::showMessage(const std::wstring& title, const std::wstring& message, window_handle_t hWindow, const MsgBox::button_set_t buttons, const MsgBox::message_icon_t icon)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    int32_t result = MessageBox(reinterpret_cast<HWND>(hWindow), (LPCWSTR)message.c_str(), (LPCWSTR)title.c_str(), 
                                static_cast<int32_t>(buttons) | static_cast<int32_t>(icon));
    switch (result)
    {
        case IDABORT:    result = IDCANCEL; break;
        case IDCONTINUE: result = IDYES; break;
        case IDIGNORE:   result = IDNO; break;
        case IDTRYAGAIN: result = IDRETRY; break;
        default:         break;
    }
    return static_cast<MsgBox::result_t>(result);
}
#else
{
    //...
}
#endif
