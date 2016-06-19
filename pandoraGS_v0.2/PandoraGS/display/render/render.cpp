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
#include "input_manager.h"
#include "psx_core_memory.h"
#include "render.h"
#include "render_software.h"
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
        case (unsigned int)RenderApi_OpenGL: 
            renderer = new RenderOpenGL(pConfig);
            break;

        #ifdef _WINDOWS
        case (unsigned int)RenderApi_DirectX: 
            renderer = new RenderDirectX(pConfig);
            break;

        default: // RenderApi_SoftwareWarp
            renderer = new RenderSoftware(pConfig); 
            break;
        #endif
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
        DWORD dwStyle = GetWindowLong(PsxCoreMemory::gen_hWindow, GWL_STYLE);
        PsxCoreMemory::gen_origStyle = dwStyle;

        // set new window style
        if (m_pConfig->dsp_isFullscreen) // fullscreen mode
        {
            if (m_pConfig->rnd_renderApiCode != RenderApi_SoftwareWarp)
                dwStyle = CS_OWNDC;
            else
                dwStyle &= ~(WS_THICKFRAME | WS_BORDER | WS_CAPTION);
        }
        else // window mode
        {
            if (m_pConfig->dsp_isWindowResizable == false)
                dwStyle &= ~WS_THICKFRAME;
            if (m_pConfig->rnd_renderApiCode != RenderApi_SoftwareWarp)
                dwStyle |= (WS_BORDER | WS_CAPTION | CS_OWNDC);
            else
                dwStyle |= (WS_BORDER | WS_CAPTION);
        }
        SetWindowLong(PsxCoreMemory::gen_hWindow, GWL_STYLE, dwStyle);

        // hide emulator menu
        PsxCoreMemory::gen_hMenu = GetMenu(PsxCoreMemory::gen_hWindow);
        if (PsxCoreMemory::gen_hMenu)
            SetMenu(PsxCoreMemory::gen_hWindow, NULL);
        #endif
    }

    // close display window
    else 
    {
        closeApi();

        #ifdef _WINDOWS
        // restore window style
        SetWindowLong(PsxCoreMemory::gen_hWindow, GWL_STYLE, PsxCoreMemory::gen_origStyle);
        // restore emulator menu
        if (PsxCoreMemory::gen_hMenu)
            SetMenu(PsxCoreMemory::gen_hWindow, PsxCoreMemory::gen_hMenu);
        PsxCoreMemory::gen_hMenu = NULL;
        #endif
    }
    _initialized = false;
}

/// <summary>Change window mode, depending on current settings</summary>
void Render::changeWindowMode()
{
    closeApi();
    //...
    //...
    initApi();
}
