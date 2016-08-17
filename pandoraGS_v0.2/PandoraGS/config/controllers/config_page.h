/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_page.h
Description : configuration dialog page - business logic
*******************************************************************************/
#ifndef _CONFIG_PAGE_H
#define _CONFIG_PAGE_H
#include "globals.h"

#ifndef _CONFIG_DIALOG_H_END
class ConfigDialog;
#endif
#include "config_io.h"
#include "config_page_qtview.h"
#include "config_page_winview.h"


// Configuration dialog page (generic) - business logic
class ConfigPage
{
protected:
    ConfigDialog* m_pParent; // parent viewmodel reference
    ConfigPageView* m_pView; // visuals container

public:
    ConfigPage(ConfigDialog* pParent, ConfigPageView* pView);
    ~ConfigPage();
};

#define _CONFIG_PAGE_H_END
#include "config_dialog.h"

#endif
