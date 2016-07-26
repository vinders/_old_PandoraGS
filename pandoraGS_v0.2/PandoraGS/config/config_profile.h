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

// custom fixes
#define CFG_FIX_FRAMEBUFFER_FF7        0x1uL // FF7 battle cursor fix - adjusted framebuffer access
#define CFG_FIX_FRAMEBUFFER_DIRECT     0x2uL // speed frame upload fix - direct framebuffer updates
#define CFG_FIX_IGNORE_BLACK           0x4uL // black screens & Lunar fix - ignore black brightness
#define CFG_FIX_SWAP_FRONTBACK         0x8uL // Speed Freaks & Killer Loop fix - swap front/back detection
#define CFG_FIX_NO_COORD_CHECK        0x10uL // coord compatibility mode
#define CFG_FIX_REMOVE_BLUE           0x20uL // Legacy of Dragoon fix - remove blue glitches (needs alpha multipass)
#define CFG_FIX_REACTIVE_FPSLIMIT     0x40uL // frame displayed before frame time waiting - feels more reactive but less steady
#define CFG_FIX_AUTO_FPSLIMIT         0x80uL // special FPS limit (theoretical values, no wait if skipped)
#define CFG_FIX_HALF_SKIPPING        0x100uL // skip one frame out of two
#define CFG_FIX_POLYGON_CACHE_FF9    0x200uL // FF9 battle rect fix - G4 polygon cache
#define CFG_FIX_IGNORE_SMALL_MOVE    0x400uL //+ FF7 & 8 smooth fix - ignore small framebuffer moves
#define CFG_FIX_LAZY_UPLOAD          0x800uL // Dynasty Warriors 7 fix - lazy upload detection
#define CFG_FIX_STATUS_INTERLACE    0x1000uL // Chronocross fix - switch during status read instead of update lace
#define CFG_FIX_EXPAND_SCREEN       0x2000uL // Capcom fix - fix screen width to show full area
#define CFG_FIX_FILTERING_BLACK     0x4000uL // old filtering fix - removes black areas
#define CFG_FIX_SPECIAL_UPLOAD      0x8000uL // splash screens fix - special upload detection
#define CFG_FIX_FAKE_LOWCOMP_READ  0x10000uL //+ LOD & RPGmaker fix - fake low compatibility frame read
#define CFG_FIX_FAKE_GPU_BUSY      0x20000uL // fake busy emulation hack
#define CFG_FIX_19                 0x40000uL
#define CFG_FIX_20                 0x80000uL


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

    // miscellaneous
    unsigned long misc_fixBits; // custom fixes
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

    /// <summary>Enable fix bits/summary>
    /// <param name="fixBits">Fix bits mask</param>
    inline void setFix(unsigned long fixBits)
    {
        misc_fixBits |= fixBits;
    }
    /// <summary>Disable fix bits</summary>
    /// <param name="fixBits">Fix bits mask</param>
    inline void unsetFix(unsigned long fixBits)
    {
        misc_fixBits &= ~fixBits;
    }
    /// <summary>Check if fix bit is active</summary>
    /// <param name="fixBits">Bit(s) mask (will return true if all are active)</param>
    inline bool getFix(long fixBits)
    {
        return ((misc_fixBits & fixBits) == fixBits);
    }
    /// <summary>Check if fix bit is disabled</summary>
    /// <param name="fixBits">Bit(s) mask (will return true if all are disabled)</param>
    inline bool getNotFix(long fixBits)
    {
        return ((misc_fixBits & fixBits) == 0);
    }
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
#define CFGSTRETCHING_LAST CfgStretching_SemiAspectCut
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
