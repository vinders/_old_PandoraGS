/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   display_state.cpp
Description : display state, data and settings
*******************************************************************************/
#ifndef _DISPLAY_STATE_CPP
#define _DISPLAY_STATE_CPP
#include "globals.h"
#ifndef _DISPLAY_STATE_H
using namespace std;
#endif
#include "config.h"

#ifndef _DISPLAY_STATE_H // if not sub-inclusion from header file (for inline methods)
#include "display_state.h"

// available psx display widths
short DisplayState::s_displayWidths[8] = { 256, 320, 512, 640, 368, 384, 512, 640 };

/// <summary>Initialize display state</summary>
void DisplayState::init()
{
    memset(m_pDrawInfo, 0x0, DRAWINFO_SIZE * sizeof(buffer_t));
    m_region = Region_ntsc;
    m_isInterlaced = false;
    setOddFrameFlag(0);

    // display state
    dsp_dispSize.x = 320L;
    dsp_dispSize.y = 240L;
    dsp_heightMultiplier = 1L;

    dsp_curFrame.colorMode = Colormode_rgb15;
    dsp_curFrame.drawArea.x0 = dsp_curFrame.drawArea.y0 = 0;
    dsp_curFrame.drawArea.x1 = 320;
    dsp_curFrame.drawArea.y1 = 240;
    dsp_curFrame.drawOffset.x = dsp_curFrame.drawOffset.y = 0;
    dsp_curFrame.height = 0L;
    dsp_curFrame.displayPos.x = dsp_curFrame.displayPos.y = 1;
    dsp_curFrame.displayEnd.x = dsp_curFrame.displayEnd.y = 0;
    dsp_curFrame.range.x0 = dsp_curFrame.range.x1 = 0;
    dsp_curFrame.range.y0 = dsp_curFrame.range.y1 = 0;
    dsp_curFrame.isDisabled = false;

    dsp_prevFrame.colorMode = Colormode_rgb15;
    dsp_prevFrame.drawArea.x0 = dsp_prevFrame.drawArea.y0 = 0;
    dsp_prevFrame.drawArea.x1 = dsp_prevFrame.drawArea.y1 = 0;
    dsp_prevFrame.drawOffset.x = dsp_prevFrame.drawOffset.y = 0;
    dsp_prevFrame.height = 0;
    dsp_prevFrame.displayPos.x = dsp_prevFrame.displayPos.y = 1;
    dsp_prevFrame.displayEnd.x = dsp_prevFrame.displayEnd.y = 0;
    dsp_prevFrame.range.x0 = dsp_prevFrame.range.x1 = 0;
    dsp_prevFrame.range.y0 = dsp_prevFrame.range.y1 = 0;
    dsp_prevFrame.isDisabled = false;

    // pending changes
    ch_displaySizePending.x = 0L;
    ch_displaySizePending.y = 0L;
    ch_rgbModePending = Colormode_rgb15;
    ch_interlacingPending = Change_none;
    ch_dualInterlaceCheck = 0;
    ch_prevHeightOffset = 0L;
}
#endif

// -- STATUS COMMANDS -- -------------------------------------------------------

/// <summary>Reset GPU information</summary>
inline void DisplayState::reset()
{
    memset(m_pDrawInfo, 0x0, DRAWINFO_SIZE * sizeof(buffer_t));
    dsp_curFrame.isDisabled = true;
    dsp_curFrame.drawOffset.x = dsp_curFrame.drawOffset.y = 0;
    dsp_curFrame.colorMode = Colormode_rgb15;
    m_isInterlaced = false;
    //...
}

/// <summary>Enable/disable display</summary>
/// <param name="isDisabled">Display status</param>
inline void DisplayState::toggleDisplay(bool isDisabled)
{
    // enable/disable
    dsp_prevFrame.isDisabled = dsp_curFrame.isDisabled; // copy previous
    dsp_curFrame.isDisabled = isDisabled;
    if (isDisabled) // set status register
        StatusRegister::setStatus(GPUSTATUS_DISPLAYDISABLED);
    else
        StatusRegister::unsetStatus(GPUSTATUS_DISPLAYDISABLED);

    // now enabled + full offscreen drawing -> upload
    if (/*offscreenDrawing == 4 && */dsp_prevFrame.isDisabled && dsp_curFrame.isDisabled == false) //!
    {
        if (dsp_curFrame.colorMode == Colormode_rgb15)
        {
            //! ... prepare screen full upload
            //! ... upload screen
            //! ... update display
            //...
        }
    }
}

