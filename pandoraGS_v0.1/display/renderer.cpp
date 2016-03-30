/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   renderer.cpp
    Description : graphic API rendering engine (abstract factory)
*******************************************************************************/
using namespace std;
#include "../globals.h"
#if _SYSTEM == _WINDOWS
    #include <windowsx.h>
#endif
#include "framerate.h"
#include "renderer.h"
#include "software/renderer_softd3d.h"
#include "opengl/renderer_ogl4.h"
//#include "directx/renderer_dx11.h"

#if _SYSTEM == _WINDOWS
    extern HWND  gpu_window;
#endif
extern bool zincEmulation;
extern unsigned long zincGPUVersion;
//extern const unsigned char primTableCX[256];


/* instantiate appropriate renderer (factory) ------------------------------- */
Renderer* Renderer::getInstance(unsigned int renderer)
{
    Renderer* gameRenderer = NULL;
    switch (renderer)
    {
        case GLAPICODE_OGL4: 
            gameRenderer = new RendererOgl4();  break;
            
        #if _SYSTEM == _WINDOWS
            /*case GLAPICODE_DX11: 
                gameRenderer = new RendererD3D11(); break;*/
            default:
                gameRenderer = new RendererSoftwareD3D(); break;
          #else
            default:
                gameRenderer = new RendererOgl4();  break;
        #endif
    }
    return gameRenderer;
}


// COMMON RENDERER CREATION
//______________________________________________________________________________

/* renderer constructor - initialize values - common ------------------------ */
Renderer::Renderer()
{
    // initialize variables
    #if _SYSTEM == _WINDOWS
        _hEmulatorMenu = NULL;
        _dwOriginalStyle = 0;
    #endif
    _bApiState    = false;
    _langContent  = NULL;
    bVsyncReady   = true;
    playersCursorsActive = 0;
    busySequence   = saveSlot   = 0;
    
    // initialize vram variables (no allocation yet)
    _consoleVram  = NULL;
    outputMode = DR_NORMAL;
    inputMode  = DR_NORMAL;
    memset(&consoleVramOutput, 0, sizeof(ConsoleVramIO_t));
    memset(&consoleVramInput,  0, sizeof(ConsoleVramIO_t));
    vramSize = PSVRAM_SIZE*1024;
    if (zincEmulation)
        vramSize = ZINCVRAM_SIZE*1024;
    
    // initialize low-level variables
    gpuRawData   = GPUDATA_INIT;
    gpuRawStatus = 0;
    memset(&gpuRawInfo,   0, PSRAWINFO_SIZE*sizeof(unsigned long));
    memset(statusControl, 0, PSSTATUSCONTROL_SIZE*sizeof(unsigned long));
    
    //! szDebugText[0]=0;//directdraw
}

/* renderer destructor - common --------------------------------------------- */
void Renderer::destroyRenderer()
{
    // free allocated memory
    if (_langContent)
        delete _langContent;
    _langContent = NULL;
    consoleVramByte  = NULL;
    consoleVramWord  = NULL;
    consoleVramDword = NULL;
    consoleVramEOM   = NULL;
    if (_consoleVram)
        free(_consoleVram);
    _consoleVram = NULL;
}

/* initialize dynamic allocations - common ---------------------------------- */
long Renderer::initCommon()
{
    // set playstation vram image
    _consoleVram = (unsigned char*)malloc(vramSize*2 + PSVRAM_SECURE_EXTRA*1024);
    if (_consoleVram == NULL)
        return -1;

    // initialize vram + set different access modes
    memset(_consoleVram, 0x00, vramSize*2 + PSVRAM_SECURE_EXTRA*1024);
    consoleVramByte = _consoleVram + PSVRAM_SECURE_OFFSET*1024;
    consoleVramWord = (unsigned short*)consoleVramByte;
    consoleVramDword = (unsigned long*)consoleVramByte;
    consoleVramEOM = consoleVramWord + vramSize; // end limit of memory
    
    // get language content
    Config* globalConfig = ConfigManager::getInstance();
    _langContent = LangMenu::getInstance(globalConfig->getLangCode());
    if (_langContent == NULL)
        return -1;
    
    // check resolution
    if (globalConfig->fullResX == 0 || globalConfig->fullResY == 0)
    {
        #if _SYSTEM == _WINDOWS
            globalConfig->fullResX = GetSystemMetrics(SM_CXSCREEN);
            globalConfig->fullResY = GetSystemMetrics(SM_CYSCREEN);
            if (globalConfig->fullResX < DISPLAY_BASEWIDTH 
             || globalConfig->fullResY < DISPLAY_BASEHEIGHT)
            {
                globalConfig->fullResX = 800;
                globalConfig->fullResY = 600;
            }
          #else
            globalConfig->fullResX = 800;
            globalConfig->fullResY = 600;
        #endif
    }
    
    gpuRawStatus = GPUSTATUS_INIT;  // idle, ready, disabled
    return 0;
}


