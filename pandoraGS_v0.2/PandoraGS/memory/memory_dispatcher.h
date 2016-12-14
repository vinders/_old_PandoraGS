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
#include "geometry.hpp"
#include "system_tools.h"

// data types
typedef uint8_t gpucmd_t;
typedef struct MEMLOAD // memory I/O
{
    loadmode_t mode;
    quad_t range;
    short  rowsRemaining;
    short  colsRemaining;
    VideoMemory::iterator vramPos;
} memoryload_t;

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
private:
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

    // gpu operations
    static gpucmd_t  gpu_command;
    static ubuffer_t gpu_dataM[256];
    static buffer_t  gpu_dataColor;
    static buffer_t  gpu_dataPoly;

public:
#ifdef _WINDOWS
    static HWND  s_hWindow;    // main emulator window handle
    static HMENU s_hMenu;      // emulator menu handle
    static DWORD s_origStyle;  // original window style
#endif


public:
    /// <summary>Initialize memory, status and dispatcher</summary>
    void init();

    /// <summary>Set fake busy/idle status sequence</summary>
    inline void fakeBusySequence();

    /// <summary>Set VRAM data transfer mode</summary>
    /// <param name="gdata">Status info</param>
    static inline void setDataTransferMode(ubuffer_t gdata);

    
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
    /// <param name="isZincSupport">Zinc alternative enabled</param>
    /// <returns>Coordinates</returns>
    static inline point_t extractSmallPos(buffer_t raw, bool isZincSupport)
    {
        if (isZincSupport && mem_vram.isDoubledSize() && mem_vram.version() == 2)
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

#endif
