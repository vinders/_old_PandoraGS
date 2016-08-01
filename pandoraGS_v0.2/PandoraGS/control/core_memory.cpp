/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   core_memory.cpp
Description : virtual video memory/registers unit
*******************************************************************************/
#include <string>
using namespace std;
#include "config.h"
#include "core_memory.h"
#include "framerate_manager.h"

extern bool g_isZincEmu; // zinc interface emulation
extern Config* g_pConfig;


#ifdef _WINDOWS
HWND  CoreMemory::gen_hWindow = NULL;        // main emulator window handle
HMENU CoreMemory::gen_hMenu = NULL;          // emulator menu handle
DWORD CoreMemory::gen_origStyle = 0uL;       // original window style
#endif

// psx emulated memory
int           CoreMemory::mem_gpuVramSize = PSXVRAM_SIZE;
PsxVram_t     CoreMemory::mem_vramImage = { 0,0,0,0,0,0,0,0 };   // PSX VRAM memory image
VramLoad_t    CoreMemory::mem_vramRead;                          // PSX VRAM frame reader
int           CoreMemory::mem_vramReadMode = DR_NORMAL;          // read transfer mode
VramLoad_t    CoreMemory::mem_vramWrite;                         // PSX VRAM frame writer
int           CoreMemory::mem_vramWriteMode = DR_NORMAL;         // write transfer mode
long          CoreMemory::mem_gpuDataTransaction = GPUDATA_INIT; // GPU data read/written by emulator
unsigned long CoreMemory::mem_gpuDmaAddresses[3];                // DMA address check
bool          CoreMemory::mem_isWriteUploadPending = false;      // image needs to be uploaded to VRAM

// gpu data memory
unsigned char CoreMemory::gpu_command = 0;
unsigned long CoreMemory::gpu_dataM[256];
long          CoreMemory::gpu_dataC = 0;
long          CoreMemory::gpu_dataP = 0;

// gpu emulated status and information
long          CoreMemory::st_statusReg;               // GPU status register
unsigned long CoreMemory::st_pStatusControl[STATUSCTRL_SIZE];   // GPU status control
unsigned long CoreMemory::st_pGpuDrawInfo[DRAWINFO_SIZE];       // GPU draw information
long          CoreMemory::st_selectedSlot = 0L;                 // save-state selected slot
int           CoreMemory::st_fixBusyEmuSequence = 0;  // 'GPU busy' emulation hack - sequence value

// display settings
bool CoreMemory::dsp_isDisplaySet = false;
DisplayState_t CoreMemory::dsp_displayState;          // display information
short CoreMemory::dsp_displayWidths[8] = { 256, 320, 512, 640, 368, 384, 512, 640 }; // native display widths
unsigned long CoreMemory::dsp_displayFlags = 0;       // 00 -> digital, 01 -> analog, 02 -> mouse, 03 -> gun
int CoreMemory::dsp_firstPositionFlag = 2;            // indicates first position drawing after startup


/// <summary>Initialize memory instance values</summary>
/// <exception cref="std::exception">Memory allocation failure</exception>
void CoreMemory::initMemory()
{
    // data transaction init
    mem_vramImage.pData = NULL;
    mem_gpuDataTransaction = GPUDATA_INIT;
    initDisplayState(dsp_displayState);
    resetDmaCheck();

    // define VRAM size (standard or zinc emu)
    mem_gpuVramSize = (g_isZincEmu) ? ZINCVRAM_SIZE : PSXVRAM_SIZE;
    mem_vramImage.bufferSize = mem_gpuVramSize * 1024;
    // alloc double buffered PSX VRAM image
    mem_vramImage.length = (mem_vramImage.bufferSize * 2) + (PSXVRAM_SECURE_EXTRA * 1024); // extra security for drawing functions
    if ((mem_vramImage.pData = (unsigned char*)malloc(mem_vramImage.length)) == NULL)
        throw new std::exception("VRAM allocation failure");

    // initialize VRAM
    memset(mem_vramImage.pData, 0x0, mem_vramImage.length);
    mem_vramImage.oddFrame = 0;
    mem_vramImage.pByte = mem_vramImage.pData + (PSXVRAM_SECURE_OFFSET * 1024); // start limit
    mem_vramImage.pWord = (unsigned short*)mem_vramImage.pByte;
    mem_vramImage.pDword = (unsigned long*)mem_vramImage.pByte;
    mem_vramImage.pEOM = mem_vramImage.pWord + mem_vramImage.bufferSize; // end limit

    // initialize VRAM load
    mem_vramReadMode = DR_NORMAL;
    memset(&mem_vramRead, 0x0, sizeof(VramLoad_t));
    mem_vramWriteMode = DR_NORMAL;
    memset(&mem_vramWrite, 0x0, sizeof(VramLoad_t));

    // initialize status and information
    memset(st_pGpuDrawInfo, 0x0, DRAWINFO_SIZE * sizeof(unsigned long));
    memset(st_pStatusControl, 0x0, STATUSCTRL_SIZE * sizeof(unsigned long));
    st_statusReg = GPUSTATUS_INIT;
    setStatus(GPUSTATUS_IDLE);
    setStatus(GPUSTATUS_READYFORCOMMANDS);
}

