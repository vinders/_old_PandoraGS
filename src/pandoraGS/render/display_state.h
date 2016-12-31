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
#include <string>
#include "geometry.hpp"
#include "status_register.h"
#include "system_tools.h"

// data types
typedef unsigned short gpuversion_t;
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
#define REQ_TW                  0x02u
#define REQ_DRAWSTART           0x03u
#define REQ_DRAWEND             0x04u
#define REQ_DRAWOFFSET1         0x05u
#define REQ_DRAWOFFSET2         0x06u
#define REQ_GPUTYPE             0x07u
#define REQ_BIOSADDR1           0x08u
#define REQ_BIOSADDR2           0x0Fu
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
    ubuffer_t m_pDrawInfo[DRAWINFO_SIZE]; // draw status
    regionmode_t m_region;      // region display mode (NTSC/PAL)
    bool m_isInterlaced;        // interlacing (on/off)
    gpuversion_t m_gpuVersion;  // GPU version type (for Zinc)

    // display flags
    bool m_isDisplaySet;
    long m_firstPositionFlag;  // indicates 'first position' drawing after startup
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
    long ch_dualInterlaceCheck;    // make sure interlacing is complete
    long ch_prevHeightOffset;      // previous Y offset (display position)

public:
    static short s_displayWidths[8]; // available psx display widths


public:
    /// <summary>Display state container creation</summary>
    DisplayState()
    {
        m_gpuVersion = 1;
        m_isDisplaySet = false;
        m_firstPositionFlag = 2L;
    }
    /// <summary>Initialize display state</summary>
    void init();

    /// <summary>Mark rendering state</summary>
    /// <param name="state">Rendering status</param>
    inline void set(bool state)
    {
        m_isDisplaySet = state;
    }
    /// <summary>Check if rendering is set</summary>
    /// <returns>Rendering status</returns>
    inline bool isSet()
    {
        return m_isDisplaySet;
    }


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
    /// <param name="gdata">Type of request</param>
    /// <returns>Information value</returns>
    inline ubuffer_t getDrawInfo(ubuffer_t gdata);
    /// <summary>Update display state</summary>
    /// <param name="gdata">Display data</param>
    inline void setDisplayState(ubuffer_t gdata);


    // -- BASIC GETTERS / SETTERS -- -----------------------------------------------

    /// <summary>Get GPU version</summary>
    /// <returns>GPU version</returns>
    inline gpuversion_t version()
    {
        return m_gpuVersion;
    }
    /// <summary>Get GPU version</summary>
    /// <param name="ver">GPU version</param>
    inline void setVersion(gpuversion_t ver)
    {
        m_gpuVersion = ver;
    }
    /// <summary>Get region mode (PAL / NTSC)</summary>
    /// <returns>Region mode</returns>
    inline regionmode_t getRegionmode()
    {
        return m_region;
    }
    /// <summary>Get interlacing mode</summary>
    /// <returns>Interlaced or not</returns>
    inline bool isInterlaced()
    {
        return m_isInterlaced;
    }

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
    /// <summary>Set even/odd frame status</summary>
    /// <param name="val">Odd frame flag value</returns>
    inline void setOddFrameFlag(int val)
    {
        m_oddFrameFlag = val;
    }

    /// <summary>Get main display size</summary>
    /// <returns>Display size (x, y)</returns>
    inline wdimension_t getDisplaySize()
    {
        return dsp_dispSize;
    }
    /// <summary>Get height multiplier</summary>
    /// <returns>Simple / double</returns>
    inline long getHeightMultiplier()
    {
        return dsp_heightMultiplier;
    }
    /// <summary>Get current frame state</summary>
    /// <returns>Frame state</returns>
    inline framestatus_t getCurrentFrame()
    {
        return dsp_curFrame;
    }
};

#include "display_state.cpp" // inline definitions
#endif
