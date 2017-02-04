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

/// <summary>Set upscaling type list, based on factor</summary>
/// <param name="ppTypeList">Upscaling type list to allocate</param>
/// <param name="length">Allocated list length</param>
/// <param name="factor">Multiplication factor</param>
/// <param name="value">Config value</param>
/// <returns>Selected index</returns>
uint32_t ConfigPageProfile::setUpscalingList(std::wstring** ppTypeList, int* length, uint32_t factor, uint32_t value)
{
    if (*ppTypeList != NULL) // free previous entry
        delete [] *ppTypeList;
    *ppTypeList = NULL;

    switch (factor) // set upscaling types + selection
    {
        case 2:
        {
            *length = 6;
            *ppTypeList = new std::wstring[*length];
            (*ppTypeList)[0] = L"SaI";
            (*ppTypeList)[1] = L"xBR";
            (*ppTypeList)[2] = L"xBRZ";
            (*ppTypeList)[3] = L"NNEDI3";
            (*ppTypeList)[4] = L"Super-xBR";
            (*ppTypeList)[5] = L"Super-xBR bilateral";
            switch (value)
            {
                case CFG_upsc_xbr:         return 1u; break;
                case CFG_upsc_xbrz:        return 2u; break;
                case CFG_upsc_nnedi3:      return 3u; break;
                case CFG_upsc_superxbr:    return 4u; break;
                case CFG_upsc_superxbr_fb: return 5u; break;
                default: return 0u; break;
            }
            break;
        }
        case 3:
        {
            *length = 3;
            *ppTypeList = new std::wstring[*length];
            (*ppTypeList)[0] = L"xBR";
            (*ppTypeList)[1] = L"xBRZ";
            (*ppTypeList)[2] = L"xBRZ smoothed";
            switch (value)
            {
                case CFG_upsc_xbrz:    return 1u; break;
                case CFG_upsc_xbrz_ex: return 2u; break;
                default: return 0u; break;
            }
            break;
        }
        case 4:
        {
            *length = 6;
            *ppTypeList = new std::wstring[*length];
            (*ppTypeList)[0] = L"xBR";
            (*ppTypeList)[1] = L"xBRZ";
            (*ppTypeList)[2] = L"xBRZ smoothed";
            (*ppTypeList)[3] = L"NNEDI3";
            (*ppTypeList)[4] = L"Super-xBR";
            (*ppTypeList)[5] = L"Super-xBR bilateral";
            switch (value)
            {
                case CFG_upsc_xbrz:        return 1u; break;
                case CFG_upsc_xbrz_ex:     return 2u; break;
                case CFG_upsc_nnedi3:      return 3u; break;
                case CFG_upsc_superxbr:    return 4u; break;
                case CFG_upsc_superxbr_fb: return 5u; break;
                default: return 0u; break;
            }
            break;
        }
        case 5:
        {
            *length = 2;
            *ppTypeList = new std::wstring[*length];
            (*ppTypeList)[0] = L"xBRZ";
            (*ppTypeList)[1] = L"xBRZ smoothed";
            switch (value)
            {
                case CFG_upsc_xbrz_ex: return 1u; break;
                default: return 0u; break;
            }
            break;
        }
        case 8:
        {
            *length = 3;
            *ppTypeList = new std::wstring[*length];
            (*ppTypeList)[0] = L"NNEDI3";
            (*ppTypeList)[1] = L"Super-xBR";
            (*ppTypeList)[2] = L"Super-xBR bilateral";
            switch (value)
            {
                case CFG_upsc_superxbr:    return 1u; break;
                case CFG_upsc_superxbr_fb: return 2u; break;
                default: return 0u; break;
            }
            break;
        }
        default:
        {
            *length = 1;
            *ppTypeList = new std::wstring[*length];
            (*ppTypeList)[0] = L"native";
            return 0u; break;
        }
    }
    return 0u;
}