// RENDERER PIPELINE CREATION
//______________________________________________________________________________

/* rendering pipeline preparation - common ---------------------------------- */
void Renderer::setPipelineCommon(bool bGpuApi)
{
    // set renderer fixes
    setRenderFixes();
    bVsyncReady  = true;
    bWriteUpload = false;
    
    #if _SYSTEM == _WINDOWS
        // adjust window style
        DWORD dwStyle = GetWindowLong(gpu_window, GWL_STYLE);
        _dwOriginalStyle = dwStyle; // save style backup to restore it
        if (ConfigManager::getInstance()->isFullscreen()) // fullscreen mode
        {
            if (bGpuApi)
                dwStyle = CS_OWNDC;
            else
                dwStyle &= ~(WS_THICKFRAME | WS_BORDER | WS_CAPTION);
        }
        else // window mode
        {
            dwStyle &= ~WS_THICKFRAME;
            if (bGpuApi)
                dwStyle |= (WS_BORDER | WS_CAPTION | CS_OWNDC);
            else
                dwStyle |= (WS_BORDER | WS_CAPTION);
        }
        SetWindowLong(gpu_window, GWL_STYLE, dwStyle);
        
        // hide emulator menu
        _hEmulatorMenu = GetMenu(gpu_window);
        if (_hEmulatorMenu) 
            SetMenu(gpu_window, NULL);
    #endif
    
    // disable screensaver
    if(ConfigManager::getInstance()->bDisableScreensaver)
        setScreenSaver(false);
    
    //! bSetClip=TRUE;//opengl
}

/* rendering pipeline removal - common -------------------------------------- */
void Renderer::unsetPipelineCommon()
{
    #if _SYSTEM == _WINDOWS
        // restore window style
        SetWindowLong(gpu_window, GWL_STYLE, _dwOriginalStyle); 
        
        // restore emulator menu
        if(_hEmulatorMenu)
            SetMenu(gpu_window, _hEmulatorMenu);
        _hEmulatorMenu = NULL;
    #endif
    
    // re-enable screensaver
    if(ConfigManager::getInstance()->bDisableScreensaver)
        setScreenSaver(true);
}

/* enable/disable screensaver ----------------------------------------------- */
void Renderer::setScreenSaver(bool bState)
{
    #if _SYSTEM == _WINDOWS
        // dynamic thread state system call
        EXECUTION_STATE (WINAPI *pSetThreadState)(EXECUTION_STATE esFlags);
        HINSTANCE hLibKernel32 = NULL;

        // load the kernel32.dll library
        hLibKernel32 = LoadLibrary(L"kernel32.dll");
        if(hLibKernel32 != NULL)
        {
            pSetThreadState = (EXECUTION_STATE (WINAPI *)(EXECUTION_STATE))GetProcAddress(hLibKernel32, "SetThreadExecutionState");
            if(pSetThreadState != NULL)
            {
                // set screensaver
                if(bState)
                    pSetThreadState(ES_SYSTEM_REQUIRED|ES_DISPLAY_REQUIRED);
                else
                    pSetThreadState(ES_SYSTEM_REQUIRED|ES_DISPLAY_REQUIRED|ES_CONTINUOUS);
            }
            FreeLibrary(hLibKernel32);
        }
    #endif
}

/* set renderer special fixes ----------------------------------------------- */
void Renderer::setRenderFixes()
{
    consoleDisplay.setDisplayFixes((ConfigManager::getInstance()->bugFixes&FIX_DISPWIDTHS) != 0);
}


