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
#include "input_manager.h"

#define MAX_LACE_NUMBER 16
#define MAX_SUCCESSIVE_SKIPPING 4
extern Config* g_pConfig;
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
int  FramerateManager::s_framesToSkip = 0;          // number of frames to skip

// frame skipping
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
    s_framesToSkip = 0;

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
        if (g_pConfig->getCurrentProfile()->getFix(CFG_FIX_AUTO_FPSLIMIT))
            s_isInterlaced = PsxCoreMemory::dsp_displayState.isInterlaced;
        else
            s_isInterlaced = PsxCoreMemory::getStatus(GPUSTATUS_INTERLACED);
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
        if (g_pConfig->getCurrentProfile()->getFix(CFG_FIX_AUTO_FPSLIMIT))
        {
            if (PsxCoreMemory::dsp_displayState.localize == LocalizationMode_Pal)
                g_framerateHz = (s_isInterlaced) ? 50.0f : 25.0f;
            else 
                g_framerateHz = (s_isInterlaced) ? 60.0f : 30.0f;
        }
        else // emulation values
        {
            if (PsxCoreMemory::dsp_displayState.localize == LocalizationMode_Pal)
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
        s_maxFrameWait = ((7 * s_frameDuration) >> 3);
    #else
    s_maxFrameWait = ((7 * s_frameDuration) >> 3);
    #endif

    // reset frame skipping
    s_framesToSkip = 0;
    s_lateTicks = 0;
    // reset frame management values
    s_currentFps = 0.0f;
    s_framesAfterFpsRef = 0;
    s_isReset = true;
}


// -- FRAMERATE MANAGEMENT -- --------------------------------------------------

/// <summary>Frame rate limiter/sync + check frame skipping</summary>
/// <param name="frameSpeed">Speed modifier (normal/slow/fast)</param>
/// <param name="isOddFrame">Odd line (if interlaced)</param>
void FramerateManager::waitFrameTime(int frameSpeed, bool isOddFrame)
{
    #ifdef _WINDOWS
    static DWORD lastTicks, currentTicks, elapsedTicks, ticksToWait = 0;
    static LARGE_INTEGER lastTimeRef, currentTimeRef;
    #else
    static unsigned long lastTicks, currentTicks, elapsedTicks, ticksToWait = 0;
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

    // set FPS indicator
    static unsigned int laceCount = MAX_LACE_NUMBER;
    if (++laceCount >= MAX_LACE_NUMBER)
    {
        if (g_pConfig->rnd_isFpsDisplayed)
            checkCurrentFramerate();
        laceCount = 0;
    }
    s_framesAfterFpsRef++;

    // speed modifiers
    if (frameSpeed) 
    {
        // fast forward
        if (frameSpeed == FrameSpeed_FastForward) 
        {
            int nextFramesToSkip = 0;

            // skip 3 frames out of 4
            static int fastIterations = 0;
            if (++fastIterations < 4)
                nextFramesToSkip = 1;
            else
                fastIterations = 0;

            // set frame skipping
            if (s_framesToSkip > 0) // current was skipped -> no wait
            {
                s_framesToSkip = nextFramesToSkip;
                return;
            }
            s_framesToSkip = 1 + nextFramesToSkip; // will be decremented at the end of the function
        }
        // slow motion
        else 
        {
            // double frame time
            #ifdef _WINDOWS
            if (g_pConfig->sync_timeMode == TimingMode_HighResCounter)
                ticksToWait += s_frameDurationHiRes;
            else
                ticksToWait += s_frameDuration;
            #else
            ticksToWait += s_frameDuration;
            #endif
            s_framesToSkip = 1; // bypass frame skipping (will be decremented at the end of the function)
        }
    }

    // apply framerate limit
    if (g_pConfig->sync_isFrameLimit)
    {
        #ifdef _WINDOWS
        // high resolution time sync (qpc)
        if (g_pConfig->sync_timeMode == TimingMode_HighResCounter)
        {
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
            // wait for frame duration
            static unsigned long waitCount;
            waitCount = 0uL;
            do
            {
                // get current time reference
                currentTicks = timeGetTime();
                elapsedTicks = currentTicks - lastTicks;
                waitCount++;
            } 
            while (elapsedTicks <= ticksToWait && currentTicks >= lastTicks);

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


    // frame skipping
    if (s_framesToSkip > 0) // if current was skipped, don't check time
        s_framesToSkip--;
    else if (g_pConfig->sync_isFrameSkip)
    {
        // count number of frames missed
        static float lateFramesNumber = 0.0f;
        #ifdef _WINDOWS
        if (g_pConfig->sync_timeMode == TimingMode_HighResCounter)
            lateFramesNumber = (float)s_lateTicks / (float)s_frameDurationHiRes;
        else
            lateFramesNumber = (float)s_lateTicks / (float)s_frameDuration;
        #else
        lateFramesNumber = (float)s_lateTicks / (float)s_frameDuration;
        #endif
        s_lateTicks = 0;

        // standard skipping mode
        if (g_pConfig->getCurrentProfile()->getNotFix(CFG_FIX_HALF_SKIPPING))
        {
            if (lateFramesNumber > 0.85f)
            {
                if (s_isInterlaced) // interlacing
                {
                    if (isOddFrame == false) // always skip 'even' frame first
                    {
                        s_framesToSkip = (int)lateFramesNumber;
                        if (s_framesToSkip > MAX_SUCCESSIVE_SKIPPING) // display at least 2 frames out of 6
                            s_framesToSkip = MAX_SUCCESSIVE_SKIPPING;
                        else if (s_framesToSkip % 2 != 0) // skip by groups of 2 frames
                            s_framesToSkip++;
                    }
                }
                else // not interlaced
                {
                    lateFramesNumber += 0.15f;
                    s_framesToSkip = (int)lateFramesNumber;
                    if (s_framesToSkip >= MAX_SUCCESSIVE_SKIPPING) // display at least 1 frame out of 4
                        s_framesToSkip = MAX_SUCCESSIVE_SKIPPING - 1;
                }
            }
        }
        // special skipping mode - one frame out of two
        else
        {
            if (s_isInterlaced) // interlacing
            {
                if (lateFramesNumber > 0.10f && isOddFrame == false)
                    s_framesToSkip = 2; // display 2, skip 2
            }
            else if (lateFramesNumber > 0.05f)
                s_framesToSkip = 1; // display 1, skip 1
        }
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


#ifndef _WINDOWS
/// <summary>Linux-Unix compatible time system</summary>
unsigned long timeGetTime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec * 100000 + tv.tv_usec / 10; 
}
#endif
