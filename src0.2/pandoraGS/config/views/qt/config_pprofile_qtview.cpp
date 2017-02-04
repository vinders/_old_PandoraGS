/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_pprofile_qtview.h
Description : configuration dialog page - profile settings - view
*******************************************************************************/
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_QT
using namespace std;
#include "config_pprofile_qtview.h"

/// <summary>Create page view container</summary>
/// <param name="pController">Controller reference</param>
ConfigPageProfileView::ConfigPageProfileView(ConfigPage* pController) : ConfigPageView(pController)
{
}
/// <summary>Destroy dialog view container</summary>
ConfigPageProfileView::~ConfigPageProfileView()
{
}

/// <summary>Create new dialog page</summary>
/// <param name="pController">Controller reference</param>
/// <returns>Page created</returns>
/// <exception cref="std::exception">Creation failure</exception>
ConfigPageProfileView* ConfigPageProfileView::createPage(ConfigPageProfile* pController)
{
    ConfigPageProfileView* pPage = new ConfigPageProfileView(pController);
    return pPage;
}

#endif
