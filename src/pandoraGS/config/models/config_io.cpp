/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_io.cpp
Description : configuration IO toolbox (load/save)
*******************************************************************************/
#ifdef _WINDOWS
#include "globals.h"
#include <string>
#include <string.h>

#if _RENDERAPI == RENDERAPI_DIRECTX
  #define REG_KEY_PATH L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS_D3D"
  #define REG_KEY_SUBPATH_PROFILE L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS_D3D\\profile"
  #define REG_KEY_SUBPATH_GAMES L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS_D3D\\games"
#else
  #define REG_KEY_PATH L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS"
  #define REG_KEY_SUBPATH_PROFILE L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS\\profile"
  #define REG_KEY_SUBPATH_GAMES L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS\\games"
#endif
#define REG_SUBKEY_NAME_PROFILE L"profile"
#define REG_SUBKEY_NAME_GAMES L"games"
#endif

using namespace std;
#include "config_io.h"
#include "registry_io.hpp"


#ifdef _WINDOWS
// -- WINDOWS - REGISTRY IO -- ---------------------------------------------------

/// <summary>List profile names</summary>
/// <returns>Array of profile names</returns>
std::wstring* ConfigIO::listProfiles()
{
    uint32_t profilesNb = 1;

    // count profiles
    HKEY configKey;
    DWORD type, size;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_KEY_PATH, 0, KEY_ALL_ACCESS, &configKey) == ERROR_SUCCESS)
    {
        readRegDword<uint32_t>(&profilesNb, &configKey, L"ProfileCount", &type, &size);
        RegCloseKey(configKey); // close
    }

    // get profile names
    std::wstring* pNames = NULL;
    if (profilesNb < 1)
    {
        pNames = new std::wstring[1];
        pNames[0] = L"<default>";
    }
    else
    {
        pNames = new std::wstring[profilesNb];
        for (uint32_t id = 0; id < profilesNb; ++id)
        {
            std::wstring keyPath = REG_KEY_SUBPATH_PROFILE;
            keyPath += std::to_wstring(id);
            if (RegOpenKeyEx(HKEY_CURRENT_USER, keyPath.c_str(), 0, KEY_ALL_ACCESS, &configKey) == ERROR_SUCCESS)
            {
                readRegWString(&(pNames[id]), &configKey, L"ProfileName", &type, &size);
                RegCloseKey(configKey); // close
            }
            else if (id == 0)
                pNames[id] = L"<default>";
            else
                pNames[id] = L"<undefined>";
        }
    }
    return pNames;
}

