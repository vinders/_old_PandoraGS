/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_page_profile.h
Description : configuration dialog page - profile settings - business logic
*******************************************************************************/
#ifndef _CONFIG_PAGE_PROFILE_H
#define _CONFIG_PAGE_PROFILE_H
#include "globals.h"

#include <stdint.h>
#include "config_page.h"


// Configuration dialog page - profile settings - business logic
class ConfigPageProfile : public ConfigPage
{
public:
    /// <summary>Initialize controller variables</summary>
    ConfigPageProfile(ConfigDialog* pParent) : ConfigPage(pParent) {}
    /// <summary>Destroy controller data</summary>
    ~ConfigPageProfile();

    /// <summary>Create config page view</summary>
    /// <exception cref="std::exception">Creation failure</exception>
    virtual void createView();

    /// <summary>Set upscaling type list, based on factor</summary>
    /// <param name="ppTypeList">Upscaling type list to allocate</param>
    /// <param name="length">Allocated list length</param>
    /// <param name="factor">Multiplication factor</param>
    /// <param name="value">Config value</param>
    /// <returns>Selected index</returns>
    static uint32_t setUpscalingList(std::wstring** ppTypeList, int* length, uint32_t factor, uint32_t value);
};

#endif
