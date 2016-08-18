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
    ConfigPage* m_pController; // controller reference

public:
    /// <summary>Create page view container</summary>
    /// <param name="pController">Controller reference</param>
    ConfigPageView(ConfigPage* pController);
    /// <summary>Destroy dialog view container</summary>
    ~ConfigPageView();

    /// <summary>Create new dialog page</summary>
    /// <param name="pController">Controller reference</param>
    /// <returns>Page created</returns>
    /// <exception cref="std::exception">Creation failure</exception>
    static ConfigPageView* createPage(ConfigPage* pController);
};

#include "config_page.h"

#endif
#endif
