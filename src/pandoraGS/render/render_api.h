/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   render_api.h
Description : rendering api management - OpenGL 4.1
*******************************************************************************/
#ifndef _RENDER_API_H
#define _RENDER_API_H

class RenderApi
{
private:
    static bool m_isInitialized;
    // initialisé à false
    // lors de demande d'affichage, si vaut false, appel vers initApi()

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


public:
    // -- RENDERING API MANAGEMENT -- ------------------------------------------

    /// <summary>Initialize OpenGL API</summary>
    /// <exception cref="std::exception">OpenGL API opening failure</exception>
    static void init();
    /// <summary>Cleanup and shutdown OpenGL API</summary>
    static void close();

    /// <summary>Reload rendering pipeline</summary>
    static void reload();

    /// <summary>Draw current frame to the screen</summary>
    static void drawFrame();

    /// <summary>Set display window</summary>
    /// <param name="isOpened">Open or close window</param>
    static void setWindow(bool isOpened);
    /// <summary>Change window mode, depending on current settings</summary>
    static void changeWindowMode();
    /// <summary>Change window size or set stretching mode</summary>
    /// <param name="isWindowResized">Check new window size</param>
    static void setDrawingSize(bool isWindowResized);
};

#endif