/// <summary>Close memory objects</summary>
void CoreMemory::closeMemory()
{
    if (mem_vramImage.pData != NULL)
        free(mem_vramImage.pData);
    mem_vramImage.pData = NULL;
}


// -- MEMORY IO -- -------------------------------------------------------------

/// <summary>Read entire chunk of data from video memory (vram)</summary>
/// <param name="pDwMem">Pointer to chunk of data (destination)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUreadDataMem(unsigned long* pDwMem, int size)
{
    CoreMemory::unsetStatus(GPUSTATUS_IDLE); // busy

    // check/adjust vram reader position
    while (CoreMemory::mem_vramRead.pVramImage >= CoreMemory::mem_vramImage.pEOM) // max position
        CoreMemory::mem_vramRead.pVramImage -= CoreMemory::mem_vramImage.bufferSize;
    while (CoreMemory::mem_vramRead.pVramImage < CoreMemory::mem_vramImage.pWord) // min position
        CoreMemory::mem_vramRead.pVramImage += CoreMemory::mem_vramImage.bufferSize;

    //! //opengl
    if((iFrameReadType&1 && iSize>1) &&
        !(iDrawnSomething==2 &&
        VRAMRead.x      == VRAMWrite.x     &&
        VRAMRead.y      == VRAMWrite.y     &&
        VRAMRead.Width  == VRAMWrite.Width &&
        VRAMRead.Height == VRAMWrite.Height))
            CheckVRamRead(VRAMRead.x,VRAMRead.y,
                VRAMRead.x+VRAMRead.RowsRemaining,
                VRAMRead.y+VRAMRead.ColsRemaining,
                TRUE);//!

    // read memory chunk of data
    int i = 0;
    bool bitLevel = false;
    if (CoreMemory::mem_vramRead.colsRemaining > 0 && CoreMemory::mem_vramRead.rowsRemaining > 0)
    {
        while (i < size) // check if end of memory chunk
        {
            // 2 separate 16 bits reads (compatibility: wrap issues)
            if (bitLevel == false) // read lower 16 bits
            {
                CoreMemory::mem_gpuDataTransaction = (unsigned long)(*(CoreMemory::mem_vramRead.pVramImage));
            }
            else // read higher 16 bits
            {
                CoreMemory::mem_gpuDataTransaction |= (unsigned long)(*(CoreMemory::mem_vramRead.pVramImage)) << 16;
                // update pointed memory
                *pDwMem = CoreMemory::mem_gpuDataTransaction;
                pDwMem++; // increment memory location

                if (CoreMemory::mem_vramRead.colsRemaining <= 0) // check last column
                    break;
                i++;
            }

            // update cursor to higher bits position
            CoreMemory::mem_vramRead.pVramImage++;
            if (CoreMemory::mem_vramRead.pVramImage >= CoreMemory::mem_vramImage.pEOM) // check max position
                CoreMemory::mem_vramRead.pVramImage -= CoreMemory::mem_vramImage.bufferSize;
            CoreMemory::mem_vramRead.rowsRemaining--;

            // if column is empty, check next column
            if (CoreMemory::mem_vramRead.rowsRemaining <= 0)
            {
                CoreMemory::mem_vramRead.rowsRemaining = CoreMemory::mem_vramRead.width; // reset rows
                CoreMemory::mem_vramRead.pVramImage += (1024 - CoreMemory::mem_vramRead.width);
                if (CoreMemory::mem_vramRead.pVramImage >= CoreMemory::mem_vramImage.pEOM) // check max position
                    CoreMemory::mem_vramRead.pVramImage -= CoreMemory::mem_vramImage.bufferSize;

                CoreMemory::mem_vramRead.colsRemaining--;
                if (bitLevel && CoreMemory::mem_vramRead.colsRemaining <= 0) // check last column
                    break;
            }
            bitLevel = !bitLevel; // toggle low/high bits
        }
    }

    // if no columns remaining (transfer <= mem chunk size)
    if (i < size)
    {
        // reset transfer mode and values
        CoreMemory::mem_vramReadMode = DR_NORMAL;
        CoreMemory::mem_vramRead.x = CoreMemory::mem_vramRead.y = 0;
        CoreMemory::mem_vramRead.width = CoreMemory::mem_vramRead.height = 0;
        CoreMemory::mem_vramRead.rowsRemaining = CoreMemory::mem_vramRead.colsRemaining = 0;
        // signal VRAM transfer end
        CoreMemory::unsetStatus(GPUSTATUS_READYFORVRAM); // no longer ready
    }
    CoreMemory::setStatus(GPUSTATUS_IDLE); // idle
}