// STATUS REGISTER + DATA IO
//______________________________________________________________________________

/* reset renderer default values -------------------------------------------- */
void Renderer::resetRenderer()
{
    outputMode = inputMode = DR_NORMAL;
    consoleDisplay.resetDisplay();
    
    gpuRawStatus = GPUSTATUS_INIT;
    memset(&gpuRawInfo, 0, PSRAWINFO_SIZE*sizeof(unsigned long));
}

/* set data transfer mode : normal or Vram DMA ------------------------------ */
void Renderer::setTransferMode(unsigned long gdata)
{
    // keep only transfer mode bits (2 bits)
    gdata &= 0x03;
    
    // set IO modes
    outputMode = inputMode = DR_NORMAL;
    if (gdata&0x02) // bit 2
    {
        outputMode = DR_VRAMTRANSFER;
        if (gdata == 0x03) // bits 1 + 2
            inputMode = DR_VRAMTRANSFER;
    }
    
    // set DMA bits
    unsetGpuStatus(gpuRawStatus, GPUSTATUS_DMABITS);
    setGpuStatus(gpuRawStatus, (gdata<<29));
}

/* enable/disable display --------------------------------------------------- */
void Renderer::setDisplayState(unsigned long gdata)
{
    // set display status
    bool displayStatus = ((gdata&0x01) != 0);
    if (displayStatus == false)
        setGpuStatus(gpuRawStatus, GPUSTATUS_DISPLAYDISABLED);
    else
        unsetGpuStatus(gpuRawStatus, GPUSTATUS_DISPLAYDISABLED);

    // prepare screen upload
    if (consoleDisplay.isBeingEnabled(displayStatus) /*&& iOffscreenDrawing==4*/)//opengl
    {
        if(consoleDisplay.gameColorDepth == DISPLAY_RGB15)
        {
            //! PrepareFullScreenUpload(TRUE);
            //! UploadScreen(TRUE); 
            //render();
        }
    }
}

/* set display information -------------------------------------------------- */
void Renderer::setDisplayInfo(unsigned long gdata)
{
    consoleDisplay.setDisplayInfo(gdata);

    // set status width bits
    unsetGpuStatus(gpuRawStatus, GPUSTATUS_WIDTHBITS);
    setGpuStatus(gpuRawStatus, (((gdata&0x03)<<17) | ((gdata&0x40)<<10)));
    
    // interlacing status
    if(consoleDisplay.gameInterlacing)
        setGpuStatus(gpuRawStatus, GPUSTATUS_INTERLACED);
    else 
        unsetGpuStatus(gpuRawStatus, GPUSTATUS_INTERLACED);
    // game localization status
    if (consoleDisplay.gameLocalization == DISPLAY_PAL)
        setGpuStatus(gpuRawStatus, GPUSTATUS_PAL);
    else 
        unsetGpuStatus(gpuRawStatus, GPUSTATUS_PAL);
    // height multiplier status
    if (consoleDisplay.multiplier == 2)
        setGpuStatus(gpuRawStatus, GPUSTATUS_DOUBLEHEIGHT);
    else 
        unsetGpuStatus(gpuRawStatus, GPUSTATUS_DOUBLEHEIGHT);
    // color depth status
    if (consoleDisplay.gameColorDepth == DISPLAY_RGB24)
        setGpuStatus(gpuRawStatus, GPUSTATUS_RGB24);
    else 
        unsetGpuStatus(gpuRawStatus, GPUSTATUS_RGB24);

    // update display
    if (ConfigManager::getInstance()->bFramerateLimitAuto) // framerate auto-detect
        Framerate::setFramerateConfig(consoleDisplay.gameLocalization, 
                                      consoleDisplay.gameInterlacing);
    //! updateDisplayIfChanged();
}

/* set display position ----------------------------------------------------- */
void Renderer::setDisplayPosition(unsigned long gdata)
{
    consoleDisplay.setDisplayPosition(gdata, vramSize);
    bVsyncReady = true;
    
    if (consoleDisplay.gameInterlacing == false)
    {
        if (Framerate::framesToSkip > 0) // if currently skipping, update display
            render();
    }
}

