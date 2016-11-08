/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_pprofile_winview.h
Description : configuration dialog page - profile settings - view
*******************************************************************************/
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
using namespace std;
#include "win_toolbox.hpp"
#include "config_pprofile_winview.h"

ConfigPageProfileView* ConfigPageProfileView::s_pCurrentPage = NULL; // current page (static access)
static HBRUSH gs_hBrushColor = NULL;

/// <summary>Create page view container</summary>
/// <param name="pController">Controller reference</param>
ConfigPageProfileView::ConfigPageProfileView(ConfigPage* pController) : ConfigPageView(pController)
{
    m_hPage = NULL;
    for (int i = 0; i < CONFIG_PROFILE_TABS_NB; ++i)
        res_tabPages[i] = NULL;
    m_activeTab = 0;
    for (int t = 0; t < PPROFILE_TOOLTIPS_NB; ++t)
        res_tooltips[t] = NULL;
}
/// <summary>Destroy dialog view container</summary>
ConfigPageProfileView::~ConfigPageProfileView()
{
    // delete tooltips
    for (int t = 0; t < PPROFILE_TOOLTIPS_NB; ++t)
    {
        if (res_tooltips[t] != NULL) DestroyWindow(res_tooltips[t]);
        res_tooltips[t] = NULL;
    }
    // delete tabs
    for (int i = 0; i < CONFIG_PROFILE_TABS_NB; ++i)
    {
        if (res_tabPages[i] != NULL) DestroyWindow(res_tabPages[i]);
    }
    // delete items
    if (m_hPage != NULL)
        DestroyWindow(m_hPage);
    m_hPage = NULL;
    // delete brush
    if (gs_hBrushColor)
        DeleteObject(gs_hBrushColor);
    gs_hBrushColor = NULL;
}

/// <summary>Create new dialog page</summary>
/// <param name="pController">Controller reference</param>
/// <returns>Page created</returns>
/// <exception cref="std::exception">Creation failure</exception>
ConfigPageProfileView* ConfigPageProfileView::createPage(ConfigPageProfile* pController)
{
    ConfigPageProfileView* pPage = new ConfigPageProfileView(pController);
    return pPage;
}


/// <summary>Refresh language-dependent page content</summary>
/// <param name="isFirstInit">First time (only labels) or update (all)</param>
void ConfigPageProfileView::resetLanguage(bool isFirstInit)
{
    LanguageDialogResource* pLang = m_pController->getLangResource();

    // set labels
    //
    if (isFirstInit == false)
    {
        // set tooltips
        //
    }
}
/// <summary>Copy UI settings to global configuration</summary>
void ConfigPageProfileView::updateConfig()
{

}

/// <summary>Load page content</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="phInstance">Instance handle reference</param>
/// <param name="pPageSize">Page limits (rectangle)</param>
/// <param name="isVisible">Visibility (show/hide)</param>
/// <exception cref="std::exception">Creation failure</exception>
void ConfigPageProfileView::loadPage(HWND hWindow, HINSTANCE* phInstance, RECT* pPageSize, bool isVisible)
{
    s_pCurrentPage = this;
    m_hPage = CreateDialog(*phInstance, MAKEINTRESOURCE(IDD_PROFILE_PAGE), hWindow, (DLGPROC)eventHandler);
    if (!m_hPage)
        throw std::exception();
    SetWindowPos(m_hPage, NULL, pPageSize->left, pPageSize->top,
        pPageSize->right - pPageSize->left, pPageSize->bottom - pPageSize->top, 0);
    if (isVisible)
    {
        ShowWindow(m_hPage, SW_SHOW);
        EnableWindow(m_hPage, TRUE);
    }
    else
    {
        ShowWindow(m_hPage, SW_HIDE);
        EnableWindow(m_hPage, FALSE);
    }

    // tab control
    HWND hTabControl = GetDlgItem(m_hPage, IDC_PROFILE_TABS);
    TCITEM ti;
    ti.mask = TCIF_TEXT;
    ti.pszText = L"Filters";//...lang
    TabCtrl_InsertItem(hTabControl, 0, &ti);
    ti.pszText = L"Screen stretching";//...lang
    TabCtrl_InsertItem(hTabControl, 1, &ti);
    ti.pszText = L"Compatibility settings";//...lang
    TabCtrl_InsertItem(hTabControl, 2, &ti);
    TabCtrl_SetCurSel(hTabControl, 0);
    // tabs content
    m_activeTab = 0;
    res_tabPages[CONFIG_PROFILE_TAB_FILTERS] = CreateDialog(*phInstance, MAKEINTRESOURCE(IDD_PROFILE_FILTERS_TAB), m_hPage, tabEventHandler);
    res_tabPages[CONFIG_PROFILE_TAB_STRETCH] = CreateDialog(*phInstance, MAKEINTRESOURCE(IDD_PROFILE_STRETCHING_TAB), m_hPage, tabEventHandler);
    res_tabPages[CONFIG_PROFILE_TAB_COMPAT] = CreateDialog(*phInstance, MAKEINTRESOURCE(IDD_PROFILE_COMPAT_TAB), m_hPage, tabEventHandler);
    ShowWindow(res_tabPages[0], SW_SHOW);
    EnableWindow(res_tabPages[0], TRUE);
    for (int t = 1; t < CONFIG_PROFILE_TABS_NB; ++t)
    {
        ShowWindow(res_tabPages[t], SW_HIDE);
        EnableWindow(res_tabPages[t], FALSE);
    }
    // config values
    loadConfig();
}

