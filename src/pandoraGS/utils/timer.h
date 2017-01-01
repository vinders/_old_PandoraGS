/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   timer.h
Description : frame time management
*******************************************************************************/
#ifndef _TIMER_H
#define _TIMER_H
#include <cstdint>

// data types
#ifdef _WINDOWS
#include <Windows.h>
typedef DWORD ticks_t;
#else
typedef unsigned long ticks_t;
#endif
enum timemode_t : uint32_t // timing modes
{
    Timemode_multimediaClock = 0u, // low res, steady
    Timemode_highResCounter = 1u   // high res, unstable
};
enum regionsync_t : int32_t // regional sync modes
{
    Regionsync_undefined = -1,
    Regionsync_ntsc = 0,
    Regionsync_pal = 1
};
enum speed_t : int32_t // speed changes
{
    Speed_slow = -1,
    Speed_normal = 0,
    Speed_fast = 1
};
enum skipmode_t : uint32_t // skipping modes
{
    Skipmode_disabled = 0u,
    Skipmode_standard = 1u,
    Skipmode_half = 2u
};


// Frame time management
class Timer
{
private:
    // timing mode
#ifdef _WINDOWS
    static LARGE_INTEGER s_cpuFreq; // CPU ticks frequency
    static ticks_t s_periodHiRes;   // high res period duration
#endif
    static ticks_t s_period;        // CPU ticks per period
    static ticks_t s_periodTimeout; // max waited time before skipping (7/8 period time)
    static float s_targetFreq;      // configured frequency
    static timemode_t s_timeMode;   // timing mode
    static bool s_isInterlaced;     // interlaced periods
    static skipmode_t s_skipMode;   // period skipping mode

    // timer execution
    static bool s_isResetPending;     // time reference reset query
    static float s_measuredFreq;      // current measured frequency
    static int s_periodsSinceMeasure; // number of periods since last check
    static int s_periodsToSkip;       // number of periods to skip (to stay in sync)
    static ticks_t s_lateTicks;       // current lateness (compared to time reference)


public:
    // -- TIMER SETTINGS -- ----------------------------------------------------

    /// <summary>Set time management mode</summary>
    static void setTimeMode(timemode_t type);
    /// <summary>Set timer frequency</summary>
    /// <param name="freqLimit">Frequency max limit</param>
    /// <param name="regMode">Regional sync mode</param>
    /// <param name="isInterlaced">Interlacing</param>
    static void setFrequency(float freqLimit, regionsync_t regMode, bool isInterlaced);
    /// <summary>Set period skipping mode</summary>
    /// <param name="isEnabled">Enable skipping</param>
    /// <param name="isHalfSkip">Skip max one period out of two</param>
    static void setSkippingMode(bool isEnabled, bool isHalfSkip)
    {
        if (isEnabled)
            s_skipMode = (isHalfSkip) ? Skipmode_half : Skipmode_standard;
        else
            s_skipMode = Skipmode_disabled;
    }

    /// <summary>Reset time reference</summary>
    static inline void resetTimeReference()
    {
        if (s_periodsToSkip > 0 && s_isInterlaced)
            s_periodsToSkip--;
        else // current period is skipped -> if interlaced, skip next too
            s_periodsToSkip = (s_isInterlaced) ? 1 : 0;
        s_lateTicks = 0;
        s_isResetPending = true;
    }


    // -- TIME OPERATIONS -- ---------------------------------------------------

    /// <summary>Wait for one period (after previous time reference) + skipping</summary>
    /// <param name="isLimited">Limit frequency</param>
    /// <param name="speedChange">Speed modifier (normal/slow/fast)</param>
    /// <param name="isOddPeriod">Odd period (if interlaced)</param>
    static void wait(bool isLimited, speed_t speedChange, bool isOddPeriod);

    /// <summary>Calculate current effective frequency</summary>
    static void calcFrequency();
    /// <summary>Get current frames per second</summary>
    /// <returns>Current FPS value</returns>
    static inline float getMeasuredFrequency()
    {
        return s_measuredFreq;
    }
    /// <summary>Check if current period processing should be skipped</summary>
    /// <returns>Skip indicator (boolean)</returns>
    static inline bool isPeriodSkipped()
    {
        return (s_periodsToSkip > 0);
    }
};

#ifndef _WINDOWS
/// <summary>Linux-Unix compatible time system</summary>
unsigned long timeGetTime();
#endif

#endif
