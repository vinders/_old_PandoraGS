/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   dialog_about.h
    Description : information dialog
*******************************************************************************/
#ifndef _DIALOG_ABOUT_H
#define _DIALOG_ABOUT_H

/* ABOUT DIALOG ------------------------------------------------------------- */
class DialogAbout
{
  public:
    DialogAbout() {};
    
    // dialog management
    void showDialog();
    void showWarning(const char*, const char*);
    #if _SYSTEM == _WINDOWS
        static BOOL CALLBACK eventHandler(HWND, UINT, WPARAM, LPARAM);
    #endif
};

#endif
