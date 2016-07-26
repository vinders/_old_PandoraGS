/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   psx_core_memory.cpp
Description : playstation virtual video memory unit
*******************************************************************************/
#include <string>
using namespace std;
#include "config.h"
#include "psx_core_memory.h"
#include "framerate_manager.h"

extern bool g_isZincEmu; // zinc interface emulation
extern Config* g_pConfig;


#ifdef _WINDOWS
HWND  PsxCoreMemory::gen_hWindow = NULL;        // main emulator window handle
HMENU PsxCoreMemory::gen_hMenu = NULL;          // emulator menu handle
DWORD PsxCoreMemory::gen_origStyle = 0uL;       // original window style
#endif

// psx emulated memory
int           PsxCoreMemory::mem_gpuVramSize = PSXVRAM_SIZE;
PsxVram_t     PsxCoreMemory::mem_vramImage = { 0,0,0,0,0,0,0,0 };   // PSX VRAM memory image
VramLoad_t    PsxCoreMemory::mem_vramRead;                          // PSX VRAM frame reader
int           PsxCoreMemory::mem_vramReadMode = DR_NORMAL;          // read transfer mode
VramLoad_t    PsxCoreMemory::mem_vramWrite;                         // PSX VRAM frame writer
int           PsxCoreMemory::mem_vramWriteMode = DR_NORMAL;         // write transfer mode
long          PsxCoreMemory::mem_gpuDataTransaction = GPUDATA_INIT; // GPU data read/written by emulator
unsigned long PsxCoreMemory::mem_gpuDmaAddresses[3];     // DMA address check

// gpu data memory
unsigned char PsxCoreMemory::gpu_command = 0;
unsigned long PsxCoreMemory::gpu_dataM[256];
long          PsxCoreMemory::gpu_dataC = 0;
long          PsxCoreMemory::gpu_dataP = 0;

// gpu emulated status and information
long          PsxCoreMemory::st_statusReg;               // GPU status register
unsigned long PsxCoreMemory::st_pStatusControl[STATUSCTRL_SIZE];   // GPU status control
unsigned long PsxCoreMemory::st_pGpuDrawInfo[DRAWINFO_SIZE];       // GPU draw information
long          PsxCoreMemory::st_selectedSlot = 0L;                 // save-state selected slot
int           PsxCoreMemory::st_fixBusyEmuSequence = 0;  // 'GPU busy' emulation hack - sequence value

// display settings
bool PsxCoreMemory::dsp_isDisplaySet = false;
DisplayState_t PsxCoreMemory::dsp_displayState;          // display information
short PsxCoreMemory::dsp_displayWidths[8] = { 256, 320, 512, 640, 368, 384, 512, 640 }; // native display widths
unsigned long PsxCoreMemory::dsp_displayFlags = 0;       // 00 -> digital, 01 -> analog, 02 -> mouse, 03 -> gun
int PsxCoreMemory::dsp_firstPositionFlag = 2;            // indicates first position drawing after startup


/// <summary>Initialize memory instance values</summary>
/// <exception cref="std::exception">Memory allocation failure</exception>
void PsxCoreMemory::initMemory()
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
void PsxCoreMemory::closeMemory()
{
    if (mem_vramImage.pData != NULL)
        free(mem_vramImage.pData);
}


// -- MEMORY IO -- -------------------------------------------------------------