/// <summary>Set display width</summary>
/// <param name="pos">X Start/end range</param>
inline void DisplayState::setWidth(point_t pos)
{
    dsp_curFrame.range.x0 = pos.x;
    dsp_curFrame.range.x1 = pos.y;
    dsp_curFrame.range.x1 -= dsp_curFrame.range.x0; // coord to width
    //! ... change display offsets X
    //...
}

/// <summary>Set display height</summary>
/// <param name="pos">Y Start/end range</param>
inline void DisplayState::setHeight(point_t pos)
{
    // set new height
    dsp_prevFrame.height = dsp_curFrame.height; // copy previous height
    dsp_curFrame.range.y0 = pos.x;
    dsp_curFrame.range.y1 = pos.y;
    dsp_curFrame.height = dsp_curFrame.range.y1 - dsp_curFrame.range.y0 + ch_prevHeightOffset;
    // new height different -> update
    if (dsp_prevFrame.height != dsp_curFrame.height)
    {
        ch_displaySizePending.y = dsp_curFrame.height * dsp_heightMultiplier;
        //! ... change display offsets Y
        //...
        //! ... update display if changed
        //...
    }
}

/// <summary>Set display position</summary>
/// <param name="pos">Horizontal/vertical position</param>
inline void DisplayState::setDisplayPos(point_t pos)
{
    // check offset limits
    if (pos.y & 0x200)
    {
        pos.y |= 0x0FC00;
        ch_prevHeightOffset = pos.y / dsp_heightMultiplier;
        pos.y = 0;
    }
    else
        ch_prevHeightOffset = 0;
    if (pos.x > 1000)
        pos.x = 0;

    // store first time display position
    if (m_firstPositionFlag)
    {
        --m_firstPositionFlag;
        if (m_firstPositionFlag)
        {
            dsp_prevFrame.displayPos.x = pos.x;
            dsp_prevFrame.displayPos.y = pos.y;
            dsp_curFrame.displayPos.x = pos.x;
            dsp_curFrame.displayPos.y = pos.y;
        }
    }

    // swap front/back - detection fix
    if (Config::getCurrentProfile()->getFix(CFG_FIX_SWAP_FRONTBACK))
    {
        if (m_isInterlaced == false &&
          dsp_prevFrame.displayPos.x == pos.x && dsp_prevFrame.displayPos.y == pos.y)
            return;
        dsp_curFrame.displayPos.x = dsp_prevFrame.displayPos.x;
        dsp_curFrame.displayPos.y = dsp_prevFrame.displayPos.y;
        dsp_prevFrame.displayPos.x = pos.x;
        dsp_prevFrame.displayPos.y = pos.y;
    }
    else
    {
        if (m_isInterlaced == false &&
          dsp_curFrame.displayPos.x == pos.x && dsp_curFrame.displayPos.y == pos.y)
            return;
        dsp_prevFrame.displayPos.x = dsp_curFrame.displayPos.x;
        dsp_prevFrame.displayPos.y = dsp_curFrame.displayPos.y;
        dsp_curFrame.displayPos.x = pos.x;
        dsp_curFrame.displayPos.y = pos.y;
    }

    // adjust display end positions
    dsp_curFrame.displayEnd.x = dsp_curFrame.displayPos.x + dsp_dispSize.x;
    dsp_curFrame.displayEnd.y = dsp_curFrame.displayPos.y + dsp_dispSize.y + ch_prevHeightOffset;
    dsp_prevFrame.displayEnd.x = dsp_prevFrame.displayPos.x + dsp_dispSize.x;
    dsp_prevFrame.displayEnd.y = dsp_prevFrame.displayPos.y + dsp_dispSize.y + ch_prevHeightOffset;

    // update display
    m_isDisplaySet = false;
    if (m_isInterlaced == false)
    {
        //! ... update display
        //...
    }
    else // interlaced
    {
        if (ch_dualInterlaceCheck &&
          ((dsp_prevFrame.displayPos.x != dsp_curFrame.displayPos.x) || (dsp_prevFrame.displayPos.y != dsp_curFrame.displayPos.y)))
            --ch_dualInterlaceCheck;
    }
}

