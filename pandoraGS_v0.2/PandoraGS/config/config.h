/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config.h
Description : configuration container
*******************************************************************************/
#ifndef _CONFIG_H
#define _CONFIG_H
#include "globals.h"

#include <string>
#include "config_profile.h"

// config constants
#define RESOLUTION_AUTODETECT 0u
#define FRAMELIMIT_Auto 0.0f

// plugin key bindings
#define GPUKEYS_LENGTH 10           // 9 + NULL ending
#define VK_NOKEY 0x07
enum GpuKeys
{
    GpuKeys_ProfileMenu = 0,        // menu on/off
    GpuKeys_ProfilePrev = 1,        // menu shows prev profile
    GpuKeys_ProfileNext = 2,        // menu shows next profile
    GpuKeys_ProfileDefault = 3,     // menu shows default profile
    GpuKeys_ProfileConfirm = 4,     // apply selected profile
    GpuKeys_Stretching = 5,         // next streching mode
    GpuKeys_FastForward = 6,        // faster while pressed
    GpuKeys_SlowMotion = 7,         // slow mode on/off
    GpuKeys_Debug = 8               // FPS display / next debug mode
};
// timing mode
enum TimingMode
{
    TimingMode_MultimediaClock = 0, // low res, steady
    TimingMode_HighResCounter = 1  // high res, unstable
};

// general fixes
#define GEN_FIX_MIXED_SOFT_FRAMEBUFFER 0x1 // speed framebuffer access fix for ATI
#define GEN_FIX_FAKE_SUB_BLENDING      0x2 // for buggy drivers - fake substractive blending
#define GEN_FIX_RELOAD_CFG_AFTER_OPEN  0x4 // don't keep same profile after disc change


// Driver configuration container
class Config
{
private:
    static ConfigProfile** m_pProfiles;    // config profiles array
    static uint32_t m_profilesArrayLength; // profiles array length
    static uint32_t m_currentProfile;      // active profile ID
    static bool m_isReady; // mutex

public:
    // general
    static uint32_t   gen_langCode;        // language identifier
    static uint32_t   rnd_floatAccuracy;   // anti-jitter GTE accuracy
    static uint32_t   rnd_debugMode;       // debug mode (or 0)
    static bool       rnd_isFpsDisplayed;  // show FPS (on/off)

    // display
    static bool       dsp_isFullscreen;    // display mode (fullscreen/window)
    static uint32_t   dsp_fullscnResX;     // fullscreen display resolution [x]
    static uint32_t   dsp_fullscnResY;     // fullscreen display resolution [y]
    static uint32_t   dsp_windowResX;      // window display resolution [x]
    static uint32_t   dsp_windowResY;      // window display resolution [y]
    static bool       dsp_isWindowResizable; // resizable window mode
    static bool       dsp_isColorDepth32;  // color depth mode (32/16-bit)

    // framerate
    static bool       sync_isVerticalSync;  // vsync (on/off)
    static bool       sync_isFrameSkip;     // frame skipping mode (on/off)
    static bool       sync_isFrameLimit;    // frame limit mode (on/off)
    static float      sync_framerateLimit;  // framerate limit (0=auto / value=fixed)
    static TimingMode sync_timeMode;        // type of timer

    // miscellaneous
    static bool misc_isScreensaverDisabled;   // disable screensaver
    static char misc_gpuKeys[GPUKEYS_LENGTH]; // plugin key bindings
    static uint32_t   misc_genFixBits;        // general fixes
    static uint32_t   misc_emuFixBits;        // fixes set by emulator


public:
    /// <summary>Create config container (default values, no profile)</summary>
    static void init();
    /// <summary>Close config container and profiles</summary>
    static void close();

    // -- GETTERS - SETTERS -- -------------------------------------------------

    /// <summary>Set base config default values</summary>
    static void setDefaultValues();
    /// <summary>Reset default plugin key bindings</summary>
    static void setDefaultKeyBindings();

