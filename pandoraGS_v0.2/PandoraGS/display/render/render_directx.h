/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   render_directx.h
Description : API rendering pipeline - DirectX 11.0
*******************************************************************************/
#ifndef _RENDER_DIRECTX_H
#define _RENDER_DIRECTX_H
#ifdef _WINDOWS
#include "globals.h"

#include "render.h"


// Rendering pipeline - DirectX 11.0
class RenderDirectX : public Render
{
public:
    /// <summary>Render instance construction</summary>
    /// <param name="pConfig">Thread configuration reference</param>
    RenderDirectX(Config* pConfig);
    /// <summary>API shutdown and instance destruction</summary>
    ~RenderDirectX();


    // -- RENDERING API MANAGEMENT -- ------------------------------------------

    /// <summary>Initialize DX API</summary>
    /// <exception cref="std::exception">DX API opening failure</exception>
    virtual void initApi();
    /// <summary>Cleanup and shutdown DX API</summary>
    virtual void closeApi();
    /// <summary>Reload rendering pipeline</summary>
    virtual void reloadApi();

    /// <summary>Draw current frame to the screen</summary>
    virtual void drawFrame();

    //...
};

#endif
#endif
