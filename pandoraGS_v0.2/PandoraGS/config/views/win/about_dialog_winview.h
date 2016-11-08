/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   about_dialog_winview.h
Description : about dialog - view
*******************************************************************************/
#ifndef _ABOUT_DIALOG_WINVIEW_H
#define _ABOUT_DIALOG_WINVIEW_H
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <string>

// About dialog box - view
class AboutDialogView
{
private:
    HWND m_hWindow; // main window handle
    HINSTANCE m_hInstance; // DLL instance handle
    
protected:
    /// <summary>Initialize dialog</summary>
    AboutDialogView();

public:   
    /// <summary>Create new dialog</summary>
    /// <returns>Window created</returns>
    /// <exception cref="std::exception">Creation failure</exception>
    static AboutDialogView* createWindow();

    /// <summary>Display window</summary>
    /// <exception cref="std::exception">No window or event exception</exception>
    void setVisible();
    
private:
    // -- DIALOG TEMPLATE ------------------------------------------------------
    
    /// <summary>Retrieve window and instances handles</summary>
    /// <exception cref="std::exception">Definition failure</exception>
    void setDialogHandles();

    /// <summary>Window event handler</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="msg">Event message</param>
    /// <param name="wParam">Command</param>
    /// <param name="lParam">Informations</param>
    /// <returns>Action code</returns>
    static INT_PTR CALLBACK eventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif
#endif
