/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   render.cpp
Description : API rendering pipeline - abstract factory
*******************************************************************************/
using namespace std;
#include "render.h"
#include "render_software.h"
#include "render_opengl.h"
#include "render_directx.h"


/// <summary>Common render instance construction</summary>
/// <param name="pConfig">Thread configuration reference</param>
Render::Render(Config* pConfig)
{
    m_pConfig = pConfig;
}

/// <summary>Factory to create renderer instance, based on config</summary>
/// <param name="pConfig">Thread configuration reference</param>
Render* Render::createInstance(Config* pConfig)
{
    // factory pattern
    Render* renderer = NULL;
    switch (pConfig->rnd_renderApiCode)
    {
        case (unsigned int)RenderApi_OpenGL: 
            renderer = new RenderOpenGL(pConfig);
            break;

        #ifdef _WINDOWS
        case (unsigned int)RenderApi_DirectX: 
            renderer = new RenderDirectX(pConfig);
            break;

        default: // RenderApi_SoftwareWarp
            renderer = new RenderSoftware(pConfig); 
            break;
        #endif
    }

    if (renderer == NULL)
        throw new std::exception("createInstance: Rendering instance creation error");
    return renderer;
}
