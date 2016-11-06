/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_dialog_winview.cpp
Description : configuration dialog - view
*******************************************************************************/
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
using namespace std;
#include <windowsx.h>
#include "config_dialog_winview.h"
#include "config.h"
#include "pandoraGS.h"
#include "lang.h"

// dialog sizes
#define LOGO_WIDTH   140
#define LOGO_HEIGHT  44
#define ICONS_SIZE   48
#define TAB_INTERVAL 6
#define TAB_FIRST_Y  46
#define TAB_HEIGHT   90
#define TAB_TEXT_Y   64

ConfigDialogView* ConfigDialogView::s_pCurrentWindow = NULL; // current instance (static access)


/// <summary>Create dialog view container</summary>
/// <param name="pController">Controller reference</param>
ConfigDialogView::ConfigDialogView(ConfigDialog* pController)
{
    // subpages
    m_pPages = NULL;
    m_pController = pController;
    // define handles
    m_hWindow = (HWND)INVALID_HANDLE_VALUE;
    m_hInstance = (HINSTANCE)INVALID_HANDLE_VALUE;
    onBoot(); // init items
}
/// <summary>Destroy dialog view container</summary>
ConfigDialogView::~ConfigDialogView()
{
    if (m_pPages != NULL)
        delete[] m_pPages; //sub-pages items are deleted by sub-controllers
    m_pPages = NULL;
    onClose(); // remove allocated items
}

/// <summary>Create new dialog</summary>
/// <param name="pController">Controller reference</param>
/// <returns>Window created</returns>
/// <exception cref="std::exception">Creation failure</exception>
ConfigDialogView* ConfigDialogView::createWindow(ConfigDialog* pController)
{
    // create main window
    ConfigDialogView* pWindow = new ConfigDialogView(pController);

    // get sub-pages from sub-controllers
    pWindow->m_pPages = new ConfigPageView*[CONFIG_DIALOG_PAGES_NB];
    for (int p = 0; p < CONFIG_DIALOG_PAGES_NB; ++p)
        pWindow->m_pPages[p] = pController->getConfigPage(p)->getView();

    pWindow->setDialogHandles();
    return pWindow;
}

/// <summary>Display window</summary>
/// <exception cref="std::exception">No window or event exception</exception>
void ConfigDialogView::setVisible()
{
    // visual style init (Activation Context API)
    HANDLE hActCtx;
    ACTCTX actCtx;
    ZeroMemory(&actCtx, sizeof(actCtx));
    actCtx.cbSize = sizeof(actCtx);
    actCtx.hModule = m_hInstance;
    actCtx.lpResourceName = MAKEINTRESOURCE(IDS_ACTIVATION_CONTEXT);
    actCtx.dwFlags = ACTCTX_FLAG_HMODULE_VALID | ACTCTX_FLAG_RESOURCE_NAME_VALID;

    // visual style activation
    hActCtx = CreateActCtx(&actCtx);
    ULONG_PTR cookie;
    if (hActCtx != INVALID_HANDLE_VALUE)
        ActivateActCtx(hActCtx, &cookie);

    // open modal dialog box
    InitCommonControls();
    ConfigDialogView* pPrevWindow = s_pCurrentWindow; // support for multiple instances
    s_pCurrentWindow = this;
    bool isSuccess = (DialogBox(m_hInstance, MAKEINTRESOURCE(IDD_CONFIG_DIALOG), m_hWindow, (DLGPROC)eventHandler) >= 0);
    s_pCurrentWindow = pPrevWindow;

    // disable visual style
    if (hActCtx != INVALID_HANDLE_VALUE)
    {
        DeactivateActCtx(0, cookie);
        ReleaseActCtx(hActCtx);
    }

    // error notification
    if (isSuccess == false)
        throw std::exception("Could not open config window");
}


// -- DIALOG TEMPLATE ----------------------------------------------------------

