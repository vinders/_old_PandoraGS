/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_page_general.cpp
Description : configuration dialog page - general - business logic
*******************************************************************************/
using namespace std;
#include "config_page_general.h"
#include "config_pgeneral_winview.h"
#include "config_pgeneral_qtview.h"

/// <summary>Destroy controller data</summary>
ConfigPageGeneral::~ConfigPageGeneral()
{
    if (m_pView != NULL)
        delete m_pView;
    m_pView = NULL;
}

/// <summary>Create config page view</summary>
/// <exception cref="std::exception">Creation failure</exception>
void ConfigPageGeneral::createView()
{
    m_pView = ConfigPageGeneralView::createPage(this);
}
