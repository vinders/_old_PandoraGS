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
#define CFG_FIX_FRAMEBUFFER_FF7        0x1 // FF7 battle cursor fix - adjusted framebuffer access
#define CFG_FIX_FRAMEBUFFER_DIRECT     0x2 // speed frame upload fix - direct framebuffer updates
#define CFG_FIX_IGNORE_BLACK           0x4 // black screens & Lunar fix - ignore black brightness
#define CFG_FIX_SWAP_FRONTBACK         0x8 // Speed Freaks & Killer Loop fix - swap front/back detection
#define CFG_FIX_NO_COORD_CHECK        0x10 // coord compatibility mode
#define CFG_FIX_REMOVE_BLUE           0x20 // Legacy of Dragoon fix - remove blue glitches (needs alpha multipass)
#define CFG_FIX_REACTIVE_FPSLIMIT     0x40 // frame displayed before frame time waiting - feels more reactive but less steady
#define CFG_FIX_PC_FPSLIMIT           0x80 // MGS fix - special FPS limit (theoretical values, no wait if skipped)
#define CFG_FIX_HALF_SKIPPING        0x100 // skip one frame out of two
#define CFG_FIX_POLYGON_CACHE_FF9    0x200 // FF9 battle rect fix - G4 polygon cache
#define CFG_FIX_IGNORE_SMALL_MOVE    0x400 //+ FF7 & 8 smooth fix - ignore small framebuffer moves
#define CFG_FIX_LAZY_UPLOAD          0x800 // Dynasty Warriors 7 fix - lazy upload detection
#define CFG_FIX_STATUS_INTERLACE    0x1000 // Chronocross fix - switch during status read instead of update lace
#define CFG_FIX_EXPAND_SCREEN       0x2000 // Capcom fix - fix screen width to show full area
#define CFG_FIX_FILTERING_BLACK     0x4000 // old filtering fix - removes black areas
#define CFG_FIX_SPECIAL_UPLOAD      0x8000 // splash screens fix - special upload detection
#define CFG_FIX_FAKE_LOWCOMP_READ  0x10000 //+ LOD & RPGmaker fix - fake low compatibility frame read
#define CFG_FIX_FAKE_GPU_BUSY      0x20000 // fake busy emulation hack
//#define CFG_FIX_19               0x40000
//#define CFG_FIX_20               0x80000


// Driver configuration profile container
class ConfigProfile
{
public:
    // general
    uint32_t gen_profileId;       // profile unique identifier
    std::string  gen_profileName; // profile displayed name

    // smooth/scale filters
    uint32_t scl_textureSmooth;   // textures smoothing
    uint32_t scl_textureUpscale;  // textures hi-res upscaling
    uint32_t scl_spriteSmooth;    // 2D/sprites smoothing
    uint32_t scl_spriteUpscale;   // sprites hi-res upscaling
    uint32_t scl_screenSmooth;    // global screen smoothing
    bool     scl_isShaderUpscale;   // upscaling algorithm using shader (faster, less precision)
    bool     scl_isMdec;          // MDEC video filter
    //ajout bruit aux textures (pour les rendre plus précises) -> taux : 0 - 10
    //...
    
    // shading
    uint32_t shd_antiAliasing;    // anti-aliasing
    //bump mapping
    //motion blur
    //HDR bloom
    //HDR lumasharpen
    //renforcement (masque laplacien, unsharp masking) + param force (1 - 10)
    //effets (cel-shading V1 à 4, kirsch-négatif, verre brisé, storybook, ...)
    //...

    // screen adjustment
    uint32_t dsp_internalResX;    // internal resolution [x]
    uint32_t dsp_internalResY;    // internal resolution [y]
    uint32_t dsp_ratioType;       // screen ratio type (aspect ratio, pixel ratio)
    uint32_t dsp_stretchCutRatio; // screen "keep ratio and cut" (0) / "full stretch" (8) - ratio (0 - 8)
    uint32_t dsp_cutStrength;     // screen "black sides" (0) / "cut" (8) - strength (0 - 8)
    bool     dsp_isScreenMirror;  // screen mirroring (mirrored/normal)
    uint32_t dsp_borderSize;      // add black borders (0 = none)
    uint32_t dsp_screenCurved;    // emulate curved CRT screen (0 to 2 ; 0 = none)

    // color filters
    //couleurs -> natural vision, couleurs daltoniens
    //inscrustations -> couleur, type incrustation, transparence
    //scanlines -> plusieurs sortes (noir, ligne dédoublée en plus foncé, quadrillage, points, triangles)
    //scanlines -> choix intensité et transparence
    //corrections -> gamma (+ presets PAL, NTSC, neutre), contraste
    //...

    // miscellaneous
    uint32_t misc_fixBits; // custom fixes
    //alpha/maskbit/... 
    //framebuffer/...


public:
    /// <summary>Create profile container (with preset values)</summary>
    /// <param name="id">Profile unique identifier</param>
    /// <param name="name">Profile name</param>
    ConfigProfile(uint32_t id, std::string name);
    /// <summary>Copy profile container</summary>
    /// <param name="copy">Profile container to copy</param>
    ConfigProfile(ConfigProfile& copy);

