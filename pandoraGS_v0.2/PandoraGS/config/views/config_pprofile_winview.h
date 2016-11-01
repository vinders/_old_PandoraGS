/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_pprofile_winview.h
Description : configuration dialog page - profile settings - view
*******************************************************************************/
#ifndef _CONFIG_PPROFILE_WINVIEW_H
#define _CONFIG_PPROFILE_WINVIEW_H
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32

#include "config_page_winview.h"


// Configuration dialog page - profile settings - view
class ConfigPageProfileView : public ConfigPageView
{
public:
    /// <summary>Create page view container</summary>
    /// <param name="pController">Controller reference</param>
    ConfigPageProfileView(ConfigPage* pController);
    /// <summary>Destroy dialog view container</summary>
    ~ConfigPageProfileView();

    /// <summary>Create new dialog page</summary>
    /// <param name="pController">Controller reference</param>
    /// <returns>Page created</returns>
    /// <exception cref="std::exception">Creation failure</exception>
    static ConfigPageProfileView* createPage(ConfigPageProfile* pController);

    /// <summary>Refresh language-dependent page content</summary>
    virtual void resetLanguage();
    /// <summary>Copy UI settings to global configuration</summary>
    virtual void updateConfig();

    /// <summary>Load page content</summary>
    /// <param name="isVisible">Visibility (show/hide)</param>
    virtual void loadPage(bool isVisible);
};

#endif
#endif
