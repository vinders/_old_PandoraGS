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
#include <string>
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

// save-states
#define SAVESTATE_LOAD          0u
#define SAVESTATE_SAVE          1u
#define SAVESTATE_INFO          2u
#define GPUFREEZE_SUCCESS       1
#define GPUFREEZE_ERR           0
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
    static unsigned long mem_dataExchangeBuffer; // data buffer read/written by emulator

    // execution and display status
    static DisplayState st_displayState;
    static unsigned long st_pControlReg[CTRLREG_SIZE]; // GPU status control
    static uint32_t st_displayDevFlags; // 00 -> digital, 01 -> analog, 02 -> mouse, 03 -> gun
    static bool st_isFirstOpen;      // first call to GPUopen()
    static bool st_isUploadPending;  // image needs to be uploaded to VRAM
    static long st_busyEmuSequence;  // 'GPU busy' emulation hack - sequence value
    static long st_selectedSaveSlot;  // selected save-state slot

    // gpu operation
    static gpucmd_t  s_gpuCommand;
    static ubuffer_t s_gpuMemCache[256]; // memory cache
    static long s_gpuDataCount;          // data set length
    static long s_gpuDataProcessed;      // current number of values cached

#ifdef _WINDOWS
public:
    static HWND  s_hWindow;    // main emulator window handle
    static HMENU s_hMenu;      // emulator menu handle
    static DWORD s_origStyle;  // original window style
#endif
    static bool s_isZincEmu;   // Zinc emulation


public:
    /// <summary>Initialize memory, status and dispatcher</summary>
    static inline void init()
    {
        st_isFirstOpen = true;
        // initialize control data
        st_displayState.init();
        mem_dataExchangeBuffer = GPUDATA_INIT;
        memset(st_pControlReg, 0x0, CTRLREG_SIZE * sizeof(unsigned long));
        st_displayDevFlags = 0u;
        // initialize VRAM
        mem_vram.init(s_isZincEmu);
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

    /// <summary>Close memory</summary>
    static inline void close()
    {
        mem_vram.close();
    }

    /// <summary>Display data summary in debug window</summary>
    static void printDebugSummary();
    /// <summary>Export full status and VRAM data</summary>
    static void MemoryDispatcher::exportData();

    /// <summary>Process and send chunk of display data (normal mode)</summary>
    /// <param name="pDwMem">Pointer to chunk of data (source)</param>
    /// <param name="size">Memory chunk size</param>
    /// <param name="pDest">Destination gdata reference</param>
    /// <param name="pI">Counter reference</param>
    /// <returns>Indicator if VRAM data to write</returns>
    static bool writeCachedDataChunk(unsigned long* pDwMem, int size, unsigned long* pDest, int* pI);


    // -- SET SYNC/TRANSFER INFORMATION -- -----------------------------------------

    /// <summary>Set VRAM data transfer mode</summary>
    /// <param name="gdata">Status command</param>
    static inline void setDataTransferMode(ubuffer_t gdata)
    {
        gdata &= 0x3u; // extract last 2 bits (LSB+1, LSB)
        mem_vramWriter.mode = (gdata == 0x2) ? Loadmode_vramTransfer : Loadmode_normal;
        mem_vramReader.mode = (gdata == 0x3) ? Loadmode_vramTransfer : Loadmode_normal;
        // set direct memory access bits
        StatusRegister::unsetStatus(GPUSTATUS_DMABITS); // clear current DMA settings
        StatusRegister::setStatus(gdata << 29); // set DMA (MSB-1, MSB-2)
    }

    /// <summary>Initialize time mode and frame rate</summary>
    static inline void initFrameRate()
    {
        Timer::setTimeMode((timemode_t)Config::sync_timeMode);
        if (Config::getCurrentProfile()->getFix(CFG_FIX_PC_FPSLIMIT))
            Timer::setFrequency(Config::sync_framerateLimit, Regionsync_undefined, st_displayState.isInterlaced());
        else
            Timer::setFrequency(Config::sync_framerateLimit, Regionsync_undefined, StatusRegister::getStatus(GPUSTATUS_INTERLACED));
    }
    /// <summary>Set frame rate with region info</summary>
    static inline void setFrameRate()
    {
        regionsync_t timerReg;
        if (Config::getCurrentProfile()->getFix(CFG_FIX_PC_FPSLIMIT))
        {
            timerReg = (st_displayState.getRegionmode() == Region_pal) ? Regionsync_pal_pcfix : Regionsync_ntsc_pcfix;
            Timer::setFrequency(Config::sync_framerateLimit, timerReg, st_displayState.isInterlaced());
        }
        else
        {
            timerReg = (st_displayState.getRegionmode() == Region_pal) ? Regionsync_pal : Regionsync_ntsc;
            Timer::setFrequency(Config::sync_framerateLimit, timerReg, StatusRegister::getStatus(GPUSTATUS_INTERLACED));
        }
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

    
    // -- COMMAND EXTRACTION -- ----------------------------------------------------

    /// <summary>Extract display command type ID from raw data</summary>
    /// <param name="raw">Display bits</param>
    /// <returns>Command identifier</returns>
    static inline ubuffer_t extractGpuCommandType(ubuffer_t raw)
    {
        return ((raw >> 24) & 0x0FFu); // extract bits 25 and 26
    }
    /// <summary>Extract display position (X) from raw data</summary>
    /// <param name="raw">Display bits</param>
    /// <returns>Coordinates</returns>
    static inline point_t extractPos(ubuffer_t raw)
    {
        uint32_t x = (raw & 0x7FFu);
        uint32_t y = ((raw >> 12) & 0xFFFu);
        return point_t((short)x, (short)y);
    }
    /// <summary>Extract small display position (Y) from raw data</summary>
    /// <param name="raw">Display bits</param>
    /// <param name="isZincSupport">Zinc emu support enabled</param>
    /// <returns>Coordinates</returns>
    static inline point_t extractSmallPos(ubuffer_t raw, bool isZincSupport)
    {
        uint32_t x = (raw & 0x3FFu);
        uint32_t y = ((raw >> 10) & 0x3FFu);
        if (isZincSupport && mem_vram.isDoubledSize() && st_displayState.version() == 2)
            y = ((raw >> 12) & 0x3FFu);
        return point_t((short)x, (short)y);
    }
};

// -- DISPLAY STATUS CONTROL -- ------------------------------------------------

/// <summary>Set special display flags</summary>
/// <param name="dwFlags">Display flags</param>
void CALLBACK GPUdisplayFlags(unsigned long dwFlags);

/// <summary>Read data from GPU status register</summary>
/// <returns>GPU status register data</returns>
unsigned long CALLBACK GPUreadStatus();
/// <summary>Process data sent to GPU status register</summary>
/// <param name="gdata">Status register command</param>
void CALLBACK GPUwriteStatus(unsigned long gdata);


// -- DATA TRANSFER -- ---------------------------------------------------------

/// <summary>Get data transfer mode</summary>
/// <returns>Image transfer mode</returns>
long CALLBACK GPUgetMode();
/// <summary>Set data transfer mode (deprecated)</summary>
/// <param name="gdataMode">Image transfer mode</param>
void CALLBACK GPUsetMode(unsigned long gdataMode);

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
/// <summary>Direct core memory chain transfer to GPU driver</summary>
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

#endif
