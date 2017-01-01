/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   engine.cpp
Description : rendering engine - OpenGL 4.1
*******************************************************************************/
// GLEW 1.9.0 ([%VS%]/VC/lib & [%VS%]/VC/include)
// dependencies : opengl32.lib;glu32.lib;glew32s.lib;
#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
using namespace std;
#include "geometry.hpp"
#include "config.h"
#include "system_tools.h"
#include "engine.h"

#ifdef _WINDOWS
extern HWND g_hWindow; // main emulator window handle
HDC g_globalDeviceContext = 0;
HGLRC g_openGlRenderContext = 0;
#else
GLFWwindow* g_openGlWindow = NULL;
#endif
bool Engine::s_isInitialized = false;
GLuint Engine::s_prog = 0; // rendering pipeline program identifier
Shader* Engine::s_pVertShader = NULL; // vertex shader (polygon management)
Shader* Engine::s_pFragShader = NULL; // fragment shader (pixel management)


// -- RENDERING API MANAGEMENT -- ----------------------------------------------

#ifdef _WINDOWS
/// <summary>Initialize screen + device pixel format</summary>
void Engine::initScreen()
{
    if (g_globalDeviceContext) // context already in use
        return;
    HDC hDC = GetDC(g_hWindow);

    // define pixel format
    PIXELFORMATDESCRIPTOR pxFD = { sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // support OpenGL + double buffering
        PFD_TYPE_RGBA, 16, // output color depth
        0, 0, 0, 0, 0, 0,  // color bits ignored
        0, 0,              // no alpha buffer + shift bit ignored
        0, 0, 0, 0, 0,     // no accumulation buffer + accumulation bits (R,G,B,A) ignored
        0,                 // no z-buffer
        0, 0,              // no stencil buffer + no auxillary buffer
        PFD_MAIN_PLANE, 0, // main layer
        0, 0, 0            // layer masks, ignored
    };
    if (Config::dsp_isColorDepth32)
        pxFD.cColorBits = 32;
    if (Config::dsp_isZbuffer)
        pxFD.cDepthBits = 16;

    // set pixel format
    unsigned int pxFormat = ChoosePixelFormat(hDC, &pxFD);
    if (!pxFormat || !SetPixelFormat(hDC, pxFormat, &pxFD))
    {
        ReleaseDC(g_hWindow, hDC);
        throw std::exception("RenderApi: failed to set pixel format");
    }
    // fill screen (while loading)
    if (Config::dsp_isFullscreen)
        SystemTools::fillDisplayWindow(hDC, Config::dsp_fullscnResX, Config::dsp_fullscnResY);
    else
        SystemTools::fillDisplayWindow(hDC, Config::dsp_windowResX, Config::dsp_windowResY);

    ReleaseDC(g_hWindow, hDC);
}
#endif

/// <summary>Initialize OpenGL API</summary>
/// <exception cref="std::exception">OpenGL API init failure</exception>
void Engine::initGL()
{
    #ifdef _WINDOWS // WIN32
    g_globalDeviceContext = GetDC(g_hWindow);

    // assign context window
    g_openGlRenderContext = wglCreateContext(g_globalDeviceContext);
    wglMakeCurrent(g_globalDeviceContext, g_openGlRenderContext);
    if (Config::dsp_isFullscreen == false) // window mode -> release context
    {
        ReleaseDC(g_hWindow, g_globalDeviceContext);
        g_globalDeviceContext = 0;
    }

    #else // GLFW
    // context manager
    if (!glfwInit())
        throw std::exception("RenderApi: failed to initialize GLFW");
    // OS X compatibility
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create context window
    if (Config::dsp_isFullscreen)
        g_openGlWindow = glfwCreateWindow(Config::dsp_fullscnResX, Config::dsp_fullscnResY, "PandoraGS", glfwGetPrimaryMonitor(), NULL);
    else
        g_openGlWindow = glfwCreateWindow(Config::dsp_windowResX, Config::dsp_windowResY, "PandoraGS", NULL, NULL);
    if (!g_openGlWindow) 
    {
        glfwTerminate();
        throw std::exception("RenderApi: failed to create context window");
    }
    glfwMakeContextCurrent(g_openGlWindow);
    #endif

    // extension manager
    glewExperimental = GL_TRUE;
    glewInit();

    // set display + shaders
    s_prog = glCreateProgram();
    setViewport(false);
    loadPipeline();
    s_isInitialized = true;
}

/// <summary>Cleanup and shutdown OpenGL API</summary>
void Engine::close()
{
    if (s_isInitialized == false)
        return;

    //...

    // destroy shaders
    if (s_pVertShader != NULL)
    {
        delete s_pVertShader;
        s_pVertShader = NULL;
    }
    if (s_pFragShader != NULL)
    {
        delete s_pFragShader;
        s_pFragShader = NULL;
    }
    glDeleteProgram(s_prog);

    // destroy window context
    #ifdef _WINDOWS
    wglMakeCurrent(NULL, NULL);
    if(g_openGlRenderContext) 
        wglDeleteContext(g_openGlRenderContext);
    if (Config::dsp_isFullscreen == false && g_globalDeviceContext) // fullscreen -> release device context
        ReleaseDC(g_hWindow, g_globalDeviceContext);
    g_globalDeviceContext = 0;
    #else
    glfwDestroyWindow(g_openGlWindow);
    glfwTerminate();
    #endif

    s_isInitialized = false;
}

/// <summary>Load/reload rendering pipeline</summary>
void Engine::loadPipeline()
{
    if (s_isInitialized == false)
    {
        initGL(); return; // initGL will call loadPipeline() -> return
    }

    // remove previous shaders
    if (s_pVertShader != NULL)
    {
        delete s_pVertShader;
        s_pVertShader = NULL;
    }
    if (s_pFragShader != NULL)
    {
        delete s_pFragShader;
        s_pFragShader = NULL;
    }
    // create shaders, based on current config
    s_pVertShader = new Shader(s_prog, Shadertype_vertex);
    s_pVertShader = new Shader(s_prog, Shadertype_fragment);
}

/// <summary>Render current frame</summary>
void Engine::render()
{
    if (s_isInitialized == false)
    {
        initGL(); return;
    }

    //... dessin VBO et VAO
    //...
    //... conversion buffer internal res -> buffer d'affichage (+ centrer/tronquer)
    //...
    //... swap buffers
}

/// <summary>Set display size and stretching mode</summary>
/// <param name="isWindowResized">Check new window size</param>
void Engine::setViewport(bool isWindowResized)
{
    // get window size
    wtile_t displaySize;
    if (Config::dsp_isFullscreen)
    {
        displaySize.width = Config::dsp_fullscnResX;
        displaySize.height = Config::dsp_fullscnResY;
    }
    else
    {
        if (isWindowResized)
        {
            //...
        }
        else
        {
            displaySize.width = Config::dsp_windowResX;
            displaySize.height = Config::dsp_windowResY;
        }
    }

    // set display
    //glViewport(rRatioRect.left, displaySize.height - (rRatioRect.top + rRatioRect.bottom), rRatioRect.right, rRatioRect.bottom); // init viewport
    glScissor(0, 0, Config::dsp_fullscnResX, Config::dsp_fullscnResY); // init clipping
    glEnable(GL_SCISSOR_TEST);

    //...
}