/// <summary>Read entire chunk of data from video memory (vram)</summary>
/// <param name="pDwMem">Pointer to chunk of data (destination)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUreadDataMem(unsigned long* pDwMem, int size)
{
    PsxCoreMemory::unsetStatus(GPUSTATUS_IDLE); // busy

    // check/adjust vram reader position
    while (PsxCoreMemory::mem_vramRead.pVramImage >= PsxCoreMemory::mem_vramImage.pEOM) // max position
        PsxCoreMemory::mem_vramRead.pVramImage -= PsxCoreMemory::mem_vramImage.bufferSize;
    while (PsxCoreMemory::mem_vramRead.pVramImage < PsxCoreMemory::mem_vramImage.pWord) // min position
        PsxCoreMemory::mem_vramRead.pVramImage += PsxCoreMemory::mem_vramImage.bufferSize;

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
    if (PsxCoreMemory::mem_vramRead.colsRemaining > 0 && PsxCoreMemory::mem_vramRead.rowsRemaining > 0)
    {
        while (i < size) // check if end of memory chunk
        {
            // 2 separate 16 bits reads (compatibility: wrap issues)
            if (bitLevel == false) // read lower 16 bits
            {
                PsxCoreMemory::mem_gpuDataTransaction = (unsigned long)(*(PsxCoreMemory::mem_vramRead.pVramImage));
            }
            else // read higher 16 bits
            {
                PsxCoreMemory::mem_gpuDataTransaction |= (unsigned long)(*(PsxCoreMemory::mem_vramRead.pVramImage)) << 16;
                // update pointed memory
                *pDwMem = PsxCoreMemory::mem_gpuDataTransaction;
                pDwMem++; // increment memory location

                if (PsxCoreMemory::mem_vramRead.colsRemaining <= 0) // check last column
                    break;
                i++;
            }

            // update cursor to higher bits position
            PsxCoreMemory::mem_vramRead.pVramImage++;
            if (PsxCoreMemory::mem_vramRead.pVramImage >= PsxCoreMemory::mem_vramImage.pEOM) // check max position
                PsxCoreMemory::mem_vramRead.pVramImage -= PsxCoreMemory::mem_vramImage.bufferSize;
            PsxCoreMemory::mem_vramRead.rowsRemaining--;

            // if column is empty, check next column
            if (PsxCoreMemory::mem_vramRead.rowsRemaining <= 0)
            {
                PsxCoreMemory::mem_vramRead.rowsRemaining = PsxCoreMemory::mem_vramRead.width; // reset rows
                PsxCoreMemory::mem_vramRead.pVramImage += (1024 - PsxCoreMemory::mem_vramRead.width);
                if (PsxCoreMemory::mem_vramRead.pVramImage >= PsxCoreMemory::mem_vramImage.pEOM) // check max position
                    PsxCoreMemory::mem_vramRead.pVramImage -= PsxCoreMemory::mem_vramImage.bufferSize;

                PsxCoreMemory::mem_vramRead.colsRemaining--;
                if (bitLevel && PsxCoreMemory::mem_vramRead.colsRemaining <= 0) // check last column
                    break;
            }
            bitLevel = !bitLevel; // toggle low/high bits
        }
    }

    // if no columns remaining (transfer <= mem chunk size)
    if (i < size)
    {
        // reset transfer mode and values
        PsxCoreMemory::mem_vramReadMode = DR_NORMAL;
        PsxCoreMemory::mem_vramRead.x = PsxCoreMemory::mem_vramRead.y = 0;
        PsxCoreMemory::mem_vramRead.width = PsxCoreMemory::mem_vramRead.height = 0;
        PsxCoreMemory::mem_vramRead.rowsRemaining = PsxCoreMemory::mem_vramRead.colsRemaining = 0;
        // signal VRAM transfer end
        PsxCoreMemory::unsetStatus(GPUSTATUS_READYFORVRAM); // no longer ready
    }
    PsxCoreMemory::setStatus(GPUSTATUS_IDLE); // idle
}

