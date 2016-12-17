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
#include "timer.h"
#include "config.h"
#include "status_register.h"
#include "memory_dispatcher.h"
#define This MemoryDispatcher

// video memory management
VideoMemory  MemoryDispatcher::mem_vram;       // emulated console video memory
memoryload_t MemoryDispatcher::mem_vramReader; // output memory load
memoryload_t MemoryDispatcher::mem_vramWriter; // input memory load
unsigned long MemoryDispatcher::mem_dataExchangeBuffer = GPUDATA_INIT; // data buffer read/written by emulator

// execution and display status
DisplayState MemoryDispatcher::st_displayState;
unsigned long MemoryDispatcher::st_pControlReg[CTRLREG_SIZE]; // GPU status control
bool MemoryDispatcher::st_isFirstOpen = true;      // first call to GPUopen()
bool MemoryDispatcher::st_isUploadPending = false; // image needs to be uploaded to VRAM
long MemoryDispatcher::st_busyEmuSequence = 0L;    // 'GPU busy' emulation hack - sequence value
long MemoryDispatcher::st_selectedSaveSlot = 0L;   // selected save-state slot

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
            This::st_displayState.toggleOddFrameFlag();
        }
    }
    // fake busy status fix (while drawing)
    This::setFakeBusyStep();

    // read status register
    return StatusRegister::getStatusRegister();
}

/// <summary>Process data sent to GPU status register</summary>
/// <param name="gdata">Status register command</param>
void CALLBACK GPUwriteStatus(unsigned long gdata)
{
     // get command indicator
    ubuffer_t command = This::extractGpuCommandType((ubuffer_t)gdata);
    This::st_pControlReg[command] = gdata; // store command (for save-states)

    switch (command)
    {
        // set VRAM transfer mode
        case CMD_SETTRANSFERMODE: 
            This::setDataTransferMode((ubuffer_t)gdata); return;
        // check GPU information (version, draw info, ...)
        case CMD_GPUREQUESTINFO: 
            This::mem_dataExchangeBuffer = (unsigned long)This::st_displayState.getDrawInfo((ubuffer_t)gdata); return;
        // set display
        case CMD_TOGGLEDISPLAY: 
            This::st_displayState.toggleDisplay((gdata & 0x1) != 0); return;
        case CMD_SETDISPLAYPOSITION: 
            This::st_displayState.setDisplayPos(This::extractSmallPos((ubuffer_t)gdata, true)); return;
        case CMD_SETDISPLAYWIDTH:  
            This::st_displayState.setWidth(This::extractPos((ubuffer_t)gdata)); return;
        case CMD_SETDISPLAYHEIGHT: 
            This::st_displayState.setHeight(This::extractSmallPos((ubuffer_t)gdata, false)); return;
        case CMD_SETDISPLAYINFO:
        {
            This::st_displayState.setDisplayState((ubuffer_t)gdata);
            if (Config::sync_framerateLimit <= 0.02f) // == 0.0 (+ float error offset) = auto-detect
                This::setFrameRate();
            
            //... update display if changed //!
            return;
        }
        // reset GPU info
        case CMD_RESETGPU:
            This::reset(); return;
    }
}


// -- DATA TRANSFER -- ---------------------------------------------------------

/// <summary>Get data transfer mode</summary>
/// <returns>Image transfer mode</returns>
long CALLBACK GPUgetMode()
{
    long imageTransfer = 0L;
    if (This::mem_vramWriter.mode == Loadmode_vramTransfer)
        imageTransfer |= 0x1;
    if (This::mem_vramReader.mode == Loadmode_vramTransfer)
        imageTransfer |= 0x2;
    return imageTransfer;
}
/// <summary>Set data transfer mode (deprecated)</summary>
/// <param name="gdataMode">Image transfer mode</param>
void CALLBACK GPUsetMode(unsigned long gdataMode)
{
    // This::mem_vramWriter.mode = (gdataMode&0x1) ? Loadmode_vramTransfer : Loadmode_normal;
    // This::mem_vramReader.mode = (gdataMode&0x2) ? Loadmode_vramTransfer : Loadmode_normal;
}

/// <summary>Read data from video memory (vram)</summary>
/// <returns>Raw GPU data</returns>
unsigned long CALLBACK GPUreadData()
{
    unsigned long gdata;
    GPUreadDataMem(&gdata, 1);
    return This::mem_dataExchangeBuffer;
}
/// <summary>Process and send data to video data register</summary>
/// <param name="gdata">Written data</param>
void CALLBACK GPUwriteData(unsigned long gdata)
{
    GPUwriteDataMem(&gdata, 1);
}

