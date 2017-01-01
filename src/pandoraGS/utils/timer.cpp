/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   timer.cpp
Description : frame time management
*******************************************************************************/
#ifdef _WINDOWS
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#endif
#include <cstdlib>
#include <ctime>
using namespace std;
#include "timer.h"

#define MAX_SUCCESSIVE_SKIPPING 4

// timing mode
#ifdef _WINDOWS
LARGE_INTEGER Timer::s_cpuFreq; // CPU ticks frequency
ticks_t Timer::s_periodHiRes;   // high res period duration
#endif
ticks_t Timer::s_period;            // CPU ticks per period
ticks_t Timer::s_periodTimeout;     // max waited time before skipping (7/8 period time)
float Timer::s_targetFreq = 0.0f;   // configured frequency
timemode_t Timer::s_timeMode;       // timing mode
bool Timer::s_isInterlaced = false; // interlaced periods
skipmode_t Timer::s_skipMode = Skipmode_disabled; // period skipping mode

// timer execution
bool Timer::s_isResetPending = true;  // time reference reset query
float Timer::s_measuredFreq = 0.0f;   // current measured frequency
int Timer::s_periodsSinceMeasure = 0; // number of periods since last check
int Timer::s_periodsToSkip = 0;       // number of periods to skip (to stay in sync)
ticks_t Timer::s_lateTicks = 0uL;     // current lateness (compared to time reference)


// -- TIMER SETTINGS -- ----------------------------------------------------

/// <summary>Set time management mode</summary>
void Timer::setTimeMode(timemode_t type)
{
    s_isInterlaced = false;
    s_periodsToSkip = 0;
    s_skipMode = Skipmode_disabled;

    #ifdef _WINDOWS
    if (type == Timemode_highResCounter && QueryPerformanceFrequency(&s_cpuFreq) == false)
        s_timeMode = Timemode_multimediaClock;
    else
        s_timeMode = type;
    #else
    s_timeMode = Timemode_multimediaClock;
    #endif
}

/// <summary>Set timer frequency</summary>
/// <param name="freqLimit">Frequency max limit</param>
/// <param name="regMode">Regional sync mode</param>
/// <param name="isInterlaced">Interlacing</param>
void Timer::setFrequency(float freqLimit, regionsync_t regMode, bool isInterlaced)
{
    s_isInterlaced = isInterlaced;
    
    // set fixed framerate limit
    s_targetFreq = 59.94f;
    if (freqLimit > 0.02f) // != 0 (+ float error offset) -> manual
    {
        s_targetFreq = freqLimit;
    }
    // set auto framerate limit
    else if (regMode != Regionsync_undefined)
    {
        if (regMode == Regionsync_ntsc)
        {
            if (s_isInterlaced)
                s_targetFreq = 33868800.0f / 565031.25f; // 59.94146
            else
                s_targetFreq = 33868800.0f / 566107.50f; // 59.82750
        }
        else // Regionsync_pal
        {
            if (s_isInterlaced)
                s_targetFreq = 33868800.0f / 677343.75f; // 50.00238
            else
                s_targetFreq = 33868800.0f / 680595.00f; // 49.76351
        }
    }

    // set period duration + timeout
    double duration = 1000.0 / (double)s_targetFreq;
    s_period = (ticks_t)duration;
    #ifdef _WINDOWS
    if (s_timeMode == Timemode_highResCounter)
    {
        double durationHiRes = (double)(s_cpuFreq.LowPart) / (double)s_targetFreq;
        s_periodHiRes = (ticks_t)durationHiRes;
        s_periodTimeout = ((7 * s_periodHiRes) >> 3);
    }
    else
        s_periodTimeout = ((7 * s_period) >> 3);
    #else
    s_periodTimeout = ((7 * s_period) >> 3);
    #endif

    // reset frame skipping
    s_periodsToSkip = 0;
    s_lateTicks = 0uL;
    // reset frame management values
    s_measuredFreq = 0.0f;
    s_periodsSinceMeasure = 0;
    s_isResetPending = true;
}


// -- TIME OPERATIONS -- ---------------------------------------------------

