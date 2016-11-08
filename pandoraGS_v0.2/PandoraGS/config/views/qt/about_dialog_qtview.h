/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   about_dialog_qtview.h
Description : about dialog - view
*******************************************************************************/
#ifndef _ABOUT_DIALOG_QTVIEW_H
#define _ABOUT_DIALOG_QTVIEW_H
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_QT
#include <string>

// About dialog box - view
class AboutDialogView
{
public:
    /// <summary>Initialize variables</summary>
    AboutDialogView();
    /// <summary>Destroy dialog data</summary>
    ~AboutDialogView();

    /// <summary>Create new dialog</summary>
    /// <returns>Window created</returns>
    /// <exception cref="std::exception">Creation failure</exception>
    static AboutDialogView* createWindow();

    /// <summary>Display window</summary>
    /// <exception cref="std::exception">No window or event exception</exception>
    void setVisible();
};

#endif
#endif