/// <summary>Direct core memory chain transfer to GPU driver</summary>
/// <param name="pDwBaseAddress">Pointer to memory chain</param>
/// <param name="offset">Memory offset</param>
/// <returns>Success indicator</returns>
long CALLBACK GPUdmaChain(unsigned long* pDwBaseAddress, unsigned long offset)
{
    unsigned char* pByteBaseAddress = (unsigned char*)pDwBaseAddress;
    uint32_t dmaCommandCounter = 0u;
    StatusRegister::unsetStatus(GPUSTATUS_IDLE); // busy

    // direct memory access loop
    short curCount;
    unsigned long dmaOffset;
    This::mem_vram.resetDmaCheck();
    do
    {
        // prevent out of range memory access
        if (This::mem_vram.isDoubledSize() == false) // not Zinc emu
            offset &= PSXVRAM_MASK;
        if (dmaCommandCounter++ > PSXVRAM_THRESHOLD || This::mem_vram.checkDmaEndlessChain(offset))
            break;

        // read and process data (if not empty)
        curCount = pByteBaseAddress[(sizeof(offset) - 1u) + offset];
        dmaOffset = offset / sizeof(offset); // convert address to dword array index
        if (curCount > 0)
            GPUwriteDataMem(&pDwBaseAddress[1uL + dmaOffset], curCount);

        offset = pDwBaseAddress[dmaOffset] & THREEBYTES_MASK; // follow chain
    } while (offset != THREEBYTES_MASK);

    StatusRegister::setStatus(GPUSTATUS_IDLE); // idle
    return PSE_GPU_SUCCESS;
}

/// <summary>Read entire chunk of data from video memory (vram)</summary>
/// <param name="pDwMem">Pointer to chunk of data (destination)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUreadDataMem(unsigned long* pDwMem, int size)
{
    StatusRegister::unsetStatus(GPUSTATUS_IDLE); // busy

    // check/adjust vram reader position
    while (This::mem_vramReader.vramPos.getPos() >= This::mem_vram.end()) // max position
        This::mem_vramReader.vramPos -= This::mem_vram.size();
    while (This::mem_vramReader.vramPos.getPos() < This::mem_vram.rend()) // min position
        This::mem_vramReader.vramPos += This::mem_vram.size();

    // ... check opengl drawing status //!

    // read memory chunk of data
    int i = 0;
    bool bitLevel = false;
    if (This::mem_vramReader.colsRemaining > 0 && This::mem_vramReader.rowsRemaining > 0)
    {
        while (i < size) // check if end of memory chunk
        {
            // 2 separate 16 bits reads (compatibility: wrap issues)
            if (bitLevel == false) // read lower 16 bits
            {
                This::mem_dataExchangeBuffer = (unsigned long)(This::mem_vramReader.vramPos.getValue());
            }
            else // read higher 16 bits
            {
                This::mem_dataExchangeBuffer |= (unsigned long)(This::mem_vramReader.vramPos.getValue()) << 16;
                // update pointed memory
                *pDwMem = This::mem_dataExchangeBuffer;
                pDwMem++; // increment memory location

                if (This::mem_vramReader.colsRemaining <= 0) // check last column
                    break;
                i++;
            }

            // update cursor to higher bits position
            ++This::mem_vramReader.vramPos;
            if (This::mem_vramReader.vramPos.getPos() >= This::mem_vram.end()) // check max position
                This::mem_vramReader.vramPos -= This::mem_vram.size();
            This::mem_vramReader.rowsRemaining--;

            // if column is empty, check next column
            if (This::mem_vramReader.rowsRemaining <= 0)
            {
                This::mem_vramReader.rowsRemaining = This::mem_vramReader.range.width; // reset rows
                This::mem_vramReader.vramPos += (1024 - This::mem_vramReader.range.width);
                if (This::mem_vramReader.vramPos.getPos() >= This::mem_vram.end()) // check max position
                    This::mem_vramReader.vramPos -= This::mem_vram.size();

                This::mem_vramReader.colsRemaining--;
                if (bitLevel && This::mem_vramReader.colsRemaining <= 0) // high bits -> check if last column
                    break;
            }
            bitLevel = !bitLevel; // alternate low/high bits
        }
    }

    // if no columns remaining (transfer <= mem chunk size)
    if (i < size)
    {
        // reset transfer mode and values
        This::mem_vramReader.mode = Loadmode_normal;
        This::mem_vramReader.range.x = This::mem_vramReader.range.y = 0;
        This::mem_vramReader.range.width = This::mem_vramReader.range.height = 0;
        This::mem_vramReader.rowsRemaining = This::mem_vramReader.colsRemaining = 0;
        // signal VRAM transfer end
        StatusRegister::unsetStatus(GPUSTATUS_READYFORVRAM); // no longer ready
    }
    StatusRegister::setStatus(GPUSTATUS_IDLE); // idle
}

