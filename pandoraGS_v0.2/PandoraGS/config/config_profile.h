/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_profile.h
Description : configuration profile container
*******************************************************************************/
#ifndef _CONFIG_PROFILE_H
#define _CONFIG_PROFILE_H
#include "globals.h"

#include <string>

// presets
enum ProfilePreset
{
    ProfilePreset_Fastest = 0,
    ProfilePreset_Standard = 1,
    ProfilePreset_Enhanced2D = 2,
    ProfilePreset_Enhanced3D = 3,
};


// Driver configuration profile container
class ConfigProfile
{
public:
    // general
    unsigned int gen_profileId;     // profile unique identifier
    std::string  gen_profileName;   // profile displayed name

    // smooth/scale filters
    unsigned int scl_textureSmooth;     // textures smoothing
    unsigned int scl_textureUpscale;    // textures hi-res upscaling
    unsigned int scl_spriteSmooth;      // 2D/sprites smoothing
    unsigned int scl_spriteUpscale;     // sprites hi-res upscaling
    unsigned int scl_screenSmooth;      // global screen smoothing
    bool         scl_isMdec;            // MDEC video filter
    bool         scl_isGpu2dScaling;    // scaling sprites/screen with GPU shaders (true) or software
    //ajout bruit aux textures (pour les rendre plus précises) -> taux : 0 - 10
    //...
    
    // shading
    unsigned int shd_antiAliasing;      // anti-aliasing
    //bump mapping
    //motion blur
    //HDR bloom
    //HDR lumasharpen
    //renforcement (masque laplacien, unsharp masking) + param force (1 - 10)
    //effets (cel-shading V1 à 4, kirsch-négatif, verre brisé, storybook, ...)
    //...

    // screen adjustment
    unsigned int dsp_internalResX;      // internal resolution [x]
    unsigned int dsp_internalResY;      // internal resolution [y]
    unsigned int dsp_screenStretch;     // screen stretching mode
    bool         dsp_isScreenMirror;    // screen mirror (mirrored/normal)
    unsigned int dsp_borderSize;        // add black borders (0 = none)
    unsigned int dsp_screenCurved;      // emulate curved CRT screen (0 to 2 ; 0 = none)

    // color filters
    //couleurs -> natural vision, couleurs daltoniens
    //inscrustations -> couleur, type incrustation, transparence
    //scanlines -> plusieurs sortes (noir, ligne dédoublée en plus foncé, quadrillage)
    //scanlines -> choix intensité et transparence
    //corrections -> gamma (+ presets PAL, NTSC, neutre), contraste
    //...

    // custom fixes
    bool         sync_hasFixAutoLimit;  // fix - use theoretical FPS limit (25 or 30, doubled if interlaced)
    bool         sync_hasFixInterlace;  // chronocross fix - switch during status read instead of update lace
    bool         dsp_hasFixExpandScreen;// capcom fix - fix screen width to show full area
    //alpha/maskbit/...
    //framebuffer/...


public:
    /// <summary>Create profile container (with preset values)</summary>
    /// <param name="id">Profile unique identifier</param>
    /// <param name="name">Profile name</param>
    ConfigProfile(unsigned int id, std::string name);
    /// <summary>Copy profile container</summary>
    /// <param name="copy">Profile container to copy</param>
    ConfigProfile(ConfigProfile& copy);

    /// <summary>Set profile preset values</summary>
    /// <param name="preset">Default values to use</param>
    void setPresetValues(const ProfilePreset preset);
};


// smoothing modes
enum CfgSmoothing
{
    CfgSmoothing_Nearest = 0,
    CfgSmoothing_Bilinear = 1,
    CfgSmoothing_BilinearEnhanced = 2,
    CfgSmoothing_Bicubic = 3,
    CfgSmoothing_BicubicEnhanced = 4,
    CfgSmoothing_GaussianBlur = 5
};
// upscaling modes
enum CfgUpscaling
{
    CfgUpscaling_Native = 0,
    CfgUpscaling_2xSaI = 1,
    CfgUpscaling_2xSuperEagle = 2,
    CfgUpscaling_HQ2X = 3,
    CfgUpscaling_HQ3X = 4,
    CfgUpscaling_HQ4X = 5,
    CfgUpscaling_2xBRZ = 6,
    CfgUpscaling_3xBRZ = 7,
    CfgUpscaling_4xBRZ = 8
};
// screen smoothing
enum CfgScreenSmooth
{
    CfgScreenSmooth_None = 0,
    CfgScreenSmooth_Blur = 1, // blur
    CfgScreenSmooth_3xBRZ = 2,
    CfgScreenSmooth_4xBRZ = 3,
    CfgScreenSmooth_5xBRZ = 4,
    CfgScreenSmooth_Blur3xBRZ = 2, // filter + bilinear scaling, then blur
    CfgScreenSmooth_Blur4xBRZ = 3,
    CfgScreenSmooth_Blur5xBRZ = 4
    //...
    //xBR-Lv2-3D
    //...
};
// screen stretching
enum CfgStretching
{
    CfgStretching_Stretch = 0,
    CfgStretching_Pixel = 1,
    CfgStretching_PixelCut = 2,
    CfgStretching_Aspect = 3,
    CfgStretching_AspectCut = 4,
    CfgStretching_AspectHalfCut = 5,
    CfgStretching_SemiAspect = 6,
    CfgStretching_SemiAspectCut = 7
};
#define CFGSTRETCHING_LAST CfgStretching_AspectSlightCut
// anti-aliasing
enum CfgAntiAliasing
{
    CfgAntiAliasing_None = 0,
    CfgAntiAliasing_FXAA = 1,
    CfgAntiAliasing_NFAA = 2,
    CfgAntiAliasing_MSAA4 = 3,
    CfgAntiAliasing_MSAA8 = 4,
    CfgAntiAliasing_SMAA4 = 5,
    CfgAntiAliasing_SMAA8 = 6
};

#endif
