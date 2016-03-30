/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   renderer_softd3d.h
    Description : software rendering engine (Direct3D WARP)
*******************************************************************************/
#ifndef _RENDERER_SOFTD3D_H
#define _RENDERER_SOFTD3D_H
#if _SYSTEM == _WINDOWS
#include "../renderer.h"

/* GRAPHIC RENDERER (software) ---------------------------------------------- */
class RendererSoftwareD3D : public Renderer
{
  private:
    
  public:
    RendererSoftwareD3D();
    ~RendererSoftwareD3D();
    
    // graphic pipeline methods
    long init();
    long setPipeline();
    void unsetPipeline();
    long setGraphicApi();
    void unsetGraphicApi();
    void render();
    
    // misc graphic methods
    long setPixelFormat(HDC);
    //...
};

#endif
#endif
