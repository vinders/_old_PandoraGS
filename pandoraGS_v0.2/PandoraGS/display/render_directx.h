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
#if _RENDERAPI == RENDERAPI_DIRECTX
#include "config.h"
#include "shader.h"

// Rendering pipeline - DirectX 11.0
class Render
{
private:
    static bool m_isInitialized;
    // initialisé à false
    // lors de demande d'affichage, si vaut false, appel vers initApi()

    //...

public:
    // -- RENDERING API MANAGEMENT -- ------------------------------------------

    /// <summary>Initialize DX API</summary>
    /// <exception cref="std::exception">DX API opening failure</exception>
    static void initApi();
    /// <summary>Cleanup and shutdown DX API</summary>
    static void closeApi();

    /// <summary>Reload rendering pipeline</summary>
    static void reloadApi();

    /// <summary>Draw current frame to the screen</summary>
    static void drawFrame();

    /// <summary>Shutdown rendering API</summary>
    /// <param name="isOpened">Open or close window</param>
    static void setWindow(bool isOpened);
    /// <summary>Change window mode, depending on current settings</summary>
    static void changeWindowMode();
    /// <summary>Change window size or set stretching mode</summary>
    /// <param name="isWindowResized">Check new window size</param>
    static void setDrawingSize(bool isWindowResized);
};

#endif
#endif

#endif
