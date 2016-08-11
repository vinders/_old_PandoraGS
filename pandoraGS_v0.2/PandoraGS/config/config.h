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
#define RESOLUTION_AUTODETECT 0
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
// debug modes
enum DebugMode
{
    DebugMode_None = 0,
    DebugMode_BaseMode = 1,
    DebugMode_TexturedMode = 2
};
// timing mode
enum TimingMode
{
    TimingMode_MultimediaClock = 0, // low res, steady
    TimingMode_HighResCounter = 1,  // high res, unstable
};

// general fixes
#define GEN_FIX_MIXED_SOFT_FRAMEBUFFER 0x1uL // speed framebuffer access fix for ATI
#define GEN_FIX_FAKE_SUB_BLENDING      0x2uL // for buggy drivers - fake substractive blending
#define GEN_FIX_RELOAD_CFG_AFTER_OPEN  0x4uL // don't keep same profile after disc change


// Driver configuration container
class Config
{
private:
    ConfigProfile** m_pProfiles;         // config profiles array
    unsigned long m_profilesArrayLength; // profiles array length
    unsigned long m_currentProfile;      // active profile ID
    bool m_isReady;

public:
    // general
    unsigned int gen_langCode;        // language identifier
    unsigned int rnd_renderApiCode;   // graphic API identifier
    bool         rnd_isFloatAccuracy; // anti-jitter GTE accuracy
    bool         rnd_hasPsxPrimitives;// support for more PSX GPU primitives
    DebugMode    rnd_debugMode;       // debug mode (or 0)
    bool         rnd_isFpsDisplayed;  // show FPS (on/off)

    // display
    bool         dsp_isFullscreen;    // display mode (fullscreen/window)
    unsigned int dsp_fullscnResX;     // fullscreen display resolution [x]
    unsigned int dsp_fullscnResY;     // fullscreen display resolution [y]
    unsigned int dsp_windowResX;      // window display resolution [x]
    unsigned int dsp_windowResY;      // window display resolution [y]
    bool         dsp_isWindowResizable; // resizable window mode
    bool         dsp_isColorDepth32;  // color depth mode (32/16-bit)

    // framerate
    bool         sync_isVerticalSync;  // vsync (on/off)
    bool         sync_isFrameSkip;     // frame skipping mode (on/off)
    bool         sync_isFrameLimit;    // frame limit mode (on/off)
    float        sync_framerateLimit;  // framerate limit (0=auto / value=fixed)
    TimingMode   sync_timeMode;        // type of timer

    // miscellaneous
    bool misc_isScreensaverDisabled;   // disable screensaver
    char misc_gpuKeys[GPUKEYS_LENGTH]; // plugin key bindings
    unsigned long misc_genFixBits;        // general fixes
    unsigned long misc_emuFixBits;        // fixes set by emulator


public:
    /// <summary>Create config container (default values, no profile)</summary>
    Config();
    /// <summary>Close config container and profiles</summary>
    ~Config();

    // -- GETTERS - SETTERS -- -------------------------------------------------

    /// <summary>Set base config default values</summary>
    void setDefaultValues();
    /// <summary>Reset default plugin key bindings</summary>
    void setDefaultKeyBindings();

    /// <summary>Set the whole profile array</summary>
    /// <param name="ppProfiles">Allocated list of profiles</param>
    /// <param name="arrayLength">Number of profiles</param>
    void setProfiles(ConfigProfile*** ppProfiles, unsigned int arrayLength);
    /// <summary>Insert profile into profile array</summary>
    /// <param name="index">Array index</param>
    /// <param name="pProfile">Loaded profile to insert</param>
    /// <param name="isUsed">Set profile as current</param>
    void setProfile(unsigned int index, ConfigProfile* pProfile, bool isUsed = true);
    /// <summary>Get profiles array</summary>
    /// <param name="pProfiles">Profiles array reference</param>
    /// <returns>Number of profiles</returns>
    unsigned long getAllProfiles(ConfigProfile** pProfiles);
    /// <summary>Get specific profile</summary>
    /// <param name="index">Profile index (0 based)</param>
    /// <returns>Profile at the specified index (if available)</returns>
    ConfigProfile* getProfile(unsigned int index);
    /// <summary>Get current profile</summary>
    /// <returns>Current profile</returns>
    inline ConfigProfile* getCurrentProfile()
    {
        return m_pProfiles[m_currentProfile];
    }
    /// <summary>Check if profile is initialized</summary>
    /// <returns>Profile status (true = set)</returns>
    inline bool isProfileSet()
    {
        return m_isReady;
    }

    /// <summary>Get current profile ID</summary>
    /// <returns>Current profile ID</returns>
    inline unsigned long getCurrentProfileId()
    {
        return m_currentProfile;
    }
    /// <summary>Get previous profile ID</summary>
    /// <param name="start">Checked start index</param>
    unsigned int getPrevProfileId(unsigned int start);
    /// <summary>Get next profile ID</summary>
    /// <param name="start">Checked start index</param>
    unsigned int getNextProfileId(unsigned int start);


    /// <summary>Enable fix bits/summary>
    /// <param name="fixBits">Fix bits mask</param>
    inline void setGenFix(unsigned long fixBits)
    {
        misc_genFixBits |= fixBits;
    }
    /// <summary>Disable fix bits</summary>
    /// <param name="fixBits">Fix bits mask</param>
    inline void unsetGenFix(unsigned long fixBits)
    {
        misc_genFixBits &= ~fixBits;
    }
    /// <summary>Check if fix bit is active</summary>
    /// <param name="fixBits">Bit(s) mask (will return true if all are active)</param>
    inline bool getGenFix(long fixBits)
    {
        return ((misc_genFixBits & fixBits) == fixBits);
    }
    /// <summary>Check if fix bit is disabled</summary>
    /// <param name="fixBits">Bit(s) mask (will return true if all are disabled)</param>
    inline bool getNotGenFix(long fixBits)
    {
        return ((misc_genFixBits & fixBits) == 0);
    }


    // -- CURRENT PROFILE CHANGE -- --------------------------------------------

    /// <summary>Set default profile as current</summary>
    void useDefaultProfile();
    /// <summary>Set specific profile as current (if available)</summary>
    /// <param name="index">Profile index (0 based)</param>
    /// <exception cref="std::exception">Memory allocation failure</exception>
    void useProfile(unsigned int index);
};

#endif
