/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   display_state.h
Description : display state, data and settings
*******************************************************************************/
#ifndef _DISPLAY_STATE_H
#define _DISPLAY_STATE_H
#include <cstddef>
#include <cstdint>
#include "geometry.hpp"
#include "system_tools.h"

// data types
enum colormode_t : int32_t // color depth
{
    Colormode_rgb15 = 0,
    Colormode_rgb24 = 1
};
enum regionmode_t : int32_t // regional display modes
{
    Region_ntsc = 0,
    Region_pal = 1
};
typedef struct FRAMESTATUS // current/previous frame state
{
    bool isDisabled;
    colormode_t colorMode;
    wpoint_t displayPos;
    wpoint_t displayEnd;

    long        height;
    dimension_t drawOffset;
    rect_t      drawArea;
    rect_t      range;
} framestatus_t;

// draw info - request codes
#define REQ_TW                  0x02
#define REQ_DRAWSTART           0x03
#define REQ_DRAWEND             0x04
#define REQ_DRAWOFFSET1         0x05
#define REQ_DRAWOFFSET2         0x06
#define REQ_GPUTYPE             0x07
#define REQ_BIOSADDR1           0x08
#define REQ_BIOSADDR2           0x0F
// draw info - array
#define DRAWINFO_SIZE           16
#define DRAWINFO_TW             0
#define DRAWINFO_DRAWSTART      1
#define DRAWINFO_DRAWEND        2
#define DRAWINFO_DRAWOFF        3


// Display state, data and settings
class DisplayState
{
private:
    // general draw status and draw mode
    buffer_t m_pDrawInfo[DRAWINFO_SIZE]; // draw status
    regionmode_t m_region;      // region display mode (NTSC/PAL)
    bool m_isInterlaced;        // interlacing (on/off)

    // display flags
    bool m_isDisplaySet;
    long m_firstPositionFlag;  // indicates 'first position' drawing after startup
    uint32_t m_displayFlags;   // 00 -> digital, 01 -> analog, 02 -> mouse, 03 -> gun
    int m_oddFrameFlag;        // even/odd frame flag
    
    // display state
    wdimension_t dsp_dispSize;   // main display size
    long dsp_heightMultiplier;   // height multiplier (simple/double)
    framestatus_t dsp_curFrame;  // current operation status
    framestatus_t dsp_prevFrame; // previous operation status
    // pending changes
    wdimension_t ch_displaySizePending;// pending display size
    colormode_t  ch_rgbModePending;    // pending/current RGB mode
    change_t ch_interlacingPending;    // pending interlace mode
    long   ch_dualInterlaceCheck;  // make sure interlacing is complete
    long ch_prevHeightOffset;      // previous Y offset (display position)

public:
    static short s_displayWidths[8]; // available psx display widths


public:
    /// <summary>Initialize display state</summary>
    void init();


    // -- STATUS COMMANDS -- -------------------------------------------------------

    /// <summary>Reset GPU information</summary>
    inline void reset();
    /// <summary>Enable/disable display</summary>
    /// <param name="isDisabled">Display status</param>
    inline void toggleDisplay(bool isDisabled);

    /// <summary>Set display width</summary>
    /// <param name="pos">X Start/end range</param>
    inline void setWidth(point_t pos);
    /// <summary>Set display height</summary>
    /// <param name="pos">Y Start/end range</param>
    inline void setHeight(point_t pos);
    /// <summary>Set display position</summary>
    /// <param name="pos">Horizontal/vertical position</param>
    inline void setDisplayPos(point_t pos);

    /// <summary>Get GPU information (version, draw info, ...)</summary>
    /// <param name="gdata">Type of request (8 bits request ID)</param>
    /// <returns>Information value</returns>
    inline buffer_t getDrawInfo(ubuffer_t gdata);
    /// <summary>Update display state</summary>
    /// <param name="gdata">Display data</param>
    inline void setDisplayState(ubuffer_t gdata);

    /// <summary>Change even/odd frame status</summary>
    inline void toggleOddFrameFlag()
    {
        m_oddFrameFlag ^= 1;
    }
    /// <summary>Get even/odd frame status</summary>
    /// <returns>Odd frame flag value</returns>
    inline int getOddFrameFlag()
    {
        return m_oddFrameFlag;
    }
};

#endif
