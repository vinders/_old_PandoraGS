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
#include <stdexcept>
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
#define CONFIG_INTERNAL_FILE_TYPE config::registry_io_mode_t
#define CONFIG_INTERNAL_FILE_WRITE_TYPE config::registry_io_mode_t::write
#else
#define CONFIG_PATH L"games/PandoraGS"
#define CONFIG_PROFILE_PATH_PREFIX L"games/PandoraGS/profile"
#define CONFIG_GAMES_PATH L"games/PandoraGS/games"
#define CONFIG_INTERNAL_FILE_TYPE config::file_io_mode_t
#define CONFIG_INTERNAL_FILE_WRITE_TYPE config::file_io_mode_t::write
#endif


/// @brief Load configuration values
/// @param pOutProfileNames Profile name list to complete (or NULL to ignore it)
void ConfigIO::loadConfig(std::vector<std::wstring>* pOutProfileNames)
{
    // open main file/registry key (if available)
    ConfigFileIO<CONFIG_INTERNAL_FILE_TYPE> reader;
    if (reader.open(CONFIG_PATH))
    {
        uint32_t profileCount = 1;
        reader.read(L"ProfileCount", profileCount);

        //read values
        uint32_t bufferInt;
        reader.read(L"Lang", bufferInt);
        if (bufferInt > (uint32_t)LANGCODE_LAST_INTERNAL && bufferInt != static_cast<uint32_t>(lang::langcode_t::customFile))
            Config::langCode = LANGCODE_DEFAULT;
        else
            Config::langCode = static_cast<lang::langcode_t>(bufferInt);
        reader.read(L"LangFile", Config::langFilePath);

        reader.read(L"WinMode", Config::display.windowMode, WINDOW_MODE_LENGTH, display::utils::window_mode_t::fullscreen);
        reader.read(L"FullResX", Config::display.fullscreenRes.x);
        reader.read(L"FullResY", Config::display.fullscreenRes.y);
        reader.read(L"WinResX", Config::display.windowRes.x);
        reader.read(L"WinResY", Config::display.windowRes.y);
        reader.read(L"Color", Config::display.colorDepth, display::window_color_mode_t::rgb_32bit);
        reader.read(L"GteAcc", Config::display.subprecisionMode, SUBPRECISION_SETTINGS_LENGTH, config::subprecision_settings_t::disabled);

        reader.read(L"TimeMode", Config::timer.timeMode, TIMEMODE_LENGTH, events::timemode_t::highResCounter);
        reader.read(L"FrameLimit", Config::timer.frameLimitMode, FRAMELIMIT_SETTINGS_LENGTH, config::framelimit_settings_t::limit);
        reader.read(L"FrameRate", Config::timer.frameRateLimit);
        reader.read(L"ShowFps", Config::timer.isFreqDisplay);

        reader.read(L"EvtKeys", Config::events.pTriggerKeys, EVENT_KEYS_STRING_LENGTH);
        reader.read(L"NoScreenSaver", Config::events.isNoScreenSaver);
        reader.read(L"Debug", Config::events.isDebugMode);
        reader.read(L"FixBits", Config::configFixBits);

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
    ConfigFileIO<CONFIG_INTERNAL_FILE_TYPE> writer;
    if (writer.open(CONFIG_PATH, CONFIG_INTERNAL_FILE_WRITE_TYPE) == false)
        throw std::runtime_error("Could not create or replace the config file or reg key");

    // write config data
    writer.writeIntType(L"Lang", Config::langCode);
    writer.writeWideString(L"LangFile", Config::langFilePath);

    writer.writeIntType(L"WinMode", Config::display.windowMode);
    writer.writeInt(L"FullResX", Config::display.fullscreenRes.x);
    writer.writeInt(L"FullResY", Config::display.fullscreenRes.y);
    writer.writeInt(L"WinResX", Config::display.windowRes.x);
    writer.writeInt(L"WinResY", Config::display.windowRes.y);
    writer.writeBoolType(L"Color", Config::display.colorDepth, display::window_color_mode_t::rgb_16bit);
    writer.writeIntType(L"Subprec", Config::display.subprecisionMode);

    writer.writeIntType(L"TimeMode", Config::timer.timeMode);
    writer.writeIntType(L"FrameLimit", Config::timer.frameLimitMode);
    writer.writeFloat(L"FrameRate", Config::timer.frameRateLimit);
    writer.writeBool(L"ShowFps", Config::timer.isFreqDisplay);

    writer.writeString(L"EvtKeys", Config::events.pTriggerKeys, EVENT_KEYS_STRING_LENGTH);
    writer.writeBool(L"NoScreenSaver", Config::events.isNoScreenSaver);
    writer.writeBool(L"Debug", Config::events.isDebugMode);
    writer.writeInt(L"FixBits", Config::configFixBits);

    writer.writeInt(L"ProfileCount", profiles.size());
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
        ConfigFileIO<CONFIG_INTERNAL_FILE_TYPE> reader;
        for (uint32_t id = 1; id < profileCount; ++id)
        {
            // open profile file/registry key (if available)
            path = std::wstring(CONFIG_PROFILE_PATH_PREFIX) + std::to_wstring(id);
            if (reader.open(path))
            {
                reader.read(L"ProfileName", buffer); // read name
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
    ConfigFileIO<CONFIG_INTERNAL_FILE_TYPE> reader;
    std::wstring path = std::wstring(CONFIG_PROFILE_PATH_PREFIX) + std::to_wstring(id);
    if (reader.open(path))
    {
        // create profile container
        if (isNameRead)
        {
            // read profile name
            std::wstring profileName = L"<undefined>";
            reader.read(L"ProfileName", profileName);
            pProfile = new ConfigProfile(id, profileName);
        }
        else
            pProfile = new ConfigProfile(id);

        // read values
        readConfigProfileValues(reader, *pProfile);
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

/// @brief Import profile values (external file)
/// @param id New profile identifier
/// @param path Import file path
/// @return Allocated config profile container (with loaded values)
/// @throw Memory allocation failure
ConfigProfile* ConfigIO::importConfigProfile(uint32_t id, std::wstring& path)
{
    ConfigProfile* pProfile = nullptr;

    // open import file
    ConfigFileIO<file_io_mode_t> reader;
    if (reader.open(path) == false)
        throw std::invalid_argument("The file could not be opened, is not accessible or is corrupted.");

    // read profile name + create container
    std::wstring profileName = L"<undefined>";
    reader.read(L"ProfileName", profileName);
    pProfile = new ConfigProfile(id, profileName);

    // read values
    readConfigProfileValues(reader, *pProfile);
    reader.close();
    return pProfile;
}

/// @brief Load profile values
/// @param reader Data source reader
/// @param profile Profile to complete
template<typename T>
void ConfigIO::readConfigProfileValues(ConfigFileIO<T>& reader, ConfigProfile& profile)
{
    if (reader.read(L"ExternShader", profile.isExternalShaders) == false)
        profile.isExternalShaders = false;
    reader.read(L"ExternShaderPath", profile.shadersPath);

    if (reader.read(L"IntResX", profile.display.internalRes.x) == false || profile.display.internalRes.x == 0u)
        profile.display.internalRes.x = 2u;
    if (reader.read(L"IntResY", profile.display.internalRes.y) == false || profile.display.internalRes.y == 0u)
        profile.display.internalRes.y = 2u;
    reader.read(L"RatioMode", profile.display.ratioMode, SCREEN_RATIO_MODE_LENGTH, config::screen_ratio_mode_t::aspect);
    if (reader.read(L"RatioStretch", profile.display.ratioStretch) == false)
        profile.display.ratioStretch = 8u;
    if (reader.read(L"RatioCrop", profile.display.ratioCrop) == false)
        profile.display.ratioCrop = 0u;
    reader.read(L"ForceNtscRatio", profile.display.isNtscRatioForced, false);
    reader.read(L"Mirror", profile.display.isMirrored, false);
    if (reader.read(L"BorderSizeX", profile.display.blackBorders.x) == false)
        profile.display.blackBorders.x = 0u;
    if (reader.read(L"BorderSizeY", profile.display.blackBorders.y) == false)
        profile.display.blackBorders.y = 0u;
    reader.read(L"Curvature", profile.display.curvature, SCREEN_CURVATURE_LENGTH, config::screen_curvature_t::none);

    reader.read(L"TexSmooth", profile.scaling.textureSmoothing, INTERPOLATION_MODE_LENGTH, config::interpolation_mode_t::bilinearEnhanced);
    if (reader.read(L"TexUpscVal", profile.scaling.textureScaling.factor) == false || profile.scaling.textureScaling.factor == 0u)
        profile.scaling.textureScaling.factor = 1u;
    reader.read(L"TexUpsc", profile.scaling.textureScaling.mode, UPSCALING_MODE_LENGTH, config::upscaling_mode_t::native);
    reader.read(L"SpriteSmooth", profile.scaling.spriteSmoothing, INTERPOLATION_MODE_LENGTH, config::interpolation_mode_t::nearest);
    if (reader.read(L"SpriteUpscVal", profile.scaling.spriteScaling.factor) == false || profile.scaling.spriteScaling.factor == 0u)
        profile.scaling.spriteScaling.factor = 1u;
    reader.read(L"SpriteUpsc", profile.scaling.spriteScaling.mode, UPSCALING_MODE_LENGTH, config::upscaling_mode_t::native);
    reader.read(L"ScreenSmooth", profile.scaling.screenSmoothing, SCREEN_SMOOTH_MODE_LENGTH, config::screen_smooth_mode_t::none);
    if (reader.read(L"ScreenUpscVal", profile.scaling.screenScaling.factor) == false || profile.scaling.screenScaling.factor == 0u)
        profile.scaling.screenScaling.factor = 1u;
    reader.read(L"ScreenUpsc", profile.scaling.screenScaling.mode, UPSCALING_MODE_LENGTH, config::upscaling_mode_t::native);
    reader.read(L"Mdec", profile.scaling.mdecFilter, MDEC_FILTER_LENGTH, config::mdec_filter_t::standard);
    reader.read(L"Offscreen", profile.offscreen, OFFSCREEN_DRAWING_LENGTH, offscreen_drawing_t::standard);

    reader.read(L"AntiAlias", profile.effects.antiAliasing, ANTIALIASING_LENGTH, antialiasing_t::none);
    //...
    reader.read(L"FixBits", profile.fixBits);
}


/// @brief Save profile values
/// @param profile Config profile container with values
/// @throw Saving failure
void ConfigIO::saveConfigProfile(ConfigProfile& profile)
{
    // create profile file/registry key
    ConfigFileIO<CONFIG_INTERNAL_FILE_TYPE> writer;
    std::wstring path = std::wstring(CONFIG_PROFILE_PATH_PREFIX) + std::to_wstring(profile.getProfileId());
    if (writer.open(path, CONFIG_INTERNAL_FILE_WRITE_TYPE) == false)
        throw std::runtime_error("Could not create or replace config profile file or reg key");

    // write profile data
    writeConfigProfileValues(writer, profile);
    writer.close();
}

/// @brief Export profile values (external file)
/// @param profile Config profile container with values
/// @param path Export file path
/// @throw Export failure
void ConfigIO::exportConfigProfile(ConfigProfile& profile, std::wstring& path)
{
    // create export file
    ConfigFileIO<file_io_mode_t> writer;
    if (writer.open(path, file_io_mode_t::write) == false)
        throw std::runtime_error("Could not create or replace export file");

    // write profile data
    writeConfigProfileValues(writer, profile);
    writer.close();
}

/// @brief Write profile values
/// @param writer Data destination manager
/// @param profile Source profile
template<typename T>
void ConfigIO::writeConfigProfileValues(ConfigFileIO<T>& writer, ConfigProfile& profile)
{
    writer.writeWideString(L"ProfileName", profile.getProfileName());
    writer.writeBool(L"ExternShader", profile.isExternalShaders);
    writer.writeWideString(L"ExternShaderPath", profile.shadersPath);

    writer.writeInt(L"IntResX", profile.display.internalRes.x);
    writer.writeInt(L"IntResY", profile.display.internalRes.y);
    writer.writeIntType(L"RatioMode", profile.display.ratioMode);
    writer.writeInt(L"RatioStretch", profile.display.ratioStretch);
    writer.writeInt(L"RatioCrop", profile.display.ratioCrop);
    writer.writeBool(L"ForceNtscRatio", profile.display.isNtscRatioForced);
    writer.writeBool(L"Mirror", profile.display.isMirrored);
    writer.writeInt(L"BorderSizeX", profile.display.blackBorders.x);
    writer.writeInt(L"BorderSizeY", profile.display.blackBorders.y);
    writer.writeIntType(L"Curvature", profile.display.curvature);

    writer.writeIntType(L"TexSmooth", profile.scaling.textureSmoothing);
    writer.writeInt(L"TexUpscVal", profile.scaling.textureScaling.factor);
    writer.writeIntType(L"TexUpsc", profile.scaling.textureScaling.mode);
    writer.writeIntType(L"SpriteSmooth", profile.scaling.spriteSmoothing);
    writer.writeInt(L"SpriteUpscVal", profile.scaling.spriteScaling.factor);
    writer.writeIntType(L"SpriteUpsc", profile.scaling.spriteScaling.mode);
    writer.writeIntType(L"ScreenSmooth", profile.scaling.screenSmoothing);
    writer.writeInt(L"ScreenUpscVal", profile.scaling.screenScaling.factor);
    writer.writeIntType(L"ScreenUpsc", profile.scaling.screenScaling.mode);
    writer.writeIntType(L"Mdec", profile.scaling.mdecFilter);
    writer.writeIntType(L"Offscreen", profile.offscreen);

    writer.writeIntType(L"AntiAlias", profile.effects.antiAliasing);
    //...
    writer.writeInt(L"FixBits", profile.fixBits);
}


/// @brief Remove profile (won't change associations !)
/// @param id Profile identifier
/// @throw Saving failure
void ConfigIO::removeConfigProfile(uint32_t id)
{
    std::wstring fileName = std::wstring(CONFIG_FILE_PROFILE_PREFIX) + std::to_wstring(id);
    if (ConfigFileIO<CONFIG_INTERNAL_FILE_TYPE>::remove(CONFIG_PATH, fileName) == false)
        throw std::runtime_error("Could not remove config profile file or reg key");
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
    uint32_t profileId = 0;


    //...

    return profileId;
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
