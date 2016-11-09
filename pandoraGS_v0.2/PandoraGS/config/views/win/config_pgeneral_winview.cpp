/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_pgeneral_winview.cpp
Description : configuration dialog page - general - view
*******************************************************************************/
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
using namespace std;
#include "win_toolbox.hpp"
#include "config_pgeneral_winview.h"

ConfigPageGeneralView* ConfigPageGeneralView::s_pCurrentPage = NULL; // current page (static access)

/// <summary>Create page view container</summary>
/// <param name="pController">Controller reference</param>
ConfigPageGeneralView::ConfigPageGeneralView(ConfigPage* pController) : ConfigPageView(pController)
{
    m_hPage = NULL;
    for (int t = 0; t < PGENERAL_TOOLTIPS_NB; ++t)
        res_tooltips[t] = NULL;
}
/// <summary>Destroy dialog view container</summary>
ConfigPageGeneralView::~ConfigPageGeneralView()
{
    // delete tooltips
    for (int t = 0; t < PGENERAL_TOOLTIPS_NB; ++t)
    {
        if (res_tooltips[t] != NULL) DestroyWindow(res_tooltips[t]);
        res_tooltips[t] = NULL;
    }
    // delete items
    if (m_hPage != NULL)
        DestroyWindow(m_hPage);
    m_hPage = NULL;
}

/// <summary>Create new dialog page</summary>
/// <param name="pController">Controller reference</param>
/// <returns>Page created</returns>
/// <exception cref="std::exception">Creation failure</exception>
ConfigPageGeneralView* ConfigPageGeneralView::createPage(ConfigPageGeneral* pController)
{
    ConfigPageGeneralView* pPage = new ConfigPageGeneralView(pController);
    return pPage;
}


/// <summary>Refresh language-dependent page content</summary>
/// <param name="isFirstInit">First time (only labels) or update (all)</param>
void ConfigPageGeneralView::resetLanguage(bool isFirstInit)
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
void ConfigPageGeneralView::updateConfig()
{

}

/// <summary>Load page content</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="phInstance">Instance handle reference</param>
/// <param name="pPageSize">Page limits (rectangle)</param>
/// <param name="isVisible">Visibility (show/hide)</param>
/// <exception cref="std::exception">Creation failure</exception>
void ConfigPageGeneralView::loadPage(HWND hWindow, HINSTANCE* phInstance, RECT* pPageSize, bool isVisible)
{
    s_pCurrentPage = this;
    m_hPage = CreateDialog(*phInstance, MAKEINTRESOURCE(IDD_GENERAL_PAGE), hWindow, (DLGPROC)eventHandler);
    if (m_hPage)
    {
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
    }
    else
        throw std::exception();

    // set language-independant controls
    loadResolutions();
    std::wstring winRes = std::to_wstring(Config::dsp_windowResX);
    SetDlgItemText(m_hPage, IDC_GEN_WINRESX_EDIT, (LPCTSTR)winRes.c_str()); // window resolution
    winRes = std::to_wstring(Config::dsp_windowResY);
    SetDlgItemText(m_hPage, IDC_GEN_WINRESY_EDIT, (LPCTSTR)winRes.c_str());
    if (Config::dsp_isWindowResizable) // resizable
        CheckDlgButton(m_hPage, IDC_GEN_WINSIZE_CHECK, BST_CHECKED);
    std::wstring pColorDepthList[] = { L"16-bit", L"32-bit" }; // color depth
    WinToolbox::setComboboxValues(GetDlgItem(m_hPage, IDC_GEN_COLOR_LIST), (Config::dsp_isColorDepth32) ? 1 : 0, pColorDepthList, 2);
    if (Config::rnd_floatAccuracy) // GTE accuracy
        CheckDlgButton(m_hPage, IDC_GEN_GTEACC_CHECK, BST_CHECKED);
    if (Config::misc_isScreensaverDisabled) // screensaver
        CheckDlgButton(m_hPage, IDC_GEN_SCNSAVER_CHECK, BST_CHECKED);
    CheckDlgButton(m_hPage, (Config::dsp_isFullscreen) ? IDC_GEN_FULLRES : IDC_GEN_WINRES, BST_CHECKED); // display mode
    if (Config::sync_isFrameSkip) // frame skipping
        CheckDlgButton(m_hPage, IDC_GEN_FRAMESKIP_CHECK, BST_CHECKED);
    if (Config::sync_isFrameLimit) // FPS limit
        CheckDlgButton(m_hPage, IDC_GEN_FRAMELIMIT_CHECK, BST_CHECKED);
    CheckDlgButton(m_hPage, (Config::sync_framerateLimit <= 0.05f) ? IDC_GEN_FPSAUTO_RADIO : IDC_GEN_FPSFIXED_RADIO, BST_CHECKED); // frame limit mode
    winRes = (Config::sync_framerateLimit > 0.05f) ? std::to_wstring(Config::sync_framerateLimit) : L"59.94"; // FPS value
    SetDlgItemText(m_hPage, IDC_GEN_FRAMELIMIT_EDIT, (LPCTSTR)winRes.c_str());
    // set language
    resetLanguage(true);
}

