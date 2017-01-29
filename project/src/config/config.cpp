/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration container
*******************************************************************************/
#include "../globals.h"
#include <string>
#include <vector>
#include "config_common.h"
#include "config_profile.h"
#include "config_io.h"
#include "config.h"
using namespace config;

std::vector<ConfigProfile*> Config::s_profiles; ///< Config profiles (vector)
uint32_t Config::s_currentProfileId = 0u;       ///< Active profile ID
bool Config::s_isReady = false;                 ///< Fast mutual exclusion

lang::langcode_t Config::langCode; ///< Used language identifier
std::wstring Config::langFilePath; ///< External language file path

config_display_t Config::display; ///< Display settings
config_timer_t Config::timer;     ///< Timer sync settings
config_events_t Config::events;   ///< Events handling settings

uint32_t Config::configFixBits = 0u;  ///< Configured fixes
uint32_t Config::runtimeFixBits = 0u; ///< Fixes set by emulator


/// @brief Create config container (default values, no profile)
void Config::init()
{
    s_isReady = false;
    s_currentProfileId = 0u;
    langCode = LANGCODE_DEFAULT;
    langFilePath = L"./pandoraGS.lang";
    memset(events.pTriggerKeys, 0x0, EVENT_KEYS_STRING_LENGTH);
    reset(true);
}

/// @brief Close config container and profiles
void Config::close()
{
    s_profiles.clear();
    s_currentProfileId = 0u;
    s_isReady = false;
}

/// @brief Set default config values
/// @param isKeyBindingReset Also reset event-trigger key bindings
void Config::reset(bool isKeyBindingReset)
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
void Config::resetKeyBindings()
{
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::menuToggle)] = VK_INSERT;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::menuPrev)] = VK_PRIOR;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::menuNext)] = VK_NEXT;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::menuDefault)] = VK_HOME;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::menuAccept)] = VK_END;

    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::ratioMode)] = VK_F11;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::fastForward)] = VK_NOKEY;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::slowMotion)] = VK_NOKEY;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::pause)] = VK_NOKEY;
    events.pTriggerKeys[static_cast<int32_t>(events::event_trigger_t::fps)] = VK_F12;
    events.pTriggerKeys[EVENT_KEYS_STRING_LENGTH - 1] = '\0';
}


// -- profile management -- --------------------------------------------

/// @brief Get specific profile
/// @param index Profile index (0 based)
/// @return Profile at the specified index (if available)
ConfigProfile* Config::getProfile(uint32_t index)
{

}


// -- profile selection -- ---------------------------------------------

/// @brief Set default profile as current
void Config::useDefaultProfile()
{

}

/// @brief Set specific profile as current (if available)
/// @param index Profile index (0 based)
/// @throw Memory allocation failure
void Config::useProfile(uint32_t index)
{

}

/// @brief Get ID of profile before specified ID
/// @param currentId Specified ID
/// @return Previous profile ID
uint32_t Config::getPrevProfileId(uint32_t currentId)
{

}

/// @brief Get ID of profile after specified ID
/// @param currentId Specified ID
/// @return Next profile ID
uint32_t Config::getNextProfileId(uint32_t currentId)
{

}
