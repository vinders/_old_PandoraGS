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

// About dialog box - view
class AboutDialogView
{
public:
    AboutDialogView();
    ~AboutDialogView();
};

#endif
#endif