/// <summary>Process and send chunk of data to video data register</summary>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUwriteDataMem(unsigned long* pDwMem, int size)
{
    //... //!
}


// -- LOAD/SAVE MEMORY STATE -- ------------------------------------------------

/// <summary>Save/load current state</summary>
/// <param name="dataMode">Transaction type (0 = setter / 1 = getter / 2 = slot selection)</param>
/// <param name="pMem">Save-state structure pointer (to read or write)</param>
/// <returns>Success/compatibility indicator</returns>
long CALLBACK GPUfreeze(unsigned long dataMode, GPUFreeze_t* pMem)
{
    switch (dataMode)
    {
        // select save slot (for display)
        case SAVESTATE_INFO:
        {
            long slotNumber = *((long *)pMem);
            if (slotNumber < 0 || slotNumber > 8)
                return GPUFREEZE_ERR;

            This::st_selectedSaveSlot = slotNumber + 1;
            return GPUFREEZE_SUCCESS;
        }
        // save state
        case SAVESTATE_SAVE:
        {
            if (pMem == NULL || pMem->freezeVersion != 1) // check version
                return GPUFREEZE_ERR;

            // copy status into destination
            pMem->status = (unsigned long)StatusRegister::getStatusRegister();
            memcpy(pMem->pControlReg, This::st_pControlReg, CTRLREG_SIZE*sizeof(unsigned long));

            // copy data into destination memory
            #if _ARCHITECTURE == ARCHITECTURE_64
            int vramLen = This::mem_vram.size() * 2;
            uint8_t* pVram = This::mem_vram.get();
            unsigned char* pVramCopy = pMem->pPsxVram;
            for (int i = 0; i < vramLen; i++) // size of types may differ -> no memcpy
            {
                *pVramCopy = (unsigned char)*pVram;
                ++pVramCopy;
                ++pVram;
            }
            #else
            memcpy(pMem->pPsxVram, This::mem_vram.get(), This::mem_vram.size() * 2);
            #endif

            Timer::resetTimeReference(); // avoid frame skipping
            return GPUFREEZE_SUCCESS;
        }
        // load state
        case SAVESTATE_LOAD:
        {
            if (pMem == NULL || pMem->freezeVersion != 1) // check version
                return GPUFREEZE_ERR;

            // read status from source
            StatusRegister::setStatusRegister((uint32_t)pMem->status);
            memcpy(This::st_pControlReg, pMem->pControlReg, CTRLREG_SIZE*sizeof(unsigned long));

            // read data from source memory
            #if _ARCHITECTURE == ARCHITECTURE_64
            int vramLen = This::mem_vram.size() * 2;
            uint8_t* pVram = This::mem_vram.get();
            unsigned char* pVramCopy = pMem->pPsxVram;
            for (int i = 0; i < vramLen; i++) // size of types may differ -> no memcpy
            {
                *pVram = (uint8_t)*pVramCopy;
                ++pVramCopy;
                ++pVram;
            }
            #else
            memcpy(This::mem_vram.get(), pMem->pPsxVram, This::mem_vram.size() * 2);
            #endif

            //... reset opengl texture area //!

            // update status based on new control data
            GPUwriteStatus(This::st_pControlReg[0]);
            GPUwriteStatus(This::st_pControlReg[1]);
            GPUwriteStatus(This::st_pControlReg[2]);
            GPUwriteStatus(This::st_pControlReg[3]);
            GPUwriteStatus(This::st_pControlReg[8]);
            GPUwriteStatus(This::st_pControlReg[6]);
            GPUwriteStatus(This::st_pControlReg[7]);
            GPUwriteStatus(This::st_pControlReg[5]);
            GPUwriteStatus(This::st_pControlReg[4]);
            Timer::resetTimeReference(); // avoid frame skipping
            return GPUFREEZE_SUCCESS;
        }
    }
    return GPUFREEZE_ERR;
}

/// <summary>Get VRAM access pointer</summary>
/// <param name="pPtrToMem">Destination pointer (for memory access)</param>
/// <returns>Memory size (bytes)</returns>
unsigned long CALLBACK GPUgetVramRef(uint8_t** pPtrToMem)
{
    *pPtrToMem = (uint8_t*)This::mem_vram.rend();
    return (This::mem_vram.isDoubledSize()) ? 2*VRAM_SIZE : VRAM_SIZE;
}
