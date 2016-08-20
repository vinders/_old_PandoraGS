/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   about_dialog_winview.cpp
Description : about dialog - view
*******************************************************************************/
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
using namespace std;
#include <string>
#include "pandoraGS.h"
#include "about_dialog_winview.h"
#include "resource.h"


/// <summary>Initialize dialog</summary>
AboutDialogView::AboutDialogView()
{
    // define handles
    m_hWindow = (HWND)INVALID_HANDLE_VALUE;
    m_hInstance = (HINSTANCE)INVALID_HANDLE_VALUE;
}

/// <summary>Create new dialog</summary>
/// <returns>Window created</returns>
/// <exception cref="std::exception">Creation failure</exception>
AboutDialogView* AboutDialogView::createWindow()
{
    // create dialog template
    AboutDialogView* pDialog = new AboutDialogView();
    pDialog->setDialogHandles();
    return pDialog;
}

/// <summary>Display window</summary>
/// <exception cref="std::exception">No window or event exception</exception>
void AboutDialogView::setVisible()
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
    bool isSuccess = (DialogBox(m_hInstance, MAKEINTRESOURCE(IDD_ABOUT_DIALOG), m_hWindow, (DLGPROC)eventHandler) >= 0);

    // disable visual style
    if (hActCtx != INVALID_HANDLE_VALUE)
    {
        DeactivateActCtx(0, cookie);
        ReleaseActCtx(hActCtx);
    }
    
    // error notification
    if (isSuccess == false)
        throw std::exception("Could not open dialog box");
}


// -- DIALOG TEMPLATE ----------------------------------------------------------

/// <summary>Retrieve window and instances handles</summary>
/// <exception cref="std::exception">Definition failure</exception>
void AboutDialogView::setDialogHandles()
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
INT_PTR CALLBACK AboutDialogView::eventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_INITDIALOG: // dialog init
        {
            std::wstring tech = _T(PLUGIN_TECH);
            #if _RENDERAPI == RENDERAPI_DIRECTX
            tech += std::wstring(_T(PLUGIN_API_DIRECTX));
            #else
            tech += std::wstring(_T(PLUGIN_API_OPENGL));
            #endif
            SetDlgItemTextW(hWindow, IDC_ABOUT_TECH, tech.c_str());
            return (INT_PTR)TRUE;
        }
        case WM_COMMAND: // dialog inner control
        {
            switch (LOWORD(wParam)) 
            {
                case IDCANCEL:  
                    EndDialog(hWindow, TRUE);
                    return (INT_PTR)TRUE;
            } 
            break;
        }
        case WM_CLOSE: // close button/shortcut
            EndDialog(hWindow, TRUE);
            return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

#endif