/// <summary>Get GPU information (version, draw info, ...)</summary>
/// <param name="gdata">Type of request</param>
/// <returns>Information value</returns>
inline ubuffer_t DisplayState::getDrawInfo(ubuffer_t gdata)
{
    gdata &= 0x0FFu; // get type (extract last 8 bits)
    switch (gdata)
    {
        case REQ_TW:          return m_pDrawInfo[DRAWINFO_TW]; break;
        case REQ_DRAWSTART:   return m_pDrawInfo[DRAWINFO_DRAWSTART]; break;
        case REQ_DRAWEND:     return m_pDrawInfo[DRAWINFO_DRAWEND]; break;
        case REQ_DRAWOFFSET1:
        case REQ_DRAWOFFSET2: return m_pDrawInfo[DRAWINFO_DRAWOFF]; break;
        case REQ_GPUTYPE:     return (version() == 2) ? 0x1 : 0x2; break;
        case REQ_BIOSADDR1:
        case REQ_BIOSADDR2:   return PSX_BIOSADDR; break;
    }
    return 0;
}

/// <summary>Update display state</summary>
/// <param name="gdata">Display data</param>
inline void DisplayState::setDisplayState(ubuffer_t gdata)
{
    // set display width
    ch_displaySizePending.x = s_displayWidths[(gdata & 0x03u) | ((gdata & 0x40u) >> 4)];
    // set display height
    if (gdata & 0x04)
        dsp_heightMultiplier = 2;
    else
        dsp_heightMultiplier = 1;
    ch_displaySizePending.y = dsp_curFrame.height * dsp_heightMultiplier;

    //! ... change display offsets Y
    //...

    // set status width bits
    StatusRegister::unsetStatus(GPUSTATUS_WIDTHBITS);
    StatusRegister::setStatus((((gdata & 0x03u) << 17) | ((gdata & 0x40u) << 10)));

    // interlacing 
    if (gdata & 0x20) // enable
    {
        if (m_isInterlaced == false) // if different
        {
            ch_dualInterlaceCheck = 2;
            dsp_prevFrame.displayPos.x = dsp_curFrame.displayPos.x;
            dsp_prevFrame.displayPos.y = dsp_curFrame.displayPos.y;
            ch_interlacingPending = Change_enable;
        }
        StatusRegister::setStatus(GPUSTATUS_INTERLACED);
    }
    else // disable
    {
        if (m_isInterlaced) // if different
            ch_interlacingPending = Change_disable;
        ch_dualInterlaceCheck = 0;
        StatusRegister::unsetStatus(GPUSTATUS_INTERLACED);
    }
    // game localization
    if (gdata & 0x08)
    {
        m_region = Region_pal;
        StatusRegister::setStatus(GPUSTATUS_PAL);
    }
    else
    {
        m_region = Region_ntsc;
        StatusRegister::unsetStatus(GPUSTATUS_PAL);
    }
    // color depth
    if (gdata & 0x10)
    {
        ch_rgbModePending = Colormode_rgb24;
        StatusRegister::setStatus(GPUSTATUS_RGB24);
    }
    else
    {
        ch_rgbModePending = Colormode_rgb15;
        StatusRegister::unsetStatus(GPUSTATUS_RGB24);
    }
    // height multiplier status
    if (dsp_heightMultiplier == 2)
        StatusRegister::setStatus(GPUSTATUS_DOUBLEHEIGHT);
    else
        StatusRegister::unsetStatus(GPUSTATUS_DOUBLEHEIGHT);
}

#endif
