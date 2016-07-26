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
#define STRING_BUFFER_LENGTH 128
#include <Windows.h>
#include <tchar.h>
#include <string>
#include <string.h>
#endif
using namespace std;
#include "config_io.h"


#ifdef _WINDOWS
// -- WINDOWS - REGISTRY -- ----------------------------------------------------
///Advapi32.lib

/// <summary>Read DWORD registry value</summary>
/// <param name="pDest">Destination for read value</param>
/// <param name="pRegKey">Registry key containing value</param>
/// <param name="valName">Value identifier</param>
template<typename T> inline void readRegDword(T* pDest,HKEY* pRegKey,LPCWSTR valName,DWORD* pType,DWORD* pSize)
{
    DWORD val;
    *pSize = sizeof(DWORD);
    if (RegQueryValueEx(*pRegKey, valName, 0, pType, (LPBYTE)&val, pSize) == ERROR_SUCCESS)
        *pDest = (T)val;
}

/// <summary>Read bool registry value</summary>
/// <param name="pDest">Destination for read value</param>
/// <param name="pRegKey">Registry key containing value</param>
/// <param name="valName">Value identifier</param>
inline void readRegBool(bool* pDest, HKEY* pRegKey, LPCWSTR valName, DWORD* pType, DWORD* pSize)
{
    DWORD val;
    *pSize = sizeof(DWORD);
    if (RegQueryValueEx(*pRegKey, valName, 0, pType, (LPBYTE)&val, pSize) == ERROR_SUCCESS)
        *pDest = (val != 0uL);
}

