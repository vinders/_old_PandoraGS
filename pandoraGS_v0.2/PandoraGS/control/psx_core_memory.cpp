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
#include "psx_core_memory.h"

extern bool g_isZincEmu; // zinc interface emulation
extern PsxCoreMemory* g_pMemory;


/// <summary>Create empty instance</summary>
PsxCoreMemory::PsxCoreMemory()
{
    #ifdef _WINDOWS
    gen_hWindow = NULL;
    gen_hDisplayWindow = NULL;
    #endif
    mem_vramImage.pData = NULL;

    dsp_displayState.init();
    dsp_displayFlags = 0;
    st_hasFixBusyEmu = false;
    st_fixBusyEmuSequence = 0;

    // native display widths
    dsp_displayWidths[0] = 256;
    dsp_displayWidths[1] = 320;
    dsp_displayWidths[2] = 512;
    dsp_displayWidths[3] = 640;
    dsp_displayWidths[4] = 368;
    dsp_displayWidths[5] = 384;
    dsp_displayWidths[6] = 512;
    dsp_displayWidths[7] = 640;
    // data transaction init
    mem_gpuDataTransaction = GPUDATA_INIT;
}

/// <summary>Destroy memory instance</summary>
PsxCoreMemory::~PsxCoreMemory()
{
    if (mem_vramImage.pData != NULL)
        free(mem_vramImage.pData);
}