/// <summary>Process and send chunk of data to video data register</summary>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUwriteDataMem(unsigned long* pDwMem, int size)
{
    unsigned long gdata = 0;
    int i = 0;

    CoreMemory::unsetStatus(GPUSTATUS_IDLE); // busy
    CoreMemory::unsetStatus(GPUSTATUS_READYFORCOMMANDS); // not ready
    do
    {
        // write memory chunk of data to VRAM
        if (CoreMemory::mem_vramWriteMode == DR_VRAMTRANSFER)
        {
            // check vram limits
            while (CoreMemory::mem_vramWrite.pVramImage >= CoreMemory::mem_vramImage.pEOM) // max position
                CoreMemory::mem_vramWrite.pVramImage -= CoreMemory::mem_vramImage.bufferSize;
            while (CoreMemory::mem_vramWrite.pVramImage < CoreMemory::mem_vramImage.pWord) // min position
                CoreMemory::mem_vramWrite.pVramImage += CoreMemory::mem_vramImage.bufferSize;

            // upload data
            bool isChunkEnd = false;
            while (CoreMemory::mem_vramWrite.colsRemaining > 0)
            {
                while (CoreMemory::mem_vramWrite.rowsRemaining > 0)
                {
                    if (i < size)
                    {
                        // set data source
                        gdata = *pDwMem++;
                        i++;

                        // copy odd pixel data (gdata lower bits)
                        *CoreMemory::mem_vramWrite.pVramImage++ = (unsigned short)gdata;
                        if (CoreMemory::mem_vramWrite.pVramImage >= CoreMemory::mem_vramImage.pEOM)
                            CoreMemory::mem_vramWrite.pVramImage -= CoreMemory::mem_vramImage.bufferSize;
                        CoreMemory::mem_vramWrite.rowsRemaining--;

                        // if column is empty, check next column
                        if (CoreMemory::mem_vramWrite.rowsRemaining <= 0)
                        {
                            // if last column, end transfer
                            CoreMemory::mem_vramWrite.colsRemaining--;
                            if (CoreMemory::mem_vramWrite.colsRemaining <= 0) // last pixel is odd pixel
                            {
                                gdata = (gdata & 0x0FFFF) | (((unsigned long)(*CoreMemory::mem_vramWrite.pVramImage)) << 16);
                                goto ENDVRAM;
                            }
                            // set next column
                            CoreMemory::mem_vramWrite.rowsRemaining = CoreMemory::mem_vramWrite.width;
                            CoreMemory::mem_vramWrite.pVramImage += 1024 - CoreMemory::mem_vramWrite.width;
                        }

                        // copy even pixel data (gdata higher bits)
                        *CoreMemory::mem_vramWrite.pVramImage++ = (unsigned short)(gdata >> 16);
                        if (CoreMemory::mem_vramWrite.pVramImage >= CoreMemory::mem_vramImage.pEOM)
                            CoreMemory::mem_vramWrite.pVramImage -= CoreMemory::mem_vramImage.bufferSize;
                        CoreMemory::mem_vramWrite.rowsRemaining--;
                    }
                    else // mem chunk size reached
                    {
                        isChunkEnd = true;
                        goto ENDVRAM;
                    }
                }
                // set next column
                CoreMemory::mem_vramWrite.colsRemaining--;
                CoreMemory::mem_vramWrite.rowsRemaining = CoreMemory::mem_vramWrite.width;
                CoreMemory::mem_vramWrite.pVramImage += 1024 - CoreMemory::mem_vramWrite.width;
            }
            ENDVRAM:

            // data transfer end, if no columns remaining (didn't end because of mem chunk size)
            if (isChunkEnd == false)
            {
                if (CoreMemory::mem_isWriteUploadPending)
                {
                    CoreMemory::mem_isWriteUploadPending = false;
                    CheckWriteUpdate(); //opengl //!
                }
                // reset transfer mode to NORMAL operations
                CoreMemory::mem_vramWriteMode = DR_NORMAL;
                // reset transfer values (to prevent mis-transfer of data)
                CoreMemory::mem_vramWrite.colsRemaining = 0;
                CoreMemory::mem_vramWrite.rowsRemaining = 0;
            }
        }
    }
    while (CoreMemory::writeSimpleData(pDwMem, size, &gdata, &i)); // true = VRAM transfer again
    CoreMemory::mem_gpuDataTransaction = gdata;

    CoreMemory::setStatus(GPUSTATUS_READYFORCOMMANDS); // ready
    CoreMemory::setStatus(GPUSTATUS_IDLE); // idle
}

