/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_pgeneral_winview.cpp
Description : configuration dialog page - general - view
*******************************************************************************/
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
using namespace std;
#include "config_pgeneral_winview.h"

/// <summary>Create page view container</summary>
/// <param name="pController">Controller reference</param>
ConfigPageGeneralView::ConfigPageGeneralView(ConfigPage* pController) : ConfigPageView(pController)
{
}
/// <summary>Destroy dialog view container</summary>
ConfigPageGeneralView::~ConfigPageGeneralView()
{
}

/// <summary>Create new dialog page</summary>
/// <param name="pController">Controller reference</param>
/// <returns>Page created</returns>
/// <exception cref="std::exception">Creation failure</exception>
ConfigPageGeneralView* ConfigPageGeneralView::createPage(ConfigPageGeneral* pController)
{
    ConfigPageGeneralView* pPage = new ConfigPageGeneralView(pController);
    return pPage;
}


/// <summary>Refresh language-dependent page content</summary>
void ConfigPageGeneralView::resetLanguage()
{

}
/// <summary>Copy UI settings to global configuration</summary>
void ConfigPageGeneralView::updateConfig()
{

}

/// <summary>Load page content</summary>
/// <param name="isVisible">Visibility (show/hide)</param>
void ConfigPageGeneralView::loadPage(bool isVisible)
{

}

#endif
