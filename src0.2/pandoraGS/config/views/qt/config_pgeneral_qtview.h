/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_pgeneral_qtview.h
Description : configuration dialog page - general - view
*******************************************************************************/
#ifndef _CONFIG_PGENERAL_QTVIEW_H
#define _CONFIG_PGENERAL_QTVIEW_H
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_QT

#include "config_page_qtview.h"


// Configuration dialog page - general settings - view
class ConfigPageGeneralView : public ConfigPageView
{
public:
    /// <summary>Create page view container</summary>
    /// <param name="pController">Controller reference</param>
    ConfigPageGeneralView(ConfigPage* pController);
    /// <summary>Destroy dialog view container</summary>
    ~ConfigPageGeneralView();

    /// <summary>Create new dialog page</summary>
    /// <param name="pController">Controller reference</param>
    /// <returns>Page created</returns>
    /// <exception cref="std::exception">Creation failure</exception>
    static ConfigPageGeneralView* createPage(ConfigPageGeneral* pController);
};

#endif
#endif
