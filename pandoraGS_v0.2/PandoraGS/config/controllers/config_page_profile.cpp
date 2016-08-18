/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_page_profile.cpp
Description : configuration dialog page - profile settings - business logic
*******************************************************************************/
using namespace std;
#include "config_page_profile.h"
#include "config_pprofile_winview.h"
#include "config_pprofile_qtview.h"

/// <summary>Destroy controller data</summary>
ConfigPageProfile::~ConfigPageProfile()
{
    if (m_pView != NULL)
        delete m_pView;
    m_pView = NULL;
}

/// <summary>Create config page view</summary>
/// <exception cref="std::exception">Creation failure</exception>
void ConfigPageProfile::createView()
{
    m_pView = ConfigPageProfileView::createPage(this);
}
