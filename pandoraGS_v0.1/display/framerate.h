/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   framerate.h
    Description : framerate toolbox
*******************************************************************************/
#ifndef _FRAMERATE_H
#define _FRAMERATE_H

#define FRAME_MAXSKIP 120
#define FRAME_MAXLACE 16


/* FRAMERATE TOOLBOX -------------------------------------------------------- */
class Framerate
{
  private:
    // framerate members
    static bool  _bInitialized;     // frame limiter variables state
    static bool  _bFrameCountMode;  // frame count mode
    static float _framerateLimit;   // desired FPS value
    static long  _laceCount;        // number of update laces
    #if _SYSTEM == _WINDOWS
        static LARGE_INTEGER _cpuFrequency;
        static DWORD _frameDuration;  // ticks/time per frame
      #else
        static unsigned long  _frameDuration;
    #endif
    
    Framerate() {};
    
  public:
    // framerate settings
    static void initFramerateConfig();          // set frame count mode
    static void setFramerateConfig(int, bool);  // set framerate limit and ticks
    // framerate processing
    static void checkFramerate();
    static void limitFramerate();   // framerate limiting and sync
    static void getFramesPerSec();  // calculation for skipping and FPS display
    #if _SYSTEM != _WINDOWS
        static unsigned long timeGetTime();
    #endif
    
    // public members
    static int   framesToSkip;
    static float fpsCurrent;
};

#endif
