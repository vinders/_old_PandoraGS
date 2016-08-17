/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_page_qtview.h
Description : configuration dialog page - view
*******************************************************************************/
#ifndef _CONFIG_PAGE_QTVIEW_H
#define _CONFIG_PAGE_QTVIEW_H
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_QT

#ifndef _CONFIG_PAGE_H_END
class ConfigPage;
#endif

// Configuration dialog page - view
class ConfigPageView
{
protected:
    ConfigPage* m_pBinding; // viewmodel data binding


public:
    ConfigPageView(ConfigPage* pBinding);
    ~ConfigPageView();
};

#include "config_page.h"

#endif
#endif
