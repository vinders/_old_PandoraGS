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

    /// <summary>Framerate limiter/sync</summary>
    /// <param name="isSlowedDown">Slow motion mode</param>
    static void waitFrameTime(bool isSlowedDown);
protected:
    /// <summary>Calculate current frames per second</summary>
    static void checkCurrentFramerate();
public:
    /// <summary>Get current frames per second</summary>
    /// <returns>Current FPS value</returns>
    static float getCurrentFramerate()
    {
        return s_currentFps;
    }


    // -- FRAME SKIPPING -- ----------------------------------------------------

    /// <summary>Cancel planified frame skips</summary>
    static void resetFrameSkipping()
    {
        s_lateTicks = 0;
        s_isReset = true;
    }
    /// <summary>Signal that a new frame is being drawn (data has been read)</summary>
    static void signalFrameDrawing()
    {
        s_framesAfterFpsRef++;
        s_isFrameDataWaiting = false;
    }

    /// <summary>Check if current frame should be skipped</summary>
    /// <param name="isOddLine">Odd line (if interlaced)</param>
    /// <returns>Skip indicator (boolean)</returns>
    static bool isFrameSkipped(bool isOddLine);
    /// <summary>Wait until current frame data is read</summary>
    static void checkFrameStarted();
};

#endif