/* process GPU info request ------------------------------------------------- */
void Renderer::processRequest(unsigned long gdata)
{
    // keep only request bits (8 bits)
    gdata &= 0xFF;
    
    // process request
    switch (gdata)
    {
        // standard info requests
        case REQ_TW:          gpuRawData = gpuRawInfo[INFO_TW]; return;
        case REQ_DRAWSTART:   gpuRawData = gpuRawInfo[INFO_DRAWSTART]; return;
        case REQ_DRAWEND:     gpuRawData = gpuRawInfo[INFO_DRAWEND]; return;
        case REQ_DRAWOFFSET1:
        case REQ_DRAWOFFSET2: gpuRawData = gpuRawInfo[INFO_DRAWOFF]; return;
        
        // special requests
        case REQ_GPUTYPE:     if (zincGPUVersion == 2)
                                  gpuRawData = 0x01;
                              else
                                  gpuRawData = 0x02;
                              return;
        case REQ_BIOSADDR1:
        case REQ_BIOSADDR2:   gpuRawData = GPUBIOSADDR; return;
    }
}


/* read data input for memory ----------------------------------------------- */
void Renderer::readDataInput(unsigned long * pMem, int size)
{
    // check vram cursor limits
    while (consoleVramInput.vramCursor >= consoleVramEOM) // max position
        consoleVramInput.vramCursor -= vramSize;
    while (consoleVramInput.vramCursor < consoleVramWord) // min position
        consoleVramInput.vramCursor += vramSize;
    
    // read memory chunk of data
    int i = 0;
    bool bitLevel = false;
    if (consoleVramInput.remainingCols > 0 && consoleVramInput.remainingRows > 0)
    {
        while (i < size) // check if end of memory chunk
        {
            if (bitLevel == false) // read lower 16 bits
            {
                gpuRawData = (unsigned long)*consoleVramInput.vramCursor;
            }
            else // read higher 16 bits
            {
                gpuRawData |= (unsigned long)(*consoleVramInput.vramCursor)<<16;
                // update pointed memory
                *pMem = gpuRawData;
                pMem++; // increment memory location
                
                if (consoleVramInput.remainingCols <= 0) // check last column
                    break;
                i++;
            }
            
            // update cursor to higher bits position
            consoleVramInput.vramCursor++;
            if (consoleVramInput.vramCursor >= consoleVramEOM) // check max position
                consoleVramInput.vramCursor -= vramSize;
            consoleVramInput.remainingRows--;
            
            // if column is empty, check next column
            if (consoleVramInput.remainingRows <= 0)
            {
                consoleVramInput.remainingRows = consoleVramInput.area.width; // reset rows
                consoleVramInput.vramCursor += (1024 - consoleVramInput.area.width);
                if (consoleVramInput.vramCursor >= consoleVramEOM) // check max position
                    consoleVramInput.vramCursor -= vramSize;
                    
                consoleVramInput.remainingCols--;
                if (bitLevel && consoleVramInput.remainingCols <= 0) // check last column
                    break;
            }
            
            bitLevel = !bitLevel; // toggle low/high bits
        }
    }
    
    // if no columns remaining (transfer didn't end because of mem chunk size)
    if (i < size)
    {
        // reset transfer mode and values
        inputMode = DR_NORMAL;
        consoleVramInput.area.x        = consoleVramInput.area.y        = 0;
        consoleVramInput.area.width    = consoleVramInput.area.height   = 0;
        consoleVramInput.remainingRows = consoleVramInput.remainingCols = 0;
        
        // signal VRAM transfer end
        unsetGpuStatus(gpuRawStatus, GPUSTATUS_READYFORVRAM); // no longer ready
    }
}


