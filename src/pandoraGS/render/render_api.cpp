/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   render_api.cpp
Description : rendering api management - OpenGL 4.1
*******************************************************************************/
using namespace std;
#include "memory_dispatcher.h"
#include "system_tools.h"
#include "render_api.h"

bool RenderApi::m_isInitialized = false;

// -- RENDERING API MANAGEMENT -- ----------------------------------------------

/// <summary>Initialize DX API</summary>
/// <exception cref="std::exception">WARP API opening failure</exception>
void RenderApi::init()
{

}

/// <summary>Cleanup and shutdown DX API</summary>
void RenderApi::close()
{

}

/// <summary>Reload rendering pipeline</summary>
void RenderApi::reload()
{

}

/// <summary>Draw current frame to the screen</summary>
void RenderApi::drawFrame()
{

}

/// <summary>Change window size or set stretching mode</summary>
/// <param name="isWindowResized">Check new window size</param>
void RenderApi::setDrawingSize(bool isWindowResized)
{
    //...
}
