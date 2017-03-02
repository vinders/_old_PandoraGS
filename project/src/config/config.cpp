/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration container
*******************************************************************************/
#include "../globals.h"
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
using namespace std::literals::string_literals;
#include "config_common.h"
#include "config_profile.h"
#include "config_io.h"
#include "config.h"
using namespace config;

std::vector<ConfigProfile*> Config::s_profiles; ///< Config profiles (vector)
std::vector<std::wstring> Config::s_profileNames; ///< Names of the profiles
uint32_t Config::s_currentProfileId = 0u;       ///< Active profile ID
bool Config::s_isInitialized = false;           ///< Initialization status
bool Config::s_isReady = true;                  ///< Fast mutual exclusion

lang::langcode_t Config::langCode; ///< Used language identifier
std::wstring Config::langFilePath; ///< External language file path

config_display_t Config::display; ///< Display settings
config_timer_t Config::timer;     ///< Timer sync settings
config_events_t Config::events;   ///< Events handling settings

uint32_t Config::configFixBits = 0u;  ///< Configured fixes
uint32_t Config::runtimeFixBits = 0u; ///< Fixes set by emulator


/// @brief Create config container (default values + default profile)
void Config::init()
{
    lock();

    // set default values
    langCode = LANGCODE_DEFAULT;
    langFilePath = L"./pandoraGS_lang.csv"s;
    memset(events.pTriggerKeys, 0x0, EVENT_KEYS_STRING_LENGTH);
    reset(true);

    // load general config
    ConfigIO::loadConfig(&s_profileNames);
    // initialize profile array
    s_profiles.reserve(s_profileNames.size());
    for (int i = s_profileNames.size(); i > 0; --i)
        s_profiles.push_back(nullptr);

    // load default profile
    s_currentProfileId = 0u;
    s_profiles[0] = ConfigIO::loadConfigProfile(0u, false);

    s_isInitialized = true;
    unlock();
}

/// @brief Close config container and profiles
void Config::close()
{
    s_isInitialized = false;
    waitLock();
    lock();

    // clear profile array
    for (auto it = s_profiles.begin(); it != s_profiles.end(); ++it)
    {
        if (*it != nullptr)
            delete *it;
        *it = nullptr;
    }
    s_profiles.clear();
    s_profileNames.clear();
    s_currentProfileId = 0u;

    unlock();
}

/// @brief Set default config values
/// @param[in] isKeyBindingReset  Also reset event-trigger key bindings
void Config::reset(const bool isKeyBindingReset) noexcept
{
    Config::display.windowMode = display::utils::window_mode_t::fullscreen;
    Config::display.fullscreenRes = { RESOLUTION_AUTODETECT, RESOLUTION_AUTODETECT };
    Config::display.windowRes = { 800, 600 };
    Config::display.colorDepth = display::window_color_mode_t::rgb_32bit;
    Config::display.subprecisionMode = subprecision_settings_t::disabled;

    Config::timer.timeMode = events::timemode_t::highResCounter;
    Config::timer.frameLimitMode = framelimit_settings_t::limit;
    Config::timer.frameRateLimit = FRAMELIMIT_AUTODETECT;
    Config::timer.isFreqDisplay = false;

    Config::events.isNoScreenSaver = false;
    Config::events.isDebugMode = false;
    configFixBits = 0u;

    if (isKeyBindingReset)
        resetKeyBindings();
}

/// @brief Reset event-trigger key bindings
void Config::resetKeyBindings() noexcept
{
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::menuToggle)] = VK_INSERT;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::menuPrev)] = VK_PRIOR;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::menuNext)] = VK_NEXT;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::menuDefault)] = VK_HOME;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::menuAccept)] = VK_END;

    events.isWindowModeChangeable = false;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::ratioMode)] = VK_F11;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::fastForward)] = VK_NOKEY;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::slowMotion)] = VK_NOKEY;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::pause)] = VK_NOKEY;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::fps)] = VK_F12;
    events.pTriggerKeys[EVENT_KEYS_STRING_LENGTH - 1] = '\0';
}


// -- profile management -- --------------------------------------------

/// @brief Get specific profile
/// @param[in] index  Profile index (0 based)
/// @returns Profile at the specified index (if available)
ConfigProfile* Config::getProfile(const uint32_t index)
{
    waitLock();
    lock();
    if (s_isInitialized == false || index >= countProfiles())
    {
        unlock();
        return nullptr;
    }

    // if no yet loaded, load profile
    if (s_profiles[index] == nullptr)
        s_profiles[index] = ConfigIO::loadConfigProfile(index, true);
    unlock();
    return s_profiles[index];
}


