/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   config.cpp
    Description : framerate toolbox
*******************************************************************************/
#include "../globals.h"
using namespace std;
#include <climits>
#if _SYSTEM == _WINDOWS
    #include <mmsystem.h> // add winmm.lib to project
#endif
#include "../config/config_manager.h"
#include "framerate.h"

extern Renderer* gpu_gameRenderer;

bool  Framerate::_bInitialized = false;
bool  Framerate::_bFrameCountMode = FPSMODE_GETTIME;
float Framerate::_framerateLimit = FPS_NTSC;
long  Framerate::_laceCount = 0;
#if _SYSTEM == _WINDOWS
    LARGE_INTEGER Framerate::_cpuFrequency;
    DWORD Framerate::_frameDuration = 16;
  #else
    unsigned long Framerate::_frameDuration = 16;
#endif
int Framerate::framesToSkip = 0;
float Framerate::fpsCurrent = 0;


/* framerate init - get CPU frequency info ---------------------------------- */
void Framerate::initFramerateConfig()
{
    #if _SYSTEM == _WINDOWS
        if (QueryPerformanceFrequency(&_cpuFrequency))
            _bFrameCountMode = FPSMODE_PERFCOUNTER;
        else 
            _bFrameCountMode = FPSMODE_GETTIME;
      #else
        _bFrameCountMode = FPSMODE_GETTIME;
    #endif
}

/* framerate setup - read config -------------------------------------------- */
void Framerate::setFramerateConfig(int localize, bool bInterlaced)
{
    Config* globalConfig = ConfigManager::getInstance();
    
    // reset skipping value
    framesToSkip = 0;
    
    // initialize framerate limit
    if(globalConfig->bFramerateLimit || globalConfig->bFrameSkipping) 
    {
        if(globalConfig->bFramerateLimitAuto 
        || (globalConfig->bFrameSkipping && globalConfig->bFramerateLimit == false) ) // auto-detect
        {
            if(ConfigManager::getInstance()->bugFixes&FIX_FPSLIMIT)
            {
                if (localize == DISPLAY_NTSC)
                {
                    _framerateLimit = bInterlaced?FPS_NTSC_FIXIL:FPS_NTSC_FIX;
                }
                else
                {
                    _framerateLimit = bInterlaced?FPS_PAL_FIXIL:FPS_PAL_FIX;
                }
            }
            else
            {
                if (localize == DISPLAY_NTSC)
                {
                    if (gpu_gameRenderer->gpuRawStatus & GPUSTATUS_INTERLACED)
                        _framerateLimit = FPS_NTSC_IL;
                    else
                        _framerateLimit = FPS_NTSC;
                }
                else
                {
                    if (gpu_gameRenderer->gpuRawStatus & GPUSTATUS_INTERLACED)
                        _framerateLimit = FPS_PAL_IL;
                    else
                        _framerateLimit = FPS_PAL;
                }
            }
        }
        else // user defined limit
            _framerateLimit = globalConfig->framerateLimitFixed; 
    }

    // get ticks number per frame
    #if _SYSTEM == _WINDOWS
        if (ConfigManager::getInstance()->bugFixes&FIX_NOFPSCOUNTER)
            _bFrameCountMode = FPSMODE_GETTIME;
        if (_bFrameCountMode == FPSMODE_GETTIME)
            _frameDuration = (1000 / (DWORD)_framerateLimit);
        else 
            _frameDuration = (_cpuFrequency.LowPart / (DWORD)_framerateLimit);
      #else
        _frameDuration = (1000 / (long)_framerateLimit);
    #endif
}


/* check and adjust framerate ----------------------------------------------- */
void Framerate::checkFramerate()
{
    // frame skipping mode
    if (ConfigManager::getInstance()->bFrameSkipping)
    {
        if ((ConfigManager::getInstance()->bugFixes&FIX_FPSOLDSKIP) == 0) // new mode
        {
            _laceCount++; // store number of vsync between frames
            if (ConfigManager::getInstance()->bFramerateLimit && _laceCount >= FRAME_MAXLACE) 
            {
                if (_laceCount == FRAME_MAXLACE) // wait for a few laces before init
                    _bInitialized = false;
                limitFramerate();
            }
        }
        else if (ConfigManager::getInstance()->bFramerateLimit) // old mode
        {
            limitFramerate();
        }
        // set skipping
        getFramesPerSec();
    }
    // non-skipping mode
    else 
    {
        if (ConfigManager::getInstance()->bFramerateLimit) // FPS limit
            limitFramerate();
        if (ConfigManager::getInstance()->bShowFramerate) // FPS display
            getFramesPerSec();
    }
}

