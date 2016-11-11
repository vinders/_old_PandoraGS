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
#include "pandoraGS.h"

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
    ConfigProfile* pProfile = Config::getCurrentProfile();

    // anti-aliasing
    int selection = COMBOBOX_USE_PREVIOUS_INDEX;
    if (isFirstInit)
    {
        selection = pProfile->shd_antiAliasing - 1;
        if (selection < 0)
            selection = 0;
    }
    WinToolbox::setComboboxValues(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_FXAA_LIST),
                selection, pLang->profile_antialiasing, LANG_PROFILE_AA);
    // smoothing
    WinToolbox::setComboboxValues(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_TXFILTER_LIST),
                (isFirstInit) ? pProfile->scl_texSmooth : COMBOBOX_USE_PREVIOUS_INDEX, pLang->profile_interpolation, LANG_PROFILE_INTERPOLATION);
    WinToolbox::setComboboxValues(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_2DFILTER_LIST),
                (isFirstInit) ? pProfile->scl_sprSmooth : COMBOBOX_USE_PREVIOUS_INDEX, pLang->profile_interpolation, LANG_PROFILE_INTERPOLATION);
    WinToolbox::setComboboxValues(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_SCRFILTER_LIST),
                (isFirstInit) ? pProfile->scl_screenSmooth : COMBOBOX_USE_PREVIOUS_INDEX, pLang->profile_screenSmooth, LANG_PROFILE_SCREENSMOOTH);

    if (isFirstInit)
    {
        // internal resolution
        WinToolbox::setComboboxValues(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_INTRESX_LIST),
                                      (pProfile->dsp_internalResX <= 8u) ? pProfile->dsp_internalResX/2u : 1u, 
                                      pLang->profile_internal_resx, LANG_PROFILE_INTERNAL_RESX);
        WinToolbox::setComboboxValues(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_INTRESY_LIST),
                                      (pProfile->dsp_internalResY <= 4u) ? pProfile->dsp_internalResY/2u : (pProfile->dsp_internalResY - 2u)/2u, 
                                      pLang->profile_internal_resy, LANG_PROFILE_INTERNAL_RESY);
        // stretching presets
        uint32_t selection = 0u;
        if (pProfile->dsp_stretchRatio == CFG_RATIO_STRETCH_FullWindow && pProfile->dsp_cropStrength == CFG_RATIO_CROP_FullWindow)
            selection = 1u;
        else if (pProfile->dsp_stretchRatio == CFG_RATIO_STRETCH_Orig && pProfile->dsp_cropStrength == CFG_RATIO_CROP_Orig)
            selection = 2u;
        else if (pProfile->dsp_stretchRatio == CFG_RATIO_STRETCH_OrigFill && pProfile->dsp_cropStrength == CFG_RATIO_CROP_OrigFill)
            selection = 3u;
        else if (pProfile->dsp_stretchRatio == CFG_RATIO_STRETCH_CloseToOrig && pProfile->dsp_cropStrength == CFG_RATIO_CROP_CloseToOrig)
            selection = 4u;
        else
            selection = 0u;
        WinToolbox::setComboboxValues(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_PRESET_LIST),
                                      selection, pLang->profile_stretch_presets, LANG_PROFILE_STRETCH_PRESETS);
    }
    else
    {
        // internal resolution
        WinToolbox::setComboboxValues(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_INTRESX_LIST),
                                      COMBOBOX_USE_PREVIOUS_INDEX, pLang->profile_internal_resx, LANG_PROFILE_INTERNAL_RESX);
        WinToolbox::setComboboxValues(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_INTRESY_LIST),
                                      COMBOBOX_USE_PREVIOUS_INDEX, pLang->profile_internal_resy, LANG_PROFILE_INTERNAL_RESY);
        // stretching presets
        WinToolbox::setComboboxValues(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_PRESET_LIST),
                                      COMBOBOX_USE_PREVIOUS_INDEX, pLang->profile_stretch_presets, LANG_PROFILE_STRETCH_PRESETS);

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
    res_tabPages[CONFIG_PROFILE_TAB_STRETCH] = CreateDialog(*phInstance, MAKEINTRESOURCE(IDD_PROFILE_STRETCHING_TAB), m_hPage, screenStretchingTabEventHandler);
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
    ConfigProfile* pProfile = Config::getCurrentProfile();
    std::wstring* pList = NULL;
    int listLength = 0;
    uint32_t selection = 0u;
    // texture upscaling
    selection = WinToolbox::setUpscalingFactors(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_TXUPSCALE_FACTOR), 
                                                pProfile->scl_texUpscaleVal, 2u, 4u);
    selection = ConfigPageProfile::setUpscalingList(&pList, &listLength, selection, pProfile->scl_texUpscaleType);
    WinToolbox::setComboboxValues(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_TXUPSCALE_LIST), selection, pList, listLength);
    // sprite upscaling
    selection = WinToolbox::setUpscalingFactors(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_2DUPSCALE_FACTOR), 
                                                pProfile->scl_sprUpscaleVal, 2u, 8u);
    selection = ConfigPageProfile::setUpscalingList(&pList, &listLength, selection, pProfile->scl_sprUpscaleType);
    WinToolbox::setComboboxValues(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_2DUPSCALE_LIST), selection, pList, listLength);
    // screen upscaling
    selection = WinToolbox::setUpscalingFactors(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_SCRUPSCALE_FACTOR), 
                                                pProfile->scl_screenUpscaleVal, 1u, 8u);
    if (selection > 1u)
        selection = ConfigPageProfile::setUpscalingList(&pList, &listLength, selection, pProfile->scl_screenUpscaleType);
    else
    {
        selection = 0u;
        listLength = 1;
        pList[0] = L"normal";
    }
    WinToolbox::setComboboxValues(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_SCRUPSCALE_LIST), selection, pList, listLength);
    // texture/screen misc
    if(pProfile->scl_mdecFilter != 0u)
        CheckDlgButton(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_MDEC_CHECK, BST_CHECKED);
    // antialiasing
    if (pProfile->shd_antiAliasing != 0u)
        CheckDlgButton(res_tabPages[CONFIG_PROFILE_TAB_FILTERS], IDC_PRO_FXAA_CHECK, BST_CHECKED);

    // black borders
    std::wstring border = std::to_wstring(pProfile->dsp_borderSizeX);
    SetDlgItemText(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_BLACKBORDERSX_EDIT, (LPCTSTR)border.c_str());
    border = std::to_wstring(pProfile->dsp_borderSizeY);
    SetDlgItemText(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_BLACKBORDERSY_EDIT, (LPCTSTR)border.c_str());
    if (pProfile->dsp_borderSizeX != 0u || pProfile->dsp_borderSizeY != 0u)
        CheckDlgButton(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_BLACKBORDERS_CHECK, BST_CHECKED);
    // curvature
    if (pProfile->dsp_screenCurved != 0u)
        CheckDlgButton(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_CURVATURE_CHECK, BST_CHECKED);
    // screen position
    if (pProfile->dsp_isScreenMirror)
        CheckDlgButton(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_MIRROR, BST_CHECKED);
    if (pProfile->dsp_ratioType != 0u)
        CheckDlgButton(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_PXRATIO_CHECK, BST_CHECKED);
    // trackbars
    HWND hControl = NULL;
    if (hControl = GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_STRETCH_SLIDER))
    {
        SendMessageW(hControl, TBM_SETRANGE, TRUE, MAKELONG(0, 8));
        SendMessageW(hControl, TBM_SETPAGESIZE, 0, 4);
        SendMessageW(hControl, TBM_SETTICFREQ, 1, 0);
        SendMessageW(hControl, TBM_SETPOS, TRUE, (int)pProfile->dsp_stretchRatio);
    }
    if (hControl = GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_CUT_SLIDER))
    {
        SendMessageW(hControl, TBM_SETRANGE, TRUE, MAKELONG(0, 8));
        SendMessageW(hControl, TBM_SETPAGESIZE, 0, 4);
        SendMessageW(hControl, TBM_SETTICFREQ, 1, 0);
        SendMessageW(hControl, TBM_SETPOS, TRUE, (int)pProfile->dsp_cropStrength);
    }
    // redraw preview
    InvalidateRect(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_PICTUREBOX), NULL, TRUE);
    RedrawWindow(GetDlgItem(res_tabPages[CONFIG_PROFILE_TAB_STRETCH], IDC_PROSTR_PICTUREBOX), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);

    if (pList != NULL)
        delete[] pList;
    pList = NULL;

    // set language dependant controls
    resetLanguage(true);
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
    // tab change
    if (msg == WM_NOTIFY)
    {
        switch (((LPNMHDR)lParam)->code)
        {
            case TCN_SELCHANGE: 
                return ConfigPageProfileView::s_pCurrentPage->onTabChange(hWindow, ((LPNMHDR)lParam)->hwndFrom); break;
        }
        return (INT_PTR)TRUE;
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
            switch (LOWORD(wParam))
            {
                case IDC_PRO_TXUPSCALE_FACTOR: // texture upscaling
                    if (HIWORD(wParam) == CBN_SELCHANGE)
                    {
                        int selection = SendMessage(GetDlgItem(hWindow, IDC_PRO_TXUPSCALE_FACTOR), CB_GETCURSEL, NULL, NULL);
                        if (selection != CB_ERR)
                        {
                            selection = (selection >= 5) ? 8 : selection+1;
                            std::wstring* pList = NULL;
                            int listLength = 0;
                            ConfigPageProfile::setUpscalingList(&pList, &listLength, selection, 0u);
                            WinToolbox::setComboboxValues(GetDlgItem(hWindow, IDC_PRO_TXUPSCALE_LIST), 0u, pList, listLength);
                            if (pList != NULL)
                                delete[] pList;
                        }
                    }
                    break;
                case IDC_PRO_2DUPSCALE_FACTOR: // 2D/sprite upscaling
                    if (HIWORD(wParam) == CBN_SELCHANGE)
                    {
                        int selection = SendMessage(GetDlgItem(hWindow, IDC_PRO_2DUPSCALE_FACTOR), CB_GETCURSEL, NULL, NULL);
                        if (selection != CB_ERR)
                        {
                            selection = (selection >= 5) ? 8 : selection+1;
                            std::wstring* pList = NULL;
                            int listLength = 0;
                            ConfigPageProfile::setUpscalingList(&pList, &listLength, selection, 0u);
                            WinToolbox::setComboboxValues(GetDlgItem(hWindow, IDC_PRO_2DUPSCALE_LIST), 0u, pList, listLength);
                            if (pList != NULL)
                                delete[] pList;
                        }
                    }
                    break;
                case IDC_PRO_SCRUPSCALE_FACTOR: // screen upscaling
                    if (HIWORD(wParam) == CBN_SELCHANGE)
                    {
                        int selection = SendMessage(GetDlgItem(hWindow, IDC_PRO_SCRUPSCALE_FACTOR), CB_GETCURSEL, NULL, NULL);
                        if (selection != CB_ERR)
                        {
                            std::wstring* pList = NULL;
                            int listLength = 0;
                            if (selection > 0)
                            {
                                selection = (selection >= 5) ? 8 : selection + 1;
                                ConfigPageProfile::setUpscalingList(&pList, &listLength, selection, 0u);
                            }
                            else
                            {
                                listLength = 1;
                                pList = new std::wstring[1];
                                pList[0] = L"normal";
                            }
                            WinToolbox::setComboboxValues(GetDlgItem(hWindow, IDC_PRO_SCRUPSCALE_LIST), 0u, pList, listLength);
                            if (pList != NULL)
                                delete[] pList;
                        }
                    }
                    break;
            }
        }
        // button / checkbox
        else
        {
            switch (LOWORD(wParam))
            {
                case IDC_PROCPT_BTN_FIXES: // custom game fixes
                {
                    HINSTANCE hInst = PandoraGS::getInstance();
                    if (hInst == (HINSTANCE)INVALID_HANDLE_VALUE)
                        hInst = GetModuleHandle(NULL);
                    if (DialogBox(hInst, MAKEINTRESOURCE(IDD_FIXES_DIALOG), hWindow, (DLGPROC)fixesEventHandler) <= 0)
                    {
                        MessageBox(hWindow, (LPCWSTR)L"Could not open sub-dialog.", (LPCWSTR)L"Dialog error...", MB_ICONWARNING | MB_OK);
                        return (INT_PTR)FALSE;
                    }
                    return (INT_PTR)TRUE; break;
                }
            }
        }
        return (INT_PTR)FALSE;
    }
    // drawing events
    return WinToolbox::pageDrawingEventHandler(hWindow, msg, wParam, lParam, COLOR_PAGE);
}

