/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_pgeneral_winview.h
Description : configuration dialog page - general - view
*******************************************************************************/
#ifndef _CONFIG_PGENERAL_WINVIEW_H
#define _CONFIG_PGENERAL_WINVIEW_H
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32

#include <windows.h>
#include <windowsx.h>
#include "config_page_winview.h"

#define PGENERAL_TOOLTIPS_NB 1


// Configuration dialog page - general settings - view
class ConfigPageGeneralView : public ConfigPageView
{
public:
    HWND res_tooltips[PGENERAL_TOOLTIPS_NB];
    static ConfigPageGeneralView* s_pCurrentPage; // current page (static access)

    /// <summary>Create page view container</summary>
    /// <param name="pController">Controller reference</param>
    ConfigPageGeneralView(ConfigPage* pController);
    /// <summary>Destroy dialog view container</summary>
    ~ConfigPageGeneralView();

    /// <summary>Create new dialog page</summary>
    /// <param name="pController">Controller reference</param>
    /// <returns>Page created</returns>
    /// <exception cref="std::exception">Creation failure</exception>
    static ConfigPageGeneralView* createPage(ConfigPageGeneral* pController);

    /// <summary>Refresh language-dependent page content</summary>
    /// <param name="isFirstInit">First time (only labels) or update (all)</param>
    virtual void resetLanguage(bool isFirstInit);
    /// <summary>Copy UI settings to global configuration</summary>
    virtual void updateConfig();

    /// <summary>Load page content</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="phInstance">Instance handle reference</param>
    /// <param name="pPageSize">Page limits (rectangle)</param>
    /// <param name="isVisible">Visibility (show/hide)</param>
    /// <exception cref="std::exception">Creation failure</exception>
    virtual void loadPage(HWND hWindow, HINSTANCE* phInstance, RECT* pPageSize, bool isVisible);
    /// <summary>Load resolutions list</summary>
    void loadResolutions();
    /// <summary>Add custom resolution to combobox</summary>
    /// <param name="hResList">Combobox handle</param>
    /// <param name="pText">Displayed resolution</param>
    /// <param name="width">Resolution width</param>
    /// <param name="height">Resolution height</param>
    /// <param name="pCurrentIndex">Current list index</param>
    /// <param name="pSelectedIndex">List selection</param>
    static inline void addCustomResolution(HWND hResList, WCHAR* pText, uint32_t width, uint32_t height,
        uint32_t* pCurrentIndex, uint32_t* pSelectedIndex)
    {
        if (ComboBox_FindString(hResList, -1, pText) == CB_ERR)
        {
            ComboBox_AddString(hResList, pText);
            if (Config::dsp_fullscnResX == width && Config::dsp_fullscnResY == height)
                *pSelectedIndex = *pCurrentIndex;
            *pCurrentIndex += 1;
        }
    }

    /// <summary>Page event handler</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="msg">Event message</param>
    /// <param name="wParam">Command</param>
    /// <param name="lParam">Informations</param>
    /// <returns>Action code</returns>
    static INT_PTR CALLBACK eventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);

    // DIALOGS ---------------------------------------------------------------------

    /// <summary>Advanced settings dialog event handler</summary>
    /// <param name="hWindow">Dialog handle</param>
    /// <param name="msg">Event message</param>
    /// <param name="wParam">Command</param>
    /// <param name="lParam">Informations</param>
    /// <returns>Action code</returns>
    static INT_PTR CALLBACK advancedSettingsEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
    /// <summary>Key binding dialog event handler</summary>
    /// <param name="hWindow">Dialog handle</param>
    /// <param name="msg">Event message</param>
    /// <param name="wParam">Command</param>
    /// <param name="lParam">Informations</param>
    /// <returns>Action code</returns>
    static INT_PTR CALLBACK keyBindingEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif
#endif