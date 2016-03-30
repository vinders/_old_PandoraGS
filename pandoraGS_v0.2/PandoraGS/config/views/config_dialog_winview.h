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

class ConfigDialogVM;
#include "config_page_winview.h"


// Configuration dialog box - view
class ConfigDialogView
{
private:
    ConfigPageView* m_pPages; // page views
    ConfigDialogVM* m_pBinding; // viewmodel data binding


public:
    ConfigDialogView(ConfigDialogVM* pBinding);
    ~ConfigDialogView();
};


#include "config_dialog_viewmodel.h"

#endif
#endif