/// <summary>Read float registry value</summary>
/// <param name="pDest">Destination for read value</param>
/// <param name="pRegKey">Registry key containing value</param>
/// <param name="valName">Value identifier</param>
inline void readRegFloat(float* pDest, HKEY* pRegKey, LPCWSTR valName, DWORD* pType, DWORD* pSize)
{
    DWORD val;
    *pSize = sizeof(DWORD);
    if (RegQueryValueEx(*pRegKey, valName, 0, pType, (LPBYTE)&val, pSize) == ERROR_SUCCESS)
        *pDest = *((float *)(&val));

    if (*pDest == 0.0f)
    {
        std::wstring legacyName = L"Conv";
        legacyName += valName;
        if (RegQueryValueEx(*pRegKey, legacyName.c_str(), 0, pType, (LPBYTE)&val, pSize) == ERROR_SUCCESS)
        {
            if (val > 5uL && (int)val > 0)
                *pDest = (float)val / 100.0f;
        }
    }
}
/// <summary>Set float registry value</summary>
/// <param name="pDest">Source value</param>
/// <param name="pRegKey">Registry key containing value</param>
/// <param name="valName">Value identifier</param>
inline void setRegFloat(float source, HKEY* pRegKey, LPCWSTR valName)
{
    DWORD val = *((DWORD *)&source);
    RegSetValueEx(*pRegKey, valName, 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

    std::wstring legacyName = L"Conv";
    legacyName += valName;
    float conv = source * 100.0f;
    val = (DWORD)conv;
    RegSetValueEx(*pRegKey, legacyName.c_str(), 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
}

/// <summary>Read string registry value</summary>
/// <param name="pDest">Destination for read value</param>
/// <param name="pRegKey">Registry key containing value</param>
/// <param name="valName">Value identifier</param>
inline void readRegString(std::string* pDest, HKEY* pRegKey, LPCWSTR valName, DWORD* pType, DWORD* pSize)
{
    WCHAR valBuffer[STRING_BUFFER_LENGTH];
    *pSize = sizeof(valBuffer);
    if (RegQueryValueEx(*pRegKey, valName, 0, NULL, (LPBYTE)valBuffer, pSize) == ERROR_SUCCESS)
    {
        //convert from wide char to narrow char array
        char ncharBuffer[STRING_BUFFER_LENGTH];
        char defChar = ' ';
        WideCharToMultiByte(CP_ACP, 0, valBuffer, -1, ncharBuffer, STRING_BUFFER_LENGTH, &defChar, NULL);
        *pDest = ncharBuffer;
    }
}
#endif


#ifdef _WINDOWS
// -- WINDOWS - CONFIG IO -- ---------------------------------------------------

/// <summary>Load config values from registry/file</summary>
/// <param name="pConfig">Existing config container to fill</param>
/// <param name="hasProfileArray">Alloc an empty array with the appropriate size</param>
/// <param name="hasProfileValues">Fill the array with profile containers</param>
/// <exception cref="std::exception">Null config container</exception>
void ConfigIO::loadConfig(Config* pConfig, bool hasProfileArray, bool hasProfileValues)
{
    unsigned int profilesNb = 1;
    if (pConfig == NULL)
        throw new std::exception("Null config container");

    // read saved data in registry (if available)
    HKEY configKey;
    DWORD type, size;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS", 
                     0, KEY_ALL_ACCESS, &configKey) == ERROR_SUCCESS)
    {
        readRegDword<unsigned int>(&profilesNb, &configKey, L"ProfileCount", &type, &size);

        // general
        readRegDword<unsigned int>(&(pConfig->gen_langCode), &configKey, L"Lang", &type, &size);
        readRegDword<unsigned int>(&(pConfig->rnd_renderApiCode), &configKey, L"RenderApi", &type, &size);
        readRegBool(&(pConfig->rnd_isFloatAccuracy), &configKey, L"FloatAcc", &type, &size);
        readRegBool(&(pConfig->rnd_hasPsxPrimitives), &configKey, L"PsxPrim", &type, &size);
        readRegDword<DebugMode>(&(pConfig->rnd_debugMode), &configKey, L"Debug", &type, &size);
        readRegBool(&(pConfig->rnd_isFpsDisplayed), &configKey, L"ShowFps", &type, &size);

        // display
        readRegBool(&(pConfig->dsp_isFullscreen), &configKey, L"Fullscreen", &type, &size);
        readRegDword<unsigned int>(&(pConfig->dsp_fullscnResX), &configKey, L"FullResX", &type, &size);
        readRegDword<unsigned int>(&(pConfig->dsp_fullscnResY), &configKey, L"FullResY", &type, &size);
        readRegDword<unsigned int>(&(pConfig->dsp_windowResX), &configKey, L"WinResX", &type, &size);
        readRegDword<unsigned int>(&(pConfig->dsp_windowResY), &configKey, L"WinResY", &type, &size);
        readRegBool(&(pConfig->dsp_isWindowResizable), &configKey, L"WinResize", &type, &size);
        readRegBool(&(pConfig->dsp_isColorDepth32), &configKey, L"Color32", &type, &size);
        // auto-detect fullscreen resolution
        if (pConfig->dsp_fullscnResX == 0)
        {
            #ifdef _WINDOWS
            pConfig->dsp_fullscnResX = GetSystemMetrics(SM_CXSCREEN);
            if (pConfig->dsp_fullscnResX < 640)
                pConfig->dsp_fullscnResX = 800;
            #else
            m_pConfig->dsp_fullscnResX = 800;
            #endif
        }
        if (pConfig->dsp_fullscnResY == 0)
        {
            #ifdef _WINDOWS
            pConfig->dsp_fullscnResY = GetSystemMetrics(SM_CYSCREEN);
            if (pConfig->dsp_fullscnResY < 480)
                pConfig->dsp_fullscnResY = 600;
            #else
            globalConfig->fullResY = 600;
            #endif
        }
        // check min window resolution
        if (pConfig->dsp_windowResX < 320)
            pConfig->dsp_windowResX = 640;
        if (pConfig->dsp_windowResY < 240)
            pConfig->dsp_windowResY = 480;

        // framerate
        readRegBool(&(pConfig->sync_isVerticalSync), &configKey, L"Vsync", &type, &size);
        readRegBool(&(pConfig->sync_isFrameSkip), &configKey, L"FrameSkip", &type, &size);
        readRegBool(&(pConfig->sync_isFrameLimit), &configKey, L"FrameLimit", &type, &size);
        readRegFloat(&(pConfig->sync_framerateLimit), &configKey, L"Framerate", &type, &size);
        readRegDword<TimingMode>(&(pConfig->sync_timeMode), &configKey, L"TimeMode", &type, &size);

        // misc
        readRegBool(&(pConfig->misc_isScreensaverDisabled), &configKey, L"ScreensaverOff", &type, &size);
        readRegDword<unsigned long>(&(pConfig->misc_genFixBits), &configKey, L"GenFixBits", &type, &size);

        // plugin key bindings
        std::string gpuKeysStr = "";
        readRegString(&gpuKeysStr, &configKey, L"GpuKeys", &type, &size);
        if (gpuKeysStr.length() == (GPUKEYS_LENGTH - 1))
            strcpy_s(pConfig->misc_gpuKeys, gpuKeysStr.c_str());

        RegCloseKey(configKey); // close
    }

    // array of profile containers
    if (hasProfileArray)
    {
        CfgProfilePtr* ppProfiles = new CfgProfilePtr[profilesNb];
        pConfig->setProfiles(&ppProfiles, profilesNb);

        // fill array
        if (hasProfileValues)
        {
            for (unsigned int p = 0; p < profilesNb; p++) 
                ppProfiles[p] = loadConfigProfile(p);
        }
        else
        {
            for (unsigned int p = 0; p < profilesNb; p++)
                ppProfiles[p] = NULL;
        }
    }
}

