/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   render_api.cpp
Description : rendering api management - OpenGL 4.1
*******************************************************************************/
using namespace std;
#include "memory_dispatcher.h"
#include "render_api.h"

bool RenderApi::m_isInitialized = false;

// -- RENDERING API MANAGEMENT -- ----------------------------------------------

/// <summary>Initialize DX API</summary>
/// <exception cref="std::exception">WARP API opening failure</exception>
void RenderApi::init()
{

}

/// <summary>Cleanup and shutdown DX API</summary>
void RenderApi::close()
{

}

/// <summary>Reload rendering pipeline</summary>
void RenderApi::reload()
{

}

/// <summary>Draw current frame to the screen</summary>
void RenderApi::drawFrame()
{

}

/// <summary>Set display window</summary>
/// <param name="isOpened">Open or close window</param>
void RenderApi::setWindow(bool isOpened)
{
    // open display window
    if (isOpened)
    {
        #ifdef _WINDOWS
        // save style backup to restore it
        DWORD dwStyle = GetWindowLong(MemoryDispatcher::s_hWindow, GWL_STYLE);
        MemoryDispatcher::s_origStyle = dwStyle;

        // set new window style
        if (Config::dsp_isFullscreen) // fullscreen mode
        {
            dwStyle = CS_OWNDC;
        }
        else // window mode
        {
            if (Config::dsp_isWindowResizable == false)
                dwStyle &= ~WS_THICKFRAME;
            dwStyle |= (WS_BORDER | WS_CAPTION | CS_OWNDC);
        }
        SetWindowLong(MemoryDispatcher::s_hWindow, GWL_STYLE, dwStyle);

        // hide emulator menu
        MemoryDispatcher::s_hMenu = GetMenu(MemoryDispatcher::s_hWindow);
        if (MemoryDispatcher::s_hMenu)
            SetMenu(MemoryDispatcher::s_hWindow, NULL);
        #endif
    }

    // close display window
    else
    {
        #ifdef _WINDOWS
        // restore window style
        SetWindowLong(MemoryDispatcher::s_hWindow, GWL_STYLE, MemoryDispatcher::s_origStyle);
        // restore emulator menu
        if (MemoryDispatcher::s_hMenu)
            SetMenu(MemoryDispatcher::s_hWindow, MemoryDispatcher::s_hMenu);
        MemoryDispatcher::s_hMenu = NULL;
        #endif
    }
    m_isInitialized = false;
}

/// <summary>Change window mode, depending on current settings</summary>
void RenderApi::changeWindowMode()
{
    setWindow(false);
    setWindow(true);
    init();
}

/// <summary>Change window size or set stretching mode</summary>
/// <param name="isWindowResized">Check new window size</param>
void RenderApi::setDrawingSize(bool isWindowResized)
{
    //...
}
