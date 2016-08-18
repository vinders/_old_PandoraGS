/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_page.cpp
Description : configuration dialog page - business logic
*******************************************************************************/
using namespace std;
#include "config_page.h"

/// <summary>Initialize controller variables</summary>
/// <param name="pParent">Parent controller reference</param>
ConfigPage::ConfigPage(ConfigDialog* pParent)
{
    m_pView = NULL;
    m_pParent = pParent;
}
