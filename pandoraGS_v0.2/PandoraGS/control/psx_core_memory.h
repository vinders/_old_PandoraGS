/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   psx_core_memory.h
Description : playstation virtual video memory unit
*******************************************************************************/
#ifndef _PSX_CORE_MEMORY_H
#define _PSX_CORE_MEMORY_H
#include "globals.h"
#include "display_state.h"

// -- PSX MEMORY DATA -- -------------------------------------------------------

// PSX VRAM memory image
typedef struct PSX_VRAM
{
    unsigned char*  pData;    // allocated memory image
    unsigned char*  pByte;    // 8 bits mode access pointer
    unsigned short* pWord;    // 16 bits mode access pointer
    unsigned long*  pDword;   // 32 bits mode access pointer
    unsigned short* pEOM;     // end of memory

    unsigned long   bufferSize;// single vram buffer size
    unsigned long   length;   // total memory length
    int             oddFrame; // current frame has 'odd' lines
} PsxVram_t;
// PSX VRAM load indicator
typedef struct VRAM_LOADTAG
{
    short x;
    short y;
    short width;
    short height;
    short rowsRemaining;
    short colsRemaining;
    unsigned short *pVramImage;
} VramLoad_t;

// VRAM size (kilobytes)
#define PSXVRAM_SIZE          512
#define ZINCVRAM_SIZE         1024
#define PSXVRAM_SECURE_EXTRA  1024
#define PSXVRAM_SECURE_OFFSET 512
// array size
#define STATUSCTRL_SIZE       256
#define DRAWINFO_SIZE         16
// dma access masks
#define THREEBYTES_MASK       0x0FFFFFF
#define PSXVRAM_MASK          0x1FFFFC // 2097148
#define PSXVRAM_THRESHOLD     2000000

// status bits masks
#define GPUSTATUS_INIT              0x14802000 // initial values
#define GPUSTATUS_ODDLINES          0x80000000
#define GPUSTATUS_DMABITS           0x60000000 // 2 bits
#define GPUSTATUS_READYFORCOMMANDS  0x10000000
#define GPUSTATUS_READYFORVRAM      0x08000000
#define GPUSTATUS_IDLE              0x04000000
#define GPUSTATUS_DISPLAYDISABLED   0x00800000
#define GPUSTATUS_INTERLACED        0x00400000
#define GPUSTATUS_RGB24             0x00200000
#define GPUSTATUS_PAL               0x00100000
#define GPUSTATUS_DOUBLEHEIGHT      0x00080000
#define GPUSTATUS_WIDTHBITS         0x00070000 // 3 bits
#define GPUSTATUS_MASKENABLED       0x00001000
#define GPUSTATUS_MASKDRAWN         0x00000800
#define GPUSTATUS_DRAWINGALLOWED    0x00000400
#define GPUSTATUS_DITHER            0x00000200
// data transfer modes
#define DR_NORMAL               0
#define DR_VRAMTRANSFER         1
// data transaction codes
#define GPUDATA_INIT            0x400
#define GPUDATA_BIOSADDR        0xBFC03720
#define GPUINFO_TW              0
#define GPUINFO_DRAWSTART       1
#define GPUINFO_DRAWEND         2
#define GPUINFO_DRAWOFF         3

// packet information access (read commands)
#define getGpuCommand(DATA)    ((DATA>>24)&0x0FF) // get only bits 25 and 26
// commands
#define CMD_RESETGPU            0x00
#define CMD_TOGGLEDISPLAY       0x03
#define CMD_SETTRANSFERMODE     0x04
#define CMD_SETDISPLAYPOSITION  0x05
#define CMD_SETDISPLAYWIDTH     0x06
#define CMD_SETDISPLAYHEIGHT    0x07
#define CMD_SETDISPLAYINFO      0x08
#define CMD_GPUREQUESTINFO      0x10
// info request codes
#define REQ_TW                  0x02
#define REQ_DRAWSTART           0x03
#define REQ_DRAWEND             0x04
#define REQ_DRAWOFFSET1         0x05
#define REQ_DRAWOFFSET2         0x06
#define REQ_GPUTYPE             0x07
#define REQ_BIOSADDR1           0x08
#define REQ_BIOSADDR2           0x0F
#define SAVESTATE_LOAD          0
#define SAVESTATE_SAVE          1
#define SAVESTATE_INFO          2
// info request replies
#define INFO_GPUBIOSADDR        0xBFC03720 // BIOS - virtual hardware address (last bits after memory)
#define INFO_TW                 0 // draw info - array index
#define INFO_DRAWSTART          1
#define INFO_DRAWEND            2
#define INFO_DRAWOFF            3


