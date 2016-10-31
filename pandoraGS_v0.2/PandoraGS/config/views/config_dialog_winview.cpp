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
// dialog colors
#define COLOR_PAGE        RGB(255,255,255)
#define COLOR_BORDER      RGB(210,216,220)
#define COLOR_MENU_BORDER RGB(227,232,236)
#define COLOR_MENUTOP_R     235
#define COLOR_MENUTOP_G     240
#define COLOR_MENUTOP_B     245
#define COLOR_MENUBTM_R     156 //152
#define COLOR_MENUBTM_G     189 //192
#define COLOR_MENUBTM_B     220 //229

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
        case WM_INITDIALOG: return onInitDialog(hWindow, lParam);
        case WM_PAINT:      return onPaint(hWindow, lParam);
        case WM_DRAWITEM:   return onDrawItem(hWindow, lParam);

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
            return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

// -- EVENTS -------------------------------------------------------------------

/// <summary>Validation event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <returns>Success</returns>
bool ConfigDialogView::onValidation(HWND hWindow)
{
    //...mise à jour config selon valeurs d'UI
    //...

    if (ConfigDialogView::s_pCurrentWindow->getController()->saveConfig() == false)
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
    int languageCode = /**/0;//! lire dans UI
    if (ConfigDialogView::s_pCurrentWindow->getController()->setLanguage(languageCode))
    {
        //...màj UI général
        //...màj pages

        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

/// <summary>Drawing init event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="lParam">Parameter</param>
/// <returns>Changes indicator</returns>
INT_PTR ConfigDialogView::onInitDialog(HWND hWindow, LPARAM lParam)
{
    // load pages
    //...demander à chaque page de créer dialog

    // list profiles
    HWND hProfileList = GetDlgItem(hWindow, IDC_PROFILE_LIST);
    if (hProfileList)
    {
        //...charger liste de profils
        ShowWindow(hProfileList, SW_HIDE);
    }
    else
        MessageBox(hWindow, (LPCWSTR)L"Could not load profile selector. Please reload this window.",
                   (LPCWSTR)L"Initialization error...", MB_ICONWARNING | MB_OK);

    // language choice combobox
    HWND hLangList = GetDlgItem(hWindow, IDC_LANG_LIST);
    if (hLangList)
    {
        ComboBox_AddString(hLangList, _T("English"));
        ComboBox_AddString(hLangList, _T("Español"));
        ComboBox_AddString(hLangList, _T("Français"));
        ComboBox_AddString(hLangList, _T("Deutsch"));
        ComboBox_AddString(hLangList, _T("External file..."));
        switch (Config::gen_langCode)
        {
            case LangCode_English: ComboBox_SetCurSel(hLangList, 0); break;
            case LangCode_Spanish: ComboBox_SetCurSel(hLangList, 1); break;
            case LangCode_French:  ComboBox_SetCurSel(hLangList, 2); break;
            case LangCode_German:  ComboBox_SetCurSel(hLangList, 3); break;
            case LangCode_CustomFile: ComboBox_SetCurSel(hLangList, 4); break;
            default: ComboBox_SetCurSel(hLangList, 0); break;
        }
    }
    else
        MessageBox(hWindow, (LPCWSTR)L"Could not load language selector.",
                   (LPCWSTR)L"Initialization error...", MB_ICONWARNING | MB_OK);

    // menu - page tabs
    ConfigDialogView* pThis = ConfigDialogView::s_pCurrentWindow;
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
        MessageBox(hWindow, (LPCWSTR)L"Could not load menu bitmap.",
                   (LPCWSTR)L"Initialization error...", MB_ICONWARNING | MB_OK);
    // menu - font
    HDC hDC = GetDC(hWindow);
    if (hDC)
    {
        LOGFONT logFont = { 0 };
        logFont.lfHeight = -MulDiv(10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
        logFont.lfWeight = FW_NORMAL;
        //logFont.lfQuality = CLEARTYPE_QUALITY;
        _tcscpy_s(logFont.lfFaceName, _T("Tahoma"));
        pThis->res_tabFont = CreateFontIndirect(&logFont);
        ReleaseDC(hWindow, hDC);
    }
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
    HBRUSH brush = NULL;
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

    // main menu right border
    brush = CreateSolidBrush(COLOR_MENU_BORDER);
    if (brush)
    {
        GetClientRect(hWindow, &rect);
        rect.top += LOGO_HEIGHT;
        rect.bottom -= 43;
        rect.right = rect.left + LOGO_WIDTH;
        rect.left = rect.right - 1;
        FillRect(hDC, &rect, brush);
        DeleteObject(brush);
        brush = NULL;
    }

    // main menu background gradient
    RECT line;
    GetClientRect(hWindow, &rect);
    rect.top += LOGO_HEIGHT;
    rect.bottom -= 43;
    line.left = rect.left;
    line.right = rect.left + LOGO_WIDTH - 1;
    pThis->m_menuHeight = rect.bottom - rect.top;
    for (int px = 0; px < pThis->m_menuHeight; px++)
    {
        line.top = rect.top + px;
        line.bottom = line.top + 1;
        brush = CreateSolidBrush(RGB(COLOR_MENUTOP_R + (px*(COLOR_MENUBTM_R - COLOR_MENUTOP_R) / pThis->m_menuHeight),
                                     COLOR_MENUTOP_G + (px*(COLOR_MENUBTM_G - COLOR_MENUTOP_G) / pThis->m_menuHeight),
                                     COLOR_MENUTOP_B + (px*(COLOR_MENUBTM_B - COLOR_MENUTOP_B) / pThis->m_menuHeight)));
        FillRect(hDC, &line, brush);
        DeleteObject(brush);
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
    HBRUSH brushBorder = NULL;
    brushBorder = CreateSolidBrush(COLOR_MENU_BORDER);
    if (brushBorder == NULL)
        return FALSE;
    RECT itemRect = pDraw->rcItem;
    RECT titleRect = { itemRect.left, itemRect.top + TAB_TEXT_Y,
        itemRect.right - 4, itemRect.bottom };

    // init drawing (used once per button)
    SetBkMode(hDC, TRANSPARENT);
    if (pThis->m_initialize < CONFIG_DIALOG_PAGES_NB)
    {
        pThis->m_initialize++;
        if (0/*active-page*/ == number)  // active tab
        {
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
            //...redessiner texte
        }
        else  // inactive tabs
        {
            // window metrics (for gradient)
            int winHeight = pThis->m_menuHeight;
            if (winHeight == 1)
                winHeight = TAB_FIRST_Y - LOGO_HEIGHT + 3 * (TAB_HEIGHT + TAB_INTERVAL) + 70;
            int top = number*(TAB_HEIGHT + TAB_INTERVAL);

            // background gradient
            HBRUSH brushBack = NULL;
            RECT line;
            line.left = itemRect.left;
            line.right = itemRect.right - 1;
            for (int px = 0; px < TAB_HEIGHT; px++)
            {
                line.top = px;
                line.bottom = line.top + 1;
                brushBack = CreateSolidBrush(RGB(COLOR_MENUTOP_R + ((top + px)*(COLOR_MENUBTM_R - COLOR_MENUTOP_R) / winHeight),
                    COLOR_MENUTOP_G + ((top + px)*(COLOR_MENUBTM_G - COLOR_MENUTOP_G) / winHeight),
                    COLOR_MENUTOP_B + ((top + px)*(COLOR_MENUBTM_B - COLOR_MENUTOP_B) / winHeight)));
                FillRect(hDC, &line, brushBack);
                DeleteObject(brushBack);
            }

            // right border
            HBRUSH brushBorder = NULL;
            brushBorder = CreateSolidBrush(COLOR_MENU_BORDER);
            if (brushBorder)
            {
                itemRect.left = -1 + itemRect.right;
                FillRect(hDC, &itemRect, brushBorder);
                DeleteObject(brushBorder);
            }
            //...redessiner texte
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
            int prevTab = 0/*active-page*/;
            //...avertir controleur

            // change visual selection
            pThis->m_initialize = 0;
            if (prevTab == 0 || /*active-page*/0 == 0)
                SendMessage(pThis->res_tabGeneral, WM_SETFONT, (WPARAM)pThis->res_tabFont, (LPARAM)MAKELONG(TRUE, 0));
            if (prevTab == 1 || /*active-page*/0 == 1)
                SendMessage(pThis->res_tabManager, WM_SETFONT, (WPARAM)pThis->res_tabFont, (LPARAM)MAKELONG(TRUE, 0));
            if (prevTab == 2 || /*active-page*/0 == 2)
                SendMessage(pThis->res_tabProfile, WM_SETFONT, (WPARAM)pThis->res_tabFont, (LPARAM)MAKELONG(TRUE, 0));
        }
    }
    DeleteObject(brushBorder);
    ReleaseDC(hWindow, hDC);
    return (INT_PTR)TRUE;
}

/// <summary>Initialize memory</summary>
void ConfigDialogView::onBoot()
{
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
    m_initialize = 0;
    m_menuHeight = 1;
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
}

#endif
