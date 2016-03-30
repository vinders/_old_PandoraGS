/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   render_software.cpp
Description : API rendering pipeline - software (DX Warp 11)
*******************************************************************************/
#ifdef _WINDOWS
using namespace std;
#include "render_software.h"


/// <summary>Render instance construction</summary>
/// <param name="pConfig">Thread configuration reference</param>
RenderSoftware::RenderSoftware(Config* pConfig) : Render(pConfig)
{
}

/// <summary>API shutdown and instance destruction</summary>
RenderSoftware::~RenderSoftware()
{
    closeApi();
    m_pConfig = NULL;
}


// -- RENDERING API MANAGEMENT -- ----------------------------------------------

/// <summary>Initialize WARP API</summary>
/// <exception cref="std::exception">WARP API opening failure</exception>
void RenderSoftware::initApi()
{

}

/// <summary>Cleanup and shutdown WARP API</summary>
void RenderSoftware::closeApi()
{

}

//...

#endif
