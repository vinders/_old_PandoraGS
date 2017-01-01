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
#include "shader.h"
#include "engine.h"

#ifdef _WINDOWS
extern HWND g_hWindow; // main emulator window handle
HDC g_globalDeviceContext;
HGLRC g_openGlRenderContext;
#else
GLFWwindow* g_openGlWindow = NULL;
#endif
bool Engine::s_isInitialized = false;
bool Engine::s_isReady = false;
GLuint Engine::s_prog = 0; // rendering pipeline program identifier


// -- RENDERING API MANAGEMENT -- ----------------------------------------------

/// <summary>Initialize screen + device pixel format</summary>
void Engine::initScreen()
{
    #ifdef _WINDOWS
    HDC hDC = GetDC(g_hWindow);
    s_isInitialized = false;

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
    #endif

    s_isReady = true;
}

/// <summary>Initialize OpenGL API</summary>
/// <exception cref="std::exception">OpenGL API init failure</exception>
void Engine::initGL()
{
    if (s_isInitialized)
        return;
    s_isReady = false;

    #ifdef _WINDOWS // WIN32
    // assign context window
    g_globalDeviceContext = GetDC(g_hWindow);
    g_openGlRenderContext = wglCreateContext(g_globalDeviceContext);
    wglMakeCurrent(g_globalDeviceContext, g_openGlRenderContext);
    if (Config::dsp_isFullscreen == false) // window mode -> release context
    {
        ReleaseDC(g_hWindow, g_globalDeviceContext);
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
    setViewport(false);
    loadPipeline();
    s_isInitialized = true;
    s_isReady = true;
}

/// <summary>Cleanup and shutdown OpenGL API</summary>
void Engine::close()
{
    if (s_isInitialized == false)
        return;
    s_isReady = false;

    //...

    // destroy pipeline
    glUseProgram(0);
    glDeleteProgram(s_prog);
    s_prog = 0;

    // destroy window context
    #ifdef _WINDOWS
    wglMakeCurrent(NULL, NULL);
    if(g_openGlRenderContext) 
        wglDeleteContext(g_openGlRenderContext);
    if (Config::dsp_isFullscreen == false) // fullscreen -> release device context
        ReleaseDC(g_hWindow, g_globalDeviceContext);
    #else
    glfwDestroyWindow(g_openGlWindow);
    glfwTerminate();
    #endif

    s_isInitialized = false;
}

/// <summary>Load/reload rendering pipeline</summary>
void Engine::loadPipeline()
{
    if (s_isInitialized == false && s_isReady)
    {
        initGL(); return; // initGL will also call loadPipeline() -> return
    }
    if (s_prog != 0) // close previous pipeline
    {
        glUseProgram(0);
        glDeleteProgram(s_prog);
        s_prog = 0;
    }

    // create shaders, based on current config
    s_prog = glCreateProgram();
    Shader* pVertShader = new Shader(s_prog, Shadertype_vertex);
    Shader* pFragShader = new Shader(s_prog, Shadertype_fragment);
    glLinkProgram(s_prog);
    glUseProgram(s_prog);

    // remove shader source objects after linking them
    if (pVertShader != NULL)
        delete pVertShader;
    if (pFragShader != NULL)
        delete pFragShader;
}

/// <summary>Render current frame</summary>
void Engine::render()
{
    if (s_isInitialized == false)
    {
        if (s_isReady)
            initGL();
        return; // skip first frame
    }

    //... dessin VBO et VAO
    //...
    //... conversion buffer internal res -> buffer d'affichage (+ centrer/tronquer)
    //...
    //... swap buffers


    // --- TEST --- > tmp -> to remove
    {
        glViewport(0, 0, Config::dsp_windowResX, Config::dsp_windowResY);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glEnable(GL_DEPTH_TEST); // enable depth-testing
        glDepthFunc(GL_LESS);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- V1
        float points[] = {
            0.0f, 0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f,
            0.4f, 0.1f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f, -0.7f, 0.0f,
            0.0f, 0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            -0.4f, 0.1f, 0.0f,
        };
        float colours[] = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,
            0.1f, 0.1f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.7f,
            0.0f, 0.9f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.9f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.8f,
            0.0f, 0.0f, 0.0f
        };
        const int triangles = 4;
        GLuint points_vbo = 0;
        glGenBuffers(1, &points_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        glBufferData(GL_ARRAY_BUFFER, triangles * 9 * sizeof(float), points, GL_STATIC_DRAW);
        GLuint colours_vbo = 0;
        glGenBuffers(1, &colours_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
        glBufferData(GL_ARRAY_BUFFER, triangles * 9 * sizeof(float), colours, GL_STATIC_DRAW);
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1); // for colors
        glBindBuffer(GL_ARRAY_BUFFER, colours_vbo); // for colors
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL); // for colors
        // draw
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, triangles * 3);
        // free memory
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDeleteVertexArrays(1, &vao);
        GLuint vboArray[2] = { points_vbo, colours_vbo };
        glDeleteBuffers(2, vboArray);

        // --- V2
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // coords + colors
        float line_vertex[] =
        {
            0.25f, -0.35f,	//vertex 1
            0.8f, 0.5f,	    //vertex 2
            0.0f, 0.25f		//vertex 3
        };
        float line_color[] =
        {
            1.0, 0.0, 0.0, 1.0,
            0.5, 1.0, 0.0, 1.0,
            0.0, 0.5, 1.0, 1.0
        };
        // draw
        points_vbo = 0;
        glGenBuffers(1, &points_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * 2 * sizeof(float), line_vertex, GL_STATIC_DRAW);
        colours_vbo = 0;
        glGenBuffers(1, &colours_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * 4 * sizeof(float), line_color, GL_STATIC_DRAW);
        vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1); // for colors
        glBindBuffer(GL_ARRAY_BUFFER, colours_vbo); // for colors
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL); // for colors
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 0, 6);
        // free memory
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDeleteVertexArrays(1, &vao);
        vboArray[0] = points_vbo;
        vboArray[1] = colours_vbo;
        glDeleteBuffers(2, vboArray);
        // free memory
        glDisable(GL_BLEND);
        glDisable(GL_LINE_SMOOTH);

        // --- RENDER
        if (Config::dsp_isFullscreen)
            SwapBuffers(wglGetCurrentDC());
        else
        {
            g_globalDeviceContext = GetDC(g_hWindow);
            SwapBuffers(g_globalDeviceContext);
            ReleaseDC(g_hWindow, g_globalDeviceContext);
        }
    }
    // --- END OF TEST ---
}

/// <summary>Set display size and stretching mode</summary>
/// <param name="isWindowResized">Check new window size</param>
void Engine::setViewport(bool isWindowResized)
{
    if (s_isInitialized == false && s_isReady)
    {
        initGL(); return; // initGL will also call setViewport() -> return
    }

    // get window size
    wtile_t displaySize;
    if (Config::dsp_isFullscreen)
    {
        displaySize.width = Config::dsp_fullscnResX;
        displaySize.height = Config::dsp_fullscnResY;
    }
    else if (isWindowResized == false || SystemTools::getDisplayWindowSize(g_hWindow, displaySize.width, displaySize.height) == false)
    {
        displaySize.width = Config::dsp_windowResX;
        displaySize.height = Config::dsp_windowResY;
    }

    // set display
    //glViewport(rRatioRect.left, displaySize.height - (rRatioRect.top + rRatioRect.bottom), rRatioRect.right, rRatioRect.bottom); // init viewport
    glScissor(0, 0, Config::dsp_fullscnResX, Config::dsp_fullscnResY); // init clipping
    glEnable(GL_SCISSOR_TEST);

    //...
}
