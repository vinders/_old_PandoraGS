/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   win_toolbox.cpp
Description : winAPI toolbox
*******************************************************************************/
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
using namespace std;
#include "win_toolbox.hpp"

HBRUSH WinToolbox::s_hBackgroundBrush = NULL;

/// <summary>Show/hide page</summary>
/// <param name="itemId">Item associated with tooltip (ID)</param>
/// <param name="hWindow">Parent dialog handle</param>
/// <param name="phInst">Window instance</param>
/// <param name="pText">Tooltip text</param>
/// <returns>Tooltip handle</returns>
HWND WinToolbox::createToolTip(int itemId, HWND hWindow, HINSTANCE* phInst, PTSTR pText)
{
    // check arguments
    if (!itemId || !hWindow || !pText)
        return (HWND)NULL;
    HWND hItem = GetDlgItem(hWindow, itemId);
    if (!hItem)
        return (HWND)NULL;

    // tooltip creation
    HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWindow, NULL, *phInst, NULL);
    if (!hwndTip)
        return (HWND)NULL;

    // tooltip association with item
    TOOLINFO ti = { 0 };
    ti.cbSize = sizeof(ti);
    ti.hwnd = hWindow;
    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    ti.uId = (UINT_PTR)hItem;
    ti.lpszText = pText;
    SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
    return hwndTip;
}

/// <summary>Show/hide page</summary>
/// <param name="hTooltip">Tooltip handle</param>
/// <param name="itemId">Item associated with tooltip (ID)</param>
/// <param name="hWindow">Parent dialog handle</param>
/// <param name="pText">Tooltip text</param>
void WinToolbox::updateToolTip(HWND hTooltip, int itemId, HWND hWindow, PTSTR pText)
{
    // check arguments
    if (!itemId || !hTooltip || !pText)
        return;
    HWND hItem = GetDlgItem(hWindow, itemId);
    if (!hItem)
        return;

    // update tooltip definition
    TOOLINFO ti = { 0 };
    ti.cbSize = sizeof(ti);
    ti.hwnd = hWindow;
    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    ti.uId = (UINT_PTR)hItem;
    ti.lpszText = pText;
    SendMessage(hTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&ti);
}

/// <summary>Delete page background brush</summary>
void WinToolbox::destroyPageBackgroundBrush()
{
    if (s_hBackgroundBrush)
        DeleteObject(s_hBackgroundBrush);
    s_hBackgroundBrush = NULL;
}

#endif
