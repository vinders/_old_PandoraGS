/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   renderer_dx11.cpp
    Description : DirectX API rendering engine (Direct3D 11.0)
*******************************************************************************/
/*using namespace std;
#include "../../globals.h"
#if _SYSTEM == _WINDOWS
#include "renderer_dx11.h"

extern int consoleVramFactor;


// DIRECT3D11 RENDERER CREATION
//______________________________________________________________________________

/* d3d renderer constructor - initialize values ----------------------------- * /
RendererD3D11::RendererD3D11() : Renderer()
{
    
}

/* d3d renderer destructor -------------------------------------------------- * /
RendererD3D11::~RendererD3D11()
{
    destroyRenderer(); // free common allocated memory
}

/* initialize dynamic allocations ------------------------------------------- * /
long RendererD3D11::init()
{
    return initCommon();  // common memory allocation
}
    
// DIRECT3D11 PIPELINE CREATION
//______________________________________________________________________________
    
/* d3d rendering pipeline preparation --------------------------------------- * /
long RendererD3D11::setPipeline()
{
    // create render window
    setPipelineCommon(true); // disable emulator menu and screensaver
    return 0;
}

/* d3d rendering pipeline removal ------------------------------------------- * /
void RendererD3D11::unsetPipeline()
{
    unsetPipelineCommon();
}

/* initialize direct3D ------------------------------------------------------ * /
long RendererD3D11::setGraphicApi()
{
    //...
    return 0;
}

/* close direct3D ----------------------------------------------------------- * /
void RendererD3D11::unsetGraphicApi()
{
    //...
}

/* set d3d pixel format ----------------------------------------------------- * /
long RendererD3D11::setPixelFormat(HDC hDC)
{
    return 0;
}


// DIRECT3D11 FRAME RENDERING
//______________________________________________________________________________

/** /
void RendererD3D11::render()
{
    
}


#endif*/
