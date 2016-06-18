/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   display_state.h
Description : virtual display state
*******************************************************************************/
#ifndef _DISPLAY_STATE_H
#define _DISPLAY_STATE_H
#include "globals.h"

// -- GENERAL INFORMATION -- ---------------------------------------------------

// localization
enum LocalizationMode
{
    LocalizationMode_Ntsc = 0,
    LocalizationMode_Pal = 1
};
enum RgbMode
{
    RgbMode_15 = 0,
    RgbMode_24 = 1
};

// -- PSX DISPLAY DATA -- ------------------------------------------------------

// geometry point (dword)
typedef struct LPOINTTAG
{
    long x;
    long y;
} Point_t;
// geometry point (word)
typedef struct HPOINTTAG
{
    short x;
    short y;
} WPoint_t;
// geometry rectangle (word)
typedef struct PSXRECTTAG
{
    short x0; // X start
    short x1; // X end
    short y0; // Y start
    short y1; // Y end
} WRect_t;

// display step
typedef struct DISPLAYSTEP
{
    Point_t  displayPosition;
    Point_t  displayEnd;

    long     rgbMode;
    long     height;
    bool     isDisabled;
    WPoint_t drawOffset;
    WRect_t  drawArea;
    WRect_t  range;
} DisplayStep_t;


// -- DISPLAY STATE CLASS -- ---------------------------------------------------

// Playstation virtual display state
typedef struct DISPLAYSTATE
{
    // display states
    Point_t       displaySize; // main display size
    DisplayStep_t  current;    // current operation status
    DisplayStep_t previous;    // previous operation status
    long  heightMultiplier;    // height multiplier (simple/double)

    // frame sync config
    LocalizationMode localize;  // localization (NTSC/PAL)
    bool         isInterlaced;  // interlacing (on/off)
    long   dualInterlaceCheck;  // make sure interlacing is complete
        //= InterlacedTest

    // pending changes
    Point_t displaySizePending; // display size change
    long rgbModePending;        // latest RGB mode change
    bool isInterlacedPending;   // interlace mode change
    bool hasEnabledInterlacing; // interlace mode enabled during change
    long previousHeightOffset;  // previous display position Y offset
        //= PSXDisplay.DisplayModeNew
        //= PSXDisplay.RGB24New
        //= PSXDisplay.InterlacedNew
        //= PreviousPSXDisplay.InterlacedNew
        //= PreviousPSXDisplay.DisplayModeNew.y

    /*
    Point_t  GDrawOffset; // opengl
    Point_t  CumulOffset; // opengl
    */
} DisplayState_t;

/// <summary>Initialize display state values</summary>
void initDisplayState(DisplayState_t data)
{
    data.localize = LocalizationMode_Ntsc;
    data.current.rgbMode = RgbMode_15;
    data.previous.rgbMode = RgbMode_15;
    data.rgbModePending = 0;

    data.isInterlaced = false;
    data.isInterlacedPending = false;
    data.hasEnabledInterlacing = false;
    data.dualInterlaceCheck = 0;

    data.displaySize.x = 320;
    data.displaySize.y = 240;
    data.heightMultiplier = 1;
    data.current.drawArea.x1 = 320;
    data.current.drawArea.y1 = 240;
    data.current.drawArea.x0 = data.current.drawArea.y0 = 0;
    data.current.drawOffset.x = data.current.drawOffset.y = 0;
    data.current.height = 0;
    data.current.displayPosition.x = data.current.displayPosition.y = 1;
    data.current.displayEnd.x = data.current.displayEnd.y = 0;
    data.current.range.x0 = data.current.range.x1 = 0;
    data.current.range.y0 = data.current.range.y1 = 0;
    data.current.isDisabled = false;

    data.previous.drawArea.x0 = data.previous.drawArea.y0 = 0;
    data.previous.drawArea.x1 = data.previous.drawArea.y1 = 0;
    data.previous.drawOffset.x = data.previous.drawOffset.y = 0;
    data.previous.height = 0;
    data.previous.displayPosition.x = data.previous.displayPosition.y = 1;
    data.previous.displayEnd.x = data.previous.displayEnd.y = 0;
    data.previous.range.x0 = data.previous.range.x1 = 0;
    data.previous.range.y0 = data.previous.range.y1 = 0;
    data.previous.isDisabled = false;

    data.displaySizePending.x = 0;
    data.displaySizePending.y = 0;
    data.previousHeightOffset = 0;
}

#endif
