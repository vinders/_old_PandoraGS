/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_page_manager.cpp
Description : configuration dialog page - profile manager - business logic
*******************************************************************************/
using namespace std;
#include "config_page_manager.h"
#include "config_pmanager_winview.h"
#include "config_pmanager_qtview.h"

/// <summary>Destroy controller data</summary>
ConfigPageManager::~ConfigPageManager()
{
    if (m_pView != NULL)
        delete m_pView;
    m_pView = NULL;
}

/// <summary>Create config page view</summary>
/// <exception cref="std::exception">Creation failure</exception>
void ConfigPageManager::createView()
{
    m_pView = ConfigPageManagerView::createPage(this);
}
