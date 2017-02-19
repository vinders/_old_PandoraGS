/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : button icon - background image for button
*******************************************************************************/
#include "../../../globals.h"
#include <cstdint>
#include <string>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#endif
#include "common.h"
#include "button_icon.h"
using namespace config::dialog;
using namespace config::dialog::controls;


/// @brief Create icon for buttons
/// @param[in] instance         Current instance handle
/// @param[in] imageResourceId  Image resource identifier
/// @param[in] iconWidth        Icon width
/// @param[in] iconHeight       Icon height
ButtonIcon::ButtonIcon(library_instance_t instance, const int32_t imageResourceId, const uint32_t iconWidth, const uint32_t iconHeight)
{
    m_hIcon = 0;
    #if _DIALOGAPI == DIALOGAPI_WIN32
    m_hIcon = LoadImage(instance, MAKEINTRESOURCE(imageResourceId), IMAGE_ICON, iconWidth, iconHeight, NULL);
    #endif
}

/// @brief Destroy button icon
ButtonIcon::~ButtonIcon()
{
    #if _DIALOGAPI == DIALOGAPI_WIN32
    if (m_hIcon != NULL)
        DestroyIcon((HICON)m_hIcon);
    #endif
}


/// @brief Assign icon to a button
/// @param[in] hWindow           Parent container
/// @param[in] buttonResourceId  Identifier of assigned button
/// @param[in] placeHolder       Place-holder text, if icon assignment fails
void ButtonIcon::addIconToButton(window_handle_t hWindow, const int32_t buttonResourceId, std::wstring placeHolder)
{
    #if _DIALOGAPI == DIALOGAPI_WIN32
    if (m_hIcon)
        SendDlgItemMessage(hWindow, buttonResourceId, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIcon);
    else
        SendDlgItemMessage(hWindow, buttonResourceId, WM_SETTEXT, 0, (LPARAM)placeHolder.c_str());
    #endif
}
