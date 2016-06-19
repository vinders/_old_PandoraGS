/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   display_state.cpp
Description : virtual display state
*******************************************************************************/
using namespace std;
#include "display_state.h"

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
