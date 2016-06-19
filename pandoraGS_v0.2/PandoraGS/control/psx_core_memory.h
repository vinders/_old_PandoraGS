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

/// <summary>Read entire chunk of data from video memory (vram)</summary>
/// <param name="pDwMem">Pointer to chunk of data (destination)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUreadDataMem(unsigned long* pDwMem, int size);
/// <summary>Process and send chunk of data to video data register</summary>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUwriteDataMem(unsigned long* pDwMem, int size);

// Playstation virtual video memory unit
class PsxCoreMemory
{
public:
    #ifdef _WINDOWS
    static HWND gen_hWindow;        // main emulator window handle
    #endif

    // psx emulated memory
    static int           mem_gpuVramSize;
    static PsxVram_t     mem_vramImage;          // PSX VRAM memory image
    static VramLoad_t    mem_vramRead;           // PSX VRAM frame reader
    static int           mem_vramReadMode;       // read transfer mode
    static VramLoad_t    mem_vramWrite;          // PSX VRAM frame writer
    static int           mem_vramWriteMode;      // write transfer mode
    static long          mem_gpuDataTransaction; // GPU data read/written by emulator
    static unsigned long mem_gpuDmaAddresses[3]; // DMA address check

    // gpu emulated status and information
    static long          st_statusReg;           // GPU status register
    static unsigned long st_pStatusControl[STATUSCTRL_SIZE]; // GPU status control
    static unsigned long st_pGpuDrawInfo[DRAWINFO_SIZE];     // GPU draw information
    static long          st_selectedSlot;        // save-state selected slot
    static bool          st_hasFixBusyEmu;       // 'GPU busy' emulation hack on/off
    static int           st_fixBusyEmuSequence;  // 'GPU busy' emulation hack - sequence value

    // display settings
    static DisplayState_t dsp_displayState;      // display information
    static short         dsp_displayWidths[8];   // possible psx display widths
    static unsigned long dsp_displayFlags;       // 00 -> digital, 01 -> analog, 02 -> mouse, 03 -> gun


public:
    /// <summary>Initialize memory values</summary>
    /// <exception cref="std::exception">Memory allocation failure</exception>
    static void initMemory();
    /// <summary>Close memory objects</summary>
    static void closeMemory();

    /// <summary>Activate specific status bit(s) in status register</summary>
    /// <param name="statusBits">Bits mask to set</param>
    static inline void setStatus(long statusBits)
    {
        st_statusReg |= statusBits;
    }
    /// <summary>Remove specific status bit(s) in status register</summary>
    /// <param name="statusBits">Bits mask to remove</param>
    static inline void unsetStatus(long statusBits)
    {
        st_statusReg &= ~statusBits;
    }
    /// <summary>Check if status bit is active in status register</summary>
    /// <param name="statusBits">Bit(s) mask (will return true if at least one is active)</param>
    static inline bool getStatus(long statusBit)
    {
        return ((st_statusReg & statusBit) != 0);
    }
    /// <summary>Get specific status bit(s) from status register</summary>
    /// <param name="statusBits">Bits mask</param>
    static inline long getStatusBits(long statusBits)
    {
        return (st_statusReg & statusBits);
    }

public:
    // -- MEMORY IO -- -------------------------------------------------------------

    /// <summary>Initialize check values for DMA chains</summary>
    static inline void resetDmaCheck()
    {
        mem_gpuDmaAddresses[0] = THREEBYTES_MASK;
        mem_gpuDmaAddresses[1] = THREEBYTES_MASK;
        mem_gpuDmaAddresses[2] = THREEBYTES_MASK;
    }
    /// <summary>Check DMA chain for endless loop (Pete's fix)</summary>
    /// <param name="addr">Memory address to check</param>
    static inline bool checkDmaEndlessChain(unsigned long addr)
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
    static inline void cmdReset()
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
    static inline void cmdGpuQuery(unsigned long query, bool isSpecialGpu)
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
    static inline void cmdSetDisplay(bool isDisabled)
    {
        // enable/disable
        dsp_displayState.previous.isDisabled = dsp_displayState.current.isDisabled; // copy previous
        dsp_displayState.current.isDisabled = isDisabled;
        if (dsp_displayState.current.isDisabled) // set status register
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

    /// <summary>Set display informations</summary>
    /// <param name="gdata">Status register command</param>
    static void cmdSetDisplayInfo(unsigned long gdata);
    /// <summary>Set display position</summary>
    /// <param name="x">Horizontal position</param>
    /// <param name="y">Vertical position</param>
    static void cmdSetDisplayPosition(short x, short y);

    /// <summary>Set display width</summary>
    /// <param name="x0">X start range</param>
    /// <param name="x1">X end range</param>
    static inline void cmdSetWidth(short x0, short x1)
    {
        dsp_displayState.current.range.x0 = x0;
        dsp_displayState.current.range.x1 = x1;
        dsp_displayState.current.range.x1 -= dsp_displayState.current.range.x0;
        //ChangeDispOffsetsX();
    }
    /// <summary>Set display height</summary>
    /// <param name="y0">Y start range</param>
    /// <param name="y1">Y end range</param>
    static inline void cmdSetHeight(short y0, short y1)
    {
        // set new height
        dsp_displayState.previous.height = dsp_displayState.current.height; // copy previous height
        dsp_displayState.current.range.y0 = y0;
        dsp_displayState.current.range.y1 = y1;
        dsp_displayState.current.height = dsp_displayState.current.range.y1 - dsp_displayState.current.range.y0
                                        + dsp_displayState.previousHeightOffset;
        // new height different -> update
        if (dsp_displayState.previous.height != dsp_displayState.current.height)
        {
            dsp_displayState.displaySizePending.y = dsp_displayState.current.height * dsp_displayState.heightMultiplier;
            //ChangeDispOffsetsY();
            //updateDisplayIfChanged();
        }
    }
};

#endif
