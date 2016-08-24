/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   gpu_zinc.cpp
Description : zinc specification interface
*******************************************************************************/
#include <string>
using namespace std;
#include "config.h"
#include "input_manager.h"
#include "gpu_main.h"
#include "core_memory.h"

// zinc configuration structure
typedef struct GPUOTAG
{
    unsigned long version;        // version of structure - currently 1
    long          hWindow;        // window handle
    unsigned long screenRotation; // 0 = 0CW, 1 = 90CW, 2 = 180CW, 3 = 270CW = 90CCW
    unsigned long gpuVersion;     // 0 = a, 1 = b, 2 = c
    const char*   pGameName;      // game title string
    const char*   pCfgFile;       // config filepath string
} GPUConfiguration_t;

// global data
uint32_t zincGPUVersion = 0u;
bool     g_isZincEmu = false;   // zinc interface emulation
int32_t  zincTileFix = 0;


// -- DRIVER INIT INTERFACE -- -------------------------------------------------

/// <summary>Driver init (called once)</summary>
/// <returns>Success indicator</returns>
long CALLBACK ZN_GPUinit() // we always set the vram size to 2MB, if the ZN interface is used
{
    g_isZincEmu = true;
    return GPUinit();
}
/// <summary>Driver shutdown (called once)</summary>
/// <returns>Success indicator</returns>
long CALLBACK ZN_GPUshutdown()
{
    return GPUshutdown();
}

/// <summary>Driver opening/reopening (game started)</summary>
/// <param name="pCfg">Configuration pointer</param>
/// <returns>Success indicator</returns>
long CALLBACK ZN_GPUopen(void* pCfg)
{
    GPUConfiguration_t* cfg = (GPUConfiguration_t *)pCfg;
    if (!cfg || cfg->version != 1)
        return -1L;

    zincTileFix = 1; // tile erase bug fix
    zincGPUVersion = cfg->gpuVersion;

    #ifdef _WINDOWS
    return GPUopen((HWND)cfg->hWindow);
    #else
    return GPUopen(&cfg->hWnd, cfg->GameName, cfg->CfgFile);
    #endif
}
/// <summary>Driver closed (game stopped)</summary>
/// <returns>Success indicator</returns>
long CALLBACK ZN_GPUclose()
{
    return GPUclose();
}

/// <summary>Activity update (called every vsync)</summary>
void CALLBACK ZN_GPUupdateLace()
{
    GPUupdateLace();
}


// -- GETTERS - SETTERS -- -----------------------------------------------------

/// <summary>Plugin - Load test</summary>
/// <returns>Success indicator (0 = ok, 1 = warning or -1 = error)</returns>
long CALLBACK ZN_GPUtest()
{
    return GPUtest();
}

/// <summary>Get PSemu transfer mode</summary>
/// <returns>Image transfer mode</returns>
long CALLBACK ZN_GPUgetMode()
{
    return GPUgetMode();
}
/// <summary>Set PSemu transfer mode (deprecated)</summary>
/// <param name="gdataMode">Image transfer mode</param>
void CALLBACK ZN_GPUsetMode(unsigned long gdataMode)
{
    GPUsetMode(gdataMode);
}

/// <summary>Set special display flags</summary>
/// <param name="dwFlags">Display flags</param>
void CALLBACK ZN_GPUdisplayFlags(unsigned long dwFlags)
{
    GPUdisplayFlags(dwFlags);
}


// -- STATUS REGISTER CONTROL -- -----------------------------------------------

/// <summary>Read data from GPU status register</summary>
/// <returns>GPU status register data</returns>
unsigned long CALLBACK ZN_GPUreadStatus()
{
    return GPUreadStatus();
}
/// <summary>Process data sent to GPU status register</summary>
/// <param name="gdata">Status register command</param>
void CALLBACK ZN_GPUwriteStatus(unsigned long gdata)
{
    GPUwriteStatus(gdata);
}


// -- DATA TRANSFER INTERFACE -- -----------------------------------------------

/// <summary>Read data from video memory (vram)</summary>
/// <returns>Raw GPU data</returns>
unsigned long CALLBACK ZN_GPUreadData()
{
    return GPUreadData();
}
/// <summary>Read entire chunk of data from video memory (vram)</summary>
/// <param name="pDwBaseAddress">Pointer to memory chain</param>
/// <param name="offset">Memory offset</param>
/// <param name="size">Memory chunk size</param>
long CALLBACK ZN_GPUdmaSliceOut(unsigned long* pDwBaseAddress, unsigned long offset, unsigned long size)
{
    GPUreadDataMem(pDwBaseAddress + offset, size);
    return 0L;
}

/// <summary>Process and send data to video data register</summary>
/// <param name="gdata">Written data</param>
void CALLBACK ZN_GPUwriteData(unsigned long gdata)
{
    GPUwriteDataMem(&gdata, 1);
}
/// <summary>Process and send chunk of data to video data register</summary>
/// <param name="pDwBaseAddress">Pointer to memory chain</param>
/// <param name="offset">Memory offset</param>
/// <param name="size">Memory chunk size</param>
long CALLBACK ZN_GPUdmaSliceIn(unsigned long* pDwBaseAddress, unsigned long offset, unsigned long size)
{
    GPUwriteDataMem(pDwBaseAddress + offset, size);
    return 0L;
}
/// <summary>Give a direct core memory access chain to GPU driver</summary>
/// <param name="pDwBaseAddress">Pointer to memory chain</param>
/// <param name="offset">Memory offset</param>
/// <returns>Success indicator</returns>
long CALLBACK ZN_GPUdmaChain(unsigned long* pDwBaseAddress, unsigned long offset)
{
    return GPUdmaChain(pDwBaseAddress, offset);
}


// -- SAVE-STATES - SNAPSHOTS -- -----------------------------------------------

/// <summary>Set custom fixes from emulator</summary>
/// <param name="dataMode">Transaction type (0 = setter / 1 = getter / 2 = slot selection)</param>
/// <param name="pMem">Save-state structure pointer (to read or write)</param>
/// <returns>Success/compatibility indicator</returns>
long CALLBACK ZN_GPUfreeze(unsigned long dataMode, void * pMem)
{
    GPUFreeze_t* pFreeze = (GPUFreeze_t*)pMem;
    return GPUfreeze(dataMode, pFreeze);
}

/// <summary>Request snapshot (on next display)</summary>
void CALLBACK ZN_GPUmakeSnapshot()
{
    GPUmakeSnapshot();
}
/// <summary>Get screen picture</summary>
/// <param name="pMem">allocated screen picture container 128x96 px (24b/px: 8-8-8 bit BGR, no header)</param>
void CALLBACK ZN_GPUgetScreenPic(unsigned char * pMem)
{
    GPUgetScreenPic(pMem);
}
/// <summary>Store and display screen picture</summary>
/// <param name="pMem">screen picture data 128x96 px (24b/px: 8-8-8 bit BGR, no header)</param>
void CALLBACK ZN_GPUshowScreenPic(unsigned char * pMem)
{
    GPUshowScreenPic(pMem);
}


// -- MISCELLANEOUS -- ---------------------------------------------------------

#ifndef _WINDOWS
void CALLBACK ZN_GPUkeypressed(int keycode)
{
    InputManager::GPUkeypressed(keycode);
}
#endif