// -- PSX CORE MEMORY CLASS -- -------------------------------------------------

// Playstation virtual video memory unit
class PsxCoreMemory
{
public:
    #ifdef _WINDOWS
    HWND gen_hWindow;        // main emulator window handle
    HWND gen_hDisplayWindow; // psx display window handle
    #endif

    // psx emulated memory
    int           mem_gpuVramSize;
    PsxVram_t     mem_vramImage;          // PSX VRAM memory image
    VramLoad_t    mem_vramRead;           // PSX VRAM frame reader
    int           mem_vramReadMode;       // read transfer mode
    VramLoad_t    mem_vramWrite;          // PSX VRAM frame writer
    int           mem_vramWriteMode;      // write transfer mode
    long          mem_gpuDataTransaction; // GPU data read/written by emulator
    unsigned long mem_gpuDmaAddresses[3]; // DMA address check

    // gpu emulated status and information
    long          st_statusReg;           // GPU status register
    unsigned long st_pStatusControl[STATUSCTRL_SIZE]; // GPU status control
    unsigned long st_pGpuDrawInfo[DRAWINFO_SIZE];     // GPU draw information
    long          st_selectedSlot;        // save-state selected slot
    bool          st_hasFixBusyEmu;       // 'GPU busy' emulation hack on/off
    int           st_fixBusyEmuSequence;  // 'GPU busy' emulation hack - sequence value

    // display settings
    DisplayState  dsp_displayState;       // display information
    short         dsp_displayWidths[8];   // possible psx display widths
    unsigned long dsp_displayFlags;       // 00 -> digital, 01 -> analog, 02 -> mouse, 03 -> gun


public:
    /// <summary>Create empty instance</summary>
    PsxCoreMemory();
    /// <summary>Destroy memory instance</summary>
    ~PsxCoreMemory();

    /// <summary>Initialize memory instance values</summary>
    /// <exception cref="std::exception">Memory allocation failure</exception>
    void initMemory();

    /// <summary>Activate specific status bit(s) in status register</summary>
    /// <param name="statusBits">Bits mask to set</param>
    void setStatus(long statusBits)
    {
        st_statusReg |= statusBits;
    }
    /// <summary>Remove specific status bit(s) in status register</summary>
    /// <param name="statusBits">Bits mask to remove</param>
    void unsetStatus(long statusBits)
    {
        st_statusReg &= ~statusBits;
    }
    /// <summary>Check if status bit is active in status register</summary>
    /// <param name="statusBits">Bit(s) mask (will return true if at least one is active)</param>
    bool getStatus(long statusBit)
    {
        return ((st_statusReg & statusBit) != 0);
    }
    /// <summary>Get specific status bit(s) from status register</summary>
    /// <param name="statusBits">Bits mask</param>
    long getStatusBits(long statusBits)
    {
        return (st_statusReg & statusBits);
    }

public:
    // -- MEMORY IO -- -------------------------------------------------------------

    /// <summary>Read entire chunk of data from video memory (vram)</summary>
    /// <param name="pDwMem">Pointer to chunk of data (destination)</param>
    /// <param name="size">Memory chunk size</param>
    inline void readDataMem(unsigned long* pDwMem, int size);
    /// <summary>Process and send chunk of data to video data register</summary>
    /// <param name="pDwMem">Pointer to chunk of data (source)</param>
    /// <param name="size">Memory chunk size</param>
    inline void writeDataMem(unsigned long* pDwMem, int size);


    /// <summary>Initialize check values for DMA chains</summary>
    void resetDmaCheck()
    {
        mem_gpuDmaAddresses[0] = THREEBYTES_MASK;
        mem_gpuDmaAddresses[1] = THREEBYTES_MASK;
        mem_gpuDmaAddresses[2] = THREEBYTES_MASK;
    }
    /// <summary>Check DMA chain for endless loop (Pete's fix)</summary>
    /// <param name="addr">Memory address to check</param>
    bool checkDmaEndlessChain(unsigned long addr)
    {
        if (addr == mem_gpuDmaAddresses[1] || addr == mem_gpuDmaAddresses[2])
            return true;
        if (addr < mem_gpuDmaAddresses[0])
            mem_gpuDmaAddresses[1] = addr;
        else
            mem_gpuDmaAddresses[2] = addr;
        mem_gpuDmaAddresses[0] = addr;
        return false;
    }

    // -- STATUS COMMANDS -- -------------------------------------------------------

    /// <summary>Reset GPU information</summary>
    inline void cmdReset();
    /// <summary>Check GPU information (version, draw info, ...)</summary>
    /// <param name="query">Query identifier (8 bits)</param>
    /// <param name="isSpecialGpu">Use special GPU type (for zinc emu)</param>
    inline void cmdGpuQuery(unsigned long query, bool isSpecialGpu);

