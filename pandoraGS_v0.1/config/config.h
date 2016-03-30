/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   config.h
    Description : config container and toolbox
*******************************************************************************/
#ifndef _CONFIG_H
#define _CONFIG_H
#include "../display/renderer.h"
#include "config_profile.h"

#define DEFAULT_CONFIG 0
#define NO_CONFIG 0

#define PROFILE_MAX_NUMBER 32
#define PROFILE_NAME_MAX_LENGTH 24

#define CFG_COLOR_16BIT     0
#define CFG_COLOR_32BIT     1
#define CFG_DEBUG_LINE      0
#define CFG_DEBUG_FILL3D    1
#define CFG_DEBUG_FILLALL   2
#define CFG_DEBUG_TXTR3D    3
#define CFG_DEBUG_TXTRALL   4

typedef struct PROFILENAME
{
    int   profileId;
    char* profileName;
    
} ProfileName_t;

typedef struct GAMEASSOC
{
    char* gameId;
    int   profileId;
    struct GAMEASSOC* pNext;
    
} GameAssoc_t;

/* CONFIG TOOLBOX ----------------------------------------------------------- */
class Config
{
  private:
    unsigned int _langCode;     // language choice
    unsigned int _renderApi;    // graphics api
    unsigned int _apiVerMinor;
    bool         _bFullscreen;
    
    // profile content members
    ConfigProfile* _pProfiles;
  public:
    int profileNumber;
    ProfileName_t* profileNames;
    
    // create config
    Config();                   // init values + read global config
    ~Config();                  // destroy remaining profiles
    void setProfile(int, bool); // set profiles list + open active profile
    void unsetProfile(bool);    // destroy remaining profiles
    void enableProfile(ConfigProfile*);
    void loadConfigValues();
    long saveConfigValues(short*);
    void setDefaultKeyBindings();
    
    // member properties
    unsigned int getLangCode();
    unsigned int getRenderer();
    unsigned int getRendererMinor();
    bool isFullscreen();
    // profile management methods
    ConfigProfile* getProfile();          // ingame mode : get active profile
    ConfigProfile* getProfile(const int); // config mode : get profile by ID
    int  getProfileId(const int);
    
    // public config variables ---------------------------------------
    
    // display
    unsigned int fullResX;
    unsigned int fullResY;
    unsigned int windowResX;
    unsigned int windowResY;
    unsigned int colorDepth;
    bool bFloatAccuracy;
    bool bVerticalSync;
    
    // framerate
    bool bFrameSkipping;
    bool bFramerateLimit;
    bool bFramerateLimitAuto;
    float framerateLimitFixed;
    bool bEcoFramerateMode;
    
    // advanced
    unsigned int gpuVramSize;
    bool bDisableScreensaver;
    bool bShowClock;
    bool bShowFramerate;
    bool bSmoothFont;
    //bool bDepthBuffer;
    //bool bTextureWindowEmulation;
    //bool bMaskBit
    int  debugMode;
    unsigned long bugFixes;
    
    // plugin key bindings
    int keyProfileMenu;        // menu on/off                   --def = Ins
    int keyProfileMenuPrev;    // menu shows prev profile       --def = PgUp
    int keyProfileMenuNext;    // menu shows next profile       --def = PgDn
    int keyProfileMenuDefault; // menu shows default profile    --def = Home
    int keyProfileMenuConfirm; // apply selected profile        --def = End
    int keyPause;              // pause on/off                  --def = Del
    int keyStretching;         // next streching mode           --def = Backsp
    int keyFastForward;        // faster while pressed          --def = F9
    int keySlowMotion;         // slow on/off                   --def = F10
    int keyClock;              // clock on/off                  --def = F11
    int keyDebugMode;          // FPS on/off - next debug mode  --def = F12
};

#endif
