/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   render_opengl.h
Description : API rendering pipeline - OpenGL 4.1
*******************************************************************************/
#ifndef _RENDER_OPENGL_H
#define _RENDER_OPENGL_H
#include "globals.h"

#include "render.h"


// Rendering pipeline - OpenGL 4.1
class RenderOpenGL : public Render
{
public:
    /// <summary>Render instance construction</summary>
    /// <param name="pConfig">Thread configuration reference</param>
    RenderOpenGL(Config* pConfig);
    /// <summary>API shutdown and instance destruction</summary>
    ~RenderOpenGL();


    // -- RENDERING API MANAGEMENT -- ------------------------------------------

    /// <summary>Initialize OpenGL API</summary>
    /// <exception cref="std::exception">OpenGL API opening failure</exception>
    virtual void initApi();
    /// <summary>Cleanup and shutdown OpenGL API</summary>
    virtual void closeApi();

    //...
};

#endif