    /// <summary>Enable/disable display</summary>
    /// <param name="isDisabled">Display status</param>
    inline void cmdSetDisplay(bool isDisabled);
    /// <summary>Set display position</summary>
    /// <param name="x">Horizontal position</param>
    /// <param name="y">Vertical position</param>
    inline void cmdSetDisplayPosition(short x, short y);
    /// <summary>Set display width</summary>
    /// <param name="x0">X start range</param>
    /// <param name="x1">X end range</param>
    inline void cmdSetWidth(short x0, short x1);
    /// <summary>Set display height</summary>
    /// <param name="y0">Y start range</param>
    /// <param name="y1">Y end range</param>
    inline void cmdSetHeight(short y0, short y1);
    /// <summary>Set display informations</summary>
    /// <param name="gdata">Status register command</param>
    inline void cmdSetDisplayInfo(unsigned long gdata);
};



// -- MEMORY IO -- -------------------------------------------------------------

/// <summary>Read entire chunk of data from video memory (vram)</summary>
/// <param name="pDwMem">Pointer to chunk of data (destination)</param>
/// <param name="size">Memory chunk size</param>
void PsxCoreMemory::readDataMem(unsigned long* pDwMem, int size)
{
    unsetStatus(GPUSTATUS_IDLE); // busy

    // check/adjust vram reader position
    while (mem_vramRead.pVramImage >= mem_vramImage.pEOM) // max position
        mem_vramRead.pVramImage -= mem_vramImage.bufferSize;
    while (mem_vramRead.pVramImage < mem_vramImage.pWord) // min position
        mem_vramRead.pVramImage += mem_vramImage.bufferSize;

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
    if (mem_vramRead.colsRemaining > 0 && mem_vramRead.rowsRemaining > 0)
    {
        while (i < size) // check if end of memory chunk
        {
            // 2 separate 16 bits reads (compatibility: wrap issues)
            if (bitLevel == false) // read lower 16 bits
            {
                mem_gpuDataTransaction = (unsigned long)*mem_vramRead.pVramImage;
            }
            else // read higher 16 bits
            {
                mem_gpuDataTransaction |= (unsigned long)(*mem_vramRead.pVramImage) << 16;
                // update pointed memory
                *pDwMem = mem_gpuDataTransaction;
                pDwMem++; // increment memory location

                if (mem_vramRead.colsRemaining <= 0) // check last column
                    break;
                i++;
            }

            // update cursor to higher bits position
            mem_vramRead.pVramImage++;
            if (mem_vramRead.pVramImage >= mem_vramImage.pEOM) // check max position
                mem_vramRead.pVramImage -= mem_vramImage.bufferSize;
            mem_vramRead.rowsRemaining--;

            // if column is empty, check next column
            if (mem_vramRead.rowsRemaining <= 0)
            {
                mem_vramRead.rowsRemaining = mem_vramRead.width; // reset rows
                mem_vramRead.pVramImage += (1024 - mem_vramRead.width);
                if (mem_vramRead.pVramImage >= mem_vramImage.pEOM) // check max position
                    mem_vramRead.pVramImage -= mem_vramImage.bufferSize;

                mem_vramRead.colsRemaining--;
                if (bitLevel && mem_vramRead.colsRemaining <= 0) // check last column
                    break;
            }

            bitLevel = !bitLevel; // toggle low/high bits
        }
    }

    // if no columns remaining (transfer <= mem chunk size)
    if (i < size)
    {
        // reset transfer mode and values
        mem_vramReadMode = DR_NORMAL;
        mem_vramRead.x = mem_vramRead.y = 0;
        mem_vramRead.width = mem_vramRead.height = 0;
        mem_vramRead.rowsRemaining = mem_vramRead.colsRemaining = 0;
        // signal VRAM transfer end
        unsetStatus(GPUSTATUS_READYFORVRAM); // no longer ready
    }
    setStatus(GPUSTATUS_IDLE); // idle
}

