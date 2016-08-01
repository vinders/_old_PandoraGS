/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   render.cpp
Description : API rendering pipeline - abstract factory
*******************************************************************************/
#include <Windows.h>
using namespace std;
#include "core_memory.h"
#include "render.h"
#include "render_opengl.h"
#include "render_directx.h"


/// <summary>Common render instance construction</summary>
/// <param name="pConfig">Thread configuration reference</param>
Render::Render(Config* pConfig)
{
    m_pConfig = pConfig;
}

/// <summary>Factory to create renderer instance, based on config</summary>
/// <param name="pConfig">Thread configuration reference</param>
Render* Render::createInstance(Config* pConfig)
{
    // factory pattern
    Render* renderer = NULL;
    switch (pConfig->rnd_renderApiCode)
    {
        #ifdef _WINDOWS
        case (unsigned int)RenderApi_DirectX: 
            renderer = new RenderDirectX(pConfig);
            break;
        #endif
        default: // RenderApi_OpenGL
            renderer = new RenderOpenGL(pConfig);
            break;
    }

    if (renderer == NULL)
        throw new std::exception("createInstance: Rendering instance creation error");
    return renderer;
}

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
        if (m_pConfig->dsp_isFullscreen) // fullscreen mode
        {
            //if (m_pConfig->rnd_renderApiCode != RenderApi_SoftwareWarp)
                dwStyle = CS_OWNDC;
            //else dwStyle &= ~(WS_THICKFRAME | WS_BORDER | WS_CAPTION);
        }
        else // window mode
        {
            if (m_pConfig->dsp_isWindowResizable == false)
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
    _initialized = false;
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
void setDrawingSize(bool isWindowResized)
{
    //...
}
