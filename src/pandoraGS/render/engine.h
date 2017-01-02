/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   engine.h
Description : rendering engine - OpenGL 4.1
*******************************************************************************/
#ifndef _ENGINE_H
#define _ENGINE_H

#include "opengl.h" // openGL includes

class Engine
{
private:
    static bool s_isInitialized;
    static bool s_isReady;
    static GLuint s_prog;         // rendering pipeline program identifier

    
public:
    // -- RENDERING API MANAGEMENT -- ------------------------------------------

    /// <summary>Initialize screen + device pixel format</summary>
    static void initScreen();
    /// <summary>Initialize OpenGL API</summary>
    /// <exception cref="std::exception">OpenGL API init failure</exception>
    static void initGL();
    /// <summary>Cleanup and shutdown OpenGL API</summary>
    static void close();

    /// <summary>Load/reload rendering pipeline</summary>
    static void loadPipeline();

    /// <summary>Render current frame</summary>
    static void render();

    /// <summary>Set display size and stretching mode</summary>
    /// <param name="isWindowResized">Check new window size</param>
    static void setViewport(bool isWindowResized);


    // -- GETTERS -- -----------------------------------------------------------

    /// <summary>Check if window has been initialized</summary>
    static bool isReady()
    {
        return s_isReady;
    }
};

#endif
