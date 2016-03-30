/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_page_viewmodel.h
Description : configuration dialog page - business logic
*******************************************************************************/
#ifndef _CONFIG_PAGE_VIEWMODEL_H
#define _CONFIG_PAGE_VIEWMODEL_H
#include "globals.h"

class ConfigDialogVM;
#include "config_io.h"
#include "config_page_qtview.h"
#include "config_page_winview.h"


// Configuration dialog page (generic) - business logic
class ConfigPageVM
{
protected:
    ConfigDialogVM* m_pParent; // parent viewmodel reference
    ConfigPageView* m_pView; // visuals container

public:
    ConfigPageVM(ConfigDialogVM* pParent, ConfigPageView* pView);
    ~ConfigPageVM();
};


#include "config_dialog_viewmodel.h"

#endif
