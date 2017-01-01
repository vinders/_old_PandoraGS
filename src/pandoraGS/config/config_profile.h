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

#include <cstdint>
#include <cstddef>
#include <string>

// data types
enum CFG_preset_t : uint32_t // presets
{
    CFG_preset_fastest = 0u,
    CFG_preset_standard = 1u,
    CFG_preset_enhanced2D = 2u,
    CFG_preset_enhanced3D = 3u
};
#define CFG_preset_LENGTH 4
enum CFG_offscreen_t : uint32_t // off-screen drawing modes
{
    CFG_offscreen_none = 0u,
    CFG_offscreen_standard = 1u,
    CFG_offscreen_full = 2u
};
#define CFG_offscreen_LENGTH 3
enum CFG_interp_t : uint32_t // interpolation modes
{
    CFG_interp_nearest = 0u,
    CFG_interp_bilinear = 1u,
    CFG_interp_bilinear_ex = 2u,
    CFG_interp_bicubic = 3u,
    CFG_interp_gauss = 4u
};
#define CFG_interp_LENGTH 5
enum CFG_upsc_t : uint32_t // upscaling modes
{
    CFG_upsc_native = 0u,  // 1x
    CFG_upsc_sai = 1u,         // 2x
    CFG_upsc_xbr = 2u,  //LV2  // 2x, 3x, 4x
    CFG_upsc_xbrz = 3u,        // 2x, 3x, 4x, 5x
    CFG_upsc_xbrz_ex = 4u,     //     3x, 4x, 5x // deposterize pre-filter -> utiliser option indépendante du filtre ?
    CFG_upsc_superxbr = 5u,    // 2x,     4x,     8x
    CFG_upsc_superxbr_fb = 6u, // 2x,     4x,     8x  // FB = fast-bilateral
    CFG_upsc_nnedi3 = 7u       // 2x,     4x,     8x
};
#define CFG_upsc_LENGTH 8
enum CFG_mdec_t : uint32_t // MDEC filters
{
    CFG_mdec_none = 0u,
    CFG_mdec_standard = 1u,
    CFG_mdec_superxbr = 2u,
    CFG_mdec_nnedi3 = 3u
};
#define CFG_mdec_LENGTH 4
enum CFG_smooth_t : uint32_t // screen smoothing
{
    CFG_smooth_none = 0u,
    CFG_smooth_slight = 1u,
    CFG_smooth_blur = 2u,
    CFG_smooth_noise = 3u
};
#define CFG_smooth_LENGTH 4
enum CFG_aa_t : uint32_t // anti-aliasing
{
    CFG_aa_none = 0u,
    CFG_aa_fxaa = 1u, // fast
    CFG_aa_nfaa = 2u, // good with 2D
    CFG_aa_smaa4 = 3u,
    CFG_aa_smaa8 = 4u,
    CFG_aa_msaa4 = 5u,
    CFG_aa_msaa8 = 6u
};
#define CFG_aa_LENGTH 7
enum CFG_curved_t : uint32_t // curved screen
{
    CFG_curved_none = 0u,
    CFG_curved_fisheye = 1u,
    CFG_curved_shape = 2u
};
#define CFG_curved_LENGTH 3
enum CFG_ratio_t : uint32_t // screen ratio modes
{
    CFG_ratio_aspect = 0u, // 15:10 - NTSC US/J + some PAL ports / 4:3 - PAL standard
    CFG_ratio_pixel = 1u
};
#define CFG_ratio_LENGTH 2

// screen stretching presets
#define CFG_RATIO_MAX  8
#define CFG_RATIO_HALF 4
#define CFG_RATIO_STRETCH_FullWindow  CFG_RATIO_MAX  // full window stretch - best if emulator uses widescreen hack
#define CFG_RATIO_CROP_FullWindow     0
#define CFG_RATIO_STRETCH_Orig        0              // keep ratio - best with 2D
#define CFG_RATIO_CROP_Orig           0
#define CFG_RATIO_STRETCH_OrigFill    0              // keep ratio & crop - strong top/bottom cut
#define CFG_RATIO_CROP_OrigFill       CFG_RATIO_MAX
#define CFG_RATIO_STRETCH_CloseToOrig CFG_RATIO_HALF // half stretch & slight cropping - best with 3D
#define CFG_RATIO_CROP_CloseToOrig    CFG_RATIO_HALF

// custom fixes
#define CFG_FIX_FRAMEBUFFER_FF7        0x1 // FF7 battle cursor fix - adjusted framebuffer access
#define CFG_FIX_FRAMEBUFFER_DIRECT     0x2 // speed frame upload fix - direct framebuffer updates
#define CFG_FIX_IGNORE_BLACK           0x4 // black screens & Lunar fix - ignore black brightness
#define CFG_FIX_SWAP_FRONTBACK         0x8 // Speed Freaks & Killer Loop fix - swap front/back detection
#define CFG_FIX_NO_COORD_CHECK        0x10 // coord compatibility mode
#define CFG_FIX_REMOVE_BLUE           0x20 // Legacy of Dragoon fix - remove blue glitches (needs alpha multipass)
//#define CFG_FIX_7                   0x40 
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