    /// <summary>Set profile preset values</summary>
    /// <param name="preset">Default values to use</param>
    void setPresetValues(const ProfilePreset preset);

    /// <summary>Enable fix bits/summary>
    /// <param name="fixBits">Fix bits mask</param>
    inline void setFix(uint32_t fixBits)
    {
        misc_fixBits |= fixBits;
    }
    /// <summary>Disable fix bits</summary>
    /// <param name="fixBits">Fix bits mask</param>
    inline void unsetFix(uint32_t fixBits)
    {
        misc_fixBits &= ~fixBits;
    }
    /// <summary>Check if fix bit is active</summary>
    /// <param name="fixBits">Bit(s) mask (will return true if all are active)</param>
    inline bool getFix(uint32_t fixBits)
    {
        return ((misc_fixBits & fixBits) == fixBits);
    }
    /// <summary>Check if fix bit is disabled</summary>
    /// <param name="fixBits">Bit(s) mask (will return true if all are disabled)</param>
    inline bool getNotFix(uint32_t fixBits)
    {
        return ((misc_fixBits & fixBits) == 0);
    }
};


// screen ratio modes
enum CFG_ScreenRatio
{
    CFG_Ratio_Aspect = 0,       // auto - Pete's method
    CFG_Ratio_Aspect_15_10 = 1, // 15:10 - NTSC US/J + some PAL ports
    CFG_Ratio_Aspect_4_3 = 2,   // 4:3 - PAL standard
    CFG_Ratio_Pixel = 3
};
#define CFG_ScreenRatio_LENGTH 2
// screen stretching presets
#define CFG_RATIO_MAX  8
#define CFG_RATIO_HALF 4
#define CFG_RATIO_STRETCH_FullWindow  CFG_RATIO_MAX  // full window stretch - best if emulator uses widescreen hack
#define CFG_RATIO_CUT_FullWindow      0
#define CFG_RATIO_STRETCH_Orig        0              // keep ratio - best with 2D
#define CFG_RATIO_CUT_Orig            0
#define CFG_RATIO_STRETCH_OrigFill    0              // keep ratio & cut - strong top/bottom cut
#define CFG_RATIO_CUT_OrigFill        CFG_RATIO_MAX
#define CFG_RATIO_STRETCH_CloseToOrig CFG_RATIO_HALF // half stretch & slight cut - best with 3D
#define CFG_RATIO_CUT_CloseToOrig     CFG_RATIO_MAX

// interpolation modes
enum CFG_Interpolation
{
    CFG_Intp_Nearest = 0,
    CFG_Intp_Bilinear = 1,
    CFG_Intp_Bilinear_Enhanced = 2,
    CFG_Intp_Bicubic = 3,
    CFG_Intp_Bicubic_Enhanced = 4,
    CFG_Intp_GaussianBlur = 5
};
#define CFG_Interpolation_LENGTH 6
// upscaling modes
enum CFG_UpScaling
{
    CFG_UpSc_Native = 0,
    CFG_UpSc_2xSaI = 1,
    CFG_UpSc_2xSuperEagle = 2,
    CFG_UpSc_2xHQ = 3,
    CFG_UpSc_2xBRZ = 4,
    CFG_UpSc_2xBRZ_Depolarize = 5,
    CFG_UpSc_3xHQ = 6,
    CFG_UpSc_3xBRZ = 7,
    CFG_UpSc_3xBRZ_Depolarize = 8,
    CFG_UpSc_4xHQ = 9,
    CFG_UpSc_4xBRZ = 10,
    CFG_UpSc_4xBRZ_Depolarize = 11,
    CFG_UpSc_5xBRZ = 12,
    CFG_UpSc_5xBRZ_Depolarize = 13
};
#define CFG_UpScaling_LENGTH 14
// screen smoothing
enum CFG_ScreenSmoothing
{
    CFG_ScrSm_None = 0,
    CFG_ScrSm_Blur = 1,
    CFG_ScrSm_3xBRZ = 2,
    CFG_ScrSm_3xBRZ_Blur = 3,
    CFG_ScrSm_4xBRZ = 4,
    CFG_ScrSm_4xBRZ_Blur = 5,
    CFG_ScrSm_5xBRZ = 6
    //...
    //xBR-Lv2-3D
    //...
};
#define CFG_ScreenSmoothing_LENGTH 7

// anti-aliasing
enum CFG_AntiAliasing
{
    CFG_AA_None = 0,
    CFG_AA_FXAA = 1, // fast
    CFG_AA_NFAA = 2, // good with 2D
    CFG_AA_MSAA4 = 3,
    CFG_AA_MSAA8 = 4,
    CFG_AA_SMAA4 = 5,
    CFG_AA_SMAA8 = 6
};
#define CFG_AntiAliasing_LENGTH 7

#endif