/// <summary>Process and send chunk of data to video data register</summary>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="size">Memory chunk size</param>
/// <param name="pDest">Destination gdata pointer</param>
/// <param name="pI">Counter pointer</param>
/// <returns>Indicator if VRAM data to write</returns>
bool CoreMemory::writeSimpleData(unsigned long* pDwMem, int size, unsigned long* pDest, int* pI)
{
    unsigned long gdata = 0;
    unsigned char command;
    int i = *pI;

    if (mem_vramWriteMode == DR_NORMAL)
    {
        // set processing type
        void(**primFunc)(unsigned char *);
        if (FramerateManager::isFrameSkipped())
            primFunc = primTableSkip;//!
        else
            primFunc = primTableJ;//!

        // process data
        while (i < size)
        {
            // if back to VRAM write mode, end function
            if (mem_vramWriteMode == DR_VRAMTRANSFER)
            {
                *pDest = gdata;
                *pI = i;
                return true;
            }

            // follow memory chain
            gdata = *pDwMem++;
            i++;
            
            if (gpu_dataC == 0) // new data set
            {
                command = (unsigned char)((gdata >> 24) & 0x0FF);
                if (primTableCX[command])//!
                {
                    gpu_dataC = primTableCX[command];//!
                    gpu_command = command;
                    gpu_dataM[0] = gdata;
                    gpu_dataP = 1;
                }
                else
                    continue;
            }
            else // same data set
            {
                gpu_dataM[gpu_dataP] = gdata;
                if (gpu_dataC > 128)
                {
                    if ((gpu_dataC == 254 && gpu_dataP >= 3) || (gpu_dataC == 255 && gpu_dataP >= 4 && !(gpu_dataP & 1)))
                    {
                        if ((gpu_dataM[gpu_dataP] & 0xF000F000) == 0x50005000)
                            gpu_dataP = gpu_dataC - 1;
                    }
                }
                gpu_dataP++;
            }
            if (gpu_dataP == gpu_dataC) // end of data set
            {
                gpu_dataC = gpu_dataP = 0;
                primFunc[gpu_command]((unsigned char *)gpu_dataM); // process data set

                // 'GPU busy' emulation hack
                if (g_pConfig->misc_emuFixBits & 0x0001 || g_pConfig->getCurrentProfile()->getFix(CFG_FIX_FAKE_GPU_BUSY))
                    CoreMemory::st_fixBusyEmuSequence = 4;
            }
        }
    }

    *pDest = gdata;
    return false; // no more data
}


// -- STATUS COMMANDS -- -------------------------------------------------------

