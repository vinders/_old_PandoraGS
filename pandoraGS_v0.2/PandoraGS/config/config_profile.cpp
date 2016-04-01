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
#include "config_profile.h"


/// <summary>Create profile container (with preset values)</summary>
/// <param name="id">Profile unique identifier</param>
/// <param name="name">Profile name</param>
ConfigProfile::ConfigProfile(unsigned int id, std::string name)
{
    gen_profileId = id;
    gen_profileName = name;
}

/// <summary>Copy profile container</summary>
/// <param name="copy">Profile container to copy</param>
ConfigProfile::ConfigProfile(ConfigProfile& copy)
{
    gen_profileId = copy.gen_profileId;
    gen_profileName = copy.gen_profileName;

    scl_textureSmooth = copy.scl_textureSmooth;
    scl_textureUpscale = copy.scl_textureUpscale;
    scl_spriteSmooth = copy.scl_spriteSmooth;
    scl_spriteUpscale = copy.scl_spriteUpscale;
    scl_screenSmooth = copy.scl_screenSmooth;
    scl_isMdec = copy.scl_isMdec;
    scl_isGpuTxScaling = copy.scl_isGpuTxScaling;
    scl_isGpu2dScaling = copy.scl_isGpu2dScaling;

    shd_antiAliasing = copy.shd_antiAliasing;
    //...

    dsp_internalResX = copy.dsp_internalResX;
    dsp_internalResY = copy.dsp_internalResY;
    dsp_screenStretch = copy.dsp_screenStretch;
    dsp_isExtraRender = copy.dsp_isExtraRender;
    dsp_isScreenMirror = copy.dsp_isScreenMirror;
    dsp_borderSize = copy.dsp_borderSize;
    //...

    sync_hasFixAutoLimit = copy.sync_hasFixAutoLimit;
    sync_hasFixInterlace = copy.sync_hasFixInterlace;
    dsp_hasFixExpandScreen = copy.dsp_hasFixExpandScreen;
    //...
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
            scl_textureSmooth = CfgSmoothing_BilinearEnhanced;
            scl_textureUpscale = CfgUpscaling_Native;
            scl_spriteSmooth = CfgSmoothing_Nearest;
            scl_spriteUpscale = CfgUpscaling_2xSaI;
            scl_screenSmooth = CfgScreenSmooth_None;
            scl_isMdec = false;
            scl_isGpuTxScaling = true;
            scl_isGpu2dScaling = true;

            shd_antiAliasing = CfgAntiAliasing_FXAA;
            //...

            dsp_internalResX = 2;
            dsp_internalResY = 4;
            dsp_screenStretch = CfgStretching_AspectSlight;
            //...
            break;
        }

        case ProfilePreset_Fastest:
        {
            scl_textureSmooth = CfgSmoothing_Nearest;
            scl_textureUpscale = CfgUpscaling_Native;
            scl_spriteSmooth = CfgSmoothing_Nearest;
            scl_spriteUpscale = CfgUpscaling_Native;
            scl_screenSmooth = CfgScreenSmooth_None;
            scl_isMdec = false;
            scl_isGpuTxScaling = true;
            scl_isGpu2dScaling = true;

            shd_antiAliasing = CfgAntiAliasing_None;
            //...

            dsp_internalResX = 1;
            dsp_internalResY = 1;
            dsp_screenStretch = CfgStretching_FullWindow;
            //...
            break;
        }

        case ProfilePreset_Enhanced2D:
        {
            scl_textureSmooth = CfgSmoothing_Nearest;
            scl_textureUpscale = CfgUpscaling_Native;
            scl_spriteSmooth = CfgSmoothing_Nearest;
            scl_spriteUpscale = CfgUpscaling_Native;
            scl_screenSmooth = CfgScreenSmooth_Blur4xBRZ;
            scl_isMdec = true;
            scl_isGpuTxScaling = true;
            scl_isGpu2dScaling = false;

            shd_antiAliasing = CfgAntiAliasing_NFAA;
            //...

            dsp_internalResX = 4;
            dsp_internalResY = 4;
            dsp_screenStretch = CfgStretching_AspectRatio;
            //...
            break;
        }

        case ProfilePreset_Enhanced3D:
        {
            scl_textureSmooth = CfgSmoothing_BilinearEnhanced;
            scl_textureUpscale = CfgUpscaling_2xSuperEagle;
            scl_spriteSmooth = CfgSmoothing_Bilinear;
            scl_spriteUpscale = CfgUpscaling_3xBRZ;
            scl_screenSmooth = CfgScreenSmooth_None;
            scl_isMdec = true;
            scl_isGpuTxScaling = true;
            scl_isGpu2dScaling = true;

            shd_antiAliasing = CfgAntiAliasing_SMAA4;
            //...

            dsp_internalResX = 4;
            dsp_internalResY = 8;
            dsp_screenStretch = CfgStretching_AspectSlight;
            //...
            break;
        }
    }

    // common values
    dsp_isExtraRender = false;
    dsp_isScreenMirror = false;
    dsp_borderSize = 0;
    //...
    sync_hasFixAutoLimit = false;
    sync_hasFixInterlace = false;
    dsp_hasFixExpandScreen = false;
    //...
}