/// <summary>Retrieve window and instances handles</summary>
/// <exception cref="std::exception">Definition failure</exception>
void ConfigDialogView::setDialogHandles()
{
    // get main window and DLL instance handles
    m_hWindow = GetActiveWindow();
    m_hInstance = PandoraGS::getInstance();
    if (m_hWindow == (HWND)INVALID_HANDLE_VALUE)
        throw std::exception("Could not obtain main window handle");
    if (m_hInstance == (HINSTANCE)INVALID_HANDLE_VALUE)
        m_hInstance = GetModuleHandle(NULL);
    if (m_hInstance == (HINSTANCE)INVALID_HANDLE_VALUE)
        throw std::exception("Could not obtain library instance handle");
}

/// <summary>Window event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="msg">Event message</param>
/// <param name="wParam">Command</param>
/// <param name="lParam">Informations</param>
/// <returns>Action code</returns>
INT_PTR CALLBACK ConfigDialogView::eventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        // drawing
        case WM_INITDIALOG: return onInitDialog(hWindow, lParam); break;
        case WM_PAINT:      return onPaint(hWindow, lParam); break;
        case WM_DRAWITEM:   return onDrawItem(hWindow, lParam); break;

        // dialog controls
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                // save and close button
                case IDOK: 
                    if (onValidation(hWindow))
                        EndDialog(hWindow, TRUE);
                    return TRUE;

                // cancel and close button
                case IDCANCEL:
                    EndDialog(hWindow, TRUE);
                    return (INT_PTR)TRUE;

                // language combobox change
                case IDC_LANG_LIST: 
                    if (HIWORD(wParam) == CBN_SELCHANGE)
                        return onLanguageChange(hWindow);
                    break;
            }
            break;
        }
        case WM_CLOSE: // close button/shortcut
            EndDialog(hWindow, TRUE);
            return (INT_PTR)TRUE; break;
    }
    return (INT_PTR)FALSE;
}

/// <summary>Create and display pages</summary>
/// <param name="hSubWindow">Form handle</param>
/// <returns>Success</returns>
bool ConfigDialogView::loadPages(HWND hSubWindow)
{
    // get page size
    RECT pageSize;
    GetClientRect(hSubWindow, &pageSize);
    pageSize.left += LOGO_WIDTH;
    pageSize.bottom -= 43;
    // display pages
    try
    {
        for (int p = 1; p < CONFIG_DIALOG_PAGES_NB; ++p)
            m_pPages[p]->loadPage(hSubWindow, &m_hInstance, &pageSize, false);
        m_pPages[0]->loadPage(hSubWindow, &m_hInstance, &pageSize, true);
    }
    catch (std::exception exc)
    {
        return false;
    }
    return true;
}

/// <summary>Copy UI settings to global configuration</summary>
void ConfigDialogView::updateConfig()
{
    for (int p = 0; p < CONFIG_DIALOG_PAGES_NB; ++p)
        m_pPages[p]->updateConfig();
    int languageCode = SendMessage(GetDlgItem(m_hWindow, IDC_LANG_LIST), CB_GETCURSEL, NULL, NULL);
    if (languageCode != CB_ERR)
        Config::gen_langCode = languageCode;
}


// -- EVENTS -------------------------------------------------------------------

/// <summary>Validation event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <returns>Success</returns>
bool ConfigDialogView::onValidation(HWND hWindow)
{
    // update config
    ConfigDialog* pController = ConfigDialogView::s_pCurrentWindow->getController();
    ConfigDialogView::s_pCurrentWindow->updateConfig();
    // save changes
    if (pController->saveConfig() == false)
    {
        MessageBox(hWindow, (LPCWSTR)L"Could not save config in registry.\nPlease make sure the application has admin rights.", 
                   (LPCWSTR)L"Validation error...", MB_ICONWARNING | MB_OK);
        return false;
    }
    return true;
}