/// <summary>Wait for one period (after previous time reference) + skipping</summary>
/// <param name="isLimited">Limit frequency</param>
/// <param name="speedChange">Speed modifier (normal/slow/fast)</param>
/// <param name="isOddPeriod">Odd period (if interlaced)</param>
void Timer::wait(bool isLimited, speed_t speedChange, bool isOddPeriod)
{
    #ifdef _WINDOWS
    static LARGE_INTEGER lastTimeRef, currentTimeRef;
    #endif
    static ticks_t lastTicks, currentTicks, elapsedTicks, ticksToWait = 0;

    // reset time reference
    if (s_isResetPending)
    {
        s_isResetPending = false;
        #ifdef _WINDOWS
        if (s_timeMode == Timemode_highResCounter)
            QueryPerformanceCounter(&lastTimeRef);
        #endif
        lastTicks = timeGetTime();
        s_lateTicks = ticksToWait = 0;
        return;
    }
    ++s_periodsSinceMeasure;

    // speed modifiers
    if (speedChange)
    {
        if (speedChange == Speed_fast)
        {
            // skip 3 frames out of 4
            int nextPeriodsToSkip = 0;
            static int fastIterations = 0;
            if (++fastIterations < 4)
                nextPeriodsToSkip = 1;
            else
                fastIterations = 0;

            // set period skipping
            if (s_periodsToSkip > 0) // current period was skipped -> no wait
            {
                s_periodsToSkip = nextPeriodsToSkip;
                return;
            }
            s_periodsToSkip = 1 + nextPeriodsToSkip; // bypass skipping calculations (will decrement instead of calculating)
        }
        else // Speed_slow
        {
            // double period time
            #ifdef _WINDOWS
            if (s_timeMode == Timemode_highResCounter)
                ticksToWait += s_periodHiRes;
            else
                ticksToWait += s_period;
            #else
            ticksToWait += s_period;
            #endif
            s_periodsToSkip = 1; // bypass skipping calculations (will decrement instead of calculating)
        }
    }

    // apply frequency limit
    if (isLimited)
    {
        #ifdef _WINDOWS
        // high resolution time sync (qpc)
        if (s_timeMode == Timemode_highResCounter)
        {
            unsigned long waitLoopCount = 0uL;
            do
            {
                // get current time reference
                QueryPerformanceCounter(&currentTimeRef);
                currentTicks = timeGetTime();
                elapsedTicks = currentTimeRef.LowPart - lastTimeRef.LowPart;

                // "more" than 0,5 sec elapsed (negative offset) -> use mm timer instead
                if (elapsedTicks > (s_cpuFreq.LowPart >> 1))
                {
                    if (currentTicks < lastTicks) // max integer value reached -> start from 0
                        elapsedTicks = s_periodHiRes + ticksToWait + 1;
                    else
                        elapsedTicks = (s_cpuFreq.LowPart * (currentTicks - lastTicks)) / 1000;
                }
                waitLoopCount++;
            }
            while (elapsedTicks <= ticksToWait && currentTimeRef.LowPart >= lastTimeRef.LowPart);

            // adjust next period duration
            if (waitLoopCount == 1uL) // time elapsed > frame time
            {
                s_lateTicks = elapsedTicks - ticksToWait;
                if (s_lateTicks > s_periodHiRes)
                {
                    ticksToWait = 0;
                    if (currentTimeRef.LowPart < lastTimeRef.LowPart)
                        s_lateTicks = 0;
                }
                else
                    ticksToWait = s_periodHiRes - s_lateTicks;
            }
            else // time elapsed <= frame time
            {
                ticksToWait = s_periodHiRes;
                s_lateTicks = 0;
            }

            // copy time reference for next update
            lastTimeRef.HighPart = currentTimeRef.HighPart;
            lastTimeRef.LowPart = currentTimeRef.LowPart;
            lastTicks = currentTicks;
        }

        // low resolution time sync (mm timer)
        else
        {
            #endif
            unsigned long waitCount = 0uL;
            do
            {
                // get current time reference
                currentTicks = timeGetTime();
                elapsedTicks = currentTicks - lastTicks;
                waitCount++;
            } 
            while (elapsedTicks <= ticksToWait && currentTicks >= lastTicks);

            // adjust next period duration
            if (waitCount == 1uL) // time elapsed > frame time
            {
                s_lateTicks = elapsedTicks - ticksToWait;
                if (s_lateTicks > s_period)
                {
                    ticksToWait = 0;
                    if (currentTicks < lastTicks)
                        s_lateTicks = 0;
                }
                else
                    ticksToWait = s_period - s_lateTicks;
            }
            else // time elapsed <= frame time
            {
                ticksToWait = s_period;
                s_lateTicks = 0;
            }

            // copy time reference for next update
            lastTicks = currentTicks;
            #ifdef _WINDOWS
        }
        #endif
    }

    // skipping
    if (s_periodsToSkip > 0) // if current was skipped, don't check time
        s_periodsToSkip--;
    else if (s_skipMode != Skipmode_disabled)
    {
        // standard skipping mode
        if (s_skipMode == Skipmode_standard)
        {
            if (s_lateTicks >= s_periodTimeout)
            {
                 // count number of periods missed
                float latePeriods;
                #ifdef _WINDOWS
                if (s_timeMode == Timemode_highResCounter)
                    latePeriods = (float)s_lateTicks / (float)s_periodHiRes;
                else
                    latePeriods = (float)s_lateTicks / (float)s_period;
                #else
                latePeriods = (float)s_lateTicks / (float)s_frameDuration;
                #endif

                if (s_isInterlaced) // interlacing
                {
                    if (isOddPeriod == false) // always skip 'even' period first
                    {
                        s_periodsToSkip = (int)latePeriods;
                        // set skipping
                        if (s_periodsToSkip > MAX_SUCCESSIVE_SKIPPING) // keep at least 2 periods out of 6
                            s_periodsToSkip = MAX_SUCCESSIVE_SKIPPING;
                        else if (s_periodsToSkip % 2 != 0) // skip by groups of 2
                            ++s_periodsToSkip;
                    }
                }
                else // not interlaced
                {
                    latePeriods += 0.15f;
                    s_periodsToSkip = (int)latePeriods;
                    if (s_periodsToSkip >= MAX_SUCCESSIVE_SKIPPING) // keep at least 1 period out of 4
                        s_periodsToSkip = MAX_SUCCESSIVE_SKIPPING - 1;
                }
            }
        }
        // skip max one period out of two
        else
        {
            if (s_isInterlaced) // interlacing -> keep 2, skip 2
            {
                if (s_lateTicks > (s_periodTimeout / 8.0f) && isOddPeriod == false)
                    s_periodsToSkip = 2;
            }
            else if (s_lateTicks > (s_periodTimeout / 16.0f)) // keep 1, skip 1
                s_periodsToSkip = 1;
        }
        s_lateTicks = 0;
    }
}

