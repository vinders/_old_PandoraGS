/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : track-bar toolset
*******************************************************************************/
#include "../../../globals.h"
#include <cstdint>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#endif
#include "../../../res/resource.h"
#include "common.h"
#include "track_bar.h"
using namespace config::dialog;
using namespace config::dialog::controls;


/// @brief Initialize track-bar control
/// @param[in] hWindow     Parent window handle
/// @param[in] resourceId  Track-bar resource identifier
/// @param[in] maxValue    Max value (range = 0 to max)
/// @param[in] unitSize    Minimum step unit
/// @param[in] pageSize    Page-step unit
/// @returns Success
bool TrackBar::initControl(window_handle_t hWindow, const int32_t resourceId, const uint32_t maxValue, const uint32_t unitSize, const uint32_t pageSize)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    HWND hControl = GetDlgItem(hWindow, resourceId);
    if (!hControl || hControl == (HWND)INVALID_HANDLE_VALUE)
        return false;

    SendMessageW(hControl, TBM_SETRANGE, TRUE, MAKELONG(0, maxValue));
    SendMessageW(hControl, TBM_SETPAGESIZE, 0, pageSize);
    SendMessageW(hControl, TBM_SETSEL, 0, MAKELONG(0, maxValue));
    SendMessageW(hControl, TBM_SETTICFREQ, unitSize, 0);
    return true;
}
#else
{
    //...
    return true;
}
#endif


/// @brief Initialize track-bar control
/// @param[in] hWindow     Parent window handle
/// @param[in] resourceId  Track-bar resource identifier
/// @param[in] value       Value to set
void TrackBar::setValue(window_handle_t hWindow, const int32_t resourceId, const uint32_t value)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    HWND hControl = GetDlgItem(hWindow, resourceId);
    if (!hControl || hControl == (HWND)INVALID_HANDLE_VALUE)
        return;
    SendMessageW(hControl, TBM_SETPOS, TRUE, (int32_t)value);
}
#else
{
    //...
}
#endif


/// @brief Initialize track-bar control
/// @param[in] hWindow     Parent window handle
/// @param[in] resourceId  Track-bar resource identifier
/// @returns Current value (or -1)
uint32_t TrackBar::getValue(window_handle_t hWindow, const int32_t resourceId)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    HWND hControl = GetDlgItem(hWindow, resourceId);
    if (!hControl || hControl == (HWND)INVALID_HANDLE_VALUE)
        return -1;
    return SendMessage(hControl, TBM_GETPOS, 0, 0);
}
#else
{
    //...
}
#endif
