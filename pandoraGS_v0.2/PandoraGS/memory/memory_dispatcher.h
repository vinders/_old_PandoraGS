/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   memory_dispatcher.h
Description : display memory manager and dispatcher
*******************************************************************************/
#ifndef _MEMORY_DISPATCHER_H
#define _MEMORY_DISPATCHER_H
#include "globals.h"
#include <cstddef>
#include <cstdint>
#include "video_memory.h"
#include "display_state.h"
#include "system_tools.h"
#include "geometry.hpp"

// data types
typedef uint8_t gpucmd_t;
typedef struct MEMLOAD // memory I/O
{
    loadmode_t mode;
    quad_t range;
    short  rowsRemaining;
    short  colsRemaining;
    VideoMemory::iterator vramPos;
    MEMLOAD()
    {
        mode = Loadmode_normal;
    }
} memoryload_t;
typedef struct GPUFREEZETAG // save-state structure
{
    unsigned long freezeVersion;    // system version = 1 (set by emulator)
    unsigned long status;           // current virtual GPU status
    unsigned long pControlReg[256]; // latest control register values
    unsigned char pPsxVram[1024*1024 * 2]; // current video memory image
} GPUFreeze_t;

// control register
#define CTRLREG_SIZE            256
// commands
#define CMD_RESETGPU            0x00
#define CMD_TOGGLEDISPLAY       0x03
#define CMD_SETTRANSFERMODE     0x04
#define CMD_SETDISPLAYPOSITION  0x05
#define CMD_SETDISPLAYWIDTH     0x06
#define CMD_SETDISPLAYHEIGHT    0x07
#define CMD_SETDISPLAYINFO      0x08
#define CMD_GPUREQUESTINFO      0x10


// Display memory manager and dispatcher
class MemoryDispatcher
{
public: // treat PSEmu memory functions as member methods

    // video memory management
    static VideoMemory  mem_vram;               // emulated console video memory
    static memoryload_t mem_vramReader;         // output memory load
    static memoryload_t mem_vramWriter;         // input memory load
    static buffer_t     mem_dataExchangeBuffer; // data buffer read/written by emulator

    // execution and display status
    static DisplayState st_displayState;
    static ubuffer_t st_pControlReg[CTRLREG_SIZE]; // GPU status control
    static bool st_isFirstOpen;      // first call to GPUopen()
    static bool st_isUploadPending;  // image needs to be uploaded to VRAM
    static long st_busyEmuSequence;  // 'GPU busy' emulation hack - sequence value
    static long s_selectedSaveSlot;  // selected save-state slot

    // gpu operations
    static gpucmd_t  gpu_command;
    static ubuffer_t gpu_dataM[256];
    static buffer_t  gpu_dataColor;
    static buffer_t  gpu_dataPoly;

#ifdef _WINDOWS
public:
    static HWND  s_hWindow;    // main emulator window handle
    static HMENU s_hMenu;      // emulator menu handle
    static DWORD s_origStyle;  // original window style
#endif


public:
    /// <summary>Initialize memory, status and dispatcher</summary>
    static inline void init()
    {
        // initialize control data
        st_displayState.init();
        mem_dataExchangeBuffer = GPUDATA_INIT;
        memset(st_pControlReg, 0x0, CTRLREG_SIZE * sizeof(unsigned long));
        // initialize VRAM
        mem_vram.init();
        memset(&mem_vramReader, 0x0, sizeof(memoryload_t)); // mode = Loadmode_normal = 0
        memset(&mem_vramWriter, 0x0, sizeof(memoryload_t)); // mode = Loadmode_normal = 0
        // initialize status
        StatusRegister::init();
        StatusRegister::setStatus(GPUSTATUS_IDLE | GPUSTATUS_READYFORCOMMANDS);
    }

    /// <summary>Reset GPU information</summary>
    static inline void reset()
    {
        StatusRegister::init();
        mem_vramReader.mode = Loadmode_normal;
        mem_vramWriter.mode = Loadmode_normal;
        st_displayState.reset();
    }

    /// <summary>Set fake busy/idle sequence step</summary>
    static inline void setFakeBusyStep()
    {
        // busy/idle sequence (while drawing)
        if (st_busyEmuSequence)
        {
            --st_busyEmuSequence;
            if (st_busyEmuSequence & 0x1) // busy if odd number
                StatusRegister::unsetStatus(GPUSTATUS_IDLE | GPUSTATUS_READYFORCOMMANDS);
            else // idle
                StatusRegister::setStatus(GPUSTATUS_IDLE | GPUSTATUS_READYFORCOMMANDS);
        }
    }

