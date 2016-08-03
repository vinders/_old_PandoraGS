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


// -- DISPLAY STATE -- ---------------------------------------------------------

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

} DisplayState_t;

/// <summary>Initialize display state values</summary>
void initDisplayState(DisplayState_t data);

#endif
