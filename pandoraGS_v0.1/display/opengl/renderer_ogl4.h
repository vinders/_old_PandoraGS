/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   renderer_ogl4.h
    Description : OpenGL API rendering engine (OpenGL 4)
*******************************************************************************/
#ifndef _RENDERER_OGL4_H
#define _RENDERER_OGL4_H
//...opengl
#include "../renderer.h"
#include "shader_ogl4.h"

/* GRAPHIC RENDERER (OpenGL4) ----------------------------------------------- */
class RendererOgl4 : public Renderer
{
  private:
    bool _bResolutionChange;
    //GLuint shader_vert;
    //GLuint shader_frag;
    //ShaderOgl4* source_vert;
    //ShaderOgl4* source_frag;

    
  public:
    RendererOgl4();
    ~RendererOgl4();
    
    // graphic pipeline methods
    long init();
    long setPipeline();
    void unsetPipeline();
    long setGraphicApi();
    void unsetGraphicApi();
    void render();
    
    // misc graphic methods
    #if _SYSTEM == _WINDOWS
        long setPixelFormat(HDC);
      #else
        long setPixelFormat();
    #endif
    
    //...
};

#endif
