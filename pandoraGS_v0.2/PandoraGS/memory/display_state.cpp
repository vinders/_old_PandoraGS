/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   display_state.cpp
Description : display state, data and settings
*******************************************************************************/
#include "globals.h"
#ifndef _DISPLAY_STATE_H
using namespace std;
#endif
#include "config.h"
#include "status_register.h"

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
    // ...
    dsp_curFrame.colorMode = Colormode_rgb15;
    m_isInterlaced = false;
}

/// <summary>Enable/disable display</summary>
/// <param name="isDisabled">Display status</param>
inline void DisplayState::toggleDisplay(bool isDisabled)
{

}

/// <summary>Set display width</summary>
/// <param name="pos">X Start/end range</param>
inline void DisplayState::setWidth(point_t pos)
{

}

/// <summary>Set display height</summary>
/// <param name="pos">Y Start/end range</param>
inline void DisplayState::setHeight(point_t pos)
{

}

/// <summary>Set display position</summary>
/// <param name="pos">Horizontal/vertical position</param>
inline void DisplayState::setDisplayPos(point_t pos)
{

}

/// <summary>Get GPU information (version, draw info, ...)</summary>
/// <param name="gdata">Type of request</param>
/// <returns>Information value</returns>
inline ubuffer_t DisplayState::getDrawInfo(ubuffer_t gdata)
{
    gdata &= 0x0FFu; // get type (extract last 8 bits)
    switch (gdata)
    {
        case REQ_TW:          return m_pDrawInfo[DRAWINFO_TW]; return;
        case REQ_DRAWSTART:   return m_pDrawInfo[DRAWINFO_DRAWSTART]; return;
        case REQ_DRAWEND:     return m_pDrawInfo[DRAWINFO_DRAWEND]; return;
        case REQ_DRAWOFFSET1:
        case REQ_DRAWOFFSET2: return m_pDrawInfo[DRAWINFO_DRAWOFF]; return;
        case REQ_GPUTYPE:     return (version() == 2) ? 0x1 : 0x2; return;
        case REQ_BIOSADDR1:
        case REQ_BIOSADDR2:   return PSX_BIOSADDR; return;
    }
    return 0;
}

/// <summary>Update display state</summary>
/// <param name="gdata">Display data</param>
inline void DisplayState::setDisplayState(ubuffer_t gdata)
{

}
