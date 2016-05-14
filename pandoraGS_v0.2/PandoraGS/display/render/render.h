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
#include "config.h"
#include "shader.h"

// graphic API identifiers
enum RenderApi
{
    RenderApi_SoftwareWarp = 0,
    RenderApi_OpenGL = 1,
    RenderApi_DirectX = 2
};
#define RENDERAPI_DEFAULT RenderApi_OpenGL


// Rendering pipeline - abstract factory
class Render
{
protected:
    Config* m_pConfig; // config container reference


    /*GLfloat         gl_z = 0.0f;
    BOOL            bNeedInterlaceUpdate = FALSE;
    BOOL            bNeedRGB24Update = FALSE;
    BOOL            bChangeWinMode = FALSE;
    #ifdef _WINDOWS
    //extern HGLRC    GLCONTEXT;
    #endif

    long            lClearOnSwap;
    long            lClearOnSwapColor;
    BOOL            bSkipNextFrame = FALSE;
    int             iColDepth;
    BOOL            bChangeRes;
    BOOL            bWindowMode;
    int             iWinSize;

    TWin_t          TWin;
    short           imageX0, imageX1;
    short           imageY0, imageY1;
    BOOL            bDisplayNotSet = TRUE;
    GLuint          uiScanLine = 0;
    int             iUseScanLines = 0;
    long            lSelectedSlot = 0;
    unsigned char * pGfxCardScreen = 0;
    int             iBlurBuffer = 0;
    int             iScanBlend = 0;
    int             iRenderFVR = 0;
    int             iNoScreenSaver = 0;
    unsigned long   ulGPUInfoVals[16];
    int             iFakePrimBusy = 0;
    int             iRumbleVal = 0;
    int             iRumbleTime = 0; */


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