    /// <summary>Set VRAM data transfer mode</summary>
    /// <param name="gdata">Status command</param>
    static inline void setDataTransferMode(ubuffer_t gdata)
    {
        gdata &= 0x3; // extract last 2 bits (LSB+1, LSB)
        mem_vramWriter.mode = (gdata == 0x2) ? Loadmode_vramTransfer : Loadmode_normal;
        mem_vramReader.mode = (gdata == 0x3) ? Loadmode_vramTransfer : Loadmode_normal;
        // set direct memory access bits
        StatusRegister::unsetStatus(GPUSTATUS_DMABITS); // clear current DMA settings
        StatusRegister::setStatus(gdata << 29); // set DMA (MSB-1, MSB-2)
    }

    
    // -- COMMAND EXTRACTION -- ----------------------------------------------------

    /// <summary>Extract display command type ID from raw data</summary>
    /// <param name="raw">Display bits</param>
    /// <returns>Command identifier</returns>
    static inline ubuffer_t extractGpuCommandType(ubuffer_t raw)
    {
        return ((raw >> 24) & 0x0FF); // extract bits 25 and 26
    }
    /// <summary>Extract display position (X) from raw data</summary>
    /// <param name="raw">Display bits</param>
    /// <returns>Coordinates</returns>
    static inline point_t extractPos(buffer_t raw)
    {
        return point_t((short)(raw & 0x7FF), (short)((raw >> 12) & 0xFFF));
    }
    /// <summary>Extract small display position (Y) from raw data</summary>
    /// <param name="raw">Display bits</param>
    /// <param name="isZincSupport">Zinc emu support enabled</param>
    /// <returns>Coordinates</returns>
    static inline point_t extractSmallPos(buffer_t raw, bool isZincSupport)
    {
        if (isZincSupport && mem_vram.isDoubledSize() && st_displayState.version() == 2)
            return point_t((short)(raw & 0x3FF), (short)((raw >> 12) & 0x3FF));
        return point_t((short)(raw & 0x3FF), (short)((raw >> 10) & 0x3FF));
    }
};

// -- DISPLAY STATUS CONTROL -- ------------------------------------------------

/// <summary>Read data from GPU status register</summary>
/// <returns>GPU status register data</returns>
unsigned long CALLBACK GPUreadStatus();
/// <summary>Process data sent to GPU status register</summary>
/// <param name="gdata">Status register command</param>
void CALLBACK GPUwriteStatus(unsigned long gdata);


// -- DATA TRANSFER INTERFACE -- -----------------------------------------------

/// <summary>Read data from video memory (vram)</summary>
/// <returns>Raw GPU data</returns>
unsigned long CALLBACK GPUreadData();
/// <summary>Process and send data to video data register</summary>
/// <param name="gdata">Written data</param>
void CALLBACK GPUwriteData(unsigned long gdata);

/// <summary>Read entire chunk of data from video memory (vram)</summary>
/// <param name="pDwMem">Pointer to chunk of data (destination)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUreadDataMem(unsigned long* pDwMem, int size);
/// <summary>Process and send chunk of data to video data register</summary>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUwriteDataMem(unsigned long* pDwMem, int size);
/// <summary>Give a direct core memory access chain to GPU driver</summary>
/// <param name="pDwBaseAddress">Pointer to memory chain</param>
/// <param name="offset">Memory offset</param>
/// <returns>Success indicator</returns>
long CALLBACK GPUdmaChain(unsigned long* pDwBaseAddress, unsigned long offset);


// -- LOAD/SAVE MEMORY STATE -- ------------------------------------------------

/// <summary>Save/load current state</summary>
/// <param name="dataMode">Transaction type (0 = setter / 1 = getter / 2 = slot selection)</param>
/// <param name="pMem">Save-state structure pointer (to read or write)</param>
/// <returns>Success/compatibility indicator</returns>
long CALLBACK GPUfreeze(unsigned long dataMode, GPUFreeze_t* pMem);
/// <summary>Get VRAM access pointer</summary>
/// <param name="pPtrToMem">Destination pointer (for memory access)</param>
/// <returns>Memory size (bytes)</returns>
unsigned long CALLBACK GPUgetVramRef(uint8_t** pPtrToMem);

#endif
