/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_page_winview.cpp
Description : configuration dialog page - view
*******************************************************************************/
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
using namespace std;
#include "config_page_winview.h"

/// <summary>Create page view container</summary>
/// <param name="pController">Controller reference</param>
ConfigPageView::ConfigPageView(ConfigPage* pController)
{
    m_pController = pController;
}

/// <summary>Destroy dialog view container</summary>
ConfigPageView::~ConfigPageView()
{
}

/// <summary>Show/hide page</summary>
/// <param name="isVisible">Visibility (show/hide)</param>
void ConfigPageView::showPage(bool isVisible)
{
    if (isVisible)
    {
        ShowWindow(m_hPage, SW_SHOW);
        EnableWindow(m_hPage, TRUE);
    }
    else
    {
        ShowWindow(m_hPage, SW_HIDE);
        EnableWindow(m_hPage, FALSE);
    }
}

#endif