/// <summary>Load config values from registry/file</summary>
/// <param name="hasProfileArray">Alloc an empty array with the appropriate size</param>
/// <param name="hasProfileValues">Fill the array with profile containers</param>
/// <exception cref="std::exception">Null config container</exception>
void ConfigIO::loadConfig(bool hasProfileArray, bool hasProfileValues)
{
    uint32_t profilesNb = 1;

    // read saved data in registry (if available)
    HKEY configKey;
    DWORD type, size;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_KEY_PATH, 0, KEY_ALL_ACCESS, &configKey) == ERROR_SUCCESS)
    {
        readRegDword<uint32_t>(&profilesNb, &configKey, L"ProfileCount", &type, &size);

        // general
        readRegDword<uint32_t>(&(Config::gen_langCode), &configKey, L"Lang", &type, &size);
        readRegWString(&(Config::gen_langFilePath), &configKey, L"LangFile", &type, &size);
        readRegDword<uint32_t>(&(Config::rnd_floatAccuracy), &configKey, L"FloatAcc", &type, &size);
        readRegBool(&(Config::rnd_isDebugMode), &configKey, L"Debug", &type, &size);
        readRegBool(&(Config::rnd_isFpsDisplayed), &configKey, L"ShowFps", &type, &size);

        // display
        readRegBool(&(Config::dsp_isFullscreen), &configKey, L"Fullscreen", &type, &size);
        readRegDword<uint32_t>(&(Config::dsp_fullscnResX), &configKey, L"FullResX", &type, &size);
        readRegDword<uint32_t>(&(Config::dsp_fullscnResY), &configKey, L"FullResY", &type, &size);
        readRegDword<uint32_t>(&(Config::dsp_windowResX), &configKey, L"WinResX", &type, &size);
        readRegDword<uint32_t>(&(Config::dsp_windowResY), &configKey, L"WinResY", &type, &size);
        readRegBool(&(Config::dsp_isWindowResizable), &configKey, L"WinResize", &type, &size);
        readRegBool(&(Config::dsp_isColorDepth32), &configKey, L"Color32", &type, &size);
        // auto-detect fullscreen resolution
        if (Config::dsp_fullscnResX == 0u)
        {
            Config::dsp_fullscnResX = GetSystemMetrics(SM_CXSCREEN);
            if (Config::dsp_fullscnResX < 640u)
                Config::dsp_fullscnResX = 800u;
        }
        if (Config::dsp_fullscnResY == 0u)
        {
            Config::dsp_fullscnResY = GetSystemMetrics(SM_CYSCREEN);
            if (Config::dsp_fullscnResY < 480u)
                Config::dsp_fullscnResY = 600u;
        }
        // check min window resolution
        if (Config::dsp_windowResX < 320uL)
            Config::dsp_windowResX = 640u;
        if (Config::dsp_windowResY < 240u)
            Config::dsp_windowResY = 480u;

        // framerate
        readRegBool(&(Config::sync_isVerticalSync), &configKey, L"Vsync", &type, &size);
        readRegBool(&(Config::sync_isFrameSkip), &configKey, L"FrameSkip", &type, &size);
        readRegBool(&(Config::sync_isFrameLimit), &configKey, L"FrameLimit", &type, &size);
        readRegFloat(&(Config::sync_framerateLimit), &configKey, L"Framerate", &type, &size);
        readRegDword<timemode_t>(&(Config::sync_timeMode), &configKey, L"TimeMode", &type, &size);

        // misc
        readRegBool(&(Config::misc_isScreensaverDisabled), &configKey, L"ScreensaverOff", &type, &size);
        readRegDword<uint32_t>(&(Config::misc_genFixBits), &configKey, L"GenFixBits", &type, &size);

        // plugin key bindings
        std::string gpuKeysStr = "";
        readRegString(&gpuKeysStr, &configKey, L"GpuKeys", &type, &size);
        if (gpuKeysStr.length() == (CTRLKEYS_LENGTH - 1))
            strcpy_s(Config::misc_gpuKeys, gpuKeysStr.c_str());

        RegCloseKey(configKey); // close
    }

    // array of profile containers
    if (hasProfileArray)
    {
        if (profilesNb < 1)
            profilesNb = 1;
        CfgProfilePtr* ppProfiles = new CfgProfilePtr[profilesNb];
        Config::setProfiles(&ppProfiles, profilesNb);

        // fill array
        if (hasProfileValues)
        {
            for (uint32_t p = 0; p < profilesNb; p++)
                ppProfiles[p] = loadConfigProfile(p);
        }
        else
        {
            for (uint32_t p = 0; p < profilesNb; p++)
                ppProfiles[p] = NULL;
        }
    }
}