/// <summary>Load page config</summary>
void ConfigPageProfileView::loadConfig()
{
    // set language-independant controls
    std::wstring* pList = NULL;
    int listLength = 0;
    uint32_t selection = 0;
    HWND hControl = NULL;
    if (hControl = GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_TXUPSCALE_FACTOR))
    {
        selection = Config::getCurrentProfile()->scl_texUpscaleVal;
        if (selection < 1 || selection > 5)
            selection = 2;
        ComboBox_AddString(hControl, (LPCTSTR)L"1x");
        ComboBox_AddString(hControl, (LPCTSTR)L"2x");
        ComboBox_AddString(hControl, (LPCTSTR)L"3x");
        ComboBox_AddString(hControl, (LPCTSTR)L"4x");
        ComboBox_AddString(hControl, (LPCTSTR)L"5x");
        ComboBox_SetCurSel(hControl, selection - 1);
    }
    selection = ConfigPageProfile::setUpscalingList(&pList, &listLength, selection, Config::getCurrentProfile()->scl_texUpscaleType);
    WinToolbox::setComboboxValues(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_TXUPSCALE_LIST), selection, pList, listLength);

    hControl = NULL;
    if (hControl = GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_2DUPSCALE_FACTOR))
    {
        selection = Config::getCurrentProfile()->scl_sprUpscaleVal;
        if (selection < 1 || selection > 8 || selection == 6 || selection == 7)
            selection = 2;
        ComboBox_AddString(hControl, (LPCTSTR)L"1x");
        ComboBox_AddString(hControl, (LPCTSTR)L"2x");
        ComboBox_AddString(hControl, (LPCTSTR)L"3x");
        ComboBox_AddString(hControl, (LPCTSTR)L"4x");
        ComboBox_AddString(hControl, (LPCTSTR)L"5x");
        ComboBox_AddString(hControl, (LPCTSTR)L"8x");
        ComboBox_SetCurSel(hControl, (selection == 8) ? 8 : (selection - 1));
    }
    selection = ConfigPageProfile::setUpscalingList(&pList, &listLength, selection, Config::getCurrentProfile()->scl_sprUpscaleType);
    WinToolbox::setComboboxValues(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_2DUPSCALE_LIST), selection, pList, listLength);

    hControl = NULL;
    if (hControl = GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_FXAA_LIST))
    {
        ComboBox_AddString(hControl, (LPCTSTR)L"FXAA");
        ComboBox_AddString(hControl, (LPCTSTR)L"NFAA");
        ComboBox_AddString(hControl, (LPCTSTR)L"MSAA 4x");
        ComboBox_AddString(hControl, (LPCTSTR)L"MSAA 8x");
        ComboBox_AddString(hControl, (LPCTSTR)L"SMAA 4x");
        ComboBox_AddString(hControl, (LPCTSTR)L"SMAA 8x");
        ComboBox_SetCurSel(hControl, 0L);
    }

    // set language
    resetLanguage(true);

    if (pList != NULL)
        delete[] pList;
    pList = NULL;
}

// EVENTS ----------------------------------------------------------------------

/// <summary>Tabcontrol event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="hTabControl">Tab control handle</param>
/// <returns>Success</returns>
INT_PTR ConfigPageProfileView::onTabChange(HWND hWindow, HWND hTabControl)
{
    // tab change
    if (res_tabPages[m_activeTab] != NULL)
    {
        ShowWindow(res_tabPages[m_activeTab], SW_HIDE);
        EnableWindow(res_tabPages[m_activeTab], FALSE);
        m_activeTab = TabCtrl_GetCurSel(hTabControl); // set new tab
        if (m_activeTab == -1)
        {
            m_activeTab = 0;
            TabCtrl_SetCurSel(hTabControl, 0);
        }
        ShowWindow(res_tabPages[m_activeTab], SW_SHOW);
        EnableWindow(res_tabPages[m_activeTab], TRUE);
    }
    return (INT_PTR)TRUE;
}

/// <summary>Page event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="msg">Event message</param>
/// <param name="wParam">Command</param>
/// <param name="lParam">Informations</param>
/// <returns>Action code</returns>
INT_PTR CALLBACK ConfigPageProfileView::eventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        // tab change
        case WM_NOTIFY:
        {
            switch (((LPNMHDR)lParam)->code)
            {
                case TCN_SELCHANGE: 
                    return ConfigPageProfileView::s_pCurrentPage->onTabChange(hWindow, ((LPNMHDR)lParam)->hwndFrom); break;
            }
            return (INT_PTR)TRUE;
        }

        // controls interaction
        case WM_COMMAND:
        {
            // combobox
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                //switch (LOWORD(wParam))
                {
                }
            }
            // button
            else
            {
                //switch (LOWORD(wParam))
                {
                }
            }
            return (INT_PTR)FALSE; break;
        } // WM_COMMAND
    }
    // drawing events
    return WinToolbox::pageDrawingEventHandler(hWindow, msg, wParam, lParam, COLOR_PAGE);
}

/// <summary>Simple sub-tab event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="msg">Event message</param>
/// <param name="wParam">Command</param>
/// <param name="lParam">Informations</param>
/// <returns>Action code</returns>
INT_PTR CALLBACK ConfigPageProfileView::tabEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // controls interaction
    if (msg == WM_COMMAND)
    {
        // combobox
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            //switch (LOWORD(wParam))
            {
            }
        }
        // button
        else
        {
            //switch (LOWORD(wParam))
            {
            }
        }
        return (INT_PTR)FALSE;
    }
    // drawing events
    return WinToolbox::pageDrawingEventHandler(hWindow, msg, wParam, lParam, COLOR_PAGE);
}

#endif
