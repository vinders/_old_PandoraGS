/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_dialog_winview.h
Description : configuration dialog - view
*******************************************************************************/
#ifndef _CONFIG_DIALOG_WINVIEW_H
#define _CONFIG_DIALOG_WINVIEW_H
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32

#ifndef _CONFIG_DIALOG_H_END
class ConfigDialog;
#endif
#include "config_page_winview.h"


// Configuration dialog box - view
class ConfigDialogView
{
private:
    ConfigPageView* m_pPages; // page views
    ConfigDialog* m_pBinding; // viewmodel data binding


public:
    ConfigDialogView(ConfigDialog* pBinding);
    ~ConfigDialogView();
};


#include "config_dialog.h"

#endif
#endif
