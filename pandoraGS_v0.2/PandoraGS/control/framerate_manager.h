/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   framerate_manager.h
Description : framerate and sync management toolbox
*******************************************************************************/
#ifndef _FRAMERATE_MANAGER_H
#define _FRAMERATE_MANAGER_H
#include "globals.h"

#include "config.h"

#ifndef _WINDOWS
#define LARGE_INTEGER unsigned long
#endif


// Framerate and sync management toolbox
class FramerateManager
{
private:
    // framerate mode
    #ifdef _WINDOWS
    static LARGE_INTEGER s_cpuFrequency; // CPU ticks frequency
    static DWORD s_frameDurationHiRes;   // high res frame duration
    static DWORD s_frameDuration;        // ticks per frame
    static DWORD s_maxFrameWait;         // max wait time before skipping (7/8 frame time)
    #else
    static unsigned long s_frameDuration;// ticks per frame
    static unsigned long s_maxFrameWait; // max wait time before skipping (7/8 frame time)
    #endif
    static bool s_isInterlaced;          // interlaced frames
    static bool s_isSkipped;             // skip current frame
    static bool s_isReset;               // init indicator

    // frame skipping
    static bool s_isDelayedFrame;        // delay next frame sync
    static bool s_isPrevSkippedOdd;      // last skipped frame with odd lines (interlacing)
    static bool s_isFrameDataWaiting;    // frame data not read
    #ifdef _WINDOWS
    static DWORD s_lateTicks;            // time spent waiting (in addition to normal frame time)
    #else
    static unsigned long s_lateTicks;    // time spent waiting (in addition to normal frame time)
    #endif

    // fps counter
    static float s_currentFps;           // framerate indicator
    static unsigned int s_framesAfterFpsRef; // number of frames displayed since last check


public:
    // -- FRAMERATE SETTINGS -- ------------------------------------------------

    /// <summary>Initialize framerate management mode</summary>
    static void initFramerate();
    /// <summary>Set framerate values based on config</summary>
    /// <param name="hasFrameInfo">Frame information is available (interlace, locale)</param>
    static void setFramerate(bool hasFrameInfo);


    // -- FRAMERATE MANAGEMENT -- ----------------------------------------------

    /// <summary>Frame rate limiter/sync + check frame skipping</summary>
    /// <param name="frameSpeed">Speed modifier (normal/slow/fast)</param>
    /// <param name="isOddFrame">Odd line (if interlaced)</param>
    static void waitFrameTime(int frameSpeed, bool isOddFrame);
protected:
    /// <summary>Calculate current frames per second</summary>
    static void checkCurrentFramerate();
public:
    /// <summary>Get current frames per second</summary>
    /// <returns>Current FPS value</returns>
    static inline float getCurrentFramerate()
    {
        return s_currentFps;
    }


    // -- FRAME SKIPPING -- ----------------------------------------------------

    /// <summary>Cancel planified frame skips</summary>
    static inline void resetFrameSkipping()
    {
        s_lateTicks = 0;
        s_isReset = true;
        s_isSkipped = false;
    }
    /// <summary>Signal that a new frame is being drawn (data has been read)</summary>
    static inline void signalFrameDrawing()
    {
        s_framesAfterFpsRef++;
        s_isFrameDataWaiting = false;
    }

    /// <summary>Check if current frame is skipped</summary>
    /// <returns>Skip indicator (boolean)</returns>
    static inline bool isFrameSkipped()
    {
        return s_isSkipped;
    }
};

#endif
