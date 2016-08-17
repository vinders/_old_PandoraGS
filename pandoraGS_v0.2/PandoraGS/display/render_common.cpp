/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   render.cpp
Description : API rendering pipeline - common
*******************************************************************************/
#include <Windows.h>
using namespace std;
#include "render_opengl.h"
#include "render_directx.h"
#include "core_memory.h"


/// <summary>Shutdown rendering API</summary>
/// <param name="isOpened">Open or close window</param>
void Render::setWindow(bool isOpened)
{
    // open display window
    if (isOpened)
    {
        #ifdef _WINDOWS
        // save style backup to restore it
        DWORD dwStyle = GetWindowLong(CoreMemory::gen_hWindow, GWL_STYLE);
        CoreMemory::gen_origStyle = dwStyle;

        // set new window style
        if (Config::dsp_isFullscreen) // fullscreen mode
        {
            //if (m_pConfig->rnd_renderApiCode != RenderApi_SoftwareWarp)
                dwStyle = CS_OWNDC;
            //else dwStyle &= ~(WS_THICKFRAME | WS_BORDER | WS_CAPTION);
        }
        else // window mode
        {
            if (Config::dsp_isWindowResizable == false)
                dwStyle &= ~WS_THICKFRAME;
            //if (m_pConfig->rnd_renderApiCode != RenderApi_SoftwareWarp)
                dwStyle |= (WS_BORDER | WS_CAPTION | CS_OWNDC);
            //else dwStyle |= (WS_BORDER | WS_CAPTION);
        }
        SetWindowLong(CoreMemory::gen_hWindow, GWL_STYLE, dwStyle);

        // hide emulator menu
        CoreMemory::gen_hMenu = GetMenu(CoreMemory::gen_hWindow);
        if (CoreMemory::gen_hMenu)
            SetMenu(CoreMemory::gen_hWindow, NULL);
        #endif
    }

    // close display window
    else 
    {
        closeApi();

        #ifdef _WINDOWS
        // restore window style
        SetWindowLong(CoreMemory::gen_hWindow, GWL_STYLE, CoreMemory::gen_origStyle);
        // restore emulator menu
        if (CoreMemory::gen_hMenu)
            SetMenu(CoreMemory::gen_hWindow, CoreMemory::gen_hMenu);
        CoreMemory::gen_hMenu = NULL;
        #endif
    }
    m_isInitialized = false;
}

/// <summary>Change window mode, depending on current settings</summary>
void Render::changeWindowMode()
{
    setWindow(false);
    setWindow(true);
    initApi();
}

/// <summary>Change window size or set stretching mode</summary>
/// <param name="isWindowResized">Check new window size</param>
void Render::setDrawingSize(bool isWindowResized)
{
    //...
}