// Configuration profile container
class ConfigProfile
{
public:
    // general
    uint32_t gen_profileId;        // profile unique identifier
    std::wstring  gen_profileName; // profile displayed name

    // smooth/scale filters
    CFG_interp_t scl_texSmooth;        // textures smoothing
    uint32_t     scl_texUpscaleVal;    // textures upscaling - 1-5x
    CFG_upsc_t   scl_texUpscaleType;   // textures upscaling - type
    CFG_interp_t scl_sprSmooth;        // 2D/sprites smoothing
    uint32_t     scl_sprUpscaleVal;    // 2D/sprites upscaling - 1-5x
    CFG_upsc_t   scl_sprUpscaleType;   // 2D/sprites upscaling - type
    CFG_smooth_t scl_screenSmooth;     // global screen smoothing
    uint32_t     scl_screenUpscaleVal; // global screen upscaling - 1-8x
    CFG_upsc_t   scl_screenUpscaleType;// global screen upscaling - type
    CFG_mdec_t   scl_mdecFilter;       // MDEC video filter
    //smooth texture transitions ('texture splatting' avec textures de polygones voisins)
                //-> essayer de ne le faire qu'une fois par association de textures (tableau avec ID textures ?)
                //-> paramètre étendue : léger dégradé / moyen dégradé / moyen distribué / fort dégradé / fort distribué
                //-> distribution pattern (au lieu de dégradé, baser transition sur masque 
                                        //(selon une image, ou selon nuages de plus en plus petits/transparents)
                //-> étendue dépend de taille des polygones (petits polys = petites transitions) -> évite fond flou 
                                        //(! prévoir étendue max assez petite, pour éviter skyboxes complètement floues)
    
    // shading
    CFG_aa_t shd_antiAliasing;    // anti-aliasing
    //bump mapping
    //motion blur
    //HDR bloom
    //HDR luma sharpen
    //HDR divers : tonemapping pass, light attenuation, pixel vibrance, subpixel dithering
    //effets (cel-shading V1 à 4, kirsch-négatif, storybook, bruit, incrustations, ...) + force
    //effets colorimétrie (couleurs daltoniens, natural vision, CRT, verdatre, bleuatre, désaturé cuivré, désaturé gris) + force

    // screen adjustment
    uint32_t     dsp_internalResX;    // internal resolution [x]
    uint32_t     dsp_internalResY;    // internal resolution [y]
    CFG_ratio_t  dsp_ratioType;       // screen ratio type (aspect ratio, pixel ratio)
    uint32_t     dsp_stretchRatio;    // screen "keep ratio" (0) / "full stretch" (8) - ratio (0 - 8)
    uint32_t     dsp_cropStrength;    // screen "black sides" (0) / "cropping" (8) - strength (0 - 8)
    bool         dsp_isScreenMirror;  // screen mirroring (mirrored/normal)
    uint32_t     dsp_borderSizeX;     // add black borders to sides (0 = none)
    uint32_t     dsp_borderSizeY;     // add black borders to top/bottom (0 = none)
    CFG_curved_t dsp_screenCurved;    // emulate curved CRT screen (0 = none)
        // whole screen curvature / fisheye (zoomed center)
    
    // rendering corrections
        //corrections -> gamma/contraste (+ presets PAL, NTSC, neutre, presets selon jeux) + S-curve contrast
        //zbuffer/order table ???
   
    //scanlines types :
    //      - simples lignes sombres (garder pixels originaux)
    //      - simples lignes sombres avec moyenne des valeurs des pixels
    //      - en lignes  R V B     (R et B contiennent un peu du V)
    //                    B V R   (vert + ajout beaucoup R et B)
    //      - en triangles   R    V B   (tous avec un peu plus des deux autres)
    //                      V B    R
    //scanlines: INT: couleur unie (noir) / valeur pixel -> slider (0-8)
    //scanlines: INT: sombre (noir) / coloré-clair (0-16 -> deviendra 0-255 pour couleur unie)
    
    //cursor: enabled, type, color, saturation, opacity
    
    // miscellaneous
    uint32_t misc_fixBits;                 // custom fixes
    CFG_offscreen_t misc_offscreenDrawing; // off-screen drawing mode
    //alpha/maskbit/psx texture window/...
    //framebuffer/...


public:
    /// <summary>Create profile container (with preset values)</summary>
    /// <param name="id">Profile unique identifier</param>
    /// <param name="name">Profile name</param>
    ConfigProfile(uint32_t id, std::wstring name);
    /// <summary>Copy profile container</summary>
    /// <param name="copy">Profile container to copy</param>
    ConfigProfile(ConfigProfile& copy);

    /// <summary>Set profile preset values</summary>
    /// <param name="preset">Default values to use</param>
    void setPresetValues(const CFG_preset_t preset);

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

#endif
