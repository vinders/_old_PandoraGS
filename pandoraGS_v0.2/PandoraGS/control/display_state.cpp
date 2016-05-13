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
void DisplayState::init()
{
    localize = LocalizationMode_Ntsc;
    current.rgbMode = RgbMode_15;
    previous.rgbMode = RgbMode_15;
    rgbModePending = 0;

    isInterlaced = false;
    isInterlacedPending = false;
    hasEnabledInterlacing = false;
    dualInterlaceCheck = 0;

    displaySize.x = 320;
    displaySize.y = 240;
    heightMultiplier = 1;
    current.drawArea.x1 = 320;
    current.drawArea.y1 = 240;
    current.drawArea.x0 = current.drawArea.y0 = 0;
    current.drawOffset.x = current.drawOffset.y = 0;
    current.height = 0;
    current.displayPosition.x = current.displayPosition.y = 1;
    current.displayEnd.x = current.displayEnd.y = 0;
    current.range.x0 = current.range.x1 = 0;
    current.range.y0 = current.range.y1 = 0;
    current.isDisabled = false;

    previous.drawArea.x0 = previous.drawArea.y0 = 0;
    previous.drawArea.x1 = previous.drawArea.y1 = 0;
    previous.drawOffset.x = previous.drawOffset.y = 0;
    previous.height = 0;
    previous.displayPosition.x = previous.displayPosition.y = 1;
    previous.displayEnd.x = previous.displayEnd.y = 0;
    previous.range.x0 = previous.range.x1 = 0;
    previous.range.y0 = previous.range.y1 = 0;
    previous.isDisabled = false;

    displaySizePending.x = 0;
    displaySizePending.y = 0;
    previousHeightOffset = 0;
}