/// <summary>Set display position</summary>
/// <param name="x">Horizontal position</param>
/// <param name="y">Vertical position</param>
void CoreMemory::cmdSetDisplayPosition(short x, short y)
{
    // check offset limits
    if (y & 0x200)
    {
        y |= 0x0FC00;
        dsp_displayState.previousHeightOffset = y / dsp_displayState.heightMultiplier;
        y = 0;
    }
    else
        dsp_displayState.previousHeightOffset = 0;
    if (x > 1000)
        x = 0;

    // store first time display position
    if (dsp_firstPositionFlag)
    {
        --dsp_firstPositionFlag;
        if (dsp_firstPositionFlag) 
        {
            dsp_displayState.previous.displayPosition.x = x;
            dsp_displayState.previous.displayPosition.y = y;
            dsp_displayState.current.displayPosition.x = x;
            dsp_displayState.current.displayPosition.y = y;
        }
    }

    // swap front/back detection fix
    if (g_pConfig->getCurrentProfile()->getFix(8))
    {
        if ((dsp_displayState.isInterlaced == false) &&
          dsp_displayState.previous.displayPosition.x == x && dsp_displayState.previous.displayPosition.y == y)
            return;
        dsp_displayState.current.displayPosition.x = dsp_displayState.previous.displayPosition.x;
        dsp_displayState.current.displayPosition.y = dsp_displayState.previous.displayPosition.y;
        dsp_displayState.previous.displayPosition.x = x;
        dsp_displayState.previous.displayPosition.y = y;
    }
    else
    {
        if ((dsp_displayState.isInterlaced == false) &&
          dsp_displayState.current.displayPosition.x == x  && dsp_displayState.current.displayPosition.y == y)
            return;
        dsp_displayState.previous.displayPosition.x = dsp_displayState.current.displayPosition.x;
        dsp_displayState.previous.displayPosition.y = dsp_displayState.current.displayPosition.y;
        dsp_displayState.current.displayPosition.x = x;
        dsp_displayState.current.displayPosition.y = y;
    }

    // adjust display end positions
    dsp_displayState.current.displayEnd.x = dsp_displayState.current.displayPosition.x + dsp_displayState.displaySize.x;
    dsp_displayState.current.displayEnd.y = dsp_displayState.current.displayPosition.y + dsp_displayState.displaySize.y 
                                            + dsp_displayState.previousHeightOffset;
    dsp_displayState.previous.displayEnd.x = dsp_displayState.previous.displayPosition.x + dsp_displayState.displaySize.x;
    dsp_displayState.previous.displayEnd.y = dsp_displayState.previous.displayPosition.y + dsp_displayState.displaySize.y
                                             + dsp_displayState.previousHeightOffset;

    // update display
    dsp_isDisplaySet = false;
    if (dsp_displayState.isInterlaced == false)
        updateDisplay();//!
    else // interlaced
    {
        if (dsp_displayState.dualInterlaceCheck &&
         ((dsp_displayState.previous.displayPosition.x != dsp_displayState.current.displayPosition.x) ||
          (dsp_displayState.previous.displayPosition.y != dsp_displayState.current.displayPosition.y)))
            dsp_displayState.dualInterlaceCheck--;
    }
}

/// <summary>Set display informations</summary>
/// <param name="gdata">Status register command</param>
void CoreMemory::cmdSetDisplayInfo(unsigned long gdata)
{
    // set display width
    dsp_displayState.displaySizePending.x = dsp_displayWidths[(gdata & 0x03) | ((gdata & 0x40) >> 4)];
    // set height multiplier
    if (gdata & 0x04)
        dsp_displayState.heightMultiplier = 2;
    else
        dsp_displayState.heightMultiplier = 1;
    dsp_displayState.displaySizePending.y = dsp_displayState.current.height * dsp_displayState.heightMultiplier;

    ChangeDispOffsetsY();//!

    // set status width bits
    unsetStatus(GPUSTATUS_WIDTHBITS);
    setStatus( (((gdata & 0x03) << 17) | ((gdata & 0x40) << 10)) );

    // interlacing 
    dsp_displayState.hasEnabledInterlacing = false;
    if (gdata & 0x20)
    {
        dsp_displayState.isInterlacedPending = true;
        if (dsp_displayState.isInterlaced == false)
        {
            dsp_displayState.dualInterlaceCheck = 2;
            dsp_displayState.previous.displayPosition.x = dsp_displayState.current.displayPosition.x;
            dsp_displayState.previous.displayPosition.y = dsp_displayState.current.displayPosition.y;
            dsp_displayState.hasEnabledInterlacing = true;
        }
        setStatus(GPUSTATUS_INTERLACED);
    }
    else
    {
        dsp_displayState.isInterlacedPending = false;
        dsp_displayState.dualInterlaceCheck = 0;
        unsetStatus(GPUSTATUS_INTERLACED);
    }
    // game localization
    if (gdata & 0x08)
    {
        dsp_displayState.localize = LocalizationMode_Pal;
        setStatus(GPUSTATUS_PAL);
    }
    else
    {
        dsp_displayState.localize = LocalizationMode_Ntsc;
        unsetStatus(GPUSTATUS_PAL);
    }
    // color depth
    if (gdata & 0x10)
    {
        dsp_displayState.rgbModePending = true;
        setStatus(GPUSTATUS_RGB24);
    }
    else
    {
        dsp_displayState.rgbModePending = false;
        unsetStatus(GPUSTATUS_RGB24);
    }
    // height multiplier status
    if (dsp_displayState.heightMultiplier == 2)
        setStatus(GPUSTATUS_DOUBLEHEIGHT);
    else
        unsetStatus(GPUSTATUS_DOUBLEHEIGHT);
}