/// <summary>Calculate current effective frequency</summary>
void Timer::calcFrequency()
{
    #ifdef _WINDOWS
    static LARGE_INTEGER prevTimeRef, curTimeRef;
    #endif
    static ticks_t prevTicks, curTicks;

    // high resolution counter
    #ifdef _WINDOWS
    if (s_timeMode == Timemode_highResCounter)
    {
        // get current time reference
        QueryPerformanceCounter(&curTimeRef);
        curTicks = timeGetTime();
        ticks_t elapsedTicks = curTimeRef.LowPart - prevTimeRef.LowPart;
        if (elapsedTicks > (s_cpuFreq.LowPart >> 1)) // max value exceeded -> use low resolution counter
            elapsedTicks = (s_cpuFreq.LowPart * (curTicks - prevTicks)) / 1000;
        if (elapsedTicks <= 0uL) // avoid division by zero
            elapsedTicks = 1uL;

        // calculate frequency
        s_measuredFreq = (float)s_periodsSinceMeasure * ((float)s_cpuFreq.LowPart / (float)elapsedTicks);

        prevTimeRef.HighPart = curTimeRef.HighPart;
        prevTimeRef.LowPart = curTimeRef.LowPart;
    }
    // low resolution counter
    else
    #endif
    {
        // get current time reference
        curTicks = timeGetTime();
        ticks_t elapsedTicks = curTicks - prevTicks;
        if (elapsedTicks <= 0uL) // avoid division by zero
            elapsedTicks = 1uL;
        // calculate frequency
        s_measuredFreq = (float)s_periodsSinceMeasure * (1000.0f / (float)elapsedTicks);
    }

    prevTicks = curTicks;
    s_periodsSinceMeasure = 0;
}

#ifndef _WINDOWS
/// <summary>Linux-Unix compatible time system</summary>
unsigned long timeGetTime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec * 100000 + tv.tv_usec / 10;
}
#endif
