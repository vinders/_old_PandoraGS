/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   lang_en.h
    Description : english text content
*******************************************************************************/
#ifndef _LANG_EN_H
#define _LANG_EN_H
#include "lang.h"

/* CONFIG DIALOG CONTENT ---------------------------------------------------- */
class LangConfigEN : public LangConfig
{
    public:
    LangConfigEN()
    {
        dialogTitle  = _T("PandoraGS Driver - configuration");  // dialog
        buttonOK     = _T("OK");
        buttonCancel = _T("Cancel");
        
        pageGeneralSettings = _T("General settings");           // pages
        pageProfilesManager = _T("Profile manager");
        pageProfileSettings = _T("Profile settings");
        
        pageGenGroupDisplaySettings = _T("Display settings");   // page1 labels
        pageGenDisplayResolution    = _T("Display resolution");
        pageGenDisplayFullscreen    = _T("fullscreen");
        pageGenDisplayWindow        = _T("window");
        pageGenRenderMode           = _T("Render mode");
        pageGenAntiJitterAccuracy   = _T("Enable anti-jitter accuracy");
        pageGenVerticalSync         = _T("Vertical synchronization");
        pageGenGroupFramerate      = _T("Framerate");
        pageGenFrameSkipping       = _T("Use frame skipping");
        pageGenFramerateLimit      = _T("Use framerate limit");
        pageGenAutoDetect          = _T("auto-detect");
        pageGenFixed               = _T("fixed (10-200)");
        pageGenFpsAbbr             = _T("FPS");
        pageGenPluginKeysBtn       = _T("Plugin control keys");
        pageGenAdvancedSettingsBtn = _T("Advanced settings");
        
        pageManGroupProfilesManage = _T("Profile management"); // page2 labels
        pageManIntroLine1          = _T("Lorem ipsum");
        pageManIntroLine2          = _T("Dolor sit amet");
        pageManIntroLine3          = _T("Lorem ipsum");
        pageManIntroLine4          = _T("Dolor sit amet");
        pageManIntroLine5          = _T("...");
        pageManRemoveBtn           = _T("Remove");
        pageManExportProfileBtn    = _T("Export profile");
        pageManGroupProfilePresets = _T("Profile presets");
        pageManPresetValues        = _T("Preset values");
        pageManApplyPresetBtn      = _T("Apply preset");
        pageManAddProfileBtn       = _T("Add new profile");
        pageManImportProfileBtn    = _T("Import profile");
        pageManGameAssociationsBtn = _T("Change game associations");
        
        pageProGroupFilters     = _T("Filters");                // page3 labels
        pageProTextureFiltering = _T("Texture filtering");
        pagePro2dFiltering      = _T("2D/sprite filtering");
        pageProScreenFiltering  = _T("Screen filtering");
        pageProMdecFilter       = _T("MDEC video filter");
        pageProAntiAliasing     = _T("Anti-aliasing:");
        pageProBumpMapping      = _T("Bump mapping");
        pageProHdRange          = _T("High dynamic range:");
        pageProMotionBlur       = _T("Motion blur");
        pageProSpecialRendering = _T("Special rendering:");
        pageProRenderLevel      = _T("Level (1-4):");
        pageProGroupDisplay       = _T("Display adjustment");
        pageProInternalResolution = _T("Internal resolution");
        pageProScreenStretching   = _T("Screen stretching");
        pageProMirrorScreen       = _T("Mirror screen");
        pageProExtraRendering     = _T("Extra rendering, instead of black bars (can cause visual artifacts)");
        pageProAntiFlickerBorder  = _T("Anti-flicker black border:");
        pageProPxAbbr             = _T("px");
        pageProGroupMisc         = _T("Miscellaneous");
        pageProTvScanlines       = _T("Add TV-scanlines");
        pageProGunCursor         = _T("Show gun cursor:");
        pageProBugFixesBtn       = _T("Special game fixes");
        pageProAdvancedShaderBtn = _T("Advanced shader settings");
        
        profileDefault   = _T("<default>");
        
        // combobox config choices
        
        presetFastest    = _T("Fastest");
        presetStandard   = _T("Standard");
        presetEnhanced2D = _T("Enhanced 2D");
        presetEnhanced3D = _T("Enhanced 3D");
        
        cfgTextureSmoothing0 = _T("Nearest neighbor (native)");
        cfgTextureSmoothing1 = _T("Standard filter (bilinear)");
        cfgTextureSmoothing2 = _T("Enhanced filter (bilinear)");
        cfgTextureUpscaling0 = _T("Native resolution");
        cfgTextureUpscaling1 = _T("2xSaI upscaling");
        cfgTextureUpscaling2 = _T("2xSuper-Eagle upscaling");
        cfgTextureUpscaling3 = _T("HQ2x upscaling");
        cfgTextureUpscaling4 = _T("2xBRZ upscaling");
        cfgTextureUpscaling5 = _T("HQ3x upscaling  (slow)");
        cfgTextureUpscaling6 = _T("3xBRZ upscaling (slow)");
        
        cfgSpriteSmoothing0 = _T("Nearest neighbor (native)");
        cfgSpriteSmoothing1 = _T("Standard filter (bilinear)");
        cfgSpriteSmoothing2 = _T("Enhanced filter (bilinear)");
        cfgSpriteUpscaling0 = _T("Native resolution");
        cfgSpriteUpscaling1 = _T("2xSaI upscaling");
        cfgSpriteUpscaling2 = _T("2xSuper-Eagle upscaling");
        cfgSpriteUpscaling3 = _T("HQ2x upscaling");
        cfgSpriteUpscaling4 = _T("2xBRZ upscaling");
        cfgSpriteUpscaling5 = _T("HQ3x upscaling");
        cfgSpriteUpscaling6 = _T("3xBRZ upscaling");
        cfgSpriteUpscaling7 = _T("HQ4x upscaling  (slow)");
        cfgSpriteUpscaling8 = _T("4xBRZ upscaling (slow)");
               
        cfgScreenSmoothing0 = _T("No filter");
        cfgScreenSmoothing1 = _T("Blur effect");
        cfgScreenSmoothing2 = _T("HQ3x filter");
        cfgScreenSmoothing3 = _T("3xBRZ filter");
        cfgScreenSmoothing4 = _T("HQ4x filter");
        cfgScreenSmoothing5 = _T("4xBRZ filter");
        cfgScreenSmoothing6 = _T("4xBRZ with slight blur");
        cfgScreenSmoothing7 = _T("5xBRZ filter");
        cfgScreenSmoothing8 = _T("5xBRZ with slight blur");
        
        cfgAntiAliasing0 = _T("FXAA (fast, blurry)");
        cfgAntiAliasing1 = _T("4x MSAA (standard)");
        cfgAntiAliasing2 = _T("8x MSAA (standard)");
        cfgAntiAliasing3 = _T("4x NFAA (subtle)");
        cfgAntiAliasing4 = _T("4x SMAA (nice)");
        cfgAntiAliasing5 = _T("8x SMAA (nice)");
        cfgAntiAliasing6 = _T("2x SSAA (slow, 3D only)");
        cfgAntiAliasing7 = _T("4x SSAA (slow, 3D only)");
        
        cfgHdRange0 = _T("LumaSharpen (sharp edges)");
        cfgHdRange1 = _T("Bloom (light effect)");
        cfgHdRange2 = _T("LumaSharpen and bloom");
        
        cfgSpecialRender0 = _T("Natural vision V1 (CRT colors)");
        cfgSpecialRender1 = _T("Natural vision V2 (CRT colors)");
        cfgSpecialRender2 = _T("Cel-shading V1 (cartoon)");
        cfgSpecialRender3 = _T("Cel-shading V2 (cartoon)");
        cfgSpecialRender4 = _T("Cel-shading V3 (cartoon)");
        cfgSpecialRender5 = _T("Cel-shading V4 (cartoon)");
        cfgSpecialRender6 = _T("Storybook monochrome");
        cfgSpecialRender7 = _T("Storybook with colors");
        cfgSpecialRender8 = _T("Broken glass (overlay)");
        cfgSpecialRender9 = _T("Random noise (overlay)");
        cfgSpecialRender10= _T("Video film grain (overlay)");
        cfgSpecialRender11= _T("Blueish  (color blend)");
        cfgSpecialRender12= _T("Greenish (color blend)");
        cfgSpecialRender13= _T("Grayed   (color blend)");
        cfgSpecialRender14= _T("Black and white");
        cfgSpecialRenderC= _T("Custom (advanced settings)");
        
        cfgInternalResX0 = _T("Use display resolution");
        cfgInternalResX1 = _T("X: Native (1024px texture)");
        cfgInternalResX2 = _T("X: High (2x)");
        cfgInternalResX3 = _T("X: Very high (4x)");
        cfgInternalResX4 = _T("X: Ultra (6x, recent GPU)");
        cfgInternalResX5 = _T("X: Ultra+ (8x, recent GPU)");
        cfgInternalResY0 = _T("Use display resolution");
        cfgInternalResY1 = _T("Y: Native (512px texture)");
        cfgInternalResY2 = _T("Y: High (2x)");
        cfgInternalResY3 = _T("Y: Very high (4x)");
        cfgInternalResY4 = _T("Y: Ultra (6x, slow)");
        cfgInternalResY5 = _T("Y: Ultra+ (8x, slow)");

        cfgScreenStretching0 = _T("Native resolution multiple");
        cfgScreenStretching1 = _T("Stretched (full window size)");
        cfgScreenStretching2 = _T("Keep aspect ratio (uncut)");
        cfgScreenStretching3 = _T("Keep aspect ratio (full width)");
        cfgScreenStretching4 = _T("Close to ratio (slight stretch, uncut)");
        cfgScreenStretching5 = _T("Close to ratio (slight str., full width)");
        
        cfgGunCursor0 = _T("Small dot");
        cfgGunCursor1 = _T("Large dot");
        cfgGunCursor2 = _T("Circle");
        cfgGunCursor3 = _T("Crosshair");
        cfgGunCursor4 = _T("Circle + crosshair");
        cfgGunCursor5 = _T("Arrows");
    }
};

/* INGAME MENU CONTENT ------------------------------------------------------ */
class LangMenuEN : public LangMenu
{
    public:
    LangMenuEN()
    {
        profileSelection = "Profile selection:";
        profileConfirm   = "confirm";
        profileHideMenu  = "hide menu";
        
        pause      = "PAUSED";
        slowMotion = "SLOW MOTION MODE";
        
        debugFPS      = "FPS";
        debugPolys    = "polys";
        debugVertices = "vertices";
        
        debugLinesMode      = "lines";
        debugFilledMode     = "filled (no 2D)";
        debugFilled2DMode   = "filled";
        debugTexturedMode   = "textured (no 2D)";
        debugTextured2DMode = "textured";
    }
};

#endif
