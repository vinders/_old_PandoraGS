/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   memory_dispatcher.cpp
Description : display memory manager and dispatcher
*******************************************************************************/
#include "globals.h"
using namespace std;
#include "config.h"
#include "status_register.h"
#include "memory_dispatcher.h"
#include "timer.h"

#define GPUFREEZE_SUCCESS   1
#define GPUFREEZE_ERR       0

// video memory management
VideoMemory  MemoryDispatcher::mem_vram;       // emulated console video memory
memoryload_t MemoryDispatcher::mem_vramReader; // output memory load
memoryload_t MemoryDispatcher::mem_vramWriter; // input memory load
buffer_t     MemoryDispatcher::mem_dataExchangeBuffer = GPUDATA_INIT; // data buffer read/written by emulator

// execution and display status
DisplayState MemoryDispatcher::st_displayState;
ubuffer_t MemoryDispatcher::st_pControlReg[CTRLREG_SIZE]; // GPU status control
bool MemoryDispatcher::st_isFirstOpen = true;      // first call to GPUopen()
bool MemoryDispatcher::st_isUploadPending = false; // image needs to be uploaded to VRAM
long MemoryDispatcher::st_busyEmuSequence = 0L;    // 'GPU busy' emulation hack - sequence value
long MemoryDispatcher::s_selectedSaveSlot = 0L;    // selected save-state slot

// gpu operations
gpucmd_t  MemoryDispatcher::gpu_command = 0u;
ubuffer_t MemoryDispatcher::gpu_dataM[256];
buffer_t  MemoryDispatcher::gpu_dataColor = 0;
buffer_t  MemoryDispatcher::gpu_dataPoly = 0;

#ifdef _WINDOWS
HWND  MemoryDispatcher::s_hWindow = NULL;   // main emulator window handle
HMENU MemoryDispatcher::s_hMenu = NULL;     // emulator menu handle
DWORD MemoryDispatcher::s_origStyle = 0uL;  // original window style
#endif


// -- DISPLAY STATUS CONTROL -- ------------------------------------------------

/// <summary>Read data from GPU status register</summary>
/// <returns>GPU status register data</returns>
unsigned long CALLBACK GPUreadStatus()
{
    // interlacing CC game fix
    if (Config::isProfileSet() && Config::getCurrentProfile()->getFix(CFG_FIX_STATUS_INTERLACE))
    {
        static int readCount = 0;
        if (++readCount >= 2) // interlace bit toggle - every second read
        {
            readCount = 0;
            MemoryDispatcher::st_displayState.toggleOddFrameFlag();
        }
    }
    // fake busy status fix (while drawing)
    MemoryDispatcher::setFakeBusyStep(); 

    // read status register
    return StatusRegister::getStatus();
}

/// <summary>Process data sent to GPU status register</summary>
/// <param name="gdata">Status register command</param>
void CALLBACK GPUwriteStatus(unsigned long gdata)
{
     // get command indicator
    ubuffer_t command = MemoryDispatcher::extractGpuCommandType((ubuffer_t)gdata);
    MemoryDispatcher::st_pControlReg[command] = gdata; // store command (for save-states)

    switch (command)
    {
        // set VRAM transfer mode
        case CMD_SETTRANSFERMODE: 
            MemoryDispatcher::setDataTransferMode((ubuffer_t)gdata); return;
        // check GPU information (version, draw info, ...)
        case CMD_GPUREQUESTINFO: 
            MemoryDispatcher::mem_dataExchangeBuffer = MemoryDispatcher::st_displayState.getDrawInfo((ubuffer_t)gdata); return;
        // set display
        case CMD_TOGGLEDISPLAY: 
            MemoryDispatcher::st_displayState.toggleDisplay((gdata & 0x1) != 0); return;
        case CMD_SETDISPLAYPOSITION: 
            MemoryDispatcher::st_displayState.setDisplayPos(MemoryDispatcher::extractSmallPos((buffer_t)gdata, true)); return;
        case CMD_SETDISPLAYWIDTH:  
            MemoryDispatcher::st_displayState.setWidth(MemoryDispatcher::extractPos((buffer_t)gdata)); return;
        case CMD_SETDISPLAYHEIGHT: 
            MemoryDispatcher::st_displayState.setHeight(MemoryDispatcher::extractSmallPos((buffer_t)gdata, false)); return;
        case CMD_SETDISPLAYINFO:
        {
            MemoryDispatcher::st_displayState.setDisplayState((ubuffer_t)gdata);
            if (Config::sync_framerateLimit <= 0.02f) // == 0.0 (+ float error offset) = auto-detect
                ;//Timer::setFrequency(/*...*/);
            //...
            return;
        }
        // reset GPU info
        case CMD_RESETGPU:
            MemoryDispatcher::reset(); return;
    }
}


// -- DATA TRANSFER INTERFACE -- -----------------------------------------------

/// <summary>Read data from video memory (vram)</summary>
/// <returns>Raw GPU data</returns>
unsigned long CALLBACK GPUreadData()
{

}
/// <summary>Process and send data to video data register</summary>
/// <param name="gdata">Written data</param>
void CALLBACK GPUwriteData(unsigned long gdata)
{

}

/// <summary>Read entire chunk of data from video memory (vram)</summary>
/// <param name="pDwMem">Pointer to chunk of data (destination)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUreadDataMem(unsigned long* pDwMem, int size)
{

}

/// <summary>Process and send chunk of data to video data register</summary>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUwriteDataMem(unsigned long* pDwMem, int size)
{

}

/// <summary>Give a direct core memory access chain to GPU driver</summary>
/// <param name="pDwBaseAddress">Pointer to memory chain</param>
/// <param name="offset">Memory offset</param>
/// <returns>Success indicator</returns>
long CALLBACK GPUdmaChain(unsigned long* pDwBaseAddress, unsigned long offset)
{

}


// -- LOAD/SAVE MEMORY STATE -- ------------------------------------------------

/// <summary>Save/load current state</summary>
/// <param name="dataMode">Transaction type (0 = setter / 1 = getter / 2 = slot selection)</param>
/// <param name="pMem">Save-state structure pointer (to read or write)</param>
/// <returns>Success/compatibility indicator</returns>
long CALLBACK GPUfreeze(unsigned long dataMode, GPUFreeze_t* pMem)
{

}

/// <summary>Get VRAM access pointer</summary>
/// <param name="pPtrToMem">Destination pointer (for memory access)</param>
/// <returns>Memory size (bytes)</returns>
unsigned long CALLBACK GPUgetVramRef(uint8_t** pPtrToMem)
{

}
