/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   renderer_ogl4.cpp
    Description : OpenGL API rendering engine (OpenGL 4)
*******************************************************************************/
using namespace std;
#include "../../globals.h"
#include "renderer_ogl4.h"


// OPENGL4 RENDERER CREATION
//______________________________________________________________________________

/* opengl renderer constructor - initialize values -------------------------- */
RendererOgl4::RendererOgl4() : Renderer()
{
    
}

/* opengl renderer destructor ----------------------------------------------- */
RendererOgl4::~RendererOgl4()
{
    destroyRenderer(); // free common allocated memory
}

/* initialize dynamic allocations ------------------------------------------- */
long RendererOgl4::init()
{
    return initCommon();  // common memory allocation
}
    
// OPENGL4 PIPELINE CREATION
//______________________________________________________________________________
    
/* opengl rendering pipeline preparation ------------------------------------ */
long RendererOgl4::setPipeline()
{
    // create render window
    setPipelineCommon(true); // disable emulator menu and screensaver
    return 0;
}

/* opengl rendering pipeline removal ---------------------------------------- */
void RendererOgl4::unsetPipeline()
{
    unsetPipelineCommon();
}

/* initialize opengl -------------------------------------------------------- */
long RendererOgl4::setGraphicApi()
{
    //...
    return 0;
}

/* close opengl ------------------------------------------------------------- */
void RendererOgl4::unsetGraphicApi()
{
    //...
}

/* set opengl pixel format -------------------------------------------------- */
#if _SYSTEM == _WINDOWS
long RendererOgl4::setPixelFormat(HDC hDC)
#else
long RendererOgl4::setPixelFormat()
#endif
{
    return 0;
}


// OPENGL4 FRAME RENDERING
//______________________________________________________________________________

/**/
void RendererOgl4::render()
{
    
}

