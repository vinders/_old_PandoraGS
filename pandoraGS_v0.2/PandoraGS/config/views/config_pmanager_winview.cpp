/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_pmanager_winview.cpp
Description : configuration dialog page - profiles manager - view
*******************************************************************************/
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
using namespace std;
#include "config_pmanager_winview.h"

/// <summary>Create page view container</summary>
/// <param name="pController">Controller reference</param>
ConfigPageManagerView::ConfigPageManagerView(ConfigPage* pController) : ConfigPageView(pController)
{
}
/// <summary>Destroy dialog view container</summary>
ConfigPageManagerView::~ConfigPageManagerView()
{
}

/// <summary>Create new dialog page</summary>
/// <param name="pController">Controller reference</param>
/// <returns>Page created</returns>
/// <exception cref="std::exception">Creation failure</exception>
ConfigPageManagerView* ConfigPageManagerView::createPage(ConfigPageManager* pController)
{
    ConfigPageManagerView* pPage = new ConfigPageManagerView(pController);
    return pPage;
}

#endif
