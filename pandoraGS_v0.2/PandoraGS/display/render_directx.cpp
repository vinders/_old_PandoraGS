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
#if _RENDERAPI == RENDERAPI_DIRECTX

// -- RENDERING API MANAGEMENT -- ----------------------------------------------

/// <summary>Initialize DX API</summary>
/// <exception cref="std::exception">WARP API opening failure</exception>
void Render::initApi()
{

}

/// <summary>Cleanup and shutdown DX API</summary>
void Render::closeApi()
{

}

/// <summary>Reload rendering pipeline</summary>
void Render::reloadApi()
{

}

/// <summary>Draw current frame to the screen</summary>
void Render::drawFrame()
{

}

#endif
#endif
