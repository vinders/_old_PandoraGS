/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration input/output toolbox
*******************************************************************************/
#include "../globals.h"
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <list>
#include "config_file_io.h"
#include "config_profile.h"
#include "config.h"
#include "config_io.h"
using namespace config;

#define CONFIG_DIRECTORY_PROFILE L"profile"
#define CONFIG_DIRECTORY_GAMES L"games"
#ifdef _WINDOWS
#define CONFIG_PATH L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS"
#define CONFIG_PROFILE_PATH_PREFIX L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS\\profile"
#define REG_KEY_SUBPATH_GAMES L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS\\games"
#else
#define CONFIG_PATH L"games/PandoraGS"
#define CONFIG_PROFILE_PATH_PREFIX L"games/PandoraGS/profile"
#define REG_KEY_SUBPATH_GAMES L"games/PandoraGS/games"
#endif


/// @brief Load configuration values
/// @param pOutProfileNames Profile name list to complete (or NULL to ignore it)
void ConfigIO::loadConfig(std::vector<std::wstring>* pOutProfileNames)
{
    // open file/registry key (if available)
    ConfigFileIO reader;
    if (reader.open(CONFIG_PATH) == 0)
    {
        uint32_t profileCount = 1;
        reader.readDword(L"ProfileCount", profileCount);

        // language
        uint32_t bufferDword;
        reader.readDword(L"Lang", bufferDword);
        if (bufferDword > (uint32_t)LANGCODE_LAST_INTERNAL && bufferDword != static_cast<uint32_t>(lang::langcode_t::customFile))
            Config::langCode = lang::langcode_t::english;
        else
            Config::langCode = static_cast<lang::langcode_t>(bufferDword);
        reader.readWideString(L"LangFile", Config::langFilePath);
        // display
        reader.readDwordEnum<display::utils::window_mode_t>(L"WinMode", Config::display.windowMode, WINDOW_MODE_LENGTH, display::utils::window_mode_t::fullscreen);
        reader.readDword(L"FullResX", Config::display.fullscreenRes.x);
        reader.readDword(L"FullResY", Config::display.fullscreenRes.y);
        reader.readDword(L"WinResX", Config::display.windowRes.x);
        reader.readDword(L"WinResY", Config::display.windowRes.y);
        reader.readBoolEnum<display::window_color_mode_t>(L"Color", Config::display.colorDepth);
        reader.readDwordEnum<config::subprecision_settings_t>(L"GteAcc", Config::display.subprecisionMode, SUBPRECISION_SETTINGS_LENGTH, config::subprecision_settings_t::disabled);
        // timer sync
        //...
        // event handling
        //...
        // miscellaneous
        reader.readDword(L"FixBits", Config::configFixBits);

        reader.close();

        // load list of profile names (optional)
        if (pOutProfileNames != NULL)
        {
            listProfileNames(*pOutProfileNames);
        }
    }
    // if no config available, set default profile name
    else if (pOutProfileNames != NULL)
    {
        pOutProfileNames->push_back(L"<default>");
    }
}

/// @brief Save config values
/// @param hasProfiles Also save contained profiles (true) / only save main config (false)
/// @throw Saving failure
void ConfigIO::saveConfig(bool hasProfiles)
{

}

/// @brief Initialize list of profile names
/// @param profileNames Reference to list of names to complete
void ConfigIO::listProfileNames(std::vector<std::wstring>& profileNames)
{

}


/// @brief Load specific profile values
/// @param id Profile identifier
/// @return Allocated config profile container (with loaded values)
/// @throw Memory allocation failure
ConfigProfile* ConfigIO::loadConfigProfile(uint32_t id)
{

}

/// @brief Save profile values
/// @param profile Config profile container with values
/// @throw Saving failure
void ConfigIO::saveConfigProfile(ConfigProfile& profile)
{

}

/// @brief Remove profile (won't change associations !)
/// @param id Profile identifier
/// @throw Saving failure
void ConfigIO::removeConfigProfile(uint32_t id)
{

}


/// @brief Remember a game/profile association (ingame)
/// @param profileId Profile ID to associate with game
/// @param gameExecutableId Game identifier
/// @throw Saving failure
void ConfigIO::setGameAssocation(uint32_t profileId, std::string& gameExecutableId)
{

}

/// @brief Get the profile ID associated with a game (ingame)
/// @param gameExecutableId Game identifier
/// @return Associated profile ID (or 0)
uint32_t ConfigIO::getGameAssociation(std::string& gameExecutableId)
{

}


/// @brief Set the game/profile associations (settings)
/// @param associations List of associated games/profiles
void ConfigIO::setProfileAssociations(std::list<game_profile_association_t>& associations)
{

}

/// @brief Get the list of all the game/profile associations (settings)
/// @param outAssociations Destination variable for returned list
void ConfigIO::getProfileAssociations(std::list<game_profile_association_t>& outAssociations)
{

}
