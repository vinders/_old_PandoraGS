/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   core_memory.h
Description : virtual video memory/registers unit
*******************************************************************************/
#ifndef _CORE_MEMORY_H
#define _CORE_MEMORY_H
#include "globals.h"
#include "display_state.h"
#include "lang.h"

#include "core_memory_commands.h" // vram commands + status register values

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
class CoreMemory
{
public:
    #ifdef _WINDOWS
    static HWND gen_hWindow;        // main emulator window handle
    static HMENU gen_hMenu;         // emulator menu handle
    static DWORD gen_origStyle;     // original window style
    #endif
    static std::string  gen_gameId; // game executable ID
    static bool gen_isFirstOpening; // first GPUopen()

    // psx emulated memory
    static int           mem_gpuVramSize;
    static PsxVram_t     mem_vramImage;          // PSX VRAM memory image
    static VramLoad_t    mem_vramRead;           // PSX VRAM frame reader
    static int           mem_vramReadMode;       // read transfer mode
    static VramLoad_t    mem_vramWrite;          // PSX VRAM frame writer
    static int           mem_vramWriteMode;      // write transfer mode
    static long          mem_gpuDataTransaction; // GPU data read/written by emulator
    static unsigned long mem_gpuDmaAddresses[3]; // DMA address check
    static bool          mem_isWriteUploadPending; // image needs to be uploaded to VRAM

    // gpu data memory
    static unsigned char gpu_command;
    static unsigned long gpu_dataM[256];
    static long          gpu_dataC;
    static long          gpu_dataP;

    // gpu emulated status and information
    static long          st_statusReg;           // GPU status register
    static unsigned long st_pStatusControl[STATUSCTRL_SIZE]; // GPU status control
    static unsigned long st_pGpuDrawInfo[DRAWINFO_SIZE];     // GPU draw information
    static long          st_selectedSlot;        // save-state selected slot
    static int           st_fixBusyEmuSequence;  // 'GPU busy' emulation hack - sequence value

    // display settings
    static bool          dsp_isDisplaySet;
    static DisplayState_t dsp_displayState;      // display information
    static short         dsp_displayWidths[8];   // possible psx display widths
    static unsigned long dsp_displayFlags;       // 00 -> digital, 01 -> analog, 02 -> mouse, 03 -> gun
    static int           dsp_firstPositionFlag;  // indicates first position drawing after startup


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
    static inline void resetDmaCheck();
    /// <summary>Check DMA chain for endless loop (Pete's fix)</summary>
    /// <param name="addr">Memory address to check</param>
    static inline bool checkDmaEndlessChain(unsigned long addr);

    /// <summary>Process and send chunk of data to video data register</summary>
    /// <param name="pDwMem">Pointer to chunk of data (source)</param>
    /// <param name="size">Memory chunk size</param>
    /// <param name="pDest">Destination gdata pointer</param>
    /// <param name="pI">Counter pointer</param>
    /// <returns>Indicator if VRAM data to write</returns>
    static bool writeSimpleData(unsigned long* pDwMem, int size, unsigned long* pDest, int* pI);


    // -- STATUS COMMANDS -- -------------------------------------------------------

    /// <summary>Reset GPU information</summary>
    static inline void cmdReset();

    /// <summary>Check GPU information (version, draw info, ...)</summary>
    /// <param name="query">Query identifier (8 bits)</param>
    /// <param name="isSpecialGpu">Use special GPU type (for zinc emu)</param>
    static inline void cmdGpuQuery(unsigned long query, bool isSpecialGpu);

    /// <summary>Enable/disable display</summary>
    /// <param name="isDisabled">Display status</param>
    static inline void cmdSetDisplay(bool isDisabled);

    /// <summary>Set display informations</summary>
    /// <param name="gdata">Status register command</param>
    static inline void cmdSetDisplayInfo(unsigned long gdata);
    /// <summary>Set display position</summary>
    /// <param name="x">Horizontal position</param>
    /// <param name="y">Vertical position</param>
    static inline void cmdSetDisplayPosition(short x, short y);

    /// <summary>Set display width</summary>
    /// <param name="x0">X start range</param>
    /// <param name="x1">X end range</param>
    static inline void cmdSetWidth(short x0, short x1);
    /// <summary>Set display height</summary>
    /// <param name="y0">Y start range</param>
    /// <param name="y1">Y end range</param>
    static inline void cmdSetHeight(short y0, short y1);
};
#include "core_memory_inline.hpp" // inline methods definitions

#endif