/// <summary>Save config values to registry/file</summary>
/// <param name="hasProfiles">Also save contained profiles (true) or config alone (false)</param>
/// <exception cref="std::exception">Key creation/opening failure</exception>
void ConfigIO::saveConfig(bool hasProfiles)
{
    // write data in registry
    HKEY configKey;
    DWORD keyStatus, val;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, REG_KEY_PATH, 0, NULL, 
        REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &configKey, &keyStatus) == ERROR_SUCCESS)
    {
        // general
        val = Config::gen_langCode;
        RegSetValueEx(configKey, L"Lang", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        RegSetValueEx(configKey, L"LangFile", 0, REG_BINARY,
            (LPBYTE)(Config::gen_langFilePath.c_str()), Config::gen_langFilePath.length());
        val = Config::rnd_floatAccuracy;
        RegSetValueEx(configKey, L"FloatAcc", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (Config::rnd_isDebugMode) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"Debug", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (Config::rnd_isFpsDisplayed) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"ShowFps", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        // display
        val = (Config::dsp_isFullscreen) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"Fullscreen", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = Config::dsp_fullscnResX;
        RegSetValueEx(configKey, L"FullResX", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = Config::dsp_fullscnResY;
        RegSetValueEx(configKey, L"FullResY", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = Config::dsp_windowResX;
        RegSetValueEx(configKey, L"WinResX", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = Config::dsp_windowResY;
        RegSetValueEx(configKey, L"WinResY", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (Config::dsp_isWindowResizable) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"WinResize", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (Config::dsp_isColorDepth32) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"Color32", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        // framerate
        val = (Config::sync_isVerticalSync) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"Vsync", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (Config::sync_isFrameSkip) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"FrameSkip", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (Config::sync_isFrameLimit) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"FrameLimit", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        setRegFloat(Config::sync_framerateLimit, &configKey, L"Framerate");
        val = (DWORD)(Config::sync_timeMode);
        RegSetValueEx(configKey, L"TimeMode", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        // misc
        val = (Config::misc_isScreensaverDisabled) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"ScreensaverOff", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = Config::misc_genFixBits;
        RegSetValueEx(configKey, L"GenFixBits", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        // plugin key bindings
        RegSetValueEx(configKey, L"GpuKeys", 0, REG_BINARY, (LPBYTE)(Config::misc_gpuKeys), CTRLKEYS_LENGTH - 1);

        // profiles number
        CfgProfilePtr* pProfiles = NULL;
        uint32_t arraySize = Config::getAllProfiles(pProfiles);
        val = arraySize;
        RegSetValueEx(configKey, L"ProfileCount", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        RegCloseKey(configKey); // close

        // save config profiles
        if (hasProfiles)
        {
            for (uint32_t p = 0u; p < arraySize; p++)
                saveConfigProfile(pProfiles[p]);
        }
    }
    else
        throw new std::exception("Could not open or create the config registry key");
}

/// <summary>Load only frame limiting values</summary>
void ConfigIO::loadFrameLimitConfig()
{
    // read saved data in registry (if available)
    HKEY configKey;
    DWORD type, size;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_KEY_PATH, 0, KEY_ALL_ACCESS, &configKey) == ERROR_SUCCESS)
    {
        // framerate
        readRegBool(&(Config::sync_isFrameSkip), &configKey, L"FrameSkip", &type, &size);
        readRegBool(&(Config::sync_isFrameLimit), &configKey, L"FrameLimit", &type, &size);
        readRegFloat(&(Config::sync_framerateLimit), &configKey, L"Framerate", &type, &size);

        RegCloseKey(configKey); // close
    }
}


/// <summary>Load specific profile values from registry/file</summary>
/// <param name="id">Profile identifier</param>
/// <returns>Allocated config profile container (with loaded values)</returns>
/// <exception cref="std::exception">Memory allocation failure</exception>
ConfigProfile* ConfigIO::loadConfigProfile(uint32_t id)
{
    ConfigProfile* pProfile = new ConfigProfile(id, L"<default>");
    if (pProfile == NULL)
        throw new std::exception("Profile allocation failure");
    pProfile->setPresetValues(ProfilePreset_Standard);

    // read saved profile (if available)
    HKEY profileKey;
    DWORD type, size;
    std::wstring keyPath = REG_KEY_SUBPATH_PROFILE;
    keyPath += std::to_wstring(id);
    if (RegOpenKeyEx(HKEY_CURRENT_USER, keyPath.c_str(), 0, KEY_ALL_ACCESS, &profileKey) == ERROR_SUCCESS)
    {
        readRegWString(&(pProfile->gen_profileName), &profileKey, L"ProfileName", &type, &size);

        //smooth/scale filters
        readRegDword<uint32_t>(&(pProfile->scl_texSmooth), &profileKey, L"TxIntp", &type, &size);
        if (pProfile->scl_texSmooth >= CFG_Interpolation_LENGTH)
            pProfile->scl_texSmooth = 0;
        readRegDword<uint32_t>(&(pProfile->scl_texUpscaleVal), &profileKey, L"TxUpscale", &type, &size);
        if (pProfile->scl_texUpscaleVal == 0)
            pProfile->scl_texUpscaleVal = 1;
        readRegDword<uint32_t>(&(pProfile->scl_texUpscaleType), &profileKey, L"TxUpType", &type, &size);
        if (pProfile->scl_texUpscaleType >= CFG_UpScaling_LENGTH)
            pProfile->scl_texUpscaleType = 0;
        readRegDword<uint32_t>(&(pProfile->scl_sprSmooth), &profileKey, L"SprIntp", &type, &size);
        if (pProfile->scl_sprSmooth >= CFG_Interpolation_LENGTH)
            pProfile->scl_sprSmooth = 0;
        readRegDword<uint32_t>(&(pProfile->scl_sprUpscaleVal), &profileKey, L"SprUpscale", &type, &size);
        if (pProfile->scl_sprUpscaleVal == 0)
            pProfile->scl_sprUpscaleVal = 1;
        readRegDword<uint32_t>(&(pProfile->scl_sprUpscaleType), &profileKey, L"SprUpType", &type, &size);
        if (pProfile->scl_sprUpscaleType >= CFG_UpScaling_LENGTH)
            pProfile->scl_sprUpscaleType = 0;
        readRegDword<uint32_t>(&(pProfile->scl_screenSmooth), &profileKey, L"ScnSmooth", &type, &size);
        if (pProfile->scl_screenSmooth >= CFG_ScreenSmoothing_LENGTH)
            pProfile->scl_screenSmooth = 0;
        readRegDword<uint32_t>(&(pProfile->scl_screenUpscaleVal), &profileKey, L"ScnUpscale", &type, &size);
        if (pProfile->scl_screenUpscaleVal == 0)
            pProfile->scl_screenUpscaleVal = 1;
        readRegDword<uint32_t>(&(pProfile->scl_screenUpscaleType), &profileKey, L"ScnUpType", &type, &size);
        if (pProfile->scl_screenUpscaleType >= CFG_UpScaling_LENGTH)
            pProfile->scl_screenUpscaleType = 0;
        readRegBool(&(pProfile->scl_isShaderUpscale), &profileKey, L"ShaderUpscale", &type, &size);
        readRegDword<uint32_t>(&(pProfile->scl_mdecFilter), &profileKey, L"Mdec", &type, &size);
        if (pProfile->scl_mdecFilter >= CFG_MdecFiltering_LENGTH)
            pProfile->scl_mdecFilter = 0;

        // shading
        readRegDword<uint32_t>(&(pProfile->shd_antiAliasing), &profileKey, L"FxAA", &type, &size);
        if (pProfile->shd_antiAliasing >= CFG_AntiAliasing_LENGTH)
            pProfile->shd_antiAliasing = 0;
        //...

        // screen adjustment
        readRegDword<uint32_t>(&(pProfile->dsp_internalResX), &profileKey, L"IntResX", &type, &size);
        if (pProfile->dsp_internalResX == 0)
            pProfile->dsp_internalResX = 2;
        readRegDword<uint32_t>(&(pProfile->dsp_internalResY), &profileKey, L"IntResY", &type, &size);
        if (pProfile->dsp_internalResY == 0)
            pProfile->dsp_internalResY = 2;
        readRegDword<uint32_t>(&(pProfile->dsp_ratioType), &profileKey, L"Ratio", &type, &size);
        readRegDword<uint32_t>(&(pProfile->dsp_stretchRatio), &profileKey, L"StretchCutRatio", &type, &size);
        if (pProfile->dsp_stretchRatio > CFG_RATIO_MAX)
            pProfile->dsp_stretchRatio = CFG_RATIO_MAX;
        readRegDword<uint32_t>(&(pProfile->dsp_cropStrength), &profileKey, L"CutStrength", &type, &size);
        if (pProfile->dsp_cropStrength > CFG_RATIO_MAX)
            pProfile->dsp_cropStrength = CFG_RATIO_MAX;
        readRegBool(&(pProfile->dsp_isScreenMirror), &profileKey, L"Mirror", &type, &size);
        readRegDword<uint32_t>(&(pProfile->dsp_borderSizeX), &profileKey, L"BorderSizeX", &type, &size);
        readRegDword<uint32_t>(&(pProfile->dsp_borderSizeY), &profileKey, L"BorderSizeY", &type, &size);
        readRegDword<uint32_t>(&(pProfile->dsp_screenCurved), &profileKey, L"CrtCurved", &type, &size);
        
        //...

        // custom fixes
        readRegDword<uint32_t>(&(pProfile->misc_fixBits), &profileKey, L"FixBits", &type, &size);
        readRegDword<uint32_t>(&(pProfile->misc_offscreenDrawing), &profileKey, L"Offscreen", &type, &size);
        if (pProfile->misc_offscreenDrawing >= CFG_OffScreenDrawing_LENGTH)
            pProfile->misc_offscreenDrawing = CFG_OffScr_Standard;

        RegCloseKey(profileKey); // close
    }
    return pProfile;
}

/// <summary>Save profile values to registry/file</summary>
/// <param name="pProfile">Config profile container with values</param>
/// <exception cref="std::exception">Key creation/opening failure</exception>
void ConfigIO::saveConfigProfile(ConfigProfile* pProfile)
{
    // write data in registry
    HKEY profileKey;
    DWORD keyStatus, val;
    std::wstring keyPath = REG_KEY_SUBPATH_PROFILE;
    keyPath += std::to_wstring(pProfile->gen_profileId);
    if (RegCreateKeyEx(HKEY_CURRENT_USER, keyPath.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, 
        KEY_ALL_ACCESS, NULL, &profileKey, &keyStatus) == ERROR_SUCCESS)
    {
        RegSetValueEx(profileKey, L"ProfileName", 0, REG_BINARY, 
                      (LPBYTE)(pProfile->gen_profileName.c_str()), pProfile->gen_profileName.length());

        //smooth/scale filters
        val = pProfile->scl_texSmooth;
        RegSetValueEx(profileKey, L"TxIntp", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->scl_texUpscaleVal;
        RegSetValueEx(profileKey, L"TxUpscale", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->scl_texUpscaleType;
        RegSetValueEx(profileKey, L"TxUpType", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->scl_sprSmooth;
        RegSetValueEx(profileKey, L"SprIntp", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->scl_sprUpscaleVal;
        RegSetValueEx(profileKey, L"SprUpscale", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->scl_sprUpscaleType;
        RegSetValueEx(profileKey, L"SprUpType", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->scl_screenSmooth;
        RegSetValueEx(profileKey, L"ScnSmooth", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->scl_screenUpscaleVal;
        RegSetValueEx(profileKey, L"ScnUpscale", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->scl_screenUpscaleType;
        RegSetValueEx(profileKey, L"ScnUpType", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (pProfile->scl_isShaderUpscale) ? 1uL : 0uL;
        RegSetValueEx(profileKey, L"ShaderUpscale", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->scl_mdecFilter;
        RegSetValueEx(profileKey, L"Mdec", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        // shading
        val = pProfile->shd_antiAliasing;
        RegSetValueEx(profileKey, L"FxAA", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        //...

        // screen adjustment
        val = pProfile->dsp_internalResX;
        RegSetValueEx(profileKey, L"IntResX", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->dsp_internalResY;
        RegSetValueEx(profileKey, L"IntResY", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->dsp_ratioType;
        RegSetValueEx(profileKey, L"Ratio", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->dsp_stretchRatio;
        RegSetValueEx(profileKey, L"StretchCutRatio", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->dsp_cropStrength;
        RegSetValueEx(profileKey, L"CutStrength", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (pProfile->dsp_isScreenMirror) ? 1uL : 0uL;
        RegSetValueEx(profileKey, L"Mirror", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->dsp_borderSizeX;
        RegSetValueEx(profileKey, L"BorderSizeX", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->dsp_borderSizeY;
        RegSetValueEx(profileKey, L"BorderSizeY", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->dsp_screenCurved;
        RegSetValueEx(profileKey, L"CrtCurved", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        //...

        // custom fixes
        val = pProfile->misc_fixBits;
        RegSetValueEx(profileKey, L"FixBits", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->misc_offscreenDrawing;
        RegSetValueEx(profileKey, L"Offscreen", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        RegCloseKey(profileKey); // close
    }
    else
        throw new std::exception("Could not open or create config profile registry key");
}

/// <summary>Remove profile key from registry/file (won't change associations !)</summary>
/// <param name="id">Profile identifier</param>
/// <exception cref="std::exception">Key opening/destruction failure</exception>
static void removeConfigProfile(uint32_t id) // !!! call AFTER setting new profile associations
{
    // remove complete profile
    HKEY baseKey;
    std::wstring subKeyName = REG_SUBKEY_NAME_PROFILE;
    subKeyName += std::to_wstring(id);
    if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_KEY_PATH, 0, KEY_ALL_ACCESS, &baseKey) == ERROR_SUCCESS)
    {
        RegDeleteKeyEx(baseKey, subKeyName.c_str(), KEY_WOW64_32KEY, 0);
        RegDeleteKeyEx(baseKey, subKeyName.c_str(), KEY_WOW64_64KEY, 0);
        RegCloseKey(baseKey); // close
    }
}


/// <summary>Remember a game/profile association (ingame)</summary>
/// <param name="profileId">Profile ID to associate with game</param>
/// <param name="gameId">Associated game ID</param>
/// <exception cref="std::exception">Key creation/opening failure</exception>
void ConfigIO::setGameAssocation(uint32_t profileId, std::string gameId)
{
    if (gameId.length() == 0)
        return;

    HKEY assocKey;
    DWORD keyStatus, val;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, REG_KEY_SUBPATH_GAMES, 0, NULL, 
        REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &assocKey, &keyStatus) == ERROR_SUCCESS)
    {
        // convert game ID string to wchar
        WCHAR valBuffer[CFG_STRING_BUFFER_LENGTH];
        MultiByteToWideChar(CP_ACP, 0, gameId.c_str(), gameId.length() + 1, valBuffer, CFG_STRING_BUFFER_LENGTH);

        // set profile association
        if (profileId > 0u || RegDeleteValue(assocKey, valBuffer) != ERROR_SUCCESS)
        {
            val = profileId;
            RegSetValueEx(assocKey, valBuffer, 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        }

        RegCloseKey(assocKey); // close
    }
}

/// <summary>Get the ID of the profile associated with a game (ingame)</summary>
/// <param name="gameId">Game ID</param>
/// <returns>Associated profile ID (or 0)</returns>
uint32_t ConfigIO::getGameAssociation(std::string gameId)
{
    uint32_t profileId = 0;
    if (gameId.length() > 0)
    {
        HKEY assocKey;
        DWORD type, size;
        if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_KEY_SUBPATH_GAMES, 0, KEY_ALL_ACCESS, &assocKey) == ERROR_SUCCESS)
        {
            // convert game ID string to wchar
            WCHAR valBuffer[CFG_STRING_BUFFER_LENGTH];
            MultiByteToWideChar(CP_ACP, 0, gameId.c_str(), gameId.length() + 1, valBuffer, CFG_STRING_BUFFER_LENGTH);

            // get profile association (if available)
            readRegDword<uint32_t>(&profileId, &assocKey, valBuffer, &type, &size);

            RegCloseKey(assocKey); // close
        }
    }
    return profileId;
}


/// <summary>Define all the games/profiles associations (settings)</summary>
/// <param name="pAssociations">List of associated games/profiles (will be freed)</param>
void ConfigIO::setProfileAssociations(ConfigIO_GameProfile_t* pAssociations)
{
    // remove all previous associations
    HKEY baseKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_KEY_PATH, 0, KEY_ALL_ACCESS, &baseKey) == ERROR_SUCCESS)
    {
        RegDeleteKeyEx(baseKey, REG_SUBKEY_NAME_GAMES, KEY_WOW64_32KEY, 0);
        RegDeleteKeyEx(baseKey, REG_SUBKEY_NAME_GAMES, KEY_WOW64_64KEY, 0);

        RegCloseKey(baseKey); // close
    }

    // set new associations
    ConfigIO_GameProfile_t* pRemoved;
    ConfigIO_GameProfile_t* pTmp = pAssociations; // chained list
    while (pTmp != NULL)
    {
        setGameAssocation(pTmp->profileId, pTmp->gameId);
        pRemoved = pTmp;
        pTmp = pTmp->pNext; // follow chain
        delete pRemoved;
    }
}

/// <summary>Get the list of all the games/profiles associations (settings)</summary>
/// <param name="ppAssociations">Null pointer for returned list</param>
void ConfigIO::getProfileAssociations(ConfigIO_GameProfile_t* pAssociations)
{
    // init chained list (dummy start item)
    ConfigIO_GameProfile_t* pStartAssoc = new ConfigIO_GameProfile_t();
    ConfigIO_GameProfile_t* pTmpAssoc = pStartAssoc;
    pStartAssoc->pNext = NULL;
    pAssociations = NULL;

    // get key values list
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_KEY_SUBPATH_GAMES, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        // general information data
        TCHAR    classNameBuffer[MAX_PATH] = TEXT("");
        DWORD    classNameSize = MAX_PATH;
        DWORD    subkeysNb = 0, subkeysMaxSize, maxClassData;
        DWORD    valuesNb = 0, valuesMaxSize, maxValueData;
        DWORD    securityDescriptorSize;
        FILETIME lastWriteTime;

        // get key information
        if (RegQueryInfoKey(hKey, classNameBuffer, &classNameSize, NULL, &subkeysNb, &subkeysMaxSize, &maxClassData, 
            &valuesNb, &valuesMaxSize, &maxValueData, &securityDescriptorSize, &lastWriteTime) == ERROR_SUCCESS)
        {
            // key subvalue data
            TCHAR  valueIdentifier[CFG_STRING_BUFFER_LENGTH];
            DWORD valueMaxSize = CFG_STRING_BUFFER_LENGTH;

            // enumerate the key values 
            uint32_t profileId;
            std::wstring gameId;
            DWORD type, size;
            for (uint32_t i = 0u; i < valuesNb; i++)
            {
                valueIdentifier[0] = '\0';
                valueMaxSize = CFG_STRING_BUFFER_LENGTH;
                if (RegEnumValue(hKey, (DWORD)i, valueIdentifier, &valueMaxSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
                {
                    // read identified key value
                    profileId = 0u;
                    readRegDword<uint32_t>(&profileId, &hKey, valueIdentifier, &type, &size);
                    if (profileId > 0u)
                    {
                        // chained list -> new element with data
                        pTmpAssoc->pNext = new ConfigIO_GameProfile_t();
                        pTmpAssoc = pTmpAssoc->pNext;

                        pTmpAssoc->profileId = profileId;
                        gameId = valueIdentifier;
                        pTmpAssoc->gameId = std::string(gameId.begin(), gameId.end());
                    }
                }
            }
        }
        RegCloseKey(hKey); // close
    }

    // return list
    pAssociations = pStartAssoc->pNext;
    delete pStartAssoc;
}


#else 
// -- LINUX-UNIX - FILE -- -----------------------------------------------------

/// <summary>List profile names</summary>
/// <returns>Array of profile names</returns>
std::wstring* ConfigIO::listProfiles()
{
}

/// <summary>Load config values from registry/file</summary>
/// <param name="hasProfileArray">Alloc an empty array with the appropriate size</param>
/// <param name="hasProfileValues">Fill the array with profile containers</param>
void ConfigIO::loadConfig(bool hasProfileArray, bool hasProfileValues)
{
    //...
}

/// <summary>Save config values to registry/file</summary>
/// <param name="hasProfiles">Also save contained profiles (true) or config alone (false)</param>
void ConfigIO::saveConfig(bool hasProfiles)
{
    //...
}

/// <summary>Load only frame limiting values</summary>
void ConfigIO::loadFrameLimitConfig()
{
    //...
}


/// <summary>Load specific profile values from registry/file</summary>
/// <param name="id">Profile identifier</param>
/// <returns>Allocated config profile container (with loaded values)</returns>
ConfigProfile* ConfigIO::loadConfigProfile(uint32_t id)
{
    ConfigProfile* pProfile = new ConfigProfile(id, L"<default>");
    pProfile->setPresetValues(ProfilePreset_Standard);
    //...
    return pProfile;
}

/// <summary>Save profile values to registry/file</summary>
/// <param name="pProfile">Config profile container with values</param>
void ConfigIO::saveConfigProfile(ConfigProfile* pProfile)
{
    //...
}

/// <summary>Remove profile key from registry/file (won't change associations !)</summary>
/// <param name="id">Profile identifier</param>
/// <exception cref="std::exception">Key opening/destruction failure</exception>
static void removeConfigProfile(uint32_t id)
{
    //...
}


/// <summary>Remember a game/profile association (ingame)</summary>
/// <param name="profileId">Profile ID to associate with game</param>
/// <param name="gameId">Associated game ID</param>
void ConfigIO::setGameAssocation(uint32_t profileId, std::string gameId)
{
    //...
}

/// <summary>Get the ID of the profile associated with a game (ingame)</summary>
/// <param name="gameId">Game ID</param>
/// <returns>Associated profile ID (or 0)</returns>
uint32_t ConfigIO::getGameAssociation(std::string gameId)
{
    //...
    return 0;
}


/// <summary>Define all the games/profiles associations (settings)</summary>
/// <param name="pAssociations">List of associated games/profiles (will be freed)</param>
void ConfigIO::setProfileAssociations(ConfigIO_GameProfile_t* pAssociations)
{
    // remove all previous associations
    //...

    // set new associations
    ConfigIO_GameProfile_t* pRemoved;
    ConfigIO_GameProfile_t* pTmp = pAssociations;
    while (pTmp != NULL)
    {
        setGameAssocation(pTmp->profileId, pTmp->gameId);
        pRemoved = pTmp;
        pTmp = pTmp->pNext;
        delete pRemoved;
    }
}

/// <summary>Get the list of all the games/profiles associations (settings)</summary>
/// <param name="ppAssociations">Null pointer for returned list</param>
void ConfigIO::getProfileAssociations(ConfigIO_GameProfile_t* pAssociations)
{
    pAssociations = NULL;
}

#endif