/// <summary>Load resolutions list</summary>
void ConfigPageGeneralView::loadResolutions()
{
    // fullscreen resolutions
    uint32_t selectedIndex = 0u, currentIndex = 0u;
    HWND hResList = GetDlgItem(m_hPage, IDC_GEN_FULLRES_LIST);
    if (hResList != NULL)
    {
        // desktop resolution
        std::wstring desktopRes = std::wstring(L"desktop resolution");
        ComboBox_AddString(hResList, (LPCTSTR)desktopRes.c_str());
        if (Config::dsp_fullscnResX == RESOLUTION_AUTODETECT)
            selectedIndex = currentIndex;
        ++currentIndex;

        // get driver resolutions
        DEVMODE dv;
        memset(&dv, 0, sizeof(DEVMODE));
        dv.dmSize = sizeof(DEVMODE);
        wchar_t buffer[15];
        buffer[14] = '\0';
        for (uint32_t dr = 0u; EnumDisplaySettings(NULL, dr, &dv); ++dr)
        {
            _snwprintf_s(buffer, 14, L"%5d x %4d", dv.dmPelsWidth, dv.dmPelsHeight);
            if (ComboBox_FindString(hResList, -1, (LPCTSTR)buffer) == CB_ERR)
            {
                ComboBox_AddString(hResList, (LPCTSTR)buffer);
                if (Config::dsp_fullscnResX == dv.dmPelsWidth && Config::dsp_fullscnResY == dv.dmPelsHeight)
                    selectedIndex = currentIndex;
                ++currentIndex;
            }
        }

        // add other 4:3 / 5:4
        addCustomResolution(hResList, L"  640 x  480", 640u, 480u, &currentIndex, &selectedIndex); // VGA
        addCustomResolution(hResList, L"  800 x  600", 800u, 600u, &currentIndex, &selectedIndex); // SVGA
        addCustomResolution(hResList, L" 1024 x  768", 1024u, 768u, &currentIndex, &selectedIndex); // XGA
        addCustomResolution(hResList, L" 1280 x  960", 1280u, 960u, &currentIndex, &selectedIndex); // SXGA-
        addCustomResolution(hResList, L" 1280 x 1024", 1280u, 1024u, &currentIndex, &selectedIndex); // SXGA
        // add other 16:9 / 16:10
        addCustomResolution(hResList, L" 1280 x  720", 1280u, 720u, &currentIndex, &selectedIndex); // WXGA-H
        addCustomResolution(hResList, L" 1280 x  800", 1280u, 800u, &currentIndex, &selectedIndex); // WXGA
        addCustomResolution(hResList, L" 1366 x  768", 1366u, 768u, &currentIndex, &selectedIndex); // HD
        addCustomResolution(hResList, L" 1440 x  900", 1440u, 900u, &currentIndex, &selectedIndex); // WSXGA
        addCustomResolution(hResList, L" 1600 x  900", 1600u, 900u, &currentIndex, &selectedIndex); // HD+
        addCustomResolution(hResList, L" 1920 x 1080", 1920u, 1080u, &currentIndex, &selectedIndex); // fullHD
        addCustomResolution(hResList, L" 1920 x 1200", 1920u, 1200u, &currentIndex, &selectedIndex); // WUXGA
        addCustomResolution(hResList, L" 2560 x 1600", 2560u, 1600u, &currentIndex, &selectedIndex); // WQXGA
        addCustomResolution(hResList, L" 4096 x 2304", 4096u, 2304u, &currentIndex, &selectedIndex); // 4K

        // select config resolution
        ComboBox_SetCurSel(hResList, selectedIndex);
    }
}

/// <summary>Page event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="msg">Event message</param>
/// <param name="wParam">Command</param>
/// <param name="lParam">Informations</param>
/// <returns>Action code</returns>
INT_PTR CALLBACK ConfigPageGeneralView::eventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // controls interaction
    if (msg == WM_COMMAND)
    {
        // buttons
        if (HIWORD(wParam) != CBN_SELCHANGE)
        {
            switch (LOWORD(wParam))
            {
                //open popup dialogs
            case IDC_GEN_BTN_KEYBINDING: break;// return onKeyBinding(hWindow); break;
            case IDC_GEN_BTN_ADVANCED:   break;// return onAdvancedSettings(hWindow); break;
            }
        }
        return (INT_PTR)FALSE;
    }
    // drawing events
    return WinToolbox::pageDrawingEventHandler(hWindow, msg, wParam, lParam, COLOR_PAGE);
}

#endif