/* frame limiter ------------------------------------------------------------ */
void Framerate::limitFramerate()
{
    static long long timeToSleep;
  #if _SYSTEM == _WINDOWS
    // performance counter mode
    if(_bFrameCountMode == FPSMODE_PERFCOUNTER)
    {
        static LARGE_INTEGER currentTicks;
        static LARGE_INTEGER nextUpdateTicks;
        // variables init (start / limit change)
        if(_bInitialized == false) 
        {
            _bInitialized = true;
            QueryPerformanceCounter(&currentTicks);
            nextUpdateTicks.QuadPart = currentTicks.QuadPart + (long long)_frameDuration;
        }
    
        QueryPerformanceCounter(&currentTicks); // get current time
        // slower than frame time -> adjust next update
        if (currentTicks.QuadPart >= nextUpdateTicks.QuadPart)
        {
            nextUpdateTicks.QuadPart += (long long)_frameDuration;
            if (currentTicks.QuadPart > nextUpdateTicks.QuadPart)
                nextUpdateTicks.QuadPart = currentTicks.QuadPart + (long long)_frameDuration;
        }
        // faster than frame time -> wait
        else
        {
            // eco mode : thread sleep to release CPU usage
            if (ConfigManager::getInstance()->bEcoFramerateMode)
            {
                // very approximate sleep -> sleep 4ms less than total time
                timeToSleep = ((1000LL * (nextUpdateTicks.QuadPart - currentTicks.QuadPart)) / _cpuFrequency.QuadPart) - 4LL;
                if (timeToSleep > 0 && timeToSleep < 100LL) // min 10 FPS
                    Sleep((unsigned long)timeToSleep);
            }
            // precise timing
            do
            {
                QueryPerformanceCounter(&currentTicks);
            }
            while(currentTicks.QuadPart < nextUpdateTicks.QuadPart);
            nextUpdateTicks.QuadPart = currentTicks.QuadPart + (long long)_frameDuration;
        }
    }
    
    // get-time mode (same principles using time instead of ticks)
    else
    {
        static DWORD currentTime;
        static DWORD nextUpdateTime = 0;
  #else
        static unsigned long currentTime;
        static unsigned long nextUpdateTime = 0;
  #endif
        // variables init (start / limit change)
        if(_bInitialized == false)
        {
            _bInitialized = true;
            currentTime = timeGetTime();
            nextUpdateTime = currentTime + _frameDuration;
        }
        
        currentTime = timeGetTime(); // get current time
        // slower than frame time -> adjust next update
        if (currentTime >= nextUpdateTime)
        {
            nextUpdateTime += _frameDuration;
            if (currentTime > nextUpdateTime)
                nextUpdateTime = currentTime + _frameDuration;
        }
        // faster than frame time -> wait
        else
        {
            // eco mode : thread sleep to release CPU usage
            if (ConfigManager::getInstance()->bEcoFramerateMode)
            {
                // very approximate sleep -> sleep 4ms less than total time
                timeToSleep = (long long)nextUpdateTime - ((long long)currentTime + 4LL);
                if (timeToSleep > 0 && timeToSleep < 100LL) // min 10 FPS
                    Sleep((unsigned long)timeToSleep);
            }
            // precise timing
            do
            {
                currentTime = timeGetTime();
            }
            while(currentTime < nextUpdateTime);
            nextUpdateTime = currentTime + _frameDuration;
        }
  #if _SYSTEM == _WINDOWS
    }
  #endif
}


/* calculate number of FPS (for frame skipping or FPS display) -------------- */
void Framerate::getFramesPerSec()
{
    long long duration = 0; // ms
    
  #if _SYSTEM == _WINDOWS
    // performance counter mode -> get duration
    if(_bFrameCountMode == FPSMODE_PERFCOUNTER)
    {
        static LARGE_INTEGER currentTicks;
        static LARGE_INTEGER previousTicks = {{0,0}};
        
        // get current ticks
        QueryPerformanceCounter(&currentTicks);
        // get frame duration
        duration = ((1000LL * (currentTicks.QuadPart - previousTicks.QuadPart)) / _cpuFrequency.QuadPart);
        previousTicks.QuadPart = currentTicks.QuadPart;
    }
    
    // get-time mode -> get duration
    else
    {
        static DWORD currentTime = 0;
        static DWORD previousTime;
  #else
        static unsigned long currentTime = 0;
        static unsigned long previousTime;
  #endif
        // get current time
        currentTime = timeGetTime();
        // get frame duration
        duration = currentTime - previousTime;
        previousTime = currentTime;
  #if _SYSTEM == _WINDOWS
    }
  #endif

    // if special duration (startup/shutdown/problem/...), exit function
    if (duration > 250)
    {
        fpsCurrent = 2.0f;
        return;
    }
    
    // calculate FPS value
    static unsigned long lastFpsDisplayTime = 0;
    float framerate = 1.0f/((float)duration/1000.0f);
    if (ConfigManager::getInstance()->bShowFramerate)
    {
        // displayed value (change display 5x/sec max)
        unsigned long currentDisplayTime = timeGetTime();
        if (lastFpsDisplayTime + 200 < currentDisplayTime)
        {
            fpsCurrent = framerate;
            lastFpsDisplayTime = currentDisplayTime;
        }
    }
    
    // set frame skipping
    if (ConfigManager::getInstance()->bFrameSkipping)
    {
        long long expectedDuration = (long long)_frameDuration;
        if(_bFrameCountMode == FPSMODE_PERFCOUNTER)
            expectedDuration = ((1000LL * expectedDuration) / _cpuFrequency.QuadPart);
        if (framesToSkip <= 0 && (duration - 1) > expectedDuration)
        {
            float lostFrames = _framerateLimit - framerate; // FPS difference
            if (lostFrames >= 0.5f)
            {
                lostFrames /= _framerateLimit; // delay per frame
                lostFrames += 0.8f; // round value
                framesToSkip = (int)lostFrames;
            }
            if (framesToSkip > 4) // max 4 frames skipped per frame displayed
                framesToSkip = 4;
        }
    }
}


/* get current time (for non-Windows systems) ------------------------------- */
#if _SYSTEM != _WINDOWS
    unsigned long Framerate::timeGetTime()
    {
        struct timeval tv;
        gettimeofday(&tv, 0);
        return tv.tv_sec * 100000 + tv.tv_usec/10;
    }
#endif

