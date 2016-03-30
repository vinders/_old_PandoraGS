/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   about_dialog_viewmodel.h
Description : about dialog - business logic
*******************************************************************************/
#ifndef _ABOUT_DIALOG_VIEWMODEL_H
#define _ABOUT_DIALOG_VIEWMODEL_H
#include "globals.h"

#include "about_dialog_winview.h"
#include "about_dialog_qtview.h"


// About dialog box - business logic
class AboutDialogVM
{
public:
    AboutDialogView* m_pView; // visuals container


public:
    AboutDialogVM();
    ~AboutDialogVM();

    void showDialog();
};

#endif
