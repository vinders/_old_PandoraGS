/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_page_general.h
Description : configuration dialog page - general - business logic
*******************************************************************************/
#ifndef _CONFIG_PAGE_GENERAL_H
#define _CONFIG_PAGE_GENERAL_H
#include "globals.h"

#include "config_page.h"


// Configuration dialog page - general settings - business logic
class ConfigPageGeneral : public ConfigPage
{
public:
    /// <summary>Initialize controller variables</summary>
    ConfigPageGeneral(ConfigDialog* pParent) : ConfigPage(pParent) {}
    /// <summary>Destroy controller data</summary>
    ~ConfigPageGeneral();

    /// <summary>Create config page view</summary>
    /// <exception cref="std::exception">Creation failure</exception>
    virtual void createView();
};

#endif