/// @brief Insert profile at the specified index
/// @param[in] index     Profile index (index == 0 : append at the end ; index > 0 : specified index) 
/// @param[in] pProfile  Profile data
/// @returns Success
bool Config::insertProfile(const uint32_t index, ConfigProfile* pProfile)
{
    waitLock();
    lock();
    if (s_isInitialized == false || index >= countProfiles())
    {
        unlock();
        return false;
    }

    if (index == PROFILE_INDEX_APPEND) // insert at the end
    {
        s_profileNames.push_back(pProfile->getProfileName());
        s_profiles.push_back(pProfile);
    }
    else // insert at specified index
    {
        s_profileNames.insert(s_profileNames.begin() + index, pProfile->getProfileName());
        s_profiles.insert(s_profiles.begin() + index, pProfile);
    }

    unlock();
    return true;
}

/// @brief Change profile position in the list
/// @param[in] oldIndex  Profile to move (0 based index, bigger than 0) 
/// @param[in] newIndex  New position (0 based index, bigger than 0) 
/// @param[in] pProfile  Profile data
/// @returns Success
bool Config::moveProfile(const uint32_t oldIndex, const uint32_t newIndex)
{
    waitLock();
    lock();
    if (s_isInitialized == false || oldIndex == newIndex 
     || oldIndex >= countProfiles() || oldIndex == 0 
     || newIndex >= countProfiles() || newIndex == 0)
    {
        unlock();
        return false;
    }

    ConfigProfile* pMovedProfile = s_profiles.at(oldIndex);
    if (oldIndex < newIndex) // move after current position
    {
        for (uint32_t i = oldIndex; i < newIndex; ++i)
        {
            s_profiles.at(i) = s_profiles.at(i + 1);
            s_profileNames.at(i) = s_profileNames.at(i + 1);
        }
    }
    else // move before current position
    {
        for (uint32_t i = newIndex; i < oldIndex; ++i)
        {
            s_profiles.at(i + 1) = s_profiles.at(i);
            s_profileNames.at(i + 1) = s_profileNames.at(i);
        }
    }
    s_profiles.at(newIndex) = pMovedProfile;
    s_profileNames.at(newIndex) = pMovedProfile->getProfileName();

    unlock();
    return true;
}

/// @brief Remove specific profile
/// @param[in] index  Profile index (0 based ; note: profile 0 can't be removed) 
/// @returns Success
bool Config::removeProfile(const uint32_t index)
{
    waitLock();
    lock();
    if (s_isInitialized == false || index >= countProfiles() || index == 0)
    {
        unlock();
        return false;
    }

    // remove at index
    s_profiles.erase(s_profiles.begin() + index);
    s_profileNames.erase(s_profileNames.begin() + index);

    unlock();
    return true;
}


// -- profile selection -- ---------------------------------------------

/// @brief Set default profile as current
void Config::useDefaultProfile()
{
    waitLock();
    lock();
    if (s_isInitialized == false)
    {
        unlock();
        return;
    }

    // if no yet loaded, load profile
    if (s_profiles[0] == nullptr)
    {
        s_profiles[0] = ConfigIO::loadConfigProfile(0u, false);
    }
    // set profile as active
    s_currentProfileId = 0u;
    unlock();
}

/// @brief Set specific profile as current (if available)
/// @param[in] index  Profile index (0 based)
void Config::useProfile(const uint32_t index)
{
    waitLock();
    lock();
    if (s_isInitialized == false || index >= countProfiles())
    {
        unlock();
        return;
    }

    // if no yet loaded, load profile
    if (s_profiles[index] == nullptr)
    {
        s_profiles[index] = ConfigIO::loadConfigProfile(index, false);
    }
    // set profile as active
    s_currentProfileId = index;
    unlock();
}

/// @brief Get ID of profile before specified ID
/// @param[in] currentId  Specified ID
/// @returns Previous profile ID
uint32_t Config::getPrevProfileId(const uint32_t currentId) noexcept
{
    waitLock();
    if (s_isInitialized == false)
        return 0u;

    // calculate previous ID
    uint32_t requestedId;
    if (currentId > 0u)
        requestedId = currentId - 1u;
    else if (!s_profiles.empty())
        requestedId = countProfiles() - 1u;
    else
        requestedId = 0u;
    return requestedId;
}

/// @brief Get ID of profile after specified ID
/// @param[in] currentId  Specified ID
/// @returns Next profile ID
uint32_t Config::getNextProfileId(const uint32_t currentId) noexcept
{
    waitLock();
    if (s_isInitialized == false)
        return 0u;

    // calculate next ID
    uint32_t requestedId;
    if (s_profiles.empty() || currentId >= (countProfiles() - 1u))
        requestedId = 0u;
    else
        requestedId = currentId + 1u;
    return requestedId;
}

/// @brief Get specific profile name
/// @param[in] index  Profile index (0 based)
/// @returns Name of profile at the specified index (if available)
std::wstring* Config::getProfileName(const uint32_t index) noexcept
{
    waitLock();
    if (s_isInitialized == false || index >= countProfiles())
        return nullptr;
    return &s_profileNames[index];
}
