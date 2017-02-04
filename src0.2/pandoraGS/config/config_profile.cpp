/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_profile.cpp
Description : configuration profile container
*******************************************************************************/
#include <string.h>
using namespace std;
#include "config_profile.h"


/// <summary>Create profile container (with preset values)</summary>
/// <param name="id">Profile unique identifier</param>
/// <param name="name">Profile name</param>
ConfigProfile::ConfigProfile(uint32_t id, std::wstring name)
{
    gen_profileId = id;
    gen_profileName = name;
    misc_fixBits = 0u;
}

/// <summary>Copy profile container</summary>
/// <param name="copy">Profile container to copy</param>
ConfigProfile::ConfigProfile(ConfigProfile& copy)
{
    gen_profileId = copy.gen_profileId;
    gen_profileName = copy.gen_profileName;

    scl_texSmooth = copy.scl_texSmooth;
    scl_texUpscaleVal = copy.scl_texUpscaleVal;
    scl_texUpscaleType = copy.scl_texUpscaleType;
    scl_sprSmooth = copy.scl_sprSmooth;
    scl_sprUpscaleVal = copy.scl_sprUpscaleVal;
    scl_sprUpscaleType = copy.scl_sprUpscaleType;
    scl_screenSmooth = copy.scl_screenSmooth;
    scl_screenUpscaleVal = copy.scl_screenUpscaleVal;
    scl_screenUpscaleType = copy.scl_screenUpscaleType;
    scl_mdecFilter = copy.scl_mdecFilter;

    shd_antiAliasing = copy.shd_antiAliasing;
    //...

    dsp_internalResX = copy.dsp_internalResX;
    dsp_internalResY = copy.dsp_internalResY;
    dsp_ratioType = copy.dsp_ratioType;
    dsp_stretchRatio = copy.dsp_stretchRatio;
    dsp_cropStrength = copy.dsp_cropStrength;
    dsp_isScreenMirror = copy.dsp_isScreenMirror;
    dsp_borderSizeX = copy.dsp_borderSizeX;
    dsp_borderSizeY = copy.dsp_borderSizeY;
    dsp_screenCurved = copy.dsp_screenCurved;
    //...

    misc_fixBits = copy.misc_fixBits;
    misc_offscreenDrawing = copy.misc_offscreenDrawing;
}


/// <summary>Set profile preset values</summary>
/// <param name="preset">Default values to use</param>
void ConfigProfile::setPresetValues(const CFG_preset_t preset)
{
    // preset values
    switch (preset)
    {
        case CFG_preset_standard:
        {
            scl_texSmooth = CFG_interp_bilinear_ex;
            scl_texUpscaleVal = 1u;
            scl_texUpscaleType = CFG_upsc_native;
            scl_sprSmooth = CFG_interp_nearest;
            scl_sprUpscaleVal = 2u;
            scl_sprUpscaleType = CFG_upsc_sai;
            scl_screenSmooth = CFG_smooth_none;
            scl_screenUpscaleVal = 1u;
            scl_screenUpscaleType = CFG_upsc_native;
            scl_mdecFilter = CFG_mdec_standard;

            shd_antiAliasing = CFG_aa_fxaa;
            //...

            dsp_internalResX = 2u;
            dsp_internalResY = 4u;
            dsp_ratioType = CFG_ratio_aspect;
            dsp_stretchRatio = CFG_RATIO_STRETCH_FullWindow;
            dsp_cropStrength = CFG_RATIO_CROP_FullWindow;
            //...
            break;
        }

        case CFG_preset_fastest:
        {
            scl_texSmooth = CFG_interp_nearest;
            scl_texUpscaleVal = 1u;
            scl_texUpscaleType = CFG_upsc_native;
            scl_sprSmooth = CFG_interp_nearest;
            scl_sprUpscaleVal = 1u;
            scl_sprUpscaleType = CFG_upsc_native;
            scl_screenSmooth = CFG_smooth_none;
            scl_screenUpscaleVal = 1u;
            scl_screenUpscaleType = CFG_upsc_native;
            scl_mdecFilter = CFG_mdec_none;

            shd_antiAliasing = CFG_aa_none;
            //...

            dsp_internalResX = 1u;
            dsp_internalResY = 1u;
            dsp_ratioType = CFG_ratio_aspect;
            dsp_stretchRatio = CFG_RATIO_STRETCH_Orig;
            dsp_cropStrength = CFG_RATIO_CROP_Orig;
            //...
            break;
        }

        case CFG_preset_enhanced2D:
        {
            scl_texSmooth = CFG_interp_nearest;
            scl_texUpscaleVal = 1u;
            scl_texUpscaleType = CFG_upsc_native;
            scl_sprSmooth = CFG_interp_nearest;
            scl_sprUpscaleVal = 1u;
            scl_sprUpscaleType = CFG_upsc_native;
            scl_screenSmooth = CFG_smooth_slight;
            scl_screenUpscaleVal = 4u;
            scl_screenUpscaleType = CFG_upsc_superxbr_fb;
            scl_mdecFilter = CFG_mdec_standard;

            shd_antiAliasing = CFG_aa_nfaa;
            //...

            dsp_internalResX = 1u;
            dsp_internalResY = 1u;
            dsp_ratioType = CFG_ratio_aspect;
            dsp_stretchRatio = CFG_RATIO_STRETCH_Orig;
            dsp_cropStrength = CFG_RATIO_CROP_Orig;
            //...
            break;
        }

        case CFG_preset_enhanced3D:
        {
            scl_texSmooth = CFG_interp_bilinear_ex;
            scl_texUpscaleVal = 2u;
            scl_texUpscaleType = CFG_upsc_sai;
            scl_sprSmooth = CFG_interp_bilinear_ex;
            scl_sprUpscaleVal = 3u;
            scl_sprUpscaleType = CFG_upsc_xbrz_ex;
            scl_screenSmooth = CFG_smooth_none;
            scl_screenUpscaleVal = 1u;
            scl_screenUpscaleType = CFG_upsc_native;
            scl_mdecFilter = CFG_mdec_nnedi3;

            shd_antiAliasing = CFG_aa_smaa4;
            //...

            dsp_internalResX = 4u;
            dsp_internalResY = 8u;
            dsp_ratioType = CFG_ratio_aspect;
            dsp_stretchRatio = CFG_RATIO_STRETCH_CloseToOrig;
            dsp_cropStrength = CFG_RATIO_CROP_CloseToOrig;
            //...
            break;
        }
    }

    // common values
    dsp_isScreenMirror = false;
    dsp_borderSizeX = 0u;
    dsp_borderSizeY = 0u;
    dsp_screenCurved = CFG_curved_none;
    //...
    misc_fixBits = 0u;
    misc_offscreenDrawing = CFG_offscreen_standard;
}

