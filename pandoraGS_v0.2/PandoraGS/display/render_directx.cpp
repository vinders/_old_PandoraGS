/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   render_directx.cpp
Description : API rendering pipeline - DirectX 11.0
*******************************************************************************/
#ifdef _WINDOWS
using namespace std;
#include "render_directx.h"


/// <summary>Render instance construction</summary>
/// <param name="pConfig">Thread configuration reference</param>
RenderDirectX::RenderDirectX(Config* pConfig) : Render(pConfig)
{
}

/// <summary>API shutdown and instance destruction</summary>
RenderDirectX::~RenderDirectX()
{
    closeApi();
    m_pConfig = NULL;
}


// -- RENDERING API MANAGEMENT -- ----------------------------------------------

/// <summary>Initialize DX API</summary>
/// <exception cref="std::exception">WARP API opening failure</exception>
void RenderDirectX::initApi()
{

}

/// <summary>Cleanup and shutdown DX API</summary>
void RenderDirectX::closeApi()
{

}

/// <summary>Reload rendering pipeline</summary>
void RenderDirectX::reloadApi()
{

}

/// <summary>Draw current frame to the screen</summary>
void RenderDirectX::drawFrame()
{

}

#endif
