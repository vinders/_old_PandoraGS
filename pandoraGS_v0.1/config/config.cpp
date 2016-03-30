/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   config.cpp
    Description : config container and toolbox
*******************************************************************************/
using namespace std;
#include <stdlib.h>
#include <string.h>
#include "../globals.h"
#include "../lang/lang.h"
#include "../display/renderer.h"
#include "config.h"

/* base config constructor - init static values ----------------------------- */
Config::Config()
{
    // set default config values
    _langCode    = LANGCODE_EN;
    _renderApi   = GLAPI_DEFAULTCODE;
    _apiVerMinor = GLAPI_DEFAULTMINOR;
    setDefaultKeyBindings();
    _pProfiles = NULL;
    
    _bFullscreen = true;
    colorDepth   = CFG_COLOR_32BIT;
    fullResX = fullResY = 0; // auto-detect
    windowResX   = 800;
    windowResY   = 600;
    bFloatAccuracy  = false;
    bVerticalSync   = true;
    bFrameSkipping  = false;
    bFramerateLimit = true;
    bFramerateLimitAuto = true;
    framerateLimitFixed = FPS_USERDEFAULT;
    bEcoFramerateMode = false;
    
    gpuVramSize = 0; // auto-detect
    bDisableScreensaver = true;
    bShowClock     = false;
    bShowFramerate = false;
    bSmoothFont    = true;
    //bDepthBuffer   = false;
    //bTextureWindowEmulation = true;
    //bMaskBit = true;
    debugMode = -1;
    bugFixes  = 0;
}

/* config destructor -------------------------------------------------------- */
Config::~Config()
{
    unsetProfile(false);
}

/* read general config values ----------------------------------------------- */
void Config::loadConfigValues()
{
    // read user config values (if available)
    //!pour chaque valeur trouvée (registre), modifier config
}

/* save configured config and profile values -------------------------------- */
long Config::saveConfigValues(short* changes)
{
    //...
    return 0;
}


/* config profile creation -------------------------------------------------- */
void Config::setProfile(int profile, bool configMode)
{
    // get profiles information
    profileNumber = 0;
    profileNames = NULL;
    //!lire nombre profils
    //...
    
    // load profiles
    if (profileNumber)
    {
        // get all profiles names
        if (profileNames == NULL)
        {
            // memory allocation
            char buffer[PROFILE_NAME_MAX_LENGTH+1] = "";
            int bufferId = 0;
            if (configMode) // config mode - max number
                profileNames = (ProfileName_t*)malloc(PROFILE_MAX_NUMBER*sizeof(ProfileName_t));
            else // game mode - only existing profiles
                profileNames = (ProfileName_t*)malloc(profileNumber*sizeof(ProfileName_t));
            
            // read and complete profiles names
            int pr;
            for (pr = 0; pr < profileNumber; pr++)
            {
                //!strcpy(buffer,REG);//!lire nom
                //! lire ID
                //...
                profileNames[pr].profileId = bufferId;
                profileNames[pr].profileName = new char[strlen(buffer) + 1];
                profileNames[pr].profileName[strlen(buffer)] = '\0';
                strcpy(profileNames[pr].profileName, buffer);
            }
            // config mode - fill empty profiles indexes
            if (configMode)
            {
                while (pr < PROFILE_MAX_NUMBER)
                {
                    profileNames[pr].profileId = 0;
                    profileNames[pr].profileName = NULL;
                    pr++;
                }
            }
        }

        // config mode - profiles chain
        if (configMode)
        {
            ConfigProfile* last = _pProfiles;
            while (last->pNext)
            {
                last = last->pNext;
            }
            last->pNext = new ConfigProfile(profile);
        }
        // game mode - load only selected profile
        else
        {
            if (profile > profileNumber)
                profile = DEFAULT_CONFIG;
                
            ConfigProfile* newProfile = new ConfigProfile(profile);
            if (newProfile)
                enableProfile(newProfile);
            else
            {
                //! afficher erreur
            }
        }
    }
    // load default profile
    else
    {
        ConfigProfile* newProfile = new ConfigProfile(DEFAULT_CONFIG);
        enableProfile(newProfile);
    }
}