/* write data output from memory -------------------------------------------- */
void Renderer::writeDataOutput(unsigned long * pMem, int size)
{
    unsigned long gdata = 0;
    bool bitLevel;
    int i = 0;
    
    do
    {
        // write memory chunk of data
        if (outputMode == DR_VRAMTRANSFER)
        {
            // check vram cursor limits
            while (consoleVramOutput.vramCursor >= consoleVramEOM) // max position
                consoleVramOutput.vramCursor -= vramSize;
            while (consoleVramOutput.vramCursor < consoleVramWord) // min position
                consoleVramOutput.vramCursor += vramSize;
                
            bool bWriteComplete = false; // for vsync
            bitLevel = false;
            while (i < size) // check if end of memory chunk
            {
                if (bitLevel == false)
                {
                    // update data source
                    gdata = *pMem; // read pointed memory
                    pMem++; // increment memory location
                    i++;
                
                    *consoleVramOutput.vramCursor = (unsigned short)gdata; // write lower bits
                }
                else
                {
                    *consoleVramOutput.vramCursor = (unsigned short)(gdata>>16); // write higher bits
                }
                
                // update vram cursor position
                consoleVramOutput.vramCursor++;
                if (consoleVramOutput.vramCursor >= consoleVramEOM) // check max position
                    consoleVramOutput.vramCursor -= vramSize;
                consoleVramOutput.remainingRows--;

                // if column is empty, check next column
                if (consoleVramOutput.remainingRows <= 0)
                {
                    consoleVramOutput.remainingCols--;
                    if (bitLevel == false)
                    {
                        if (consoleVramOutput.remainingCols <= 0) // last pixel is odd width
                        {
                            gdata = (gdata&DOUBLEBYTES_MASK) | (((unsigned long)*consoleVramOutput.vramCursor)<<16);
                            bWriteComplete = true;
                            break;
                        }
                        consoleVramOutput.remainingRows = consoleVramOutput.area.width;
                        consoleVramOutput.vramCursor += (1024 - consoleVramOutput.area.width);
                    }
                    else
                    {
                        consoleVramOutput.remainingRows = consoleVramOutput.area.width;
                        consoleVramOutput.vramCursor += (1024 - consoleVramOutput.area.width);
                        bWriteComplete = true;
                        if (consoleVramOutput.remainingCols <= 0)
                            break;
                    }
                }
                bitLevel = !bitLevel; // toggle low/high bits
            }
            
            // if no columns remaining (transfer didn't end because of mem chunk size)
            if (i < size)
            {
                if(bWriteUpload)
                {
                    bWriteUpload = false;
                    //CheckWriteUpdate();//opengl
                }

                // reset transfer mode and values
                outputMode = DR_NORMAL;
                consoleVramOutput.area.x        = consoleVramOutput.area.y        = 0;
                consoleVramOutput.area.width    = consoleVramOutput.area.height   = 0;
                consoleVramOutput.remainingCols = consoleVramOutput.remainingRows = 0;
                
                // if complete, set ready for vsync
                if(bWriteComplete) 
                    bVsyncReady = true;
            }
        }
    }
    while(writeNormalData(pMem, size, &gdata, &i)); //true = restart vram transfer
    gpuRawData = gdata;
}

/* write data in normal mode ------------------------------------------------ */
bool Renderer::writeNormalData(unsigned long* pMem, int size, unsigned long* pGdataDest, int* pCounter)
{
    unsigned long gdata = 0;
    unsigned char command;
    int i = *pCounter;
    
    //...
    
    if (outputMode == DR_NORMAL) // check normal output
    {
        //...
        
        while (i < size)
        {
            if (outputMode == DR_VRAMTRANSFER)
            {
                *pGdataDest = gdata;
                *pCounter = i;
                return true;
            }
            
            gdata = *pMem;
            pMem++;
            i++;
            
            //...
        }
    }
    *pGdataDest = gdata;
    return false;
}


// FRAME RENDERING
//______________________________________________________________________________

/* display window base background color ------------------------------------- */
long Renderer::displayBaseBackground()
{
    #if _SYSTEM == _WINDOWS
        RECT winRect;
        
        // create window-sized rectangle
        winRect.left  = winRect.top = 0;
        if (ConfigManager::getInstance()->isFullscreen()) // fullscreen size
        {
            winRect.right = ConfigManager::getInstance()->fullResX;
            winRect.right = ConfigManager::getInstance()->fullResY;
        }
        else // window size
        {
            winRect.right = ConfigManager::getInstance()->windowResX;
            winRect.right = ConfigManager::getInstance()->windowResY;
        }
        
        // display black rectangle
        HDC hDC = GetDC(gpu_window);
        if (hDC == NULL)
            return -1;
        FillRect(hDC, &winRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        
        // set pixel format
        long code = setPixelFormat(hDC);
        ReleaseDC(gpu_window, hDC);
        return code;
        
      #else
        return setPixelFormat();
    #endif
}
