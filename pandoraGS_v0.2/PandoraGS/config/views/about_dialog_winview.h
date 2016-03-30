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

class AboutDialogVM;


// About dialog box - view
class AboutDialogView
{
public:
    AboutDialogView();
    ~AboutDialogView();
};

#include "about_dialog_viewmodel.h"

#endif
#endif
