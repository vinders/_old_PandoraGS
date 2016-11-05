/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_page_winview.h
Description : configuration dialog page - view
*******************************************************************************/
#ifndef _CONFIG_PAGE_WINVIEW_H
#define _CONFIG_PAGE_WINVIEW_H
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <commctrl.h>

#define COLOR_GROUPBOX_BORDER RGB(210,219,226)
#define COLOR_GROUPBOX_BAR    RGB(240,243,245)
#define COLOR_GROUPBOX_TEXT   RGB(94,97,99)
#define GROUPBOX_BAR_HEIGHT   24

#ifndef _CONFIG_PAGE_H_END
class ConfigPage;
#endif

// Configuration dialog page - view
class ConfigPageView
{
protected:
    ConfigPage* m_pController; // controller reference
    HWND m_hPage; // page handle

public:
    /// <summary>Create page view container</summary>
    /// <param name="pController">Controller reference</param>
    ConfigPageView(ConfigPage* pController);
    /// <summary>Destroy dialog view container</summary>
    virtual ~ConfigPageView();

    /// <summary>Refresh language-dependent page content</summary>
    /// <param name="isFirstInit">First time (only labels) or update (all)</param>
    virtual void resetLanguage(bool isFirstInit) = 0;
    /// <summary>Copy UI settings to global configuration</summary>
    virtual void updateConfig() = 0;

    /// <summary>Load page content</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="phInstance">Instance handle reference</param>
    /// <param name="pPageSize">Page limits (rectangle)</param>
    /// <param name="isVisible">Visibility (show/hide)</param>
    /// <exception cref="std::exception">Creation failure</exception>
    virtual void loadPage(HWND hWindow, HINSTANCE* phInstance, RECT* pPageSize, bool isVisible) = 0;
    /// <summary>Show/hide page</summary>
    /// <param name="isVisible">Visibility (show/hide)</param>
    void showPage(bool isVisible);

    /// <summary>Show/hide page</summary>
    /// <param name="itemId">Item associated with tooltip (ID)</param>
    /// <param name="hWindow">Parent dialog handle</param>
    /// <param name="phInst">Window instance</param>
    /// <param name="pText">Tooltip text</param>
    /// <returns>Tooltip handle</returns>
    static HWND createToolTip(int itemId, HWND hWindow, HINSTANCE* phInst, PTSTR pText);
    /// <summary>Show/hide page</summary>
    /// <param name="hTooltip">Tooltip handle</param>
    /// <param name="itemId">Item associated with tooltip (ID)</param>
    /// <param name="hWindow">Parent dialog handle</param>
    /// <param name="pText">Tooltip text</param>
    static void updateToolTip(HWND hTooltip, int itemId, HWND hWindow, PTSTR pText);
};

#include "config_page.h"

#endif
#endif
