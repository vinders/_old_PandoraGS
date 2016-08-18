/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_page_manager.h
Description : configuration dialog page - profile manager - business logic
*******************************************************************************/
#ifndef _CONFIG_PAGE_MANAGER_H
#define _CONFIG_PAGE_MANAGER_H
#include "globals.h"

#include "config_page.h"


// Configuration dialog page - profiles manager - business logic
class ConfigPageManager : public ConfigPage
{
public:
    /// <summary>Initialize controller variables</summary>
    ConfigPageManager(ConfigDialog* pParent) : ConfigPage(pParent) {}
    /// <summary>Destroy controller data</summary>
    ~ConfigPageManager();

    /// <summary>Create config page view</summary>
    /// <exception cref="std::exception">Creation failure</exception>
    virtual void createView();
};

#endif