/// <summary>Screen stretching sub-tab event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="msg">Event message</param>
/// <param name="wParam">Command</param>
/// <param name="lParam">Informations</param>
/// <returns>Action code</returns>
INT_PTR CALLBACK ConfigPageProfileView::screenStretchingTabEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        // preview drawing
        case WM_DRAWITEM: 
        {
            if ((int)wParam == IDC_PROSTR_PICTUREBOX)
            {
                uint32_t stretchVal = SendMessage(GetDlgItem(hWindow, IDC_PROSTR_STRETCH_SLIDER), TBM_GETPOS, 0, 0);
                uint32_t cropVal = SendMessage(GetDlgItem(hWindow, IDC_PROSTR_CUT_SLIDER), TBM_GETPOS, 0, 0);
                bool isMirrored = (SendMessage(GetDlgItem(hWindow, IDC_PROSTR_MIRROR), BM_GETCHECK, 0, 0) == BST_CHECKED);
                if (WinToolbox::drawScreenStretchingPage(hWindow, COLOR_PAGE, lParam, stretchVal, cropVal, isMirrored) == (INT_PTR)TRUE)
                    return (INT_PTR)TRUE;
            }
            break;
        }
        // controls interaction
        case WM_COMMAND:
        {
            // combobox
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                switch (LOWORD(wParam))
                {
                    case IDC_PROSTR_PRESET_LIST: // screen stretching
                        if (HIWORD(wParam) == CBN_SELCHANGE)
                        {
                            bool isMirrored = (SendMessage(GetDlgItem(hWindow, IDC_PROSTR_STRETCH_SLIDER), TBM_GETPOS, 0, 0) == BST_CHECKED);

                            int selection = SendMessage(GetDlgItem(hWindow, IDC_PROSTR_PRESET_LIST), CB_GETCURSEL, NULL, NULL);
                            switch (selection)
                            {
                                case 1:
                                    SendMessageW(GetDlgItem(hWindow, IDC_PROSTR_STRETCH_SLIDER), TBM_SETPOS, TRUE, CFG_RATIO_STRETCH_FullWindow);
                                    SendMessageW(GetDlgItem(hWindow, IDC_PROSTR_CUT_SLIDER), TBM_SETPOS, TRUE, CFG_RATIO_CROP_FullWindow);
                                    break;
                                case 2:
                                    SendMessageW(GetDlgItem(hWindow, IDC_PROSTR_STRETCH_SLIDER), TBM_SETPOS, TRUE, CFG_RATIO_STRETCH_Orig);
                                    SendMessageW(GetDlgItem(hWindow, IDC_PROSTR_CUT_SLIDER), TBM_SETPOS, TRUE, CFG_RATIO_CROP_Orig);
                                    break;
                                case 3:
                                    SendMessageW(GetDlgItem(hWindow, IDC_PROSTR_STRETCH_SLIDER), TBM_SETPOS, TRUE, CFG_RATIO_STRETCH_OrigFill);
                                    SendMessageW(GetDlgItem(hWindow, IDC_PROSTR_CUT_SLIDER), TBM_SETPOS, TRUE, CFG_RATIO_CROP_OrigFill);
                                    break;
                                case 4:
                                    SendMessageW(GetDlgItem(hWindow, IDC_PROSTR_STRETCH_SLIDER), TBM_SETPOS, TRUE, CFG_RATIO_STRETCH_CloseToOrig);
                                    SendMessageW(GetDlgItem(hWindow, IDC_PROSTR_CUT_SLIDER), TBM_SETPOS, TRUE, CFG_RATIO_CROP_CloseToOrig);
                                    break;
                            }
                            if (selection > 0)
                            {
                                InvalidateRect(GetDlgItem(hWindow, IDC_PROSTR_PICTUREBOX), NULL, TRUE);
                                RedrawWindow(GetDlgItem(hWindow, IDC_PROSTR_PICTUREBOX), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
                            }
                            return (INT_PTR)TRUE;
                        }
                        break;
                }
            }
            // button / checkbox
            else
            {
                if (LOWORD(wParam) == IDC_PROSTR_MIRROR && HIWORD(wParam) == BN_CLICKED) // mirror screen
                {
                    InvalidateRect(GetDlgItem(hWindow, IDC_PROSTR_PICTUREBOX), NULL, TRUE);
                    RedrawWindow(GetDlgItem(hWindow, IDC_PROSTR_PICTUREBOX), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
                }
            }
            return (INT_PTR)FALSE; break;
        }
        // trackbar / scrollbar
        case WM_HSCROLL:
            if (LOWORD(wParam) == TB_ENDTRACK)
            {
                int controlId = GetDlgCtrlID((HWND)lParam);
                // screen stretching
                if (controlId == IDC_PROSTR_STRETCH_SLIDER || controlId == IDC_PROSTR_CUT_SLIDER)
                {
                    uint32_t presetIndex = 0u;
                    uint32_t stretchVal = SendMessage(GetDlgItem(hWindow, IDC_PROSTR_STRETCH_SLIDER), TBM_GETPOS, 0, 0);
                    uint32_t cropVal = SendMessage(GetDlgItem(hWindow, IDC_PROSTR_CUT_SLIDER), TBM_GETPOS, 0, 0);
                    if (stretchVal == CFG_RATIO_STRETCH_FullWindow && cropVal == CFG_RATIO_CROP_FullWindow)
                        presetIndex = 1u;
                    else if (stretchVal == CFG_RATIO_STRETCH_Orig && cropVal == CFG_RATIO_CROP_Orig)
                        presetIndex = 2u;
                    else if (stretchVal == CFG_RATIO_STRETCH_OrigFill && cropVal == CFG_RATIO_CROP_OrigFill)
                        presetIndex = 3u;
                    else if (stretchVal == CFG_RATIO_STRETCH_CloseToOrig && cropVal == CFG_RATIO_CROP_CloseToOrig)
                        presetIndex = 4u;
                    if (HWND hCombobox = GetDlgItem(hWindow, IDC_PROSTR_PRESET_LIST))
                        ComboBox_SetCurSel(hCombobox, presetIndex);

                    InvalidateRect(GetDlgItem(hWindow, IDC_PROSTR_PICTUREBOX), NULL, TRUE);
                    RedrawWindow(GetDlgItem(hWindow, IDC_PROSTR_PICTUREBOX), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
                    return (INT_PTR)TRUE; break;
                }
            }
    }
    // drawing events
    return WinToolbox::pageDrawingEventHandler(hWindow, msg, wParam, lParam, COLOR_PAGE);
}

// DIALOGS ---------------------------------------------------------------------

/// <summary>Custom fixes dialog event handler</summary>
/// <param name="hWindow">Dialog handle</param>
/// <param name="msg">Event message</param>
/// <param name="wParam">Command</param>
/// <param name="lParam">Informations</param>
/// <returns>Action code</returns>
INT_PTR CALLBACK ConfigPageProfileView::fixesEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        // dialog controls
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                // save and close button
                case IDOK:
                    //if (onFixesValidation(hWindow))
                        EndDialog(hWindow, TRUE);
                    return (INT_PTR)TRUE;
                // cancel and close button
                case IDCANCEL:
                    EndDialog(hWindow, TRUE);
                    return (INT_PTR)TRUE;
            }
            break;
        }
        case WM_CLOSE: // close button/shortcut
            EndDialog(hWindow, TRUE);
            return (INT_PTR)TRUE; break;
        default: return DefWindowProc(hWindow, msg, wParam, lParam); break;
    }
    return (INT_PTR)FALSE;
}

#endif