/// <summary>Save config values to registry/file</summary>
/// <param name="pConfig">Config container with values</param>
/// <param name="hasProfiles">Also save contained profiles (true) or config alone (false)</param>
/// <exception cref="std::exception">Key creation/opening failure</exception>
void ConfigIO::saveConfig(Config* pConfig, bool hasProfiles)
{
    // write data in registry
    HKEY configKey;
    DWORD keyStatus, val;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS",
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &configKey, &keyStatus) == ERROR_SUCCESS)
    {
        // general
        val = pConfig->gen_langCode;
        RegSetValueEx(configKey, L"Lang", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pConfig->rnd_renderApiCode;
        RegSetValueEx(configKey, L"RenderApi", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (pConfig->rnd_isFloatAccuracy) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"FloatAcc", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (pConfig->rnd_hasPsxPrimitives) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"PsxPrim", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (DWORD)(pConfig->rnd_debugMode);
        RegSetValueEx(configKey, L"Debug", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (pConfig->rnd_isFpsDisplayed) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"ShowFps", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        // display
        val = (pConfig->dsp_isFullscreen) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"Fullscreen", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pConfig->dsp_fullscnResX;
        RegSetValueEx(configKey, L"FullResX", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pConfig->dsp_fullscnResY;
        RegSetValueEx(configKey, L"FullResY", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pConfig->dsp_windowResX;
        RegSetValueEx(configKey, L"WinResX", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pConfig->dsp_windowResY;
        RegSetValueEx(configKey, L"WinResY", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (pConfig->dsp_isWindowResizable) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"WinResize", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (pConfig->dsp_isColorDepth32) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"Color32", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        // framerate
        val = (pConfig->sync_isVerticalSync) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"Vsync", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (pConfig->sync_isFrameSkip) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"FrameSkip", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (pConfig->sync_isFrameLimit) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"FrameLimit", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        setRegFloat(pConfig->sync_framerateLimit, &configKey, L"Framerate");
        val = (DWORD)(pConfig->sync_timeMode);
        RegSetValueEx(configKey, L"TimeMode", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        // misc
        val = (pConfig->misc_isScreensaverDisabled) ? 1uL : 0uL;
        RegSetValueEx(configKey, L"ScreensaverOff", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pConfig->misc_genFixBits;
        RegSetValueEx(configKey, L"GenFixBits", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        // plugin key bindings
        RegSetValueEx(configKey, L"GpuKeys", 0, REG_BINARY, (LPBYTE)(pConfig->misc_gpuKeys), GPUKEYS_LENGTH - 1);

        // profiles number
        CfgProfilePtr* pProfiles = NULL;
        unsigned long arraySize = pConfig->getAllProfiles(pProfiles);
        val = arraySize;
        RegSetValueEx(configKey, L"ProfileCount", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        RegCloseKey(configKey); // close

        // save config profiles
        if (hasProfiles)
        {
            for (unsigned int p = 0; p < arraySize; p++)
                saveConfigProfile(pProfiles[p]);
        }
    }
    else
        throw new std::exception("Could not open or create the config registry key");
}

/// <summary>Load only frame limiting values</summary>
/// <param name="pConfig">Existing config container to change</param>
void ConfigIO::loadFrameLimitConfig(Config* pConfig)
{
    // read saved data in registry (if available)
    HKEY configKey;
    DWORD type, size;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS",
        0, KEY_ALL_ACCESS, &configKey) == ERROR_SUCCESS)
    {
        // framerate
        readRegBool(&(pConfig->sync_isFrameSkip), &configKey, L"FrameSkip", &type, &size);
        readRegBool(&(pConfig->sync_isFrameLimit), &configKey, L"FrameLimit", &type, &size);
        readRegFloat(&(pConfig->sync_framerateLimit), &configKey, L"Framerate", &type, &size);

        RegCloseKey(configKey); // close
    }
}


/// <summary>Load specific profile values from registry/file</summary>
/// <param name="id">Profile identifier</param>
/// <returns>Allocated config profile container (with loaded values)</returns>
/// <exception cref="std::exception">Memory allocation failure</exception>
ConfigProfile* ConfigIO::loadConfigProfile(unsigned int id)
{
    ConfigProfile* pProfile = new ConfigProfile(id, "<default>");
    if (pProfile == NULL)
        throw new std::exception("Profile allocation failure");
    pProfile->setPresetValues(ProfilePreset_Standard);

    // read saved profile (if available)
    HKEY profileKey;
    DWORD type, size;
    std::wstring keyPath = L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS\\profile";
    keyPath += std::to_wstring(id);
    if (RegOpenKeyEx(HKEY_CURRENT_USER, keyPath.c_str(), 0, KEY_ALL_ACCESS, &profileKey) == ERROR_SUCCESS)
    {
        readRegString(&(pProfile->gen_profileName), &profileKey, L"ProfileName", &type, &size);

        //smooth/scale filters
        readRegDword<unsigned int>(&(pProfile->scl_textureSmooth), &profileKey, L"TxSmooth", &type, &size);
        readRegDword<unsigned int>(&(pProfile->scl_textureUpscale), &profileKey, L"TxUpscale", &type, &size);
        readRegDword<unsigned int>(&(pProfile->scl_spriteSmooth), &profileKey, L"SprSmooth", &type, &size);
        readRegDword<unsigned int>(&(pProfile->scl_spriteUpscale), &profileKey, L"SprUpscale", &type, &size);
        readRegDword<unsigned int>(&(pProfile->scl_screenSmooth), &profileKey, L"ScnSmooth", &type, &size);
        readRegBool(&(pProfile->scl_isMdec), &profileKey, L"Mdec", &type, &size);
        readRegBool(&(pProfile->scl_isGpu2dScaling), &profileKey, L"Gpu2DScale", &type, &size);

        // shading
        readRegDword<unsigned int>(&(pProfile->shd_antiAliasing), &profileKey, L"FxAA", &type, &size);
        //...

        // screen adjustment
        readRegDword<unsigned int>(&(pProfile->dsp_internalResX), &profileKey, L"IntResX", &type, &size);
        readRegDword<unsigned int>(&(pProfile->dsp_internalResY), &profileKey, L"IntResY", &type, &size);
        readRegDword<unsigned int>(&(pProfile->dsp_screenStretch), &profileKey, L"Stretch", &type, &size);
        readRegBool(&(pProfile->dsp_isScreenMirror), &profileKey, L"Mirror", &type, &size);
        readRegDword<unsigned int>(&(pProfile->dsp_borderSize), &profileKey, L"BorderSize", &type, &size);
        readRegDword<unsigned int>(&(pProfile->dsp_screenCurved), &profileKey, L"CrtCurved", &type, &size);
        
        //...

        // custom fixes
        readRegDword<unsigned long>(&(pProfile->misc_fixBits), &profileKey, L"FixBits", &type, &size);

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
    std::wstring keyPath = L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS\\profile";
    keyPath += std::to_wstring(pProfile->gen_profileId);
    if (RegCreateKeyEx(HKEY_CURRENT_USER, keyPath.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, 
        KEY_ALL_ACCESS, NULL, &profileKey, &keyStatus) == ERROR_SUCCESS)
    {
        RegSetValueEx(profileKey, L"ProfileName", 0, REG_BINARY, 
                      (LPBYTE)(pProfile->gen_profileName.c_str()), pProfile->gen_profileName.length());

        //smooth/scale filters
        val = pProfile->scl_textureSmooth;
        RegSetValueEx(profileKey, L"TxSmooth", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->scl_textureUpscale;
        RegSetValueEx(profileKey, L"TxUpscale", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->scl_spriteSmooth;
        RegSetValueEx(profileKey, L"SprSmooth", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->scl_spriteUpscale;
        RegSetValueEx(profileKey, L"SprUpscale", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->scl_screenSmooth;
        RegSetValueEx(profileKey, L"ScnSmooth", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (pProfile->scl_isMdec) ? 1uL : 0uL;
        RegSetValueEx(profileKey, L"Mdec", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (pProfile->scl_isGpu2dScaling) ? 1uL : 0uL;
        RegSetValueEx(profileKey, L"Gpu2dScale", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        // shading
        val = pProfile->shd_antiAliasing;
        RegSetValueEx(profileKey, L"FxAA", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        //...

        // screen adjustment
        val = pProfile->dsp_internalResX;
        RegSetValueEx(profileKey, L"IntResX", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->dsp_internalResY;
        RegSetValueEx(profileKey, L"IntResY", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->dsp_screenStretch;
        RegSetValueEx(profileKey, L"Stretch", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = (pProfile->dsp_isScreenMirror) ? 1uL : 0uL;
        RegSetValueEx(profileKey, L"Mirror", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->dsp_borderSize;
        RegSetValueEx(profileKey, L"BorderSize", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
        val = pProfile->dsp_screenCurved;
        RegSetValueEx(profileKey, L"CrtCurved", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        //...

        // custom fixes
        val = pProfile->misc_fixBits;
        RegSetValueEx(profileKey, L"FixBits", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

        RegCloseKey(profileKey); // close
    }
    else
        throw new std::exception("Could not open or create config profile registry key");
}


/// <summary>Remember a game/profile association (ingame)</summary>
/// <param name="profileId">Profile ID to associate with game</param>
/// <param name="gameId">Associated game ID</param>
/// <exception cref="std::exception">Key creation/opening failure</exception>
void ConfigIO::setGameAssocation(unsigned int profileId, std::string gameId)
{
    if (gameId.length() == 0)
        return;

    HKEY assocKey;
    DWORD keyStatus, val;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS\\games",
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &assocKey, &keyStatus) == ERROR_SUCCESS)
    {
        // convert game ID string to wchar
        WCHAR valBuffer[STRING_BUFFER_LENGTH];
        MultiByteToWideChar(CP_ACP, 0, gameId.c_str(), gameId.length() + 1, valBuffer, STRING_BUFFER_LENGTH);

        // set profile association
        if (profileId > 0 || RegDeleteValue(assocKey, valBuffer) != ERROR_SUCCESS)
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
unsigned int ConfigIO::getGameAssociation(std::string gameId)
{
    unsigned int profileId = 0;
    if (gameId.length() > 0)
    {
        HKEY assocKey;
        DWORD type, size;
        if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS\\games",
            0, KEY_ALL_ACCESS, &assocKey) == ERROR_SUCCESS)
        {
            // convert game ID string to wchar
            WCHAR valBuffer[STRING_BUFFER_LENGTH];
            MultiByteToWideChar(CP_ACP, 0, gameId.c_str(), gameId.length() + 1, valBuffer, STRING_BUFFER_LENGTH);

            // get profile association (if available)
            readRegDword<unsigned int>(&profileId, &assocKey, valBuffer, &type, &size);

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
    HKEY assocKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS",
        0, KEY_ALL_ACCESS, &assocKey) == ERROR_SUCCESS)
    {
        RegDeleteKeyEx(assocKey, L"games", KEY_WOW64_32KEY, 0);
        RegDeleteKeyEx(assocKey, L"games", KEY_WOW64_64KEY, 0);

        RegCloseKey(assocKey); // close
    }

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
    // init chained list (dummy start item)
    ConfigIO_GameProfile_t* pStartAssoc = new ConfigIO_GameProfile_t();
    ConfigIO_GameProfile_t* pTmpAssoc = pStartAssoc;
    pStartAssoc->pNext = NULL;
    pAssociations = NULL;

    // get key values list
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Vision Thing\\PSEmu Pro\\GPU\\PandoraGS\\games", 
        0, KEY_READ, &hKey) == ERROR_SUCCESS)
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
            TCHAR  valueIdentifier[STRING_BUFFER_LENGTH];
            DWORD valueMaxSize = STRING_BUFFER_LENGTH;

            // enumerate the key values 
            unsigned int profileId;
            std::wstring gameId;
            DWORD type, size;
            for (unsigned int i = 0; i < valuesNb; i++)
            {
                valueIdentifier[0] = '\0';
                valueMaxSize = STRING_BUFFER_LENGTH;
                if (RegEnumValue(hKey, (DWORD)i, valueIdentifier, &valueMaxSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
                {
                    // read identified key value
                    profileId = 0;
                    readRegDword<unsigned int>(&profileId, &hKey, valueIdentifier, &type, &size);
                    if (profileId > 0)
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

/// <summary>Load config values from registry/file</summary>
/// <param name="pConfig">Existing config container to fill</param>
/// <param name="hasProfileArray">Alloc an empty array with the appropriate size</param>
/// <param name="hasProfileValues">Fill the array with profile containers</param>
void ConfigIO::loadConfig(Config* pConfig, bool hasProfileArray, bool hasProfileValues)
{
    //... cfg values

    // array of profile containers
    if (hasProfileArray)
    {
        int profilesNb = 1;
        //... get profile nb
        CfgProfilePtr* ppProfiles = new CfgProfilePtr[profilesNb];
        pConfig->setProfiles(&ppProfiles, profilesNb);

        // fill array
        if (hasProfileValues)
        {
            for (int p = 0; p < profilesNb; p++) 
                ppProfiles[p] = loadConfigProfile(p);
        }
    }
}

/// <summary>Save config values to registry/file</summary>
/// <param name="pConfig">Config container with values</param>
/// <param name="hasProfiles">Also save contained profiles (true) or config alone (false)</param>
void ConfigIO::saveConfig(Config* pConfig, bool hasProfiles)
{
    //...
}

/// <summary>Load only frame limiting values</summary>
/// <param name="pConfig">Existing config container to change</param>
void ConfigIO::loadFrameLimitConfig(Config* pConfig)
{
    //...
}


/// <summary>Load specific profile values from registry/file</summary>
/// <param name="id">Profile identifier</param>
/// <returns>Allocated config profile container (with loaded values)</returns>
ConfigProfile* ConfigIO::loadConfigProfile(unsigned int id)
{
    ConfigProfile* pProfile = new ConfigProfile(id, "<default>");
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


/// <summary>Remember a game/profile association (ingame)</summary>
/// <param name="profileId">Profile ID to associate with game</param>
/// <param name="gameId">Associated game ID</param>
void ConfigIO::setGameAssocation(unsigned int profileId, std::string gameId)
{
    //...
}

/// <summary>Get the ID of the profile associated with a game (ingame)</summary>
/// <param name="gameId">Game ID</param>
/// <returns>Associated profile ID (or 0)</returns>
unsigned int ConfigIO::getGameAssociation(std::string gameId)
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
