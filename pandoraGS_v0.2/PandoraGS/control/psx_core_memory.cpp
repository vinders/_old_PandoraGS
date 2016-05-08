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


/// <summary>Create empty instance</summary>
PsxCoreMemory::PsxCoreMemory()
{
    #ifdef _WINDOWS
    gen_hWindow = NULL;
    gen_hDisplayWindow = NULL;
    #endif
    sync_localize = LocalizationMode_Ntsc;
    sync_isInterlaced = false;
    mem_vramImage.pData = NULL;

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

    dsp_displayFlags = 0;
    st_hasFixBusyEmu = false;
    st_fixBusyEmuSequence = 0;
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


// -- STATUS COMMANDS -- -------------------------------------------------------

/// <summary>Reset GPU info</summary>
void PsxCoreMemory::cmdReset()
{
    // initialize status and information
    memset(st_pGpuDrawInfo, 0x0, DRAWINFO_SIZE * sizeof(unsigned long));
    st_statusReg = GPUSTATUS_INIT;

    //PSXDisplay.Disabled = 1;

    // initialize VRAM load
    mem_vramReadMode = DR_NORMAL;
    mem_vramWriteMode = DR_NORMAL;

    /*PSXDisplay.DrawOffset.x = PSXDisplay.DrawOffset.y = 0;
    drawX = drawY = 0; drawW = drawH = 0;
    sSetMask = 0; lSetMask = 0; bCheckMask = FALSE; iSetMask = 0;
    usMirror = 0;
    GlobalTextAddrX = 0; GlobalTextAddrY = 0;
    GlobalTextTP = 0; GlobalTextABR = 0;
    PSXDisplay.RGB24 = FALSE;
    PSXDisplay.Interlaced = FALSE;
    bUsingTWin = FALSE;*/
}

/// <summary>Enable/disable display</summary>
/// <param name="isDisabled">Display status</param>
void PsxCoreMemory::cmdSetDisplay(bool isDisabled)
{
    /*PreviousPSXDisplay.Disabled = PSXDisplay.Disabled;
    PSXDisplay.Disabled = isDisabled;

    if (PSXDisplay.Disabled)
        setStatus(GPUSTATUS_DISPLAYDISABLED);
    else 
        unsetStatus(GPUSTATUS_DISPLAYDISABLED);

    if (iOffscreenDrawing == 4 &&
        PreviousPSXDisplay.Disabled &&
        !(PSXDisplay.Disabled))
    {

        if (!PSXDisplay.RGB24)
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
    /*PSXDisplay.Range.x0 = x0;
    PSXDisplay.Range.x1 = x1;
    PSXDisplay.Range.x1 -= PSXDisplay.Range.x0;

    ChangeDispOffsetsX();*/
}

/// <summary>Set display height</summary>
/// <param name="y0">Y start range</param>
/// <param name="y1">Y end range</param>
void PsxCoreMemory::cmdSetHeight(short y0, short y1)
{
    /*PreviousPSXDisplay.Height = PSXDisplay.Height;

    PSXDisplay.Range.y0 = y0;
    PSXDisplay.Range.y1 = y1;
    PSXDisplay.Height = PSXDisplay.Range.y1 -
    PSXDisplay.Range.y0 + PreviousPSXDisplay.DisplayModeNew.y;

    if (PreviousPSXDisplay.Height != PSXDisplay.Height)
    {
        PSXDisplay.DisplayModeNew.y = PSXDisplay.Height*PSXDisplay.Double;
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
