/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   framerate_manager.cpp
Description : framerate and sync management toolbox
*******************************************************************************/
using namespace std;
#include "framerate_manager.h"
#include "psx_core_memory.h"

#define MAX_LACE_NUMBER 16
#define MAX_SUCCESSIVE_SKIPPING 4
extern Config* g_pConfig;
extern PsxCoreMemory* g_pMemory;
float g_framerateHz = 59.94f;

// framerate mode
#ifdef _WINDOWS
LARGE_INTEGER FramerateManager::s_cpuFrequency;     // CPU ticks frequency
DWORD FramerateManager::s_frameDurationHiRes;       // high res frame duration
DWORD FramerateManager::s_frameDuration;            // ticks per frame
DWORD FramerateManager::s_maxFrameWait;             // max wait time before skipping (7/8 frame time)
#else
unsigned long FramerateManager::s_frameDuration;    // ticks per frame
unsigned long FramerateManager::s_maxFrameWait;     // max wait time before skipping (7/8 frame time)
#endif
bool FramerateManager::s_isInterlaced = false;      // interlaced frames
bool FramerateManager::s_isReset = true;            // init indicator

// frame skipping
bool FramerateManager::s_isDelayedFrame = false;    // delay next frame sync
bool FramerateManager::s_isPrevSkippedOdd = true;   // last skipped frame with odd lines (interlacing)
bool FramerateManager::s_isFrameDataWaiting = false;// frame data not read
#ifdef _WINDOWS
DWORD FramerateManager::s_lateTicks;                // time spent waiting (in addition to normal frame time)
#else
unsigned long FramerateManager::s_lateTicks;        // time spent waiting (in addition to normal frame time)
#endif

// fps counter
float FramerateManager::s_currentFps = 0.0f;        // framerate indicator
unsigned int FramerateManager::s_framesAfterFpsRef = 0; // number of frames displayed since last check


// -- FRAMERATE SETTINGS -- ----------------------------------------------------

/// <summary>Initialize framerate management mode</summary>
void FramerateManager::initFramerate()
{
    s_isInterlaced = false;

    #ifdef _WINDOWS
    if (g_pConfig->sync_timeMode == TimingMode_HighResCounter
        && QueryPerformanceFrequency(&s_cpuFrequency) == false)
    {
        g_pConfig->sync_timeMode = TimingMode_MultimediaClock;
    }
    #else
    g_pConfig->sync_timeMode = TimingMode_MultimediaClock;
    #endif
}

/// <summary>Set framerate values based on config</summary>
/// <param name="hasFrameInfo">Frame information is available (interlace, locale)</param>
void FramerateManager::setFramerate(bool hasFrameInfo)
{
    // check interlacing
    s_isInterlaced = false;
    if (hasFrameInfo)
    {
        if (g_pConfig->getCurrentProfile()->sync_hasFixAutoLimit)
            s_isInterlaced = g_pMemory->m_PSXDisplay.Interlaced;
        else
            s_isInterlaced = g_pMemory->getStatus(GPUSTATUS_INTERLACED);
    }
    
    // set fixed framerate limit
    g_framerateHz = 59.94f;
    if (g_pConfig->sync_framerateLimit > 0.05f) // 0 + float error offset
    {
        g_framerateHz = g_pConfig->sync_framerateLimit;
    }
    // set auto framerate limit
    else if (hasFrameInfo)
    {
        // use theoretical standard values
        if (g_pConfig->getCurrentProfile()->sync_hasFixAutoLimit)
        {
            if (g_pMemory->sync_localize == LocalizationMode_Pal)
                g_framerateHz = (s_isInterlaced) ? 50.0f : 25.0f;
            else 
                g_framerateHz = (s_isInterlaced) ? 60.0f : 30.0f;
        }
        else // emulation values
        {
            if (g_pMemory->sync_localize == LocalizationMode_Pal)
            {
                if (s_isInterlaced)
                    g_framerateHz = 33868800.0f / 677343.75f; // 50.00238
                else 
                    g_framerateHz = 33868800.0f / 680595.00f; // 49.76351
            }
            else
            {
                if (s_isInterlaced)
                    g_framerateHz = 33868800.0f / 565031.25f; // 59.94146
                else 
                    g_framerateHz = 33868800.0f / 566107.50f; // 59.82750
            }
        }
    }

    // set frame duration
    double duration = 1000.0 / (double)g_framerateHz;
    #ifdef _WINDOWS
    s_frameDuration = (DWORD)duration;
    if (g_pConfig->sync_timeMode == TimingMode_HighResCounter)
    {
        double durationHiRes = (double)s_cpuFrequency.LowPart / (double)g_framerateHz;
        s_frameDurationHiRes = (DWORD)durationHiRes;
    }
    #else
    s_frameDuration = (unsigned long)duration;
    #endif

    // set frame timeout
    #ifdef _WINDOWS
    if (g_pConfig->sync_timeMode == TimingMode_HighResCounter)
        s_maxFrameWait = ((7 * s_frameDurationHiRes) >> 3);
    else
    #endif
        s_maxFrameWait = ((7 * s_frameDuration) >> 3);

    // reset frame skipping
    s_isPrevSkippedOdd = true;
    s_isDelayedFrame = false;
    s_lateTicks = 0;
    // reset frame management values
    s_currentFps = 0.0f;
    s_framesAfterFpsRef = 0;
    s_isReset = true;
}