/// <summary>Language event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <returns>Changes indicator</returns>
INT_PTR ConfigDialogView::onLanguageChange(HWND hWindow)
{
    int languageCode = SendMessage(GetDlgItem(hWindow, IDC_LANG_LIST), CB_GETCURSEL, NULL, NULL);
    if (languageCode == CB_ERR)
    {
        MessageBox(hWindow, (LPCWSTR)L"An error occurred during selection. Please try again.",
                   (LPCWSTR)L"Language selection error...", MB_ICONWARNING | MB_OK);
        return (INT_PTR)FALSE;
    }
    ConfigDialogView* pThis = ConfigDialogView::s_pCurrentWindow;
    if (pThis->getController()->setLanguage(languageCode))
    {
        // main dialog controls translation
        SetDlgItemText(hWindow, IDOK, (LPCWSTR)pThis->getController()->getLangResource()->dialog_ok.c_str());
        SetDlgItemText(hWindow, IDCANCEL, (LPCWSTR)pThis->getController()->getLangResource()->dialog_cancel.c_str());
        SetDlgItemText(hWindow, IDS_PROFILE, (LPCWSTR)pThis->getController()->getLangResource()->dialog_profiles.c_str());
        // reset page tabs visuals
        pThis->m_initialize = 0;
        SendMessage(pThis->res_tabGeneral, WM_SETFONT, (WPARAM)pThis->res_tabFont, (LPARAM)MAKELONG(TRUE, 0));
        SendMessage(pThis->res_tabManager, WM_SETFONT, (WPARAM)pThis->res_tabFont, (LPARAM)MAKELONG(TRUE, 0));
        SendMessage(pThis->res_tabProfile, WM_SETFONT, (WPARAM)pThis->res_tabFont, (LPARAM)MAKELONG(TRUE, 0));

        // update pages
        for (int p = 0; p < CONFIG_DIALOG_PAGES_NB; ++p)
            pThis->m_pPages[p]->resetLanguage(false);
        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

/// <summary>Menu event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="tabId">Tab identifier</param>
void ConfigDialogView::onPageChange(HWND hWindow, int tabId)
{
    // page change
    m_pPages[m_activePage]->showPage(false);
    m_activePage = tabId; // set new page
    m_pPages[m_activePage]->showPage(true);

    // profile list
    HWND hProfileList = NULL;
    if (m_activePage == CONFIG_PAGE_PROFILE)
    {
        if (hProfileList = GetDlgItem(hWindow, IDC_PROFILE_LIST))
        {
            ShowWindow(hProfileList, SW_SHOW);
            EnableWindow(hProfileList, TRUE);
        }
        if (hProfileList = GetDlgItem(hWindow, IDS_PROFILE))
            ShowWindow(hProfileList, SW_SHOW);
    }
    else
    {
        if (hProfileList = GetDlgItem(hWindow, IDC_PROFILE_LIST))
        {
            ShowWindow(hProfileList, SW_HIDE);
            EnableWindow(hProfileList, FALSE);
        }
        if (hProfileList = GetDlgItem(hWindow, IDS_PROFILE))
            ShowWindow(hProfileList, SW_HIDE);
    }
}

/// <summary>Initialize memory</summary>
void ConfigDialogView::onBoot()
{
    m_activePage = CONFIG_PAGE_GENERAL;
    m_initialize = 0;
    res_tabGeneral = NULL;
    res_tabManager = NULL;
    res_tabProfile = NULL;
    res_tabIcons = NULL;
    res_tabFont = NULL;
    m_menuHeight = 1;
}
/// <summary>Free allocated memory</summary>
void ConfigDialogView::onClose()
{
    m_activePage = CONFIG_PAGE_GENERAL;
    m_initialize = 0;
    if (res_tabGeneral) DestroyWindow(res_tabGeneral);
    if (res_tabManager) DestroyWindow(res_tabManager);
    if (res_tabProfile) DestroyWindow(res_tabProfile);
    if (res_tabIcons) DeleteObject(res_tabIcons);
    if (res_tabFont) DeleteObject(res_tabFont);
    res_tabGeneral = NULL;
    res_tabManager = NULL;
    res_tabProfile = NULL;
    res_tabIcons = NULL;
    res_tabFont = NULL;
    m_menuHeight = 1;
}


// -- DISPLAY EVENTS -----------------------------------------------------------

/// <summary>Drawing init event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="lParam">Parameter</param>
/// <returns>Changes indicator</returns>
INT_PTR ConfigDialogView::onInitDialog(HWND hWindow, LPARAM lParam)
{
    ConfigDialogView* pThis = ConfigDialogView::s_pCurrentWindow;

    // list profiles
    HWND hProfileList = GetDlgItem(hWindow, IDC_PROFILE_LIST);
    if (hProfileList)
    {
        std::string* profileNames = pThis->getController()->getProfileNames();
        for (uint32_t pf = 0; pf < Config::countProfiles(); ++pf)
        {
            std::wstring profileConvert(profileNames[pf].begin(), profileNames[pf].end());
            ComboBox_AddString(hProfileList, (LPCTSTR)profileConvert.c_str());
        }
        ComboBox_SetCurSel(hProfileList, 0);
        ShowWindow(hProfileList, SW_HIDE);
    }
    else
        MessageBox(hWindow, (LPCWSTR)L"Could not load profile selector. Please reload this window.",
                   (LPCWSTR)L"Initialization error...", MB_ICONWARNING | MB_OK);
    HWND hProfileLabel = GetDlgItem(hWindow, IDS_PROFILE);
    if (hProfileLabel)
        ShowWindow(hProfileLabel, SW_HIDE);

    // load pages
    if (pThis->loadPages(hWindow) == false)
    {
        MessageBox(hWindow, (LPCWSTR)L"Failed to load page content.", (LPCWSTR)L"Initialization error...", MB_ICONWARNING | MB_OK);
        EndDialog(hWindow, (INT_PTR)FALSE);
        return (INT_PTR)FALSE;
    }

    // language choice combobox
    HWND hLangList = GetDlgItem(hWindow, IDC_LANG_LIST);
    if (hLangList)
    {
        ComboBox_AddString(hLangList, _T(" English"));
        ComboBox_AddString(hLangList, _T(" Español"));
        ComboBox_AddString(hLangList, _T(" Français"));
        ComboBox_AddString(hLangList, _T(" Deutsch"));
        ComboBox_AddString(hLangList, _T(" External file..."));
        switch (Config::gen_langCode)
        {
            case LangCode_English:
            case LangCode_Spanish:
            case LangCode_French:
            case LangCode_German:
            case LangCode_CustomFile: ComboBox_SetCurSel(hLangList, Config::gen_langCode); break;
            default: ComboBox_SetCurSel(hLangList, 0); break;
        }
    }
    else
        MessageBox(hWindow, (LPCWSTR)L"Could not load language selector.", (LPCWSTR)L"Initialization error...", MB_ICONWARNING | MB_OK);

    // main dialog buttons language
    SetDlgItemText(hWindow, IDOK, (LPCWSTR)pThis->getController()->getLangResource()->dialog_ok.c_str());
    SetDlgItemText(hWindow, IDCANCEL, (LPCWSTR)pThis->getController()->getLangResource()->dialog_cancel.c_str());
    SetDlgItemText(hWindow, IDS_PROFILE, (LPCWSTR)pThis->getController()->getLangResource()->dialog_profiles.c_str());

    // menu - page tabs
    pThis->res_tabGeneral = CreateWindow(L"button", L"General settings",
        WS_CHILD | WS_VISIBLE | BS_TEXT | BS_BITMAP | BS_BOTTOM | BS_OWNERDRAW, 4, TAB_FIRST_Y, LOGO_WIDTH - 4, TAB_HEIGHT, 
        hWindow, (HMENU)IDC_GENERAL_TAB, pThis->m_hInstance, NULL);
    pThis->res_tabManager = CreateWindow(L"button", L"Profiles manager",
        WS_CHILD | WS_VISIBLE | BS_TEXT | BS_BITMAP | BS_BOTTOM | BS_OWNERDRAW, 4, TAB_FIRST_Y + TAB_HEIGHT + TAB_INTERVAL, LOGO_WIDTH - 4, TAB_HEIGHT, 
        hWindow, (HMENU)IDC_MANAGER_TAB, pThis->m_hInstance, NULL);
    pThis->res_tabProfile = CreateWindow(L"button", L"Profile settings",
        WS_CHILD | WS_VISIBLE | BS_TEXT | BS_BITMAP | BS_BOTTOM | BS_OWNERDRAW, 4, TAB_FIRST_Y + 2 * (TAB_HEIGHT + TAB_INTERVAL), LOGO_WIDTH - 4, TAB_HEIGHT, 
        hWindow, (HMENU)IDC_PROFILE_TAB, pThis->m_hInstance, NULL);
    if (!pThis->res_tabGeneral || !pThis->res_tabManager || !pThis->res_tabProfile)
        MessageBox(hWindow, (LPCWSTR)L"Could not create main menu. Please reload this window.",
                   (LPCWSTR)L"Initialization error...", MB_ICONWARNING | MB_OK);

    // menu - tab icons
    pThis->res_tabIcons = (HBITMAP)LoadBitmap(ConfigDialogView::s_pCurrentWindow->m_hInstance, MAKEINTRESOURCE(IDB_CONFIG_ICONS));
    if (!pThis->res_tabIcons)
        MessageBox(hWindow, (LPCWSTR)L"Could not load menu bitmap.", (LPCWSTR)L"Initialization error...", MB_ICONWARNING | MB_OK);

    HDC hDC = GetDC(hWindow);
    if (hDC)
    {
        if (pThis->res_tabIcons)
        {
            // prepare icons - set information
            BITMAPINFO bmi;
            ZeroMemory(&bmi, sizeof(BITMAPINFO));
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = CONFIG_DIALOG_PAGES_NB*ICONS_SIZE;
            bmi.bmiHeader.biHeight = ICONS_SIZE;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4;
            // prepare icons - pre-multiply alpha channel
            BITMAP bm = { 0 };
            GetObject(pThis->res_tabIcons, sizeof(bm), &bm);
            GetDIBits(hDC, pThis->res_tabIcons, 0, bm.bmHeight, NULL, &bmi, DIB_RGB_COLORS); // pass effective dimensions to bmi
            LPBYTE pBitData = (LPBYTE)LocalAlloc(LPTR, bm.bmWidth * bm.bmHeight * sizeof(DWORD));
            if (pBitData != NULL)
            {
                LPBYTE pData = pBitData;
                GetDIBits(hDC, pThis->res_tabIcons, 0, bm.bmHeight, pData, &bmi, DIB_RGB_COLORS); // copy image to array
                for (int y = 0; y < bm.bmHeight; y++)
                {
                    for (int x = 0; x < bm.bmWidth; x++)
                    {
                        pData[0] = (BYTE)((DWORD)pData[0] * pData[3] / 255); // for AlphaBlend
                        pData[1] = (BYTE)((DWORD)pData[1] * pData[3] / 255);
                        pData[2] = (BYTE)((DWORD)pData[2] * pData[3] / 255);
                        pData += 4;
                    }
                }
                SetDIBits(hDC, pThis->res_tabIcons, 0, bm.bmHeight, pBitData, &bmi, DIB_RGB_COLORS); // copy array to image
                LocalFree(pBitData);
            }
        }

        // menu - font
        LOGFONT logFont = { 0 };
        logFont.lfHeight = -MulDiv(10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
        logFont.lfWeight = FW_NORMAL; //logFont.lfQuality = CLEARTYPE_QUALITY;
        _tcscpy_s(logFont.lfFaceName, _T("Tahoma"));
        pThis->res_tabFont = CreateFontIndirect(&logFont);
        ReleaseDC(hWindow, hDC);
        // set font on tabs
        SendMessage(pThis->res_tabGeneral, WM_SETFONT, (WPARAM)pThis->res_tabFont, (LPARAM)MAKELONG(TRUE, 0));
        SendMessage(pThis->res_tabManager, WM_SETFONT, (WPARAM)pThis->res_tabFont, (LPARAM)MAKELONG(TRUE, 0));
        SendMessage(pThis->res_tabProfile, WM_SETFONT, (WPARAM)pThis->res_tabFont, (LPARAM)MAKELONG(TRUE, 0));
    }
    else
        MessageBox(hWindow, (LPCWSTR)L"Could not obtain context.\nTabs fonts and icons transparency could not be set.", 
                   (LPCWSTR)L"Initialization error...", MB_ICONWARNING | MB_OK);
    return (INT_PTR)TRUE;
}

/// <summary>Paint event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="lParam">Parameter</param>
/// <returns>Changes indicator</returns>
INT_PTR ConfigDialogView::onPaint(HWND hWindow, LPARAM lParam)
{
    ConfigDialogView* pThis = ConfigDialogView::s_pCurrentWindow;
    PAINTSTRUCT paint;
    HDC hDC = NULL;
    HBRUSH brush = NULL, brush2 = NULL;
    hDC = BeginPaint(hWindow, &paint);
    if (hDC == NULL)
        return FALSE;

    // window bottom border
    RECT rect;
    brush = CreateSolidBrush(COLOR_BORDER);
    if (brush)
    {
        GetClientRect(hWindow, &rect);
        rect.bottom -= 41;
        rect.top = rect.bottom - 1;
        FillRect(hDC, &rect, brush);
        DeleteObject(brush);
        brush = NULL;
    }

    // main menu background gradient + border
    RECT line, lineBorder;
    float ratio;
    GetClientRect(hWindow, &rect);
    rect.top += LOGO_HEIGHT;
    rect.bottom -= 43;
    line.left = rect.left;
    line.right = lineBorder.left = rect.left + LOGO_WIDTH - 1;
    lineBorder.right = lineBorder.left + 1;
    pThis->m_menuHeight = rect.bottom - rect.top;
    for (int px = 0; px < pThis->m_menuHeight; px++)
    {
        line.top = lineBorder.top = rect.top + px;
        line.bottom = lineBorder.bottom = line.top + 1;
        ratio = (float)px / (float)pThis->m_menuHeight;
        brush = CreateSolidBrush(RGB(COLOR_MENUTOP_R + (ratio*COLOR_MENUOFFSET_R),
                                     COLOR_MENUTOP_G + (ratio*COLOR_MENUOFFSET_G),
                                     COLOR_MENUTOP_B + (ratio*COLOR_MENUOFFSET_B)));
        brush2 = CreateSolidBrush(RGB(COLOR_MENUTOP_BORDER_R + (ratio*COLOR_MENUOFFSET_BORDER_R),
                                      COLOR_MENUTOP_BORDER_G + (ratio*COLOR_MENUOFFSET_BORDER_G),
                                      COLOR_MENUTOP_BORDER_B + (ratio*COLOR_MENUOFFSET_BORDER_B)));
        FillRect(hDC, &lineBorder, brush2);
        FillRect(hDC, &line, brush);
        DeleteObject(brush);
        DeleteObject(brush2);
        brush = NULL;
    }

    // page bottom background color
    brush = CreateSolidBrush(COLOR_PAGE);
    if (brush)
    {
        GetClientRect(hWindow, &rect);
        rect.bottom -= 42;
        rect.top = rect.bottom - 1;
        FillRect(hDC, &rect, brush);
        DeleteObject(brush);
        brush = NULL;
    }

    pThis->m_initialize = 0;
    EndPaint(hWindow, &paint);
    if (hDC)
        ReleaseDC(hWindow, hDC);
    return (INT_PTR)TRUE;
}

/// <summary>Drawing item event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="lParam">Parameter</param>
/// <returns>Changes indicator</returns>
INT_PTR ConfigDialogView::onDrawItem(HWND hWindow, LPARAM lParam)
{
    // identify element to draw
    LPDRAWITEMSTRUCT pDraw = (LPDRAWITEMSTRUCT)lParam;
    int number = -1;
    switch (pDraw->CtlID)
    {
        case IDC_GENERAL_TAB: number = 0; break;
        case IDC_MANAGER_TAB: number = 1; break;
        case IDC_PROFILE_TAB: number = 2; break;
        default: return TRUE;
    }

    // prepare draw components
    ConfigDialogView* pThis = ConfigDialogView::s_pCurrentWindow;
    HDC  hDC = pDraw->hDC;
    if (hDC == NULL)
        return FALSE;
    HBRUSH brushBack = NULL;
    RECT itemRect = pDraw->rcItem;
    RECT titleRect = { itemRect.left, itemRect.top + TAB_TEXT_Y,
        itemRect.right - 4, itemRect.bottom };

    // init drawing (used once per button)
    SetBkMode(hDC, TRANSPARENT);
    if (pThis->m_initialize < CONFIG_DIALOG_PAGES_NB)
    {
        pThis->m_initialize++;
        if (pThis->m_activePage == number)  // active tab
        {
            HBRUSH brushBorder = NULL;
            switch (number)
            {
                case 0: brushBorder = CreateSolidBrush(COLOR_TAB_BORDER); break;
                case 1: brushBorder = CreateSolidBrush(COLOR_TAB_BORDER2); break;
                case 2: brushBorder = CreateSolidBrush(COLOR_TAB_BORDER3); break;
            }
            if (brushBorder == NULL)
                return FALSE;
            FrameRect(hDC, &itemRect, brushBorder);  // border
            brushBack = CreateSolidBrush(COLOR_PAGE);
            if (brushBack)
            {
                itemRect.left += 1;
                itemRect.top += 1;
                itemRect.bottom -= 1;
                FillRect(hDC, &itemRect, brushBack);
                DeleteObject(brushBack);
            }
            pThis->drawMenuContent(hWindow, &hDC, &titleRect, number);
            DeleteObject(brushBorder);
        }
        else  // inactive tabs
        {
            // window metrics (for gradient)
            int winHeight = pThis->m_menuHeight;
            if (winHeight == 1)
                winHeight = TAB_FIRST_Y - LOGO_HEIGHT + 3 * (TAB_HEIGHT + TAB_INTERVAL) + 70;
            int top = TAB_INTERVAL + number*(TAB_HEIGHT + TAB_INTERVAL);

            // background gradient
            HBRUSH brushBack = NULL;
            HBRUSH brushBorder = NULL;
            float ratio;
            RECT line, lineBorder;
            line.left = itemRect.left;
            line.right = lineBorder.left = itemRect.right - 1;
            lineBorder.right = lineBorder.left + 1;
            for (int px = 0; px < TAB_HEIGHT; px++)
            {
                line.top = lineBorder.top = px;
                line.bottom = lineBorder.bottom = line.top + 1;
                ratio = (float)(top + px) / (float)winHeight;
                brushBack = CreateSolidBrush(RGB((char)COLOR_MENUTOP_R + (char)(ratio*COLOR_MENUOFFSET_R),
                                                 (char)COLOR_MENUTOP_G + (char)(ratio*COLOR_MENUOFFSET_G),
                                                 (char)COLOR_MENUTOP_B + (char)(ratio*COLOR_MENUOFFSET_B)));
                brushBorder = CreateSolidBrush(RGB((char)COLOR_MENUTOP_BORDER_R + (char)(ratio*COLOR_MENUOFFSET_BORDER_R),
                                                   (char)COLOR_MENUTOP_BORDER_G + (char)(ratio*COLOR_MENUOFFSET_BORDER_G),
                                                   (char)COLOR_MENUTOP_BORDER_B + (char)(ratio*COLOR_MENUOFFSET_BORDER_B)));
                FillRect(hDC, &lineBorder, brushBorder);
                FillRect(hDC, &line, brushBack);
                DeleteObject(brushBack);
                DeleteObject(brushBorder);
            }
            pThis->drawMenuContent(hWindow, &hDC, &titleRect, number);
        }
    }
    // redraw selection
    else
    {
        // change selection
        BOOL bIsPressed = (pDraw->itemState & ODS_SELECTED);
        if (bIsPressed)
        {
            // change selection + toggle dialogs
            unsigned int prevTab = pThis->m_activePage;
            pThis->onPageChange(hWindow, number);

            // change visual selection
            pThis->m_initialize = 0;
            if (prevTab == CONFIG_PAGE_GENERAL || pThis->m_activePage == CONFIG_PAGE_GENERAL)
                SendMessage(pThis->res_tabGeneral, WM_SETFONT, (WPARAM)pThis->res_tabFont, (LPARAM)MAKELONG(TRUE, 0));
            if (prevTab == CONFIG_PAGE_MANAGER || pThis->m_activePage == CONFIG_PAGE_MANAGER)
                SendMessage(pThis->res_tabManager, WM_SETFONT, (WPARAM)pThis->res_tabFont, (LPARAM)MAKELONG(TRUE, 0));
            if (prevTab == CONFIG_PAGE_PROFILE || pThis->m_activePage == CONFIG_PAGE_PROFILE)
                SendMessage(pThis->res_tabProfile, WM_SETFONT, (WPARAM)pThis->res_tabFont, (LPARAM)MAKELONG(TRUE, 0));
        }
    }
    ReleaseDC(hWindow, hDC);
    return (INT_PTR)TRUE;
}

/// <summary>Drawing item event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="phDC">Context reference</param>
/// <param name="pTitleRect">Target rectangle reference</param>
/// <param name="tabId">Tab identifier</param>
void ConfigDialogView::drawMenuContent(HWND hWindow, HDC* phDC, RECT* pTitleRect, unsigned int tabId)
{
    // set text color
    if (tabId == m_activePage)
        SetTextColor(*phDC, COLOR_MENU_TEXT_ACTIVE);
    else
        SetTextColor(*phDC, COLOR_MENU_TEXT);

    // draw text
    switch (tabId)
    {
        case 0: DrawText(*phDC, (LPCWSTR)getController()->getLangResource()->tabTitle_generalSettings.c_str(), -1, pTitleRect, DT_CENTER); break;
        case 1: DrawText(*phDC, (LPCWSTR)getController()->getLangResource()->tabTitle_profilesManager.c_str(), -1, pTitleRect, DT_CENTER); break;
        case 2: DrawText(*phDC, (LPCWSTR)getController()->getLangResource()->tabTitle_profileSettings.c_str(), -1, pTitleRect, DT_CENTER); break;
    }

    if (res_tabIcons) // image
    {
        // prepare bitmap information
        BITMAPINFO bmi;
        ZeroMemory(&bmi, sizeof(BITMAPINFO));
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = CONFIG_DIALOG_PAGES_NB*ICONS_SIZE;
        bmi.bmiHeader.biHeight = ICONS_SIZE;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4;
        // copy bitmap to DIB
        VOID* pvBits = NULL;
        HDC hMemDC = CreateCompatibleDC(*phDC);
        HBITMAP hBmp = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
        SelectObject(hMemDC, hBmp);
        GetDIBits(*phDC, res_tabIcons, 0, ICONS_SIZE, pvBits, &bmi, DIB_RGB_COLORS);

        // blend alpha
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = 0xFF; // opaque
        bf.AlphaFormat = AC_SRC_ALPHA; // bitmap alpha
        AlphaBlend(*phDC, pTitleRect->left + ((LOGO_WIDTH / 2) - (4 + ICONS_SIZE / 2)), 
                          pTitleRect->top - (4 + ICONS_SIZE), ICONS_SIZE, ICONS_SIZE,
                   hMemDC, tabId*ICONS_SIZE, 0, ICONS_SIZE, ICONS_SIZE, bf);

        DeleteObject(hBmp);
        DeleteDC(hMemDC);
    }
}

/// <summary>Grab a part of a sprite-sheet</summary>
/*HBRUSH ConfigDialogView::getSpriteBrush(HDC* phDC, HBITMAP* phSheet, int coordX, int coordY, int sizeX, int sizeY)
{
    // define bitmap contexts
    HDC memDC = NULL, dstDC = NULL;
    HBITMAP oldMemBmp = NULL, oldDstBmp = NULL, dstBmp = NULL;
    if ((memDC = CreateCompatibleDC(NULL)) == NULL) 
        return NULL;
    if ((dstDC = CreateCompatibleDC(NULL)) == NULL)
    {
        DeleteDC(memDC); return NULL;
    }

    // create bitmap icon
    if ((dstBmp = CreateCompatibleBitmap(*phDC, sizeX, sizeY)) == NULL)
    {
        DeleteDC(memDC); DeleteDC(dstDC); return NULL;
    }
    // get part of sprite-sheet image
    oldMemBmp = (HBITMAP)SelectObject(memDC, *phSheet);
    oldDstBmp = (HBITMAP)SelectObject(dstDC, dstBmp);
    if (oldMemBmp && oldDstBmp)
        BitBlt(dstDC, 0, 0, sizeX, sizeY, memDC, coordX, coordY, SRCCOPY);

    // remove contexts
    if (oldMemBmp)
        SelectObject(memDC, oldMemBmp);
    if (oldDstBmp)
        SelectObject(dstDC, oldDstBmp);
    DeleteDC(memDC);
    DeleteDC(dstDC);

    // create image-pattern brush
    HBRUSH result = CreatePatternBrush(dstBmp);
    DeleteObject(dstBmp);
    return result;
}*/

#endif