/// <summary>Process and send chunk of data to video data register</summary>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUwriteDataMem(unsigned long* pDwMem, int size)
{
    unsigned long gdata = 0;
    bool bitLevel;
    int i = 0;

    PsxCoreMemory::unsetStatus(GPUSTATUS_IDLE); // busy
    PsxCoreMemory::unsetStatus(GPUSTATUS_READYFORCOMMANDS); // not ready
    do
    {
        if (PsxCoreMemory::mem_vramWriteMode == DR_VRAMTRANSFER)
        {
            // check vram limits
            while (PsxCoreMemory::mem_vramWrite.pVramImage >= PsxCoreMemory::mem_vramImage.pEOM) // max position
                PsxCoreMemory::mem_vramWrite.pVramImage -= PsxCoreMemory::mem_vramImage.bufferSize;
            while (PsxCoreMemory::mem_vramWrite.pVramImage < PsxCoreMemory::mem_vramImage.pWord) // min position
                PsxCoreMemory::mem_vramWrite.pVramImage += PsxCoreMemory::mem_vramImage.bufferSize;


        /*  // now do the loop
            while (VRAMWrite.ColsRemaining>0)
            {
                while (VRAMWrite.RowsRemaining>0)
                {
                    if (i >= iSize)
                        goto ENDVRAM;
                    i++;

                    gdata = *pMem++;

                    *VRAMWrite.ImagePtr++ = (unsigned short)gdata;
                    if (VRAMWrite.ImagePtr >= psxVuw_eom)
                        VRAMWrite.ImagePtr -= iGPUHeight * 1024;
                    VRAMWrite.RowsRemaining--;

                    if (VRAMWrite.RowsRemaining <= 0)
                    {
                        VRAMWrite.ColsRemaining--;
                        if (VRAMWrite.ColsRemaining <= 0)             // last pixel is odd width
                        {
                            gdata = (gdata & 0xFFFF) | (((unsigned long)(*VRAMWrite.ImagePtr)) << 16);
                            FinishedVRAMWrite();
                            goto ENDVRAM;
                        }
                        VRAMWrite.RowsRemaining = VRAMWrite.Width;
                        VRAMWrite.ImagePtr += 1024 - VRAMWrite.Width;
                    }

                    *VRAMWrite.ImagePtr++ = (unsigned short)(gdata >> 16);
                    if (VRAMWrite.ImagePtr >= psxVuw_eom)
                        VRAMWrite.ImagePtr -= iGPUHeight * 1024;
                    VRAMWrite.RowsRemaining--;
                }

                VRAMWrite.RowsRemaining = VRAMWrite.Width;
                VRAMWrite.ColsRemaining--;
                VRAMWrite.ImagePtr += 1024 - VRAMWrite.Width;
            }

            FinishedVRAMWrite();*/
        }
        /*ENDVRAM:*/
    }
    while (PsxCoreMemory::writeSimpleData(pDwMem, size, &gdata, &i)); // true = VRAM transfer again
    PsxCoreMemory::mem_gpuDataTransaction = gdata;

    PsxCoreMemory::setStatus(GPUSTATUS_READYFORCOMMANDS); // ready
    PsxCoreMemory::setStatus(GPUSTATUS_IDLE); // idle
}
/*__inline void FinishedVRAMWrite(void)
{
    if (bNeedWriteUpload)
    {
        bNeedWriteUpload = FALSE;
        CheckWriteUpdate();
    }
    // set register to NORMAL operation
    iDataWriteMode = DR_NORMAL;
    // reset transfer values, to prevent mis-transfer of data
    VRAMWrite.ColsRemaining = 0;
    VRAMWrite.RowsRemaining = 0;
}*/


/// <summary>Process and send chunk of data to video data register</summary>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="size">Memory chunk size</param>
/// <param name="pDest">Destination gdata pointer</param>
/// <param name="pI">Counter pointer</param>
/// <returns>Indicator if VRAM data to write</returns>
bool PsxCoreMemory::writeSimpleData(unsigned long* pDwMem, int size, unsigned long* pDest, int* pI)
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
                    PsxCoreMemory::st_fixBusyEmuSequence = 4;
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
void PsxCoreMemory::cmdSetDisplayPosition(short x, short y)
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
void PsxCoreMemory::cmdSetDisplayInfo(unsigned long gdata)
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
