/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_pmanager_winview.cpp
Description : configuration dialog page - profiles manager - view
*******************************************************************************/
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
using namespace std;
#include "config_pmanager_winview.h"

/// <summary>Create page view container</summary>
/// <param name="pController">Controller reference</param>
ConfigPageManagerView::ConfigPageManagerView(ConfigPage* pController) : ConfigPageView(pController)
{
    m_hPage = NULL;
    res_iconAdd = NULL;
    res_iconEdit = NULL;
    res_iconDel = NULL;
    res_iconIn = NULL;
    res_iconOut = NULL;
}
/// <summary>Destroy dialog view container</summary>
ConfigPageManagerView::~ConfigPageManagerView()
{
    if (m_hPage != NULL)
        DestroyWindow(m_hPage);
    m_hPage = NULL;
    if (res_iconAdd != NULL)
        DestroyIcon((HICON)res_iconAdd);
    res_iconAdd = NULL;
    if (res_iconEdit != NULL)
        DestroyIcon((HICON)res_iconEdit);
    res_iconEdit = NULL;
    if (res_iconDel != NULL)
        DestroyIcon((HICON)res_iconDel);
    res_iconDel = NULL;
    if (res_iconIn != NULL)
        DestroyIcon((HICON)res_iconIn);
    res_iconIn = NULL;
    if (res_iconOut != NULL)
        DestroyIcon((HICON)res_iconOut);
    res_iconOut = NULL;
}

/// <summary>Create new dialog page</summary>
/// <param name="pController">Controller reference</param>
/// <returns>Page created</returns>
/// <exception cref="std::exception">Creation failure</exception>
ConfigPageManagerView* ConfigPageManagerView::createPage(ConfigPageManager* pController)
{
    ConfigPageManagerView* pPage = new ConfigPageManagerView(pController);
    return pPage;
}


/// <summary>Refresh language-dependent page content</summary>
void ConfigPageManagerView::resetLanguage()
{

}
/// <summary>Copy UI settings to global configuration</summary>
void ConfigPageManagerView::updateConfig()
{

}

/// <summary>Load page content</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="phInstance">Instance handle reference</param>
/// <param name="pPageSize">Page limits (rectangle)</param>
/// <param name="isVisible">Visibility (show/hide)</param>
/// <exception cref="std::exception">Creation failure</exception>
void ConfigPageManagerView::loadPage(HWND hWindow, HINSTANCE* phInstance, RECT* pPageSize, bool isVisible)
{
    // load page
    m_hPage = CreateDialog(*phInstance, MAKEINTRESOURCE(IDD_MANAGER_PAGE), hWindow, (DLGPROC)eventHandler);
    if (m_hPage)
    {
        SetWindowPos(m_hPage, NULL, pPageSize->left, pPageSize->top,
            pPageSize->right - pPageSize->left, pPageSize->bottom - pPageSize->top, 0);
        // set colors
        if (isVisible)
        {
            ShowWindow(m_hPage, TRUE);
            EnableWindow(m_hPage, TRUE);
        }
        else
        {
            ShowWindow(m_hPage, FALSE);
            EnableWindow(m_hPage, FALSE);
        }
        // set button icons
        res_iconAdd = LoadImage(*phInstance, MAKEINTRESOURCE(IDI_CONFIG_BUTTON_ADD), IMAGE_ICON, 24, 24, NULL);
        res_iconEdit= LoadImage(*phInstance, MAKEINTRESOURCE(IDI_CONFIG_BUTTON_EDIT), IMAGE_ICON, 24, 24, NULL);
        res_iconDel = LoadImage(*phInstance, MAKEINTRESOURCE(IDI_CONFIG_BUTTON_DEL), IMAGE_ICON, 24, 24, NULL);
        res_iconIn =  LoadImage(*phInstance, MAKEINTRESOURCE(IDI_CONFIG_BUTTON_IN), IMAGE_ICON, 24, 24, NULL);
        res_iconOut = LoadImage(*phInstance, MAKEINTRESOURCE(IDI_CONFIG_BUTTON_OUT), IMAGE_ICON, 24, 24, NULL);
        if (res_iconAdd)
            SendDlgItemMessage(m_hPage, IDC_MNG_BTN_ADD, BM_SETIMAGE, IMAGE_ICON, (LPARAM)res_iconAdd);
        else
            SendDlgItemMessage(m_hPage, IDC_MNG_BTN_ADD, WM_SETTEXT, 0, (LPARAM)L"Add");
        if (res_iconEdit)
            SendDlgItemMessage(m_hPage, IDC_MNG_BTN_EDIT, BM_SETIMAGE, IMAGE_ICON, (LPARAM)res_iconEdit);
        else
            SendDlgItemMessage(m_hPage, IDC_MNG_BTN_EDIT, WM_SETTEXT, 0, (LPARAM)L"Edit");
        if (res_iconDel)
            SendDlgItemMessage(m_hPage, IDC_MNG_BTN_REMOVE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)res_iconDel);
        else
            SendDlgItemMessage(m_hPage, IDC_MNG_BTN_REMOVE, WM_SETTEXT, 0, (LPARAM)L"X");
        if (res_iconIn)
            SendDlgItemMessage(m_hPage, IDC_MNG_BTN_IMPORT, BM_SETIMAGE, IMAGE_ICON, (LPARAM)res_iconIn);
        else
            SendDlgItemMessage(m_hPage, IDC_MNG_BTN_IMPORT, WM_SETTEXT, 0, (LPARAM)L"In");
        if (res_iconOut)
            SendDlgItemMessage(m_hPage, IDC_MNG_BTN_EXPORT, BM_SETIMAGE, IMAGE_ICON, (LPARAM)res_iconOut);
        else
            SendDlgItemMessage(m_hPage, IDC_MNG_BTN_EXPORT, WM_SETTEXT, 0, (LPARAM)L"Out");
    }
    else
        throw std::exception();
}

/// <summary>Page event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="msg">Event message</param>
/// <param name="wParam">Command</param>
/// <param name="lParam">Informations</param>
/// <returns>Action code</returns>
INT_PTR CALLBACK ConfigPageManagerView::eventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HBRUSH hBrushColor = NULL;
    switch (msg)
    {
        case WM_PAINT: // page background color
        {
            PAINTSTRUCT paint;
            HDC hDC = BeginPaint(hWindow, &paint);
            RECT rect;
            if (!hBrushColor)
                hBrushColor = CreateSolidBrush(COLOR_PAGE);
            if (hBrushColor)
            {
                GetClientRect(hWindow, &rect);
                FillRect(hDC, &rect, hBrushColor);
            }
            EndPaint(hWindow, &paint);
            if (hDC)
                ReleaseDC(hWindow, hDC);
            return (INT_PTR)TRUE; break;
        }
        case WM_CTLCOLORSTATIC: // controls text color
        {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(0, 0, 0));
            SetBkColor(hdcStatic, COLOR_PAGE);
            if (!hBrushColor)
                hBrushColor = CreateSolidBrush(COLOR_PAGE);
            return (LRESULT)hBrushColor; break;
        }
        case WM_DESTROY:
        {
            if (hBrushColor)
                DeleteObject(hBrushColor);
            return (INT_PTR)TRUE; break;
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
            break;
        } // WM_COMMAND
    }
    return (INT_PTR)FALSE;
}

#endif
