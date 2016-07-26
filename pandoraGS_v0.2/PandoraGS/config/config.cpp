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
#include "render.h"
#include "lang.h"


/// <summary>Create config container (default values, no profile)</summary>
Config::Config()
{
    m_isReady = false;
    m_currentProfile = 0uL; // default
    m_pProfiles = NULL; // no profile yet
    m_profilesArrayLength = 0uL;

    gen_gameId = "";
    gen_langCode = (unsigned int)LANG_DEFAULT;
    misc_gpuKeys[GPUKEYS_LENGTH - 1] = '\0';
}

/// <summary>Close config container and profiles</summary>
Config::~Config()
{
    // free profiles memory
    setProfiles(NULL, 0uL);
    m_pProfiles = NULL;
}


// -- GETTERS - SETTERS -- -----------------------------------------------------

/// <summary>Set base config default values</summary>
void Config::setDefaultValues()
{
    rnd_renderApiCode = RENDERAPI_DEFAULT;
    rnd_isFloatAccuracy = false;
    rnd_hasPsxPrimitives = false;
    rnd_debugMode = DebugMode_None;
    rnd_isFpsDisplayed = false;

    dsp_isFullscreen = true;
    dsp_fullscnResX = RESOLUTION_AUTODETECT;
    dsp_fullscnResY = RESOLUTION_AUTODETECT;
    dsp_windowResX = 800;
    dsp_windowResY = 600;
    dsp_isWindowResizable = false;
    dsp_isColorDepth32 = true;

    sync_isFrameSkip = true;
    sync_isFrameLimit = true;
    sync_framerateLimit = FRAMELIMIT_Auto;
    sync_isVerticalSync = true;
    sync_timeMode = TimingMode_HighResCounter;

    misc_isScreensaverDisabled = false;

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
void Config::setProfiles(ConfigProfile*** ppProfiles, unsigned int arrayLength)
{
    m_isReady = false;
    
    // release previous profiles
    if (m_pProfiles != NULL)
    {
        for (unsigned int p = 0uL; p < m_profilesArrayLength; p++)
        {
            if (m_pProfiles[p] != NULL)
                delete m_pProfiles[p];
            m_pProfiles[p] = NULL;
        }
        delete [] m_pProfiles;
    }

    // set new profiles
    m_currentProfile = 0uL;
    m_profilesArrayLength = arrayLength;
    if (arrayLength > 0uL)
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
void Config::setProfile(unsigned int index, ConfigProfile* pProfile, bool isUsed)
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
unsigned long Config::getAllProfiles(ConfigProfile** pProfiles)
{
    pProfiles = m_pProfiles;
    return m_profilesArrayLength;
}

/// <summary>Get specific profile</summary>
/// <param name="index">Profile index (0 based)</param>
/// <returns>Profile at the specified index (if available)</returns>
ConfigProfile* Config::getProfile(unsigned int index)
{
    while (m_isReady == false);
    if (index < m_profilesArrayLength)
        return m_pProfiles[index];
    return NULL;
}


// -- CURRENT PROFILE CHANGE -- --------------------------------------------

/// <summary>Get previous profile ID</summary>
/// <param name="start">Checked start index</param>
unsigned int Config::getPrevProfileId(unsigned int start)
{
    // get ID
    unsigned int profileId = start;
    if (profileId > 0uL)
        profileId -= 1uL;
    else
        profileId = m_profilesArrayLength - 1;

    // load profile
    while (m_isReady == false);
    if (m_pProfiles[profileId] == NULL)
        setProfile(m_currentProfile, ConfigIO::loadConfigProfile(profileId), false);
    return profileId;
}

/// <summary>Get next profile ID</summary>
/// <param name="start">Checked start index</param>
unsigned int Config::getNextProfileId(unsigned int start)
{
    // get ID
    unsigned int profileId = start;
    if (profileId < (m_profilesArrayLength - 1))
        profileId += 1uL;
    else
        profileId = 0uL;

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
    m_currentProfile = 0uL;

    // load profile (if not already)
    if (m_pProfiles[0] == NULL)
        setProfile(m_currentProfile, ConfigIO::loadConfigProfile(0));
}

/// <summary>Set specific profile as current (if available)</summary>
/// <param name="index">Profile index (0 based)</param>
/// <exception cref="std::exception">Memory allocation failure</exception>
void Config::useProfile(unsigned int index)
{
    // change profile
    while (m_isReady == false);
    m_isReady = false;
    if (index < m_profilesArrayLength)
        m_currentProfile = index;

    // load profile (if not already)
    if (m_pProfiles[m_currentProfile] == NULL)
        setProfile(m_currentProfile, ConfigIO::loadConfigProfile(m_currentProfile));
}
