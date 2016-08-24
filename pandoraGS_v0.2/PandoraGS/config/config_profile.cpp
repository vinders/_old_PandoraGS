/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_profile.cpp
Description : configuration profile container
*******************************************************************************/
using namespace std;
#include <string.h>
#include "config_profile.h"


/// <summary>Create profile container (with preset values)</summary>
/// <param name="id">Profile unique identifier</param>
/// <param name="name">Profile name</param>
ConfigProfile::ConfigProfile(uint32_t id, std::string name)
{
    gen_profileId = id;
    gen_profileName = name;
    misc_fixBits = 0u;
    misc_useExternalShader = false;
    misc_externalShaderPath = NULL;
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
    scl_isShaderUpscale = copy.scl_isShaderUpscale;
    scl_mdecFilter = copy.scl_mdecFilter;

    shd_antiAliasing = copy.shd_antiAliasing;
    //...

    dsp_internalResX = copy.dsp_internalResX;
    dsp_internalResY = copy.dsp_internalResY;
    dsp_ratioType = copy.dsp_ratioType;
    dsp_stretchRatio = copy.dsp_stretchRatio;
    dsp_cropStrength = copy.dsp_cropStrength;
    dsp_isScreenMirror = copy.dsp_isScreenMirror;
    dsp_borderSize = copy.dsp_borderSize;
    dsp_screenCurved = copy.dsp_screenCurved;
    //...

    misc_fixBits = copy.misc_fixBits;
    misc_offscreenDrawing = copy.misc_offscreenDrawing;
    setExternalShader(copy.misc_useExternalShader, copy.misc_externalShaderPath);
}


/// <summary>Set profile preset values</summary>
/// <param name="preset">Default values to use</param>
void ConfigProfile::setPresetValues(const ProfilePreset preset)
{
    // preset values
    switch (preset)
    {
        case ProfilePreset_Standard:
        {
            scl_texSmooth = CFG_Intp_Bilinear_Enhanced;
            scl_texUpscaleVal = 1u;
            scl_texUpscaleType = CFG_UpSc_Native;
            scl_sprSmooth = CFG_Intp_Nearest;
            scl_sprUpscaleVal = 2u;
            scl_sprUpscaleType = CFG_UpSc_SaI;
            scl_screenSmooth = CFG_ScrSm_None;
            scl_screenUpscaleVal = 1u;
            scl_screenUpscaleType = CFG_UpSc_Native;
            scl_mdecFilter = CFG_Mdec_Standard;

            shd_antiAliasing = CFG_AA_FXAA;
            //...

            dsp_internalResX = 2u;
            dsp_internalResY = 4u;
            dsp_ratioType = CFG_Ratio_Aspect;
            dsp_stretchRatio = CFG_RATIO_STRETCH_FullWindow;
            dsp_cropStrength = CFG_RATIO_CROP_FullWindow;
            //...
            break;
        }

        case ProfilePreset_Fastest:
        {
            scl_texSmooth = CFG_Intp_Nearest;
            scl_texUpscaleVal = 1u;
            scl_texUpscaleType = CFG_UpSc_Native;
            scl_sprSmooth = CFG_Intp_Nearest;
            scl_sprUpscaleVal = 1u;
            scl_sprUpscaleType = CFG_UpSc_Native;
            scl_screenSmooth = CFG_ScrSm_None;
            scl_screenUpscaleVal = 1u;
            scl_screenUpscaleType = CFG_UpSc_Native;
            scl_mdecFilter = CFG_Mdec_None;

            shd_antiAliasing = CFG_AA_None;
            //...

            dsp_internalResX = 1u;
            dsp_internalResY = 1u;
            dsp_ratioType = CFG_Ratio_Aspect;
            dsp_stretchRatio = CFG_RATIO_STRETCH_Orig;
            dsp_cropStrength = CFG_RATIO_CROP_Orig;
            //...
            break;
        }

        case ProfilePreset_Enhanced2D:
        {
            scl_texSmooth = CFG_Intp_Nearest;
            scl_texUpscaleVal = 1u;
            scl_texUpscaleType = CFG_UpSc_Native;
            scl_sprSmooth = CFG_Intp_Nearest;
            scl_sprUpscaleVal = 1u;
            scl_sprUpscaleType = CFG_UpSc_Native;
            scl_screenSmooth = CFG_ScrSm_SlightBlur;
            scl_screenUpscaleVal = 4u;
            scl_screenUpscaleType = CFG_UpSc_SuperxBR_FB;
            scl_mdecFilter = CFG_Mdec_Standard;

            shd_antiAliasing = CFG_AA_NFAA;
            //...

            dsp_internalResX = 1u;
            dsp_internalResY = 1u;
            dsp_ratioType = CFG_Ratio_Aspect;
            dsp_stretchRatio = CFG_RATIO_STRETCH_Orig;
            dsp_cropStrength = CFG_RATIO_CROP_Orig;
            //...
            break;
        }

        case ProfilePreset_Enhanced3D:
        {
            scl_texSmooth = CFG_Intp_Bilinear_Enhanced;
            scl_texUpscaleVal = 2u;
            scl_texUpscaleType = CFG_UpSc_SuperEagle;
            scl_sprSmooth = CFG_Intp_Bilinear_Enhanced;
            scl_sprUpscaleVal = 3u;
            scl_sprUpscaleType = CFG_UpSc_xBRZ_Depolarized;
            scl_screenSmooth = CFG_ScrSm_None;
            scl_screenUpscaleVal = 1u;
            scl_screenUpscaleType = CFG_UpSc_Native;
            scl_mdecFilter = CFG_Mdec_NNEDI3;

            shd_antiAliasing = CFG_AA_SMAA4;
            //...

            scl_isShaderUpscale = true;
            dsp_internalResX = 4u;
            dsp_internalResY = 8u;
            dsp_ratioType = CFG_Ratio_Aspect;
            dsp_stretchRatio = CFG_RATIO_STRETCH_CloseToOrig;
            dsp_cropStrength = CFG_RATIO_CROP_CloseToOrig;
            //...
            break;
        }
    }

    // common values
    dsp_isScreenMirror = false;
    dsp_borderSize = 0u;
    dsp_screenCurved = 0u;
    //...
    misc_fixBits = 0u;
    setFix(CFG_FIX_REACTIVE_FPSLIMIT);
    misc_offscreenDrawing = CFG_OffScr_Standard;
    setExternalShader(false, NULL);
}

/// <summary>Set external shader</summary>
/// <param name="isEnabled">Use this shader or not</param>
/// <param name="path">External shader file path</param>
void ConfigProfile::setExternalShader(bool isEnabled, const char* path)
{
    misc_useExternalShader = isEnabled;
    if (misc_externalShaderPath != NULL)
    {
        delete [] misc_externalShaderPath;
        misc_externalShaderPath = NULL;
    }
    if (path != NULL)
    {
        uint32_t len = strlen(path);
        misc_externalShaderPath = new char[1 + len];
        strcpy_s(misc_externalShaderPath, 1 + len, path);
        misc_externalShaderPath[len] = '\0';
    }
}
