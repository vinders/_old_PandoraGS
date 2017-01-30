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
#include "../events/utils/std_exception.hpp"
#include "config_file_io.h"
#include "config_profile.h"
#include "config.h"
#include "config_io.h"
using namespace config;

#define CONFIG_FILE_PROFILE_PREFIX L"profile"
#define CONFIG_FILE_GAMES L"games"
#ifdef _WINDOWS
#define CONFIG_PATH L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS"
#define CONFIG_PROFILE_PATH_PREFIX L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS\\profile"
#define CONFIG_GAMES_PATH L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS\\games"
#else
#define CONFIG_PATH L"games/PandoraGS"
#define CONFIG_PROFILE_PATH_PREFIX L"games/PandoraGS/profile"
#define CONFIG_GAMES_PATH L"games/PandoraGS/games"
#endif


/// @brief Load configuration values
/// @param pOutProfileNames Profile name list to complete (or NULL to ignore it)
void ConfigIO::loadConfig(std::vector<std::wstring>* pOutProfileNames)
{
    // open main file/registry key (if available)
    ConfigFileIO reader;
    if (reader.open(CONFIG_PATH))
    {
        uint32_t profileCount = 1;
        reader.readDword(L"ProfileCount", profileCount);

        //read values
        uint32_t bufferDword;
        reader.readDword(L"Lang", bufferDword);
        if (bufferDword > (uint32_t)LANGCODE_LAST_INTERNAL && bufferDword != static_cast<uint32_t>(lang::langcode_t::customFile))
            Config::langCode = LANGCODE_DEFAULT;
        else
            Config::langCode = static_cast<lang::langcode_t>(bufferDword);
        reader.readWideString(L"LangFile", Config::langFilePath);

        reader.readDwordType(L"WinMode", Config::display.windowMode, WINDOW_MODE_LENGTH, display::utils::window_mode_t::fullscreen);
        reader.readDword(L"FullResX", Config::display.fullscreenRes.x);
        reader.readDword(L"FullResY", Config::display.fullscreenRes.y);
        reader.readDword(L"WinResX", Config::display.windowRes.x);
        reader.readDword(L"WinResY", Config::display.windowRes.y);
        reader.readBoolType(L"Color", Config::display.colorDepth, display::window_color_mode_t::rgb_32bit);
        reader.readDwordType(L"GteAcc", Config::display.subprecisionMode, SUBPRECISION_SETTINGS_LENGTH, config::subprecision_settings_t::disabled);

        reader.readDwordType(L"TimeMode", Config::timer.timeMode, TIMEMODE_LENGTH, events::timemode_t::highResCounter);
        reader.readDwordType(L"FrameLimit", Config::timer.frameLimitMode, FRAMELIMIT_SETTINGS_LENGTH, config::framelimit_settings_t::limit);
        reader.readFloat(L"FrameRate", Config::timer.frameRateLimit);
        reader.readBool(L"ShowFps", Config::timer.isFreqDisplay);

        reader.readString(L"EvtKeys", Config::events.pTriggerKeys, EVENT_KEYS_STRING_LENGTH);
        reader.readBool(L"NoScreenSaver", Config::events.isNoScreenSaver);
        reader.readBool(L"Debug", Config::events.isDebugMode);
        reader.readDword(L"FixBits", Config::configFixBits);

        reader.close();

        // auto-detect fullscreen resolution
        if (Config::display.fullscreenRes.x == 0u || Config::display.fullscreenRes.y == 0u)
            display::utils::DisplayWindow::readScreenSize(Config::display.fullscreenRes.x, Config::display.fullscreenRes.y);
        // check mininmum window resolution
        if (Config::display.windowRes.x < 320uL)
            Config::display.windowRes.x = 640u;
        if (Config::display.windowRes.y < 240u)
            Config::display.windowRes.y = 480u;

        // load list of profile names (optional)
        if (pOutProfileNames != NULL)
        {
            listProfileNames(profileCount, *pOutProfileNames);
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
/// @param profiles Array of config profiles (necessary, even if profiles not saved)
/// @throw Saving failure
void ConfigIO::saveConfig(bool hasProfiles, std::vector<ConfigProfile*>& profiles)
{
    // create main file/registry key
    ConfigFileIO writer;
    if (writer.open(CONFIG_PATH, file_io_mode_t::write) == false)
        throw events::utils::StdException("Could not create or replace the config file or reg key");

    // write config data
    writer.writeDwordType(L"Lang", Config::langCode);
    writer.writeWideString(L"LangFile", Config::langFilePath);

    writer.writeDwordType(L"WinMode", Config::display.windowMode);
    writer.writeDword(L"FullResX", Config::display.fullscreenRes.x);
    writer.writeDword(L"FullResY", Config::display.fullscreenRes.y);
    writer.writeDword(L"WinResX", Config::display.windowRes.x);
    writer.writeDword(L"WinResY", Config::display.windowRes.y);
    writer.writeBoolType(L"Color", Config::display.colorDepth);
    writer.writeDwordType(L"Subprec", Config::display.subprecisionMode);

    writer.writeDwordType(L"TimeMode", Config::timer.timeMode);
    writer.writeDwordType(L"FrameLimit", Config::timer.frameLimitMode);
    writer.writeFloat(L"FrameRate", Config::timer.frameRateLimit);
    writer.writeBool(L"ShowFps", Config::timer.isFreqDisplay);

    writer.writeString(L"EvtKeys", Config::events.pTriggerKeys, EVENT_KEYS_STRING_LENGTH);
    writer.writeBool(L"NoScreenSaver", Config::events.isNoScreenSaver);
    writer.writeBool(L"Debug", Config::events.isDebugMode);
    writer.writeDword(L"FixBits", Config::configFixBits);

    writer.writeDword(L"ProfileCount", profiles.size());
    writer.close();
   
    // save config profiles
    if (hasProfiles)
    {
        for (uint32_t p = 0u; p < profiles.size(); p++)
        {
            if (profiles[p] != nullptr)
                saveConfigProfile(*(profiles[p]));
        }
    }
}

/// @brief Initialize list of profile names
/// @param profileCount Number of profiles to read
/// @param profileNames Reference to list of names to complete
void ConfigIO::listProfileNames(uint32_t profileCount, std::vector<std::wstring>& profileNames)
{
    // set default profile name
    profileNames.push_back(L"<default>");

    // get names of other profiles
    if (profileCount > 1)
    {
        std::wstring path, buffer;
        ConfigFileIO reader;
        for (uint32_t id = 1; id < profileCount; ++id)
        {
            // open profile file/registry key (if available)
            path = std::wstring(CONFIG_PROFILE_PATH_PREFIX) + std::to_wstring(id);
            if (reader.open(path))
            {
                reader.readWideString(L"ProfileName", buffer); // read name
                reader.close();
                profileNames.push_back(buffer);
            }
            else
                profileNames.push_back(L"<undefined>");
        }
    }
}


/// @brief Load specific profile values
/// @param id Profile identifier
/// @param isNameRead Read profile name (true) or ignore (false)
/// @return Allocated config profile container (with loaded values)
/// @throw Memory allocation failure
ConfigProfile* ConfigIO::loadConfigProfile(uint32_t id, bool isNameRead)
{
    ConfigProfile* pProfile = nullptr;

    // open main file/registry key (if available)
    ConfigFileIO reader;
    std::wstring path = std::wstring(CONFIG_PROFILE_PATH_PREFIX) + std::to_wstring(id);
    if (reader.open(path))
    {
        // create profile container
        if (isNameRead)
        {
            // read profile name
            std::wstring profileName = L"<undefined>";
            reader.readWideString(L"ProfileName", profileName);
            pProfile = new ConfigProfile(id, profileName);
        }
        else
            pProfile = new ConfigProfile(id);

        // read values
        if (reader.readBool(L"ExternShader", pProfile->isExternalShaders) == false)
            pProfile->isExternalShaders = false;
        reader.readWideString(L"ExternShaderPath", pProfile->shadersPath);

        if (reader.readDword(L"IntResX", pProfile->display.internalRes.x) == false || pProfile->display.internalRes.x == 0u)
            pProfile->display.internalRes.x = 2u;
        if (reader.readDword(L"IntResY", pProfile->display.internalRes.y) == false || pProfile->display.internalRes.y == 0u)
            pProfile->display.internalRes.y = 2u;
        reader.readDwordType(L"RatioMode", pProfile->display.ratioMode, SCREEN_RATIO_MODE_LENGTH, config::screen_ratio_mode_t::aspect);
        if (reader.readDword(L"RatioStretch", pProfile->display.ratioStretch) == false)
            pProfile->display.ratioStretch = 8u;
        if (reader.readDword(L"RatioCrop", pProfile->display.ratioCrop) == false)
            pProfile->display.ratioCrop = 0u;
        reader.readBoolType(L"ForceNtscRatio", pProfile->display.isNtscRatioForced, false);
        reader.readBoolType(L"Mirror", pProfile->display.isMirrored, false);
        if (reader.readDword(L"BorderSizeX", pProfile->display.blackBorders.x) == false)
            pProfile->display.blackBorders.x = 0u;
        if (reader.readDword(L"BorderSizeY", pProfile->display.blackBorders.y) == false)
            pProfile->display.blackBorders.y = 0u;
        reader.readDwordType(L"Curvature", pProfile->display.curvature, SCREEN_CURVATURE_LENGTH, config::screen_curvature_t::none);

        reader.readDwordType(L"TexSmooth", pProfile->scaling.textureSmoothing, INTERPOLATION_MODE_LENGTH, config::interpolation_mode_t::bilinearEnhanced);
        if (reader.readDword(L"TexUpscVal", pProfile->scaling.textureScaling.factor) == false || pProfile->scaling.textureScaling.factor == 0u)
            pProfile->scaling.textureScaling.factor = 1u;
        reader.readDwordType(L"TexUpsc", pProfile->scaling.textureScaling.mode, UPSCALING_MODE_LENGTH, config::upscaling_mode_t::native);
        reader.readDwordType(L"SpriteSmooth", pProfile->scaling.spriteSmoothing, INTERPOLATION_MODE_LENGTH, config::interpolation_mode_t::nearest);
        if (reader.readDword(L"SpriteUpscVal", pProfile->scaling.spriteScaling.factor) == false || pProfile->scaling.spriteScaling.factor == 0u)
            pProfile->scaling.spriteScaling.factor = 1u;
        reader.readDwordType(L"SpriteUpsc", pProfile->scaling.spriteScaling.mode, UPSCALING_MODE_LENGTH, config::upscaling_mode_t::native);
        reader.readDwordType(L"ScreenSmooth", pProfile->scaling.screenSmoothing, SCREEN_SMOOTH_MODE_LENGTH, config::screen_smooth_mode_t::none);
        if (reader.readDword(L"ScreenUpscVal", pProfile->scaling.screenScaling.factor) == false || pProfile->scaling.screenScaling.factor == 0u)
            pProfile->scaling.screenScaling.factor = 1u;
        reader.readDwordType(L"ScreenUpsc", pProfile->scaling.screenScaling.mode, UPSCALING_MODE_LENGTH, config::upscaling_mode_t::native);
        reader.readDwordType(L"Mdec", pProfile->scaling.mdecFilter, MDEC_FILTER_LENGTH, config::mdec_filter_t::standard);
        reader.readDwordType(L"Offscreen", pProfile->offscreen, OFFSCREEN_DRAWING_LENGTH, offscreen_drawing_t::standard);
        
        reader.readDwordType(L"AntiAlias", pProfile->effects.antiAliasing, ANTIALIASING_LENGTH, antialiasing_t::none);
        //...
        reader.readDword(L"FixBits", pProfile->fixBits);

        reader.close();
    }
    // not found -> use default values
    else
    {
        if (isNameRead)
            pProfile = new ConfigProfile(id, (id == 0u) ? L"<default>" : L"<undefined>");
        else
            pProfile = new ConfigProfile(id);
        pProfile->setPresetValues(config::config_preset_t::standard);
    }
    return pProfile;
}

/// @brief Save profile values
/// @param profile Config profile container with values
/// @throw Saving failure
void ConfigIO::saveConfigProfile(ConfigProfile& profile)
{
    // create profile file/registry key
    ConfigFileIO writer;
    std::wstring path = std::wstring(CONFIG_PROFILE_PATH_PREFIX) + std::to_wstring(profile.getProfileId());
    if (writer.open(path, file_io_mode_t::write) == false)
        throw events::utils::StdException("Could not create or replace config profile file or reg key");

    // write profile data
    writer.writeWideString(L"ProfileName", profile.getProfileName());
    writer.writeBool(L"ExternShader", profile.isExternalShaders);
    writer.writeWideString(L"ExternShaderPath", profile.shadersPath);

    writer.writeDword(L"IntResX", profile.display.internalRes.x);
    writer.writeDword(L"IntResY", profile.display.internalRes.y);
    writer.writeDwordType(L"RatioMode", profile.display.ratioMode);
    writer.writeDword(L"RatioStretch", profile.display.ratioStretch);
    writer.writeDword(L"RatioCrop", profile.display.ratioCrop);
    writer.writeBool(L"ForceNtscRatio", profile.display.isNtscRatioForced);
    writer.writeBool(L"Mirror", profile.display.isMirrored);
    writer.writeDword(L"BorderSizeX", profile.display.blackBorders.x);
    writer.writeDword(L"BorderSizeY", profile.display.blackBorders.y);
    writer.writeDwordType(L"Curvature", profile.display.curvature);

    writer.writeDwordType(L"TexSmooth", profile.scaling.textureSmoothing);
    writer.writeDword(L"TexUpscVal", profile.scaling.textureScaling.factor);
    writer.writeDwordType(L"TexUpsc", profile.scaling.textureScaling.mode);
    writer.writeDwordType(L"SpriteSmooth", profile.scaling.spriteSmoothing);
    writer.writeDword(L"SpriteUpscVal", profile.scaling.spriteScaling.factor);
    writer.writeDwordType(L"SpriteUpsc", profile.scaling.spriteScaling.mode);
    writer.writeDwordType(L"ScreenSmooth", profile.scaling.screenSmoothing);
    writer.writeDword(L"ScreenUpscVal", profile.scaling.screenScaling.factor);
    writer.writeDwordType(L"ScreenUpsc", profile.scaling.screenScaling.mode);
    writer.writeDwordType(L"Mdec", profile.scaling.mdecFilter);
    writer.writeDwordType(L"Offscreen", profile.offscreen);

    writer.writeDwordType(L"AntiAlias", profile.effects.antiAliasing);
    //...
    writer.writeDword(L"FixBits", profile.fixBits);

    writer.close();
}

/// @brief Remove profile (won't change associations !)
/// @param id Profile identifier
/// @throw Saving failure
void ConfigIO::removeConfigProfile(uint32_t id)
{
    std::wstring fileName = std::wstring(CONFIG_FILE_PROFILE_PREFIX) + std::to_wstring(id);
    if (ConfigFileIO::remove(CONFIG_PATH, fileName) == false)
        throw events::utils::StdException("Could not remove config profile file or reg key");
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
