/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : combo box toolset
*******************************************************************************/
#include "../../../globals.h"
#include <cstdint>
#include <string>
#include <vector>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#endif
#include "../../../res/resource.h"
#include "common.h"
#include "combo_box.h"
using namespace config::dialog;
using namespace config::dialog::controls;


/// @brief Initialize list of values in combo box
/// @param[in] hWindow        Parent window handle
/// @param[in] resourceId     Combo box resource identifier
/// @param[in] values         Values inserted in combo box
/// @param[in] selectedIndex  Default index to select
/// @returns Success
bool ComboBox::initValues(window_handle_t hWindow, const int32_t resourceId, const std::vector<std::wstring>& values, int32_t selectedIndex)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    HWND hCombobox;
    hCombobox = GetDlgItem(hWindow, resourceId);
    if (!hCombobox || hCombobox == reinterpret_cast<HWND>(INVALID_HANDLE_VALUE))
        return false;

    if (selectedIndex > static_cast<int32_t>(values.size())) // index below max value
        selectedIndex = 0;

    // set values
    for (uint32_t i = 0; i < values.size(); ++i)
        ComboBox_AddString(hCombobox, (LPCTSTR)values.at(i).c_str());
    // set selected index
    return (ComboBox_SetCurSel(hCombobox, selectedIndex) != CB_ERR);
}
#else
{
    return true;
}
#endif

/// @brief Change list of values in combo box
/// @param[in] hWindow        Parent window handle
/// @param[in] resourceId     Combo box resource identifier
/// @param[in] values         Values inserted in combo box
/// @param[in] selectedIndex  Default index to select (optional : if not set, keeps same index as previous)
/// @returns Success
bool ComboBox::setValues(window_handle_t hWindow, const int32_t resourceId, const std::vector<std::wstring>& values, int32_t selectedIndex)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    HWND hCombobox;
    hCombobox = GetDlgItem(hWindow, resourceId);
    if (!hCombobox || hCombobox == reinterpret_cast<HWND>(INVALID_HANDLE_VALUE))
        return false;

    // if same index kept, read it first
    if (selectedIndex < 0)
    {
        int32_t buffer = SendMessage(hCombobox, CB_GETCURSEL, NULL, NULL);
        selectedIndex = (buffer != CB_ERR) ? buffer : 0;
    }
    if (selectedIndex > static_cast<int32_t>(values.size())) // index below max value
        selectedIndex = 0;

    // clear content + set new values
    ComboBox_ResetContent(hCombobox);
    for (uint32_t i = 0; i < values.size(); ++i)
        ComboBox_AddString(hCombobox, (LPCTSTR)values.at(i).c_str());
    // set selected index
    return (ComboBox_SetCurSel(hCombobox, selectedIndex) != CB_ERR);
}
#else
{
    return true;
}
#endif


/// @brief Get selected index in combo box
/// @param[in] hWindow     Parent window handle
/// @param[in] resourceId  Combo box resource identifier
/// @param[out] outIndex   Selected index (output)
/// @returns Success
bool ComboBox::getSelectedIndex(window_handle_t hWindow, const int32_t resourceId, int32_t& outIndex)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    HWND hCombobox;
    hCombobox = GetDlgItem(hWindow, resourceId);
    if (!hCombobox || hCombobox == reinterpret_cast<HWND>(INVALID_HANDLE_VALUE))
        return false;

    int32_t buffer = SendMessage(hCombobox, CB_GETCURSEL, NULL, NULL);
    if (buffer != CB_ERR)
    {
        outIndex = buffer;
        return true;
    }
    return false;
}
#else
{
    return true;
}
#endif


/// @brief Set selected index in combo box
/// @param[in] hWindow        Parent window handle
/// @param[in] resourceId     Combo box resource identifier
/// @param[in] selectedIndex  Selected index
/// @returns Success
bool ComboBox::setSelectedIndex(window_handle_t hWindow, const int32_t resourceId, const int32_t selectedIndex)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    HWND hCombobox;
    hCombobox = GetDlgItem(hWindow, resourceId);
    if (!hCombobox || hCombobox == reinterpret_cast<HWND>(INVALID_HANDLE_VALUE))
        return false;

    return (ComboBox_SetCurSel(hCombobox, selectedIndex) != CB_ERR);
}
#else
{
    return true;
}
#endif
