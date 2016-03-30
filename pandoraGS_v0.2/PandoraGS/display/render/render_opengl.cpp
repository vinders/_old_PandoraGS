/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   render_opengl.cpp
Description : API rendering pipeline - OpenGL 4.1
*******************************************************************************/
using namespace std;
#include "render_opengl.h"


/// <summary>Render instance construction</summary>
/// <param name="pConfig">Thread configuration reference</param>
RenderOpenGL::RenderOpenGL(Config* pConfig) : Render(pConfig)
{
}

/// <summary>API shutdown and instance destruction</summary>
RenderOpenGL::~RenderOpenGL()
{
    closeApi();
    m_pConfig = NULL;
}


// -- RENDERING API MANAGEMENT -- ----------------------------------------------

/// <summary>Initialize DX API</summary>
/// <exception cref="std::exception">WARP API opening failure</exception>
void RenderOpenGL::initApi()
{

}

/// <summary>Cleanup and shutdown DX API</summary>
void RenderOpenGL::closeApi()
{

}

//...

