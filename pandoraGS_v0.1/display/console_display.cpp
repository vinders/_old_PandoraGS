/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   console_display.cpp
    Description : console display settings
*******************************************************************************/
using namespace std;
#include "../globals.h"
#include "console_display.h"
extern bool zincEmulation;
extern unsigned long zincGPUVersion;
// display widths enumeration
short ConsoleDisplay::displayWidths[8] = {256,320,512,640,368,384,512,640};


/* console display constructor - initialize values -------------------------- */
ConsoleDisplay::ConsoleDisplay()
{
    // default game definition settings
    gameColorDepth   = DISPLAY_RGB15;
    gameLocalization = DISPLAY_NTSC;
    gameInterlacing  = false;
    
    // default display settings
    currentSizeMode.x = DISPLAY_BASEWIDTH;
    currentSizeMode.y = DISPLAY_BASEHEIGHT;
    currentPosition.x = currentPosition.y = 1;
    multiplier        = 1;
    currentRange.left = currentRange.bottom = 0;
    drawArea.right    = DISPLAY_BASEWIDTH;
    drawArea.bottom   = DISPLAY_BASEHEIGHT;
    drawArea.left     = drawArea.top = 0;
    drawOffset.x      = drawOffset.y = 0;
    rumbleStrength    = rumbleTime   = 0;
    bCurrentStatus    = true;
    
    previousSizeMode.x  = DISPLAY_BASEWIDTH;
    previousSizeMode.y  = DISPLAY_BASEHEIGHT;
    previousPosition.x  = previousPosition.y = 1;
    previousRange.left  = previousRange.top    = 0;
    previousRange.right = previousRange.bottom = 0;
}


/* set display fixes -------------------------------------------------------- */
void ConsoleDisplay::setDisplayFixes(bool bWidthFix)
{
    if (bWidthFix)
        displayWidths[4] = 384;
    else 
        displayWidths[4] = 368;
}


/* reset default display values --------------------------------------------- */
void ConsoleDisplay::resetDisplay()
{
    gameColorDepth  = DISPLAY_RGB15;
    gameInterlacing = false;
    
    drawOffset.x   = drawOffset.y = 0;
    multiplier     = 1;
    bCurrentStatus = false;
    
    /*--------------------------------------------------//
    drawX=drawY=0;drawW=drawH=0;
    sSetMask=0;lSetMask=0;bCheckMask=FALSE;
            iSetMask=0;//opengl
    usMirror=0;
    GlobalTextAddrX=0;GlobalTextAddrY=0;
    GlobalTextTP=0;GlobalTextABR=0;
    bUsingTWin = FALSE; */
}


/* enable/disable display --------------------------------------------------- */
bool ConsoleDisplay::isBeingEnabled(bool bStatus)
{
    // change display status
    bool prevState = bCurrentStatus;
    bCurrentStatus = bStatus;

    // check if disabled->enabled transition
    if (prevState == false && bStatus == true)
        return true;
    return false;
}


/* set display information -------------------------------------------------- */
void ConsoleDisplay::setDisplayInfo(unsigned long gdata)
{
    // set game original display size and multiplier
    if (gdata&0x04) 
        multiplier = 2;
    else 
        multiplier = 1;
    currentSizeMode.x = displayWidths[(gdata&0x03) | ((gdata&0x40)>>4)];
    currentSizeMode.y = multiplier*(currentRange.bottom - currentRange.top); 
                        //! ...*(... + previousSizeMode.y) ?
    //! ChangeDispOffsetsY();
  
    // set game information
    gameLocalization = (gdata&0x08)?DISPLAY_PAL:DISPLAY_NTSC;
    gameColorDepth   = (gdata&0x10)?DISPLAY_RGB24:DISPLAY_RGB15;
    gameInterlacing  = (gdata&0x20)?true:false;

    // interlacing (position)
    if(gameInterlacing)
    {
        previousPosition.x = currentPosition.x;
        previousPosition.y = currentPosition.y;
    }
}


