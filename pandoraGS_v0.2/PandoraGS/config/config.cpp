/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config.cpp
Description : configuration container
*******************************************************************************/
using namespace std;
#include "config.h"
#include "config_io.h"
#include "lang.h"

// profiles
ConfigProfile** Config::m_pProfiles = NULL;  // config profiles array
uint32_t Config::m_profilesArrayLength = 0u; // profiles array length
uint32_t Config::m_currentProfile = 0u;      // active profile ID
bool Config::m_isReady = false;
// general
uint32_t Config::gen_langCode;        // language identifier
std::wstring Config::gen_langFilePath;        // language file path
uint32_t Config::rnd_floatAccuracy;   // anti-jitter GTE accuracy
uint32_t Config::rnd_debugMode;       // debug mode (or 0)
bool     Config::rnd_isFpsDisplayed;  // show FPS (on/off)
// display
bool     Config::dsp_isFullscreen;    // display mode (fullscreen/window)
uint32_t Config::dsp_fullscnResX;     // fullscreen display resolution [x]
uint32_t Config::dsp_fullscnResY;     // fullscreen display resolution [y]
uint32_t Config::dsp_windowResX;      // window display resolution [x]
uint32_t Config::dsp_windowResY;      // window display resolution [y]
bool     Config::dsp_isWindowResizable; // resizable window mode
bool     Config::dsp_isColorDepth32;  // color depth mode (32/16-bit)
// framerate
bool     Config::sync_isVerticalSync;  // vsync (on/off)
bool     Config::sync_isFrameSkip;     // frame skipping mode (on/off)
bool     Config::sync_isFrameLimit;    // frame limit mode (on/off)
float    Config::sync_framerateLimit;  // framerate limit (0=auto / value=fixed)
TimingMode Config::sync_timeMode;      // type of timer
// miscellaneous
bool Config::misc_isScreensaverDisabled;   // disable screensaver
char Config::misc_gpuKeys[GPUKEYS_LENGTH]; // plugin key bindings
uint32_t Config::misc_genFixBits = 0u;    // general fixes
uint32_t Config::misc_emuFixBits = 0u;    // fixes set by emulator


/// <summary>Create config container (default values, no profile)</summary>
void Config::init()
{
    m_isReady = false;
    gen_langCode = (uint32_t)LANG_DEFAULT;
    gen_langFilePath = L"./pandoraGS.lang";
    misc_gpuKeys[GPUKEYS_LENGTH - 1] = '\0';
    setDefaultValues();
}

/// <summary>Close config container and profiles</summary>
void Config::close()
{
    // free profiles memory
    setProfiles(NULL, 0u);
    m_pProfiles = NULL;
}


// -- GETTERS - SETTERS -- -----------------------------------------------------

/// <summary>Set base config default values</summary>
void Config::setDefaultValues()
{
    rnd_floatAccuracy = FloatAccuracy_None;
    rnd_debugMode = DebugMode_None;
    rnd_isFpsDisplayed = false;

    dsp_isFullscreen = true;
    dsp_fullscnResX = RESOLUTION_AUTODETECT;
    dsp_fullscnResY = RESOLUTION_AUTODETECT;
    dsp_windowResX = 800u;
    dsp_windowResY = 600u;
    dsp_isWindowResizable = false;
    dsp_isColorDepth32 = true;

    sync_isFrameSkip = false;
    sync_isFrameLimit = true;
    sync_framerateLimit = FRAMELIMIT_Auto;
    sync_isVerticalSync = true;
    sync_timeMode = TimingMode_HighResCounter;

    misc_isScreensaverDisabled = false;
    misc_genFixBits = 0u;

    //...

    setDefaultKeyBindings();
}

/// <summary>Reset default plugin key bindings</summary>
void Config::setDefaultKeyBindings()
{
    misc_gpuKeys[(int)GpuKeys_ProfileMenu] = VK_INSERT;
    misc_gpuKeys[(int)GpuKeys_ProfilePrev] = VK_PRIOR;
    misc_gpuKeys[(int)GpuKeys_ProfileNext] = VK_NEXT;
    misc_gpuKeys[(int)GpuKeys_ProfileDefault] = VK_HOME;
    misc_gpuKeys[(int)GpuKeys_ProfileConfirm] = VK_END;

    misc_gpuKeys[(int)GpuKeys_FastForward] = VK_NOKEY;
    misc_gpuKeys[(int)GpuKeys_SlowMotion] = VK_NOKEY;
    misc_gpuKeys[(int)GpuKeys_Stretching] = VK_F11;
    misc_gpuKeys[(int)GpuKeys_Debug] = VK_F12;
}

