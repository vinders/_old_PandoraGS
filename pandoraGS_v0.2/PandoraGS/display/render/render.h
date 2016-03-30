/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   render.h
Description : API rendering pipeline - abstract factory
*******************************************************************************/
#ifndef _RENDER_H
#define _RENDER_H
#include "globals.h"

// graphic API identifiers
enum RenderApi
{
    RenderApi_SoftwareWarp = 0,
    RenderApi_OpenGL = 1,
    RenderApi_DirectX = 2
};
#define RENDERAPI_DEFAULT RenderApi_OpenGL

#include "config.h"


// Rendering pipeline - abstract factory
class Render
{
protected:
    Config* m_pConfig; // config container reference


protected:
    /// <summary>Common render instance construction</summary>
    /// <param name="pConfig">Thread configuration reference</param>
    Render(Config* pConfig);
public:
    /// <summary>API shutdown and instance destruction</summary>
    virtual ~Render() {}

    /// <summary>Factory to create renderer instance, based on config</summary>
    /// <param name="pConfig">Thread configuration reference</param>
    static Render* createInstance(Config* pConfig);


    // -- RENDERING API MANAGEMENT -- ------------------------------------------

    /// <summary>Initialize rendering API</summary>
    /// <exception cref="std::exception">Rendering API opening failure</exception>
    virtual void initApi() = 0;
    /// <summary>Shutdown rendering API</summary>
    virtual void closeApi() = 0;

    //...
};

#endif
