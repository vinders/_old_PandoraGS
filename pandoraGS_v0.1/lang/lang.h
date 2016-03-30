/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   lang.h
    Description : language specific content
*******************************************************************************/
#ifndef _LANG_H
#define _LANG_H
#include "../globals.h"
using namespace std;
#include <string>
#if _SYSTEM != _WINDOWS
    #define LPCTSTR string
    #define _T(msg) msg
#endif

/* language codes */
#define LANGCODE_EN 0
#define LANGCODE_ES 1
#define LANGCODE_FR 2
#define LANGCODE_DE 3

/* language labels */                           // use unicode project
#define LANG_LABEL0 " English"
#define LANG_LABEL1 " Espa\x0F1ol"
#define LANG_LABEL2 " Fran\x0E7\x061is"
#define LANG_LABEL3 " Deutsch"

/* CONFIG DIALOG CONTENT (common abstract) ---------------------------------- */
class LangConfig
{
  private:
    unsigned int _code;
    
  protected:
    LangConfig() {};
  public:
    static LangConfig* getInstance(unsigned int); // factory pattern
    
    // properties
    unsigned int getCode();
    void setCode(unsigned int);
    
    // language specific variables
    LPCTSTR dialogTitle;                    // dialog
    LPCTSTR buttonOK;
    LPCTSTR buttonCancel;
    
    LPCTSTR pageGeneralSettings;            // pages
    LPCTSTR pageProfilesManager;
    LPCTSTR pageProfileSettings;
    
    LPCTSTR pageGenGroupDisplaySettings;    // page1 labels
    LPCTSTR pageGenDisplayResolution;
    LPCTSTR pageGenDisplayFullscreen;
    LPCTSTR pageGenDisplayWindow;
    LPCTSTR pageGenRenderMode;
    LPCTSTR pageGenAntiJitterAccuracy;
    LPCTSTR pageGenVerticalSync;
    LPCTSTR pageGenGroupFramerate;
    LPCTSTR pageGenFrameSkipping;
    LPCTSTR pageGenFramerateLimit;
    LPCTSTR pageGenAutoDetect;
    LPCTSTR pageGenFixed;
    LPCTSTR pageGenFpsAbbr;
    LPCTSTR pageGenPluginKeysBtn;
    LPCTSTR pageGenAdvancedSettingsBtn;
    
    LPCTSTR pageManGroupProfilesManage;     // page2 labels
    LPCTSTR pageManIntroLine1;
    LPCTSTR pageManIntroLine2;
    LPCTSTR pageManIntroLine3;
    LPCTSTR pageManIntroLine4;
    LPCTSTR pageManIntroLine5;
    LPCTSTR pageManRemoveBtn;
    LPCTSTR pageManExportProfileBtn;
    LPCTSTR pageManGroupProfilePresets;
    LPCTSTR pageManPresetValues;
    LPCTSTR pageManApplyPresetBtn;
    LPCTSTR pageManAddProfileBtn;
    LPCTSTR pageManImportProfileBtn;
    LPCTSTR pageManGameAssociationsBtn;
    
    LPCTSTR pageProGroupFilters;            // page3 labels
    LPCTSTR pageProTextureFiltering;
    LPCTSTR pagePro2dFiltering;
    LPCTSTR pageProScreenFiltering;
    LPCTSTR pageProMdecFilter;
    LPCTSTR pageProAntiAliasing;
    LPCTSTR pageProBumpMapping;
    LPCTSTR pageProHdRange;
    LPCTSTR pageProMotionBlur;
    LPCTSTR pageProSpecialRendering;
    LPCTSTR pageProRenderLevel;
    LPCTSTR pageProGroupDisplay;
    LPCTSTR pageProInternalResolution;
    LPCTSTR pageProScreenStretching;
    LPCTSTR pageProMirrorScreen;
    LPCTSTR pageProExtraRendering;
    LPCTSTR pageProAntiFlickerBorder;
    LPCTSTR pageProPxAbbr;
    LPCTSTR pageProGroupMisc;
    LPCTSTR pageProTvScanlines;
    LPCTSTR pageProGunCursor;
    LPCTSTR pageProBugFixesBtn;
    LPCTSTR pageProAdvancedShaderBtn;
    
    LPCTSTR profileDefault;
    
    // combobox config choices
    
    LPCTSTR presetFastest;
    LPCTSTR presetStandard;
    LPCTSTR presetEnhanced2D;
    LPCTSTR presetEnhanced3D;
    
    LPCTSTR cfgTextureSmoothing0;
    LPCTSTR cfgTextureSmoothing1;
    LPCTSTR cfgTextureSmoothing2;
    LPCTSTR cfgTextureUpscaling0;
    LPCTSTR cfgTextureUpscaling1;
    LPCTSTR cfgTextureUpscaling2;
    LPCTSTR cfgTextureUpscaling3;
    LPCTSTR cfgTextureUpscaling4;
    LPCTSTR cfgTextureUpscaling5;
    LPCTSTR cfgTextureUpscaling6;
    