/// <summary>Set the whole profile array</summary>
/// <param name="ppProfiles">Allocated list of profiles</param>
/// <param name="arrayLength">Number of profiles</param>
void Config::setProfiles(ConfigProfile*** ppProfiles, uint32_t arrayLength)
{
    m_isReady = false;
    
    // release previous profiles
    if (m_pProfiles != NULL)
    {
        for (uint32_t p = 0u; p < m_profilesArrayLength; p++)
        {
            if (m_pProfiles[p] != NULL)
                delete m_pProfiles[p];
            m_pProfiles[p] = NULL;
        }
        delete [] m_pProfiles;
    }

    // set new profiles
    m_currentProfile = 0u;
    m_profilesArrayLength = arrayLength;
    if (arrayLength > 0u)
    {
        m_pProfiles = *ppProfiles;
        m_isReady = true;
    }
    else
        m_pProfiles = NULL;
}

/// <summary>Insert profile into profile array</summary>
/// <param name="index">Array index</param>
/// <param name="pProfile">Loaded profile to insert</param>
/// <param name="isUsed">Set profile as current</param>
void Config::setProfile(uint32_t index, ConfigProfile* pProfile, bool isUsed)
{
    m_isReady = false;

    if (m_pProfiles != NULL && m_profilesArrayLength > index)
    {
        if (m_pProfiles[index] != NULL)
            delete m_pProfiles[index];
        m_pProfiles[index] = pProfile;
    }
    else if (pProfile != NULL)
        delete pProfile;

    if (isUsed)
        m_currentProfile = index;
    m_isReady = true;
}

/// <summary>Get profiles array</summary>
/// <param name="pProfiles">Profiles array reference</param>
/// <returns>Number of profiles</returns>
uint32_t Config::getAllProfiles(ConfigProfile** pProfiles)
{
    pProfiles = m_pProfiles;
    return m_profilesArrayLength;
}

/// <summary>Get specific profile</summary>
/// <param name="index">Profile index (0 based)</param>
/// <returns>Profile at the specified index (if available)</returns>
ConfigProfile* Config::getProfile(uint32_t index)
{
    while (m_isReady == false);
    if (index < m_profilesArrayLength)
        return m_pProfiles[index];
    return NULL;
}

/// <summary>Count available profiles</summary>
/// <returns>Profile number</returns>
uint32_t Config::countProfiles()
{
    return m_profilesArrayLength;
}


// -- CURRENT PROFILE CHANGE -- --------------------------------------------

/// <summary>Get previous profile ID</summary>
/// <param name="start">Checked start index</param>
uint32_t Config::getPrevProfileId(uint32_t start)
{
    // get ID
    uint32_t profileId = start;
    if (profileId > 0u)
        profileId -= 1u;
    else if (m_profilesArrayLength > 0u)
        profileId = m_profilesArrayLength - 1u;
    else
        return 0u;

    // load profile
    while (m_isReady == false);
    if (m_pProfiles[profileId] == NULL)
        setProfile(m_currentProfile, ConfigIO::loadConfigProfile(profileId), false);
    return profileId;
}

/// <summary>Get next profile ID</summary>
/// <param name="start">Checked start index</param>
uint32_t Config::getNextProfileId(uint32_t start)
{
    // get ID
    uint32_t profileId = start;
    if (profileId < (m_profilesArrayLength - 1u))
        profileId += 1u;
    else
        profileId = 0u;

    // load profile
    while (m_isReady == false);
    if (m_pProfiles[profileId] == NULL)
        setProfile(m_currentProfile, ConfigIO::loadConfigProfile(profileId), false);
    return profileId;
}

/// <summary>Set default profile as current</summary>
void Config::useDefaultProfile()
{
    // change profile
    while (m_isReady == false);
    m_isReady = false;
    m_currentProfile = 0u;

    // load profile (if not already)
    if (m_pProfiles[0] == NULL)
        setProfile(m_currentProfile, ConfigIO::loadConfigProfile(0u));
    m_isReady = true;
}

/// <summary>Set specific profile as current (if available)</summary>
/// <param name="index">Profile index (0 based)</param>
/// <exception cref="std::exception">Memory allocation failure</exception>
void Config::useProfile(uint32_t index)
{
    // change profile
    while (m_isReady == false);
    m_isReady = false;
    if (index < m_profilesArrayLength)
        m_currentProfile = index;

    // load profile (if not already)
    if (m_pProfiles[m_currentProfile] == NULL)
        setProfile(m_currentProfile, ConfigIO::loadConfigProfile(m_currentProfile));
    m_isReady = true;
}
