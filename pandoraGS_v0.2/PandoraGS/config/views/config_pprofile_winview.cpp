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
#include "config_pprofile_winview.h"

/// <summary>Create page view container</summary>
/// <param name="pController">Controller reference</param>
ConfigPageProfileView::ConfigPageProfileView(ConfigPage* pController) : ConfigPageView(pController)
{
    m_hPage = NULL;
}
/// <summary>Destroy dialog view container</summary>
ConfigPageProfileView::~ConfigPageProfileView()
{
    if (m_hPage != NULL)
        DestroyWindow(m_hPage);
    m_hPage = NULL;
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
void ConfigPageProfileView::resetLanguage()
{

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
    m_hPage = CreateDialog(*phInstance, MAKEINTRESOURCE(IDD_PROFILE_PAGE), hWindow, (DLGPROC)eventHandler);
    if (m_hPage)
    {
        SetWindowPos(m_hPage, NULL, pPageSize->left, pPageSize->top,
            pPageSize->right - pPageSize->left, pPageSize->bottom - pPageSize->top, 0);
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
INT_PTR CALLBACK ConfigPageProfileView::eventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
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
