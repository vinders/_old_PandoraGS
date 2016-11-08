/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_pprofile_winview.h
Description : configuration dialog page - profile settings - view
*******************************************************************************/
#ifndef _CONFIG_PPROFILE_WINVIEW_H
#define _CONFIG_PPROFILE_WINVIEW_H
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32

#include "config_page_winview.h"

#define CONFIG_PROFILE_TABS_NB 3
#define CONFIG_PROFILE_TAB_FILTERS 0
#define CONFIG_PROFILE_TAB_STRETCH 1
#define CONFIG_PROFILE_TAB_COMPAT  2
#define PPROFILE_TOOLTIPS_NB 1


// Configuration dialog page - profile settings - view
class ConfigPageProfileView : public ConfigPageView
{
private:
    HWND res_tabPages[CONFIG_PROFILE_TABS_NB]; // tabs
    int  m_activeTab; // current tab index
public:
    HWND res_tooltips[PPROFILE_TOOLTIPS_NB];
    static ConfigPageProfileView* s_pCurrentPage; // current page (static access)

    /// <summary>Create page view container</summary>
    /// <param name="pController">Controller reference</param>
    ConfigPageProfileView(ConfigPage* pController);
    /// <summary>Destroy dialog view container</summary>
    ~ConfigPageProfileView();

    /// <summary>Create new dialog page</summary>
    /// <param name="pController">Controller reference</param>
    /// <returns>Page created</returns>
    /// <exception cref="std::exception">Creation failure</exception>
    static ConfigPageProfileView* createPage(ConfigPageProfile* pController);

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

    /// <summary>Set upscaling type list, based on factor</summary>
    /// <param name="phComboBox">Control handle reference</param>
    /// <param name="factor">Multiplication factor</param>
    /// <param name="value">Config value</param>
    void setUpscalingList(HWND* phComboBox, uint32_t factor, uint32_t value);

    // EVENTS ----------------------------------------------------------------------

    /// <summary>Tabcontrol event handler</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="hTabControl">Tab control handle</param>
    /// <returns>Success</returns>
    INT_PTR onTabChange(HWND hWindow, HWND hTabControl);

    /// <summary>Page drawing handler</summary>
    /// <param name="hWindow">Window handle</param>
    /// <returns>Action code</returns>
    static INT_PTR onPaint(HWND hWindow);
    /// <summary>Page drawing handler</summary>
    /// <param name="wParam">Command</param>
    /// <returns>Action code</returns>
    static INT_PTR onDrawColorStatic(WPARAM wParam);

    /// <summary>Page event handler</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="msg">Event message</param>
    /// <param name="wParam">Command</param>
    /// <param name="lParam">Informations</param>
    /// <returns>Action code</returns>
    static INT_PTR CALLBACK eventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
    /// <summary>Simple sub-tab event handler</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="msg">Event message</param>
    /// <param name="wParam">Command</param>
    /// <param name="lParam">Informations</param>
    /// <returns>Action code</returns>
    static INT_PTR CALLBACK tabEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif
#endif