/// <summary>Initialize memory instance values</summary>
/// <exception cref="std::exception">Memory allocation failure</exception>
void PsxCoreMemory::initMemory()
{
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


// -- MEMORY IO -- -------------------------------------------------------------

/// <summary>Read entire chunk of data from video memory (vram)</summary>
/// <param name="pDwMem">Pointer to chunk of data (destination)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUreadDataMem(unsigned long* pDwMem, int size)
{
    g_pMemory->unsetStatus(GPUSTATUS_IDLE); // busy

    // check/adjust vram reader position
    while (g_pMemory->mem_vramRead.pVramImage >= g_pMemory->mem_vramImage.pEOM) // max position
        g_pMemory->mem_vramRead.pVramImage -= g_pMemory->mem_vramImage.bufferSize;
    while (g_pMemory->mem_vramRead.pVramImage < g_pMemory->mem_vramImage.pWord) // min position
        g_pMemory->mem_vramRead.pVramImage += g_pMemory->mem_vramImage.bufferSize;

    /*! //opengl
    if((iFrameReadType&1 && iSize>1) &&
        !(iDrawnSomething==2 &&
        VRAMRead.x      == VRAMWrite.x     &&
        VRAMRead.y      == VRAMWrite.y     &&
        VRAMRead.Width  == VRAMWrite.Width &&
        VRAMRead.Height == VRAMWrite.Height))
            CheckVRamRead(VRAMRead.x,VRAMRead.y,
                VRAMRead.x+VRAMRead.RowsRemaining,
                VRAMRead.y+VRAMRead.ColsRemaining,
                TRUE);*/

    // read memory chunk of data
    int i = 0;
    bool bitLevel = false;
    if (g_pMemory->mem_vramRead.colsRemaining > 0 && g_pMemory->mem_vramRead.rowsRemaining > 0)
    {
        while (i < size) // check if end of memory chunk
        {
            // 2 separate 16 bits reads (compatibility: wrap issues)
            if (bitLevel == false) // read lower 16 bits
            {
                g_pMemory->mem_gpuDataTransaction = (unsigned long)(*(g_pMemory->mem_vramRead.pVramImage));
            }
            else // read higher 16 bits
            {
                g_pMemory->mem_gpuDataTransaction |= (unsigned long)(*(g_pMemory->mem_vramRead.pVramImage)) << 16;
                // update pointed memory
                *pDwMem = g_pMemory->mem_gpuDataTransaction;
                pDwMem++; // increment memory location

                if (g_pMemory->mem_vramRead.colsRemaining <= 0) // check last column
                    break;
                i++;
            }

            // update cursor to higher bits position
            g_pMemory->mem_vramRead.pVramImage++;
            if (g_pMemory->mem_vramRead.pVramImage >= g_pMemory->mem_vramImage.pEOM) // check max position
                g_pMemory->mem_vramRead.pVramImage -= g_pMemory->mem_vramImage.bufferSize;
            g_pMemory->mem_vramRead.rowsRemaining--;

            // if column is empty, check next column
            if (g_pMemory->mem_vramRead.rowsRemaining <= 0)
            {
                g_pMemory->mem_vramRead.rowsRemaining = g_pMemory->mem_vramRead.width; // reset rows
                g_pMemory->mem_vramRead.pVramImage += (1024 - g_pMemory->mem_vramRead.width);
                if (g_pMemory->mem_vramRead.pVramImage >= g_pMemory->mem_vramImage.pEOM) // check max position
                    g_pMemory->mem_vramRead.pVramImage -= g_pMemory->mem_vramImage.bufferSize;

                g_pMemory->mem_vramRead.colsRemaining--;
                if (bitLevel && g_pMemory->mem_vramRead.colsRemaining <= 0) // check last column
                    break;
            }

            bitLevel = !bitLevel; // toggle low/high bits
        }
    }

    // if no columns remaining (transfer <= mem chunk size)
    if (i < size)
    {
        // reset transfer mode and values
        g_pMemory->mem_vramReadMode = DR_NORMAL;
        g_pMemory->mem_vramRead.x = g_pMemory->mem_vramRead.y = 0;
        g_pMemory->mem_vramRead.width = g_pMemory->mem_vramRead.height = 0;
        g_pMemory->mem_vramRead.rowsRemaining = g_pMemory->mem_vramRead.colsRemaining = 0;
        // signal VRAM transfer end
        g_pMemory->unsetStatus(GPUSTATUS_READYFORVRAM); // no longer ready
    }
    g_pMemory->setStatus(GPUSTATUS_IDLE); // idle
}

/// <summary>Process and send chunk of data to video data register</summary>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUwriteDataMem(unsigned long* pDwMem, int size)
{
    g_pMemory->unsetStatus(GPUSTATUS_IDLE); // busy
    g_pMemory->unsetStatus(GPUSTATUS_READYFORCOMMANDS); // not ready

    unsigned long gdata = 0;
    bool bitLevel;
    int i = 0;

    //...

    // 'GPU busy' emulation hack
    if (g_pMemory->st_hasFixBusyEmu)
        g_pMemory->st_fixBusyEmuSequence = 4;

    //...

    g_pMemory->setStatus(GPUSTATUS_READYFORCOMMANDS); // ready
    g_pMemory->setStatus(GPUSTATUS_IDLE); // idle



    /*unsigned char command;
    unsigned long gdata = 0;
    int i = 0;
    GPUIsBusy;
    GPUIsNotReadyForCommands;

    STARTVRAM:

    if (iDataWriteMode == DR_VRAMTRANSFER)
    {
    // make sure we are in vram
    while (VRAMWrite.ImagePtr >= psxVuw_eom)
    VRAMWrite.ImagePtr -= iGPUHeight * 1024;
    while (VRAMWrite.ImagePtr<psxVuw)
    VRAMWrite.ImagePtr += iGPUHeight * 1024;

    // now do the loop
    while (VRAMWrite.ColsRemaining>0)
    {
    while (VRAMWrite.RowsRemaining>0)
    {
    if (i >= iSize) { goto ENDVRAM; }
    i++;

    gdata = *pMem++;

    *VRAMWrite.ImagePtr++ = (unsigned short)gdata;
    if (VRAMWrite.ImagePtr >= psxVuw_eom) VRAMWrite.ImagePtr -= iGPUHeight * 1024;
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
    if (VRAMWrite.ImagePtr >= psxVuw_eom) VRAMWrite.ImagePtr -= iGPUHeight * 1024;
    VRAMWrite.RowsRemaining--;
    }

    VRAMWrite.RowsRemaining = VRAMWrite.Width;
    VRAMWrite.ColsRemaining--;
    VRAMWrite.ImagePtr += 1024 - VRAMWrite.Width;
    }

    FinishedVRAMWrite();
    }

    ENDVRAM:

    if (iDataWriteMode == DR_NORMAL)
    {
    void(**primFunc)(unsigned char *);
    if (bSkipNextFrame) primFunc = primTableSkip;
    else               primFunc = primTableJ;

    for (; i<iSize;)
    {
    if (iDataWriteMode == DR_VRAMTRANSFER) goto STARTVRAM;

    gdata = *pMem++; i++;

    if (gpuDataC == 0)
    {
    command = (unsigned char)((gdata >> 24) & 0xff);

    if (primTableCX[command])
    {
    gpuDataC = primTableCX[command];
    gpuCommand = command;
    gpuDataM[0] = gdata;
    gpuDataP = 1;
    }
    else continue;
    }
    else
    {
    gpuDataM[gpuDataP] = gdata;
    if (gpuDataC>128)
    {
    if ((gpuDataC == 254 && gpuDataP >= 3) ||
    (gpuDataC == 255 && gpuDataP >= 4 && !(gpuDataP & 1)))
    {
    if ((gpuDataM[gpuDataP] & 0xF000F000) == 0x50005000)
    gpuDataP = gpuDataC - 1;
    }
    }
    gpuDataP++;
    }

    if (gpuDataP == gpuDataC)
    {
    gpuDataC = gpuDataP = 0;
    primFunc[gpuCommand]((unsigned char *)gpuDataM);

    if (dwEmuFixes & 0x0001 || dwActFixes & 0x20000)     // hack for emulating "gpu busy" in some games
    iFakePrimBusy = 4;
    }
    }
    }

    GPUdataRet = gdata;

    GPUIsReadyForCommands;
    GPUIsIdle;*/
}


// -- STATUS COMMANDS -- -------------------------------------------------------

/// <summary>Set display position</summary>
/// <param name="x">Horizontal position</param>
/// <param name="y">Vertical position</param>
void PsxCoreMemory::cmdSetDisplayPosition(short x, short y)
{
    /*if (sy & 0x200)
    {
    sy |= 0xfc00;
    PreviousPSXDisplay.DisplayModeNew.y = sy / PSXDisplay.Double;
    sy = 0;
    }
    else PreviousPSXDisplay.DisplayModeNew.y = 0;

    if (sx>1000) sx = 0;

    if (usFirstPos)
    {
    usFirstPos--;
    if (usFirstPos)
    {
    PreviousPSXDisplay.DisplayPosition.x = sx;
    PreviousPSXDisplay.DisplayPosition.y = sy;
    PSXDisplay.DisplayPosition.x = sx;
    PSXDisplay.DisplayPosition.y = sy;
    }
    }

    if (dwActFixes & 8)
    {
    if ((!PSXDisplay.Interlaced) &&
    PreviousPSXDisplay.DisplayPosition.x == sx  &&
    PreviousPSXDisplay.DisplayPosition.y == sy)
    return;

    PSXDisplay.DisplayPosition.x = PreviousPSXDisplay.DisplayPosition.x;
    PSXDisplay.DisplayPosition.y = PreviousPSXDisplay.DisplayPosition.y;
    PreviousPSXDisplay.DisplayPosition.x = sx;
    PreviousPSXDisplay.DisplayPosition.y = sy;
    }
    else
    {
    if ((!PSXDisplay.Interlaced) &&
    PSXDisplay.DisplayPosition.x == sx  &&
    PSXDisplay.DisplayPosition.y == sy)
    return;
    PreviousPSXDisplay.DisplayPosition.x = PSXDisplay.DisplayPosition.x;
    PreviousPSXDisplay.DisplayPosition.y = PSXDisplay.DisplayPosition.y;
    PSXDisplay.DisplayPosition.x = sx;
    PSXDisplay.DisplayPosition.y = sy;
    }

    PSXDisplay.DisplayEnd.x =
    PSXDisplay.DisplayPosition.x + PSXDisplay.DisplayMode.x;
    PSXDisplay.DisplayEnd.y =
    PSXDisplay.DisplayPosition.y + PSXDisplay.DisplayMode.y + PreviousPSXDisplay.DisplayModeNew.y;

    PreviousPSXDisplay.DisplayEnd.x =
    PreviousPSXDisplay.DisplayPosition.x + PSXDisplay.DisplayMode.x;
    PreviousPSXDisplay.DisplayEnd.y =
    PreviousPSXDisplay.DisplayPosition.y + PSXDisplay.DisplayMode.y + PreviousPSXDisplay.DisplayModeNew.y;

    bDisplayNotSet = TRUE;

    if (!(PSXDisplay.Interlaced))
    {
    updateDisplay();
    }
    else
    if (PSXDisplay.InterlacedTest &&
    ((PreviousPSXDisplay.DisplayPosition.x != PSXDisplay.DisplayPosition.x) ||
    (PreviousPSXDisplay.DisplayPosition.y != PSXDisplay.DisplayPosition.y)))
    PSXDisplay.InterlacedTest--;*/
}

/// <summary>Set display informations</summary>
/// <param name="gdata">Status register command</param>
void PsxCoreMemory::cmdSetDisplayInfo(unsigned long gdata)
{
    /*PSXDisplay.DisplayModeNew.x = dispWidths[(gdata & 0x03) | ((gdata & 0x40) >> 4)];

    if (gdata & 0x04) PSXDisplay.Double = 2;
    else            PSXDisplay.Double = 1;
    PSXDisplay.DisplayModeNew.y = PSXDisplay.Height*PSXDisplay.Double;

    ChangeDispOffsetsY();

    PSXDisplay.PAL = (gdata & 0x08) ? TRUE : FALSE; // if 1 - PAL mode, else NTSC
    PSXDisplay.RGB24New = (gdata & 0x10) ? TRUE : FALSE; // if 1 - TrueColor
    PSXDisplay.InterlacedNew = (gdata & 0x20) ? TRUE : FALSE; // if 1 - Interlace

    unsetStatus(GPUSTATUS_WIDTHBITS);                   // clear the width bits
    setStatus(  (((gdata & 0x03) << 17) |
    ((gdata & 0x40) << 10))  );                // set the width bits

    PreviousPSXDisplay.InterlacedNew = FALSE;
    if (PSXDisplay.InterlacedNew)
    {
    if (!PSXDisplay.Interlaced)
    {
    PSXDisplay.InterlacedTest = 2;
    PreviousPSXDisplay.DisplayPosition.x = PSXDisplay.DisplayPosition.x;
    PreviousPSXDisplay.DisplayPosition.y = PSXDisplay.DisplayPosition.y;
    PreviousPSXDisplay.InterlacedNew = TRUE;
    }

    setStatus(GPUSTATUS_INTERLACED);
    }
    else
    {
    PSXDisplay.InterlacedTest = 0;
    unsetStatus(GPUSTATUS_INTERLACED);
    }

    if (PSXDisplay.PAL)
    setStatus(GPUSTATUS_PAL);
    else
    unsetStatus(GPUSTATUS_PAL);

    if (PSXDisplay.Double == 2)
    setStatus(GPUSTATUS_DOUBLEHEIGHT);
    else
    unsetStatus(GPUSTATUS_DOUBLEHEIGHT);

    if (PSXDisplay.RGB24New)
    setStatus(GPUSTATUS_RGB24);
    else
    unsetStatus(GPUSTATUS_RGB24);

    updateDisplayIfChanged();*/
}
