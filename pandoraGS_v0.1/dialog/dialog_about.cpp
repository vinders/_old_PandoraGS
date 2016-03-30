/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   dialog_about.cpp
    Description : information dialog
*******************************************************************************/
using namespace std;
#include "../globals.h"
#include "../pandoraGS.h"
#include <stdio.h>
#include <string.h>
#if _SYSTEM == _WINDOWS
    #include <windowsx.h>
#endif
#include "dialog_about.h"


/* display dialog box ------------------------------------------------------- */
void DialogAbout::showDialog()
{
    #if _SYSTEM == _WINDOWS
        HWND hWindow = GetActiveWindow();
        #ifdef INSTANCE_SINGLETON
            if (hWindow == NULL || hWindow == INVALID_HANDLE_VALUE)
                return;
            HINSTANCE hInstance = PandoraGS::getInstance();
            if (hInstance == NULL || hInstance == INVALID_HANDLE_VALUE)
                hInstance = GetModuleHandle(NULL);
          #else
            HINSTANCE hInstance = GetModuleHandle(NULL);   // for executable tester
        #endif
        if (hInstance == NULL || hInstance == INVALID_HANDLE_VALUE)
            return;
        
        // visual style init (Activation Context API)
        HANDLE hActCtx;
        ACTCTX actCtx;
        ZeroMemory(&actCtx, sizeof(actCtx));
        actCtx.cbSize = sizeof(actCtx);
        actCtx.hModule = hInstance;
        actCtx.lpResourceName = MAKEINTRESOURCE(IDS_ACTIVATION_CONTEXT);
        actCtx.dwFlags = ACTCTX_FLAG_HMODULE_VALID | ACTCTX_FLAG_RESOURCE_NAME_VALID;

        // visual style activation
        hActCtx = CreateActCtx(&actCtx);
        ULONG_PTR cookie;
        if (hActCtx != INVALID_HANDLE_VALUE) 
            ActivateActCtx(hActCtx, &cookie);

        // open modal dialog box
        DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hWindow, (DLGPROC)eventHandler);
        
        // disable visual style
        if (hActCtx != INVALID_HANDLE_VALUE) 
        {
            DeactivateActCtx(0, cookie);
            ReleaseActCtx(hActCtx);
        }
      #else
        //...
    #endif
}

/* display warning message box ---------------------------------------------- */
void DialogAbout::showWarning(const char* origin, const char* message)
{
    int bufflength = 10 + strlen(origin) + strlen(message);
    char* buffer = new char[bufflength + 1];
    if (buffer == NULL)
        return;
    
    // create final message
    buffer[bufflength] = '\0';
    if (_snprintf(buffer, bufflength, "Error(%s) : %s", origin, message) < 0)
    {
        delete [] buffer;
        bufflength = 27;
        buffer = new char[28];
        if (buffer == NULL)
            return;
        
        // if failure, create generic error message
        buffer[27] = '\0';
        strcpy(buffer, "Error : dialog system error");
    }
    
    // display message box
    #if _SYSTEM == _WINDOWS
        wchar_t* wBuffer = new wchar_t[bufflength + 1];
        if (wBuffer)
        {
            MultiByteToWideChar(CP_ACP, 0, buffer, -1, wBuffer, bufflength + 1);
            MessageBox(NULL,wBuffer,(LPCWSTR)L"About dialog",MB_ICONWARNING|MB_OK);
            delete [] wBuffer;
        }
      #else
        //...
    #endif
    delete [] buffer;
}

#if _SYSTEM == _WINDOWS
    /* dialog events handler ------------------------------------------------ */
    BOOL CALLBACK DialogAbout::eventHandler(HWND hWindow, UINT uEvent, WPARAM wParam, LPARAM lParam)
    {
        switch(uEvent)
        {
            case WM_COMMAND:    // dialog inner control
            {
                switch(LOWORD(wParam)) 
                {
                    case IDOK:  EndDialog(hWindow, TRUE);
                                return TRUE;
                } 
                break;
            }
            case WM_CLOSE:      // close button/shortcut
            {
                EndDialog(hWindow, TRUE);
                return TRUE;
            }
        }
        return FALSE;
    }
#endif