/// <summary>Process and send chunk of data to video data register</summary>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="size">Memory chunk size</param>
void PsxCoreMemory::writeDataMem(unsigned long* pDwMem, int size)
{
    unsetStatus(GPUSTATUS_IDLE); // busy
    unsetStatus(GPUSTATUS_READYFORCOMMANDS); // not ready

    unsigned long gdata = 0;
    bool bitLevel;
    int i = 0;

    //...

    // 'GPU busy' emulation hack
    if (st_hasFixBusyEmu)
        st_fixBusyEmuSequence = 4;

    //...

    setStatus(GPUSTATUS_READYFORCOMMANDS); // ready
    setStatus(GPUSTATUS_IDLE); // idle



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

/// <summary>Reset GPU information</summary>
void PsxCoreMemory::cmdReset()
{
    // initialize status and information
    memset(st_pGpuDrawInfo, 0x0, DRAWINFO_SIZE * sizeof(unsigned long));
    st_statusReg = GPUSTATUS_INIT;
    dsp_displayState.current.isDisabled = true;

    // initialize VRAM load
    mem_vramReadMode = DR_NORMAL;
    mem_vramWriteMode = DR_NORMAL;

    dsp_displayState.current.drawOffset.x = dsp_displayState.current.drawOffset.y = 0;
    //drawX = drawY = 0; drawW = drawH = 0;
    //sSetMask = 0; lSetMask = 0; bCheckMask = FALSE; iSetMask = 0;
    //usMirror = 0;
    //GlobalTextAddrX = 0; GlobalTextAddrY = 0;
    //GlobalTextTP = 0; GlobalTextABR = 0;
    dsp_displayState.current.rgbMode = RgbMode_15;
    dsp_displayState.isInterlaced = false;
    //bUsingTWin = FALSE;
}

/// <summary>Check GPU information (version, draw info, ...)</summary>
/// <param name="query">Query identifier (8 bits)</param>
/// <param name="isSpecialGpu">Use special GPU type (for zinc emu)</param>
void PsxCoreMemory::cmdGpuQuery(unsigned long query, bool isSpecialGpu)
{
    switch (query)
    {
    case REQ_TW:          mem_gpuDataTransaction = st_pGpuDrawInfo[INFO_TW]; return;
    case REQ_DRAWSTART:   mem_gpuDataTransaction = st_pGpuDrawInfo[INFO_DRAWSTART]; return;
    case REQ_DRAWEND:     mem_gpuDataTransaction = st_pGpuDrawInfo[INFO_DRAWEND]; return;
    case REQ_DRAWOFFSET1:
    case REQ_DRAWOFFSET2: mem_gpuDataTransaction = st_pGpuDrawInfo[INFO_DRAWOFF]; return;
    case REQ_GPUTYPE:     mem_gpuDataTransaction = isSpecialGpu ? 0x1 : 0x2; return;
    case REQ_BIOSADDR1:
    case REQ_BIOSADDR2:   mem_gpuDataTransaction = INFO_GPUBIOSADDR; return;
    }
}

/// <summary>Enable/disable display</summary>
/// <param name="isDisabled">Display status</param>
void PsxCoreMemory::cmdSetDisplay(bool isDisabled)
{
    dsp_displayState.previous.isDisabled = dsp_displayState.current.isDisabled;
    dsp_displayState.current.isDisabled = isDisabled;

    if (dsp_displayState.current.isDisabled)
        setStatus(GPUSTATUS_DISPLAYDISABLED);
    else
        unsetStatus(GPUSTATUS_DISPLAYDISABLED);

    /*if (iOffscreenDrawing == 4 && dsp_displayState.previous.isDisabled && dsp_displayState.current.isDisabled == false)
    {
        if (dsp_displayState.current.rgbMode == RgbMode_15)
        {
            PrepareFullScreenUpload(TRUE);
            UploadScreen(TRUE);
            updateDisplay();
        }
    }*/
}

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

/// <summary>Set display width</summary>
/// <param name="x0">X start range</param>
/// <param name="x1">X end range</param>
void PsxCoreMemory::cmdSetWidth(short x0, short x1)
{
    dsp_displayState.current.range.x0 = x0;
    dsp_displayState.current.range.x1 = x1;
    dsp_displayState.current.range.x1 -= dsp_displayState.current.range.x0;

    //ChangeDispOffsetsX();
}

/// <summary>Set display height</summary>
/// <param name="y0">Y start range</param>
/// <param name="y1">Y end range</param>
void PsxCoreMemory::cmdSetHeight(short y0, short y1)
{
    dsp_displayState.previous.height = dsp_displayState.current.height;

    dsp_displayState.current.range.y0 = y0;
    dsp_displayState.current.range.y1 = y1;
    dsp_displayState.current.height = dsp_displayState.current.range.y1 - dsp_displayState.current.range.y0
                                    + dsp_displayState.previousHeightOffset;

    /*if (dsp_displayState.previous.height != dsp_displayState.current.height)
    {
        dsp_displayState.displaySizePending.y = dsp_displayState.current.height * dsp_displayState.heightMultiplier;
        ChangeDispOffsetsY();
        updateDisplayIfChanged();
    }*/
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

#endif
