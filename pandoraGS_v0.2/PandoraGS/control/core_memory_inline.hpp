/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   core_memory_inline.hpp
Description : virtual video memory/registers unit - inline memory IO methods
*******************************************************************************/
#ifndef _CORE_MEMORY_INLINE_HPP
#define _CORE_MEMORY_INLINE_HPP
#include "core_memory_commands.h"
#include "core_memory.h"

// -- MEMORY IO -- -------------------------------------------------------------

/// <summary>Initialize check values for DMA chains</summary>
inline void CoreMemory::resetDmaCheck()
{
    mem_gpuDmaAddresses[0] = THREEBYTES_MASK;
    mem_gpuDmaAddresses[1] = THREEBYTES_MASK;
    mem_gpuDmaAddresses[2] = THREEBYTES_MASK;
}
/// <summary>Check DMA chain for endless loop (Pete's fix)</summary>
/// <param name="addr">Memory address to check</param>
inline bool CoreMemory::checkDmaEndlessChain(unsigned long addr)
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
inline void CoreMemory::cmdReset()
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
inline void CoreMemory::cmdGpuQuery(unsigned long query, bool isSpecialGpu)
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
inline void CoreMemory::cmdSetDisplay(bool isDisabled)
{
    // enable/disable
    dsp_displayState.previous.isDisabled = dsp_displayState.current.isDisabled; // copy previous
    dsp_displayState.current.isDisabled = isDisabled;
    if (dsp_displayState.current.isDisabled) // set status register
        setStatus(GPUSTATUS_DISPLAYDISABLED);
    else
        unsetStatus(GPUSTATUS_DISPLAYDISABLED);

    if (/*...off-screen drawing 4 &&*/ dsp_displayState.previous.isDisabled && dsp_displayState.current.isDisabled == false)
    {
        if (dsp_displayState.current.rgbMode == RgbMode_15)
        {
            //... prepare + upload screen
            //... update display
        }
    }
}

/// <summary>Set display width</summary>
/// <param name="x0">X start range</param>
/// <param name="x1">X end range</param>
inline void CoreMemory::cmdSetWidth(short x0, short x1)
{
    dsp_displayState.current.range.x0 = x0;
    dsp_displayState.current.range.x1 = x1;
    dsp_displayState.current.range.x1 -= dsp_displayState.current.range.x0;
    //... update display offsets X
}
/// <summary>Set display height</summary>
/// <param name="y0">Y start range</param>
/// <param name="y1">Y end range</param>
inline void CoreMemory::cmdSetHeight(short y0, short y1)
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
        //... update display offsets Y
        //... update display if changed
    }
}

/// <summary>Set display position</summary>
/// <param name="x">Horizontal position</param>
/// <param name="y">Vertical position</param>
inline void CoreMemory::cmdSetDisplayPosition(short x, short y)
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
    if (Config::getCurrentProfile()->getFix(8))
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
        ; //... update display
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
inline void CoreMemory::cmdSetDisplayInfo(unsigned long gdata)
{
    // set display width
    dsp_displayState.displaySizePending.x = dsp_displayWidths[(gdata & 0x03) | ((gdata & 0x40) >> 4)];
    // set height multiplier
    if (gdata & 0x04)
        dsp_displayState.heightMultiplier = 2;
    else
        dsp_displayState.heightMultiplier = 1;
    dsp_displayState.displaySizePending.y = dsp_displayState.current.height * dsp_displayState.heightMultiplier;

    //... update display offsets Y

    // set status width bits
    unsetStatus(GPUSTATUS_WIDTHBITS);
    setStatus((((gdata & 0x03) << 17) | ((gdata & 0x40) << 10)));

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

#endif
