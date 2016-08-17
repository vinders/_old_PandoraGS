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
#if _RENDERAPI == RENDERAPI_OPENGL
#include "config.h"
#include "shader.h"

// Rendering pipeline - OpenGL 4.1
class Render
{
private:
    static bool m_isInitialized;
    // initialisé à false
    // lors de demande d'affichage, si vaut false, appel vers initApi()


public:
    // -- RENDERING API MANAGEMENT -- ------------------------------------------

    /// <summary>Initialize OpenGL API</summary>
    /// <exception cref="std::exception">OpenGL API opening failure</exception>
    static void initApi();
    /// <summary>Cleanup and shutdown OpenGL API</summary>
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