    /// <summary>Set the whole profile array</summary>
    /// <param name="ppProfiles">Allocated list of profiles</param>
    /// <param name="arrayLength">Number of profiles</param>
    static void setProfiles(ConfigProfile*** ppProfiles, uint32_t arrayLength);
    /// <summary>Insert profile into profile array</summary>
    /// <param name="index">Array index</param>
    /// <param name="pProfile">Loaded profile to insert</param>
    /// <param name="isUsed">Set profile as current</param>
    static void setProfile(uint32_t index, ConfigProfile* pProfile, bool isUsed = true);
    /// <summary>Get profiles array</summary>
    /// <param name="pProfiles">Profiles array reference</param>
    /// <returns>Number of profiles</returns>
    static uint32_t getAllProfiles(ConfigProfile** pProfiles);
    /// <summary>Get specific profile</summary>
    /// <param name="index">Profile index (0 based)</param>
    /// <returns>Profile at the specified index (if available)</returns>
    static ConfigProfile* getProfile(uint32_t index);
    /// <summary>Get current profile</summary>
    /// <returns>Current profile</returns>
    static inline ConfigProfile* getCurrentProfile()
    {
        return m_pProfiles[m_currentProfile];
    }
    /// <summary>Check if profile is initialized</summary>
    /// <returns>Profile status (true = set)</returns>
    static inline bool isProfileSet()
    {
        return m_isReady;
    }

    /// <summary>Get current profile ID</summary>
    /// <returns>Current profile ID</returns>
    static inline uint32_t getCurrentProfileId()
    {
        return m_currentProfile;
    }
    /// <summary>Get previous profile ID</summary>
    /// <param name="start">Checked start index</param>
    /// <returns>Profile ID</returns>
    static uint32_t getPrevProfileId(uint32_t start);
    /// <summary>Get next profile ID</summary>
    /// <param name="start">Checked start index</param>
    /// <returns>Profile ID</returns>
    static uint32_t getNextProfileId(uint32_t start);
    /// <summary>Count available profiles</summary>
    /// <returns>Profile number</returns>
    static uint32_t countProfiles();


    /// <summary>Enable fix bits/summary>
    /// <param name="fixBits">Fix bits mask</param>
    static inline void setGenFix(uint32_t fixBits)
    {
        misc_genFixBits |= fixBits;
    }
    /// <summary>Disable fix bits</summary>
    /// <param name="fixBits">Fix bits mask</param>
    static inline void unsetGenFix(uint32_t fixBits)
    {
        misc_genFixBits &= ~fixBits;
    }
    /// <summary>Check if fix bit is active</summary>
    /// <param name="fixBits">Bit(s) mask (will return true if all are active)</param>
    static inline bool getGenFix(uint32_t fixBits)
    {
        return ((misc_genFixBits & fixBits) == fixBits);
    }
    /// <summary>Check if fix bit is disabled</summary>
    /// <param name="fixBits">Bit(s) mask (will return true if all are disabled)</param>
    static inline bool getNotGenFix(uint32_t fixBits)
    {
        return ((misc_genFixBits & fixBits) == 0);
    }


    // -- CURRENT PROFILE CHANGE -- --------------------------------------------

    /// <summary>Set default profile as current</summary>
    static void useDefaultProfile();
    /// <summary>Set specific profile as current (if available)</summary>
    /// <param name="index">Profile index (0 based)</param>
    /// <exception cref="std::exception">Memory allocation failure</exception>
    static void useProfile(uint32_t index);
};


// debug modes
enum DebugMode
{
    DebugMode_None = 0,  // normal mode (debug off)
    DebugMode_Normal = 1,// normal mode (debug on)
    DebugMode_BaseMode = 2,    // lines + polygons (color by order)
    DebugMode_AccuracyMode = 3,// lines + polygons (color based on accuracy)
    DebugMode_TexturedMode = 4 // lines + textured polygons
};
// GTE accuracy
enum FloatAccuracy
{
    FloatAccuracy_None = 0,
    FloatAccuracy_Simple = 1,  // simple accuracy mode (float coord)
    FloatAccuracy_Advanced = 2 // with caching and original coord copy
};

#endif