// -- FRAMERATE MANAGEMENT -- --------------------------------------------------

/// <summary>Framerate limiter/sync</summary>
/// <param name="isSlowedDown">Slow motion mode</param>
void FramerateManager::waitFrameTime(bool isSlowedDown)
{
    // set FPS indicator
    static unsigned int laceCount = MAX_LACE_NUMBER;
    if (++laceCount >= MAX_LACE_NUMBER)
    {
        if (g_pConfig->rnd_isFpsDisplayed)
            checkCurrentFramerate();
        laceCount = 0;
    }

    // apply framerate limit
    if (g_pConfig->sync_isFrameLimit)
    {
        #ifdef _WINDOWS
        static DWORD lastTicks, currentTicks, elapsedTicks, ticksToWait = 0;
        static LARGE_INTEGER lastTimeRef, currentTimeRef;
        #else
        static unsigned long lastTicks, currentTicks, ticksToWait = 0;
        #endif

        // initialize first time reference
        if (s_isReset)
        {
            s_isReset = false;
            #ifdef _WINDOWS
            if (g_pConfig->sync_timeMode == TimingMode_HighResCounter)
                QueryPerformanceCounter(&lastTimeRef);
            #endif
            lastTicks = timeGetTime();
            ticksToWait = 0;
            s_lateTicks = 0;
            return;
        }

        #ifdef _WINDOWS
        // high resolution time sync (qpc)
        if (g_pConfig->sync_timeMode == TimingMode_HighResCounter)
        {
            // too slow + no frame skipping -> slow down to achieve steady framerate
            if (s_isDelayedFrame)
            {
                s_isDelayedFrame = false;
                ticksToWait += (s_frameDurationHiRes >> 4); // 1.0625x frame time -> fps: 60->56.4 / 50->47.1
            }

            // wait for frame duration
            static unsigned long waitLoopCount;
            waitLoopCount = 0uL;
            do
            {
                // get current time reference
                QueryPerformanceCounter(&currentTimeRef);
                currentTicks = timeGetTime();
                elapsedTicks = currentTimeRef.LowPart - lastTimeRef.LowPart;

                // more than 0,5 sec elapsed (negative offset) -> use mm timer
                if (elapsedTicks > (s_cpuFrequency.LowPart >> 1))
                {
                    if (currentTicks < lastTicks) // max integer value reached -> start from 0
                        elapsedTicks = s_frameDurationHiRes + ticksToWait + 1;
                    else
                        elapsedTicks = (s_cpuFrequency.LowPart * (currentTicks - lastTicks)) / 1000;
                }
                waitLoopCount++;
            }
            while (elapsedTicks <= ticksToWait && currentTimeRef.LowPart >= lastTimeRef.LowPart);

            // set next frame duration
            if (waitLoopCount == 1uL) // time elapsed > frame time
            {
                s_lateTicks = elapsedTicks - ticksToWait;
                if (s_lateTicks > s_frameDurationHiRes)
                {
                    ticksToWait = 0;
                    if (currentTimeRef.LowPart < lastTimeRef.LowPart)
                        s_lateTicks = 0;
                }
                else
                    ticksToWait = s_frameDurationHiRes - s_lateTicks;
            }
            else // time elapsed <= frame time
            {
                ticksToWait = s_frameDurationHiRes;
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
            // too slow + no frame skipping -> slow down to achieve steady framerate
            if (s_isDelayedFrame)
            {
                s_isDelayedFrame = false;
                ticksToWait += (s_frameDuration >> 4); // 1.0625x frame time -> fps: 60->56.4 / 50->47.1
            }

            // wait for frame duration
            static unsigned long waitCount;
            waitCount = 0uL;
            do
            {
                // get current time reference
                currentTicks = timeGetTime();
                elapsedTicks = currentTicks - lastTicks;
                waitCount++;
            } while (elapsedTicks <= ticksToWait && currentTicks >= lastTicks);

            // set next frame duration
            if (waitCount == 1uL) // time elapsed > frame time
            {
                s_lateTicks = elapsedTicks - ticksToWait;
                if (s_lateTicks > s_frameDuration)
                {
                    ticksToWait = 0;
                    if (currentTicks < lastTicks)
                        s_lateTicks = 0;
                }
                else
                    ticksToWait = s_frameDuration - s_lateTicks;
            }
            else // time elapsed <= frame time
            {
                ticksToWait = s_frameDuration;
                s_lateTicks = 0;
            }

            // copy time reference for next update
            lastTicks = currentTicks;
            #ifdef _WINDOWS
        }
        #endif
    }

    // slow motion mode -> wait more
    if (isSlowedDown)
    {
        // set time ref
        #ifdef _WINDOWS
        DWORD timeToWait;
        #else
        unsigned long timeToWait;
        #endif
        timeToWait = 2 * s_frameDuration;

        // wait
        DWORD currentTime = timeGetTime();
        DWORD targetTime = currentTime + timeToWait;
        do
        {
            currentTime = timeGetTime();
        }
        while (currentTime < targetTime && currentTime > timeToWait);
        s_lateTicks = 0;
    }
}


/// <summary>Calculate current frames per second</summary>
void FramerateManager::checkCurrentFramerate()
{
    static float framerateTmp;
    static unsigned int framesAfterFpsRef = 0;
    #ifdef _WINDOWS
    static DWORD lastCheckTicks, currentCheckTicks, elapsedTicks;
    static LARGE_INTEGER lastCheckTimeRef, currentCheckTimeRef;
    #else
    static unsigned long lastCheckTicks, currentCheckTicks, elapsedTicks;
    #endif

    // number of frames since last check
    framesAfterFpsRef = s_framesAfterFpsRef; // non global copy
    s_framesAfterFpsRef = 0;

    // high resolution counter (qpc)
    #ifdef _WINDOWS
    if (g_pConfig->sync_timeMode == TimingMode_HighResCounter)
    {
        // get current time reference
        QueryPerformanceCounter(&currentCheckTimeRef);
        currentCheckTicks = timeGetTime();
        elapsedTicks = currentCheckTimeRef.LowPart - lastCheckTimeRef.LowPart;
        if (elapsedTicks > (s_cpuFrequency.LowPart >> 1))
            elapsedTicks = (s_cpuFrequency.LowPart * (currentCheckTicks - lastCheckTicks)) / 1000;
        // calculate FPS value
        framerateTmp = (float)framesAfterFpsRef * ((float)s_cpuFrequency.LowPart / (float)elapsedTicks);

        // copy current time for next update
        lastCheckTimeRef.HighPart = currentCheckTimeRef.HighPart;
        lastCheckTimeRef.LowPart = currentCheckTimeRef.LowPart;
        lastCheckTicks = currentCheckTicks;
    }
    // low resolution counter (mm)
    else
    {
        #endif
        // get current time reference
        currentCheckTicks = timeGetTime();
        elapsedTicks = currentCheckTicks - lastCheckTicks;
        // calculate FPS value
        framerateTmp = (float)framesAfterFpsRef * (1000.0f / (float)elapsedTicks);

        // copy current ticks for next update
        lastCheckTicks = currentCheckTicks;
        #ifdef _WINDOWS
        }
    #endif

    // set FPS value
    if (g_pConfig->sync_isFrameLimit && framerateTmp > g_framerateHz) // avoid flickering
        framerateTmp = g_framerateHz;
    s_currentFps = framerateTmp;
}


// -- FRAME SKIPPING -- --------------------------------------------------------

/// <summary>Check if current frame should be skipped</summary>
/// <param name="isOddLine">Odd frame (if interlaced)</param>
/// <returns>Skip indicator (boolean)</returns>
bool FramerateManager::isFrameSkipped(bool isOddLine)
{
    if (g_pConfig->sync_isFrameSkip)
    {
        static unsigned int skippedNumber = 0;

        if (s_isInterlaced) // interlacing -> show/skip by groups of 2 frames
        {
            // previous 'even' frame was skipped -> skip 'odd' frame too
            if (s_isPrevSkippedOdd == false)
            {
                s_isPrevSkippedOdd = true;
                ++skippedNumber;
                s_lateTicks = 0;
                return true;
            }
            // skip only 'even' frames first
            else if (isOddLine == false) 
            {
                if (s_lateTicks > s_maxFrameWait               // more than 7/8th frame time too late
                 && ++skippedNumber < MAX_SUCCESSIVE_SKIPPING) // display at least 1 frame out of 4
                {
                    s_isPrevSkippedOdd = false; // next 'odd' frame will be skipped too
                    s_lateTicks = 0;
                    return true;
                }
            }
        }

        else // not interlaced
        {
            if (s_lateTicks > s_maxFrameWait               // more than 7/8th frame time too late
             && ++skippedNumber < MAX_SUCCESSIVE_SKIPPING) // display at least 1 frame out of 4
            {
                s_lateTicks = 0;
                return true;
            }
        }

        skippedNumber = 0; // not skipped -> reset counter
    }

    s_isFrameDataWaiting = true; // new frame to draw
    return false;
}

/// <summary>Wait until current frame data is read</summary>
void FramerateManager::checkFrameStarted()
{
    // wait for frame data to be read
    static unsigned int cnt;
    cnt = 0;
    while (s_isFrameDataWaiting)
        cnt++;

    // no skipping + waiting = delay
    if (g_pConfig->sync_isFrameSkip == false && cnt > 200uL)
        s_isDelayedFrame = true;
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