/* set display position ----------------------------------------------------- */
void ConsoleDisplay::setDisplayPosition(unsigned long gdata, unsigned long vramSize)
{
    // backup previous position
    previousPosition.x = currentPosition.x;
    previousPosition.y = currentPosition.y;
    
    // set display Y position
    if (zincEmulation)
    {
        if (zincGPUVersion == 2)
            currentPosition.y = (long)((gdata>>12)&0x3ff);
        else
            currentPosition.y = (long)((gdata>>10)&0x3ff);
    }
    else
    {
        currentPosition.y = (long)((gdata>>10)&0x1ff);
    }
    previousSizeMode.x = currentPosition.y; // store copy
    
    // vram limitations
    long dy1, dy2;
    if ((currentPosition.y + currentSizeMode.y) > (long)vramSize)
    {
        dy1 = vramSize - currentPosition.y;
        dy2 = (currentPosition.y + currentSizeMode.y) - vramSize;
        if(dy1 >= dy2)
            previousSizeMode.y = -dy2;
        else
            previousSizeMode.y = -dy1;
    }
    else
    {
        previousSizeMode.y = 0;
    }
    
    // set X position and display end
    currentPosition.x = (long)(gdata & 0x03ff);
    currentDisplayEnd.x = currentPosition.x + currentSizeMode.x;
    currentDisplayEnd.y = currentPosition.y + currentSizeMode.y; //! + previousSizeMode.y ?
    previousDisplayEnd.x = previousPosition.x + currentSizeMode.x;
    previousDisplayEnd.y = previousPosition.y + currentSizeMode.y; //! + previousSizeMode.y ?

    /*
    /*OGL ---------------------------------------------
    // setting display position
    short sx=(short)(gdata & 0x3ff);
    short sy;

    if(iGPUHeight==1024)
    {
        if(dwGPUVersion==2) 
            sy = (short)((gdata>>12)&0x3ff);
        else 
            sy = (short)((gdata>>10)&0x3ff);
    }
    else 
        sy = (short)((gdata>>10)&0x3ff);             // really: 0x1ff, but we adjust it later

    if (sy & 0x200) 
    {
        sy|=0xfc00;
        PreviousPSXDisplay.DisplayModeNew.y=sy/PSXDisplay.Double;
        sy=0;
    }
    else 
        PreviousPSXDisplay.DisplayModeNew.y=0;

    if(sx>1000) 
        sx=0;

    if(usFirstPos)
    {
        usFirstPos--;
        if(usFirstPos)
        {
            PreviousPSXDisplay.DisplayPosition.x = sx;
            PreviousPSXDisplay.DisplayPosition.y = sy;
            PSXDisplay.DisplayPosition.x = sx;
            PSXDisplay.DisplayPosition.y = sy;
        }
    }

    if(dwActFixes&8) 
    {
        if((!PSXDisplay.Interlaced) &&
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
        if((!PSXDisplay.Interlaced) &&
            PSXDisplay.DisplayPosition.x == sx  &&
            PSXDisplay.DisplayPosition.y == sy)
                return;
        PreviousPSXDisplay.DisplayPosition.x = PSXDisplay.DisplayPosition.x;
        PreviousPSXDisplay.DisplayPosition.y = PSXDisplay.DisplayPosition.y;
        PSXDisplay.DisplayPosition.x = sx;
        PSXDisplay.DisplayPosition.y = sy;
    }

    PSXDisplay.DisplayEnd.x=
    PSXDisplay.DisplayPosition.x+ PSXDisplay.DisplayMode.x;
    PSXDisplay.DisplayEnd.y=
    PSXDisplay.DisplayPosition.y+ PSXDisplay.DisplayMode.y+PreviousPSXDisplay.DisplayModeNew.y;

    PreviousPSXDisplay.DisplayEnd.x=
    PreviousPSXDisplay.DisplayPosition.x+ PSXDisplay.DisplayMode.x;
    PreviousPSXDisplay.DisplayEnd.y=
    PreviousPSXDisplay.DisplayPosition.y+ PSXDisplay.DisplayMode.y+PreviousPSXDisplay.DisplayModeNew.y;

    bDisplayNotSet = TRUE;
    */
}

/* set display width -------------------------------------------------------- */
void ConsoleDisplay::setDisplayWidth(unsigned long gdata)
{
    // set display range (width)
    currentRange.left  = (short)(gdata&0x7ff);
    currentRange.right = (short)((gdata>>12)&0xfff);
    //! ChangeDispOffsetsX();
}

/* set display height ------------------------------------------------------- */
bool ConsoleDisplay::setDisplayHeight(unsigned long gdata)
{
    // set display range (height)
    short top    = (short)(gdata&0x3ff);
    short bottom = (short)((gdata>>10)&0x3ff);
    
    if (top != currentRange.top || bottom != currentRange.bottom)
    {
        currentRange.top    = top;
        currentRange.bottom = bottom;
        currentSizeMode.y   = multiplier*(currentRange.bottom - currentRange.top); 
                              //! ...*(... + previousSizeMode.y) ?
        //! ChangeDispOffsetsY();
        return true;
    }
    return false;
}


/* set screen vibration ----------------------------------------------------- */
void ConsoleDisplay::setVisualVibration(unsigned long small, unsigned long big, unsigned int resX)
{
    int vibrationFactor = 0;
    // get shake from resolution
    if (currentSizeMode.x)
        vibrationFactor = resX / currentSizeMode.x;
    if (vibrationFactor < 1) // min 1 pixel
        vibrationFactor = 1;
    
    // set vibration strength
    if (big) // big rumble : 4...15 * shake
    {
        rumbleStrength = ((int)big*vibrationFactor)/10;
        if (rumbleStrength < 4*vibrationFactor) // min
            rumbleStrength = 4*vibrationFactor;
        else if (rumbleStrength > 15*vibrationFactor) // max
            rumbleStrength = 15*vibrationFactor;
    }
    else   // small rumble : 1...3 * shake
    {
        rumbleStrength = ((int)small*vibrationFactor)/10;
        if (rumbleStrength < vibrationFactor) // min
            rumbleStrength = vibrationFactor;
        else if (rumbleStrength > 3*vibrationFactor) // max
            rumbleStrength = 3*vibrationFactor;
    }
    // set rumble time
    rumbleTime = 15; // 16 buffer swaps
}