/* config profile ingame replacement ---------------------------------------- */
void Config::enableProfile(ConfigProfile* newProfile)
{
    // replace profile
    ConfigProfile* oldProfile = _pProfiles;
    _pProfiles = newProfile;
    if (oldProfile)
        delete oldProfile;
    
    // read profile special fixes
    bugFixes = 0;
    if (newProfile)
    {
        if (_pProfiles->bOddEvenBit)
            bugFixes |= FIX_ODDEVENBIT;
        if (_pProfiles->bLowResTimer)
            bugFixes |= FIX_NOFPSCOUNTER;
        if (_pProfiles->bExpandWidth)
            bugFixes |= FIX_DISPWIDTHS;
    }
}

/* remove all profiles ------------------------------------------------------ */
void Config::unsetProfile(bool configMode)
{
    // remove allocated names
    if (profileNames)
    {
        if (configMode)
            profileNumber = PROFILE_MAX_NUMBER;
        for (int pr = 0; pr < profileNumber; pr++)
        {
            if (profileNames[pr].profileName)
                delete [] profileNames[pr].profileName;
        }
        free(profileNames);
        profileNames = NULL;
        profileNumber = 0;
    }
    
    // remove allocated profiles
    ConfigProfile* removed = NULL;
    while (_pProfiles)
    {
        removed = _pProfiles;
        _pProfiles = _pProfiles->pNext;
        delete removed;
    }
    _pProfiles = NULL;
}


/* reset default plugin control keys ---------------------------------------- */
void Config::setDefaultKeyBindings()
{
    // profile menu keys
    keyProfileMenu        = CFG_KEY_INSERT;
    keyProfileMenuPrev    = CFG_KEY_PGUP;
    keyProfileMenuNext    = CFG_KEY_PGDOWN;
    keyProfileMenuDefault = CFG_KEY_HOME;
    keyProfileMenuConfirm = CFG_KEY_END;
    
    // misc on/off keys
    keyPause       = CFG_KEY_DELETE;
    keyStretching  = CFG_KEY_BACKSPACE;
    keyFastForward = CFG_KEY_F9;
    keySlowMotion  = CFG_KEY_F10;
    keyClock       = CFG_KEY_F11;
    keyDebugMode   = CFG_KEY_F12;
}

/* private members properties ----------------------------------------------- */
unsigned int Config::getLangCode()
{
    return _langCode;
}
unsigned int Config::getRenderer()
{
    return _renderApi;
}
unsigned int Config::getRendererMinor()
{
    return _apiVerMinor;
}
bool Config::isFullscreen()
{
    return _bFullscreen;
}

/* profile access ----------------------------------------------------------- */
ConfigProfile* Config::getProfile() // standard access
{
    return _pProfiles;
}
int Config::getProfileId(const int index) // get profile ID in config mode
{
    //find profile ID
    int profileId = 0;
    if (profileNames && index && index < PROFILE_MAX_NUMBER)
    {
        int indexCursor = 0;
        for (int i = 0; i < PROFILE_MAX_NUMBER && !profileId; i++)
        {
            if (profileNames[i].profileId)
            {
                indexCursor += 1;
                if (indexCursor == index)
                    profileId = profileNames[i].profileId;
            }
        }
    }
    return profileId;
}
ConfigProfile* Config::getProfile(const int profileId) // config mode access
{
    // find profile container
    ConfigProfile* cursor = _pProfiles;
    while (cursor->pNext && cursor->profileId != profileId)
    {
        cursor = cursor->pNext;
    }
    if (cursor->profileId != profileId)
        cursor = NULL;
    
    return cursor;
}