    LPCTSTR cfgSpriteSmoothing0;
    LPCTSTR cfgSpriteSmoothing1;
    LPCTSTR cfgSpriteSmoothing2;
    LPCTSTR cfgSpriteUpscaling0;
    LPCTSTR cfgSpriteUpscaling1;
    LPCTSTR cfgSpriteUpscaling2;
    LPCTSTR cfgSpriteUpscaling3;
    LPCTSTR cfgSpriteUpscaling4;
    LPCTSTR cfgSpriteUpscaling5;
    LPCTSTR cfgSpriteUpscaling6;
    LPCTSTR cfgSpriteUpscaling7;
    LPCTSTR cfgSpriteUpscaling8;
           
    LPCTSTR cfgScreenSmoothing0;
    LPCTSTR cfgScreenSmoothing1;
    LPCTSTR cfgScreenSmoothing2;
    LPCTSTR cfgScreenSmoothing3;
    LPCTSTR cfgScreenSmoothing4;
    LPCTSTR cfgScreenSmoothing5;
    LPCTSTR cfgScreenSmoothing6;
    LPCTSTR cfgScreenSmoothing7;
    LPCTSTR cfgScreenSmoothing8;
    
    LPCTSTR cfgAntiAliasing0;
    LPCTSTR cfgAntiAliasing1;
    LPCTSTR cfgAntiAliasing2;
    LPCTSTR cfgAntiAliasing3;
    LPCTSTR cfgAntiAliasing4;
    LPCTSTR cfgAntiAliasing5;
    LPCTSTR cfgAntiAliasing6;
    LPCTSTR cfgAntiAliasing7;
    
    LPCTSTR cfgHdRange0;
    LPCTSTR cfgHdRange1;
    LPCTSTR cfgHdRange2;
    
    LPCTSTR cfgSpecialRender0;
    LPCTSTR cfgSpecialRender1;
    LPCTSTR cfgSpecialRender2;
    LPCTSTR cfgSpecialRender3;
    LPCTSTR cfgSpecialRender4;
    LPCTSTR cfgSpecialRender5;
    LPCTSTR cfgSpecialRender6;
    LPCTSTR cfgSpecialRender7;
    LPCTSTR cfgSpecialRender8;
    LPCTSTR cfgSpecialRender9;
    LPCTSTR cfgSpecialRender10;
    LPCTSTR cfgSpecialRender11;
    LPCTSTR cfgSpecialRender12;
    LPCTSTR cfgSpecialRender13;
    LPCTSTR cfgSpecialRender14;
    LPCTSTR cfgSpecialRenderC;
    
    LPCTSTR cfgInternalResX0;
    LPCTSTR cfgInternalResX1;
    LPCTSTR cfgInternalResX2;
    LPCTSTR cfgInternalResX3;
    LPCTSTR cfgInternalResX4;
    LPCTSTR cfgInternalResX5;
    LPCTSTR cfgInternalResY0;
    LPCTSTR cfgInternalResY1;
    LPCTSTR cfgInternalResY2;
    LPCTSTR cfgInternalResY3;
    LPCTSTR cfgInternalResY4;
    LPCTSTR cfgInternalResY5;
    
    LPCTSTR cfgScreenStretching0;
    LPCTSTR cfgScreenStretching1;
    LPCTSTR cfgScreenStretching2;
    LPCTSTR cfgScreenStretching3;
    LPCTSTR cfgScreenStretching4;
    LPCTSTR cfgScreenStretching5;
    
    LPCTSTR cfgGunCursor0;
    LPCTSTR cfgGunCursor1;
    LPCTSTR cfgGunCursor2;
    LPCTSTR cfgGunCursor3;
    LPCTSTR cfgGunCursor4;
    LPCTSTR cfgGunCursor5;
};

/* INGAME MENU CONTENT (common abstract) ------------------------------------ */
class LangMenu
{
  private:
    unsigned int _code;
    
  protected:
    LangMenu() {};
  public:
    static LangMenu* getInstance(unsigned int); // factory pattern
    
    // properties
    unsigned int getCode();
    void setCode(unsigned int);
    
    // language specific variables
    string profileSelection;
    string profileConfirm;
    string profileHideMenu;
    
    string pause;
    string slowMotion;
    
    string debugFPS;
    string debugPolys;
    string debugVertices;
    
    string debugLinesMode;
    string debugFilledMode;
    string debugFilled2DMode;
    string debugTexturedMode;
    string debugTextured2DMode;
};

#endif
