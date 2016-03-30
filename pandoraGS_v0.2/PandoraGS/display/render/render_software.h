/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   render_software.h
Description : API rendering pipeline - software (DX Warp 11)
*******************************************************************************/
#ifndef _RENDER_SOFTWARE_H
#define _RENDER_SOFTWARE_H
#ifdef _WINDOWS
#include "globals.h"

#include "render.h"


// Rendering pipeline - software (DX warp 11)
class RenderSoftware : public Render
{
public:
    /// <summary>Render instance construction</summary>
    /// <param name="pConfig">Thread configuration reference</param>
    RenderSoftware(Config* pConfig);
    /// <summary>API shutdown and instance destruction</summary>
    ~RenderSoftware();


    // -- RENDERING API MANAGEMENT -- ------------------------------------------

    /// <summary>Initialize WARP API</summary>
    /// <exception cref="std::exception">WARP API opening failure</exception>
    virtual void initApi();
    /// <summary>Cleanup and shutdown WARP API</summary>
    virtual void closeApi();

    //...
};

#endif
#endif
