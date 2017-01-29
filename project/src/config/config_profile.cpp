/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration profile container
*******************************************************************************/
#include "../globals.h"
#include <cstring>
#include <string>
#include "config_profile.h"
using namespace config;


/// @brief Create profile container
/// @param id Profile unique identifier
ConfigProfile::ConfigProfile(uint32_t id)
{
    m_profileId = id;
    fixBits = 0u;
}

/// @brief Create profile container
/// @param id Profile unique identifier
/// @param name Profile name
ConfigProfile::ConfigProfile(uint32_t id, std::wstring name)
{
    m_profileId = id;
    m_profileName = name;
    fixBits = 0u;
}

/// @brief Copy profile container
/// @param copy Profile container to copy
ConfigProfile::ConfigProfile(ConfigProfile& copy)
{
    m_profileId = copy.m_profileId;
    m_profileName = copy.m_profileName;

    memcpy(&display, &(copy.display), sizeof(config_screen_t));
    offscreen = copy.offscreen;
    memcpy(&scaling, &(copy.scaling), sizeof(config_scaling_t));
    //...
    memcpy(&effects, &(copy.effects), sizeof(config_shading_t));
    //...
    fixBits = copy.fixBits;
}

/// @brief Set profile preset values
/// @param preset Default values to use
void ConfigProfile::setPresetValues(const config_preset_t preset)
{
    // preset values
    switch (preset)
    {
        case config_preset_t::standard:
        {
            scaling.textureSmoothing = interpolation_mode_t::bilinearEnhanced;
            scaling.textureScaling.factor = 1u;
            scaling.textureScaling.mode = upscaling_mode_t::native;

            scaling.spriteSmoothing = interpolation_mode_t::nearest;
            scaling.spriteScaling.factor = 2u;
            scaling.spriteScaling.mode = upscaling_mode_t::sai;

            scaling.screenSmoothing = screen_smooth_mode_t::none;
            scaling.screenScaling.factor = 1u;
            scaling.screenScaling.mode = upscaling_mode_t::native;

            scaling.mdecFilter = mdec_filter_t::standard;
            offscreen = offscreen_drawing_t::standard;

            display.internalRes.x = 2u;
            display.internalRes.y = 4u;
            display.ratioMode = screen_ratio_mode_t::aspect;
            display.ratioStretch = SCREEN_RATIO_STRETCH_FullWindow;
            display.ratioCrop = SCREEN_RATIO_CROP_FullWindow;

            effects.antiAliasing = antialiasing_t::fxaa;
            //...
            break;
        }

        case config_preset_t::fastest:
        {
            scaling.textureSmoothing = interpolation_mode_t::nearest;
            scaling.textureScaling.factor = 1u;
            scaling.textureScaling.mode = upscaling_mode_t::native;

            scaling.spriteSmoothing = interpolation_mode_t::nearest;
            scaling.spriteScaling.factor = 1u;
            scaling.spriteScaling.mode = upscaling_mode_t::native;

            scaling.screenSmoothing = screen_smooth_mode_t::none;
            scaling.screenScaling.factor = 1u;
            scaling.screenScaling.mode = upscaling_mode_t::native;

            scaling.mdecFilter = mdec_filter_t::none;
            offscreen = offscreen_drawing_t::none;
 
            display.internalRes.x = 1u;
            display.internalRes.y = 1u;
            display.ratioMode = screen_ratio_mode_t::aspect;
            display.ratioStretch = SCREEN_RATIO_STRETCH_Keep;
            display.ratioCrop = SCREEN_RATIO_CROP_Keep;

            effects.antiAliasing = antialiasing_t::none;
            //...
            break;
        }

        case config_preset_t::enhanced2D:
        {
            scaling.textureSmoothing = interpolation_mode_t::nearest;
            scaling.textureScaling.factor = 1u;
            scaling.textureScaling.mode = upscaling_mode_t::native;

            scaling.spriteSmoothing = interpolation_mode_t::nearest;
            scaling.spriteScaling.factor = 1u;
            scaling.spriteScaling.mode = upscaling_mode_t::native;

            scaling.screenSmoothing = screen_smooth_mode_t::slight;
            scaling.screenScaling.factor = 4u;
            scaling.screenScaling.mode = upscaling_mode_t::superXbrFastBlt;

            scaling.mdecFilter = mdec_filter_t::standard;
            offscreen = offscreen_drawing_t::standard;

            display.internalRes.x = 1u;
            display.internalRes.y = 1u;
            display.ratioMode = screen_ratio_mode_t::aspect;
            display.ratioStretch = SCREEN_RATIO_STRETCH_Keep;
            display.ratioCrop = SCREEN_RATIO_CROP_Keep;

            effects.antiAliasing = antialiasing_t::nfaa;
            //...
            break;
        }

        case config_preset_t::enhanced3D:
        {
            scaling.textureSmoothing = interpolation_mode_t::bilinearEnhanced;
            scaling.textureScaling.factor = 2u;
            scaling.textureScaling.mode = upscaling_mode_t::sai;

            scaling.spriteSmoothing = interpolation_mode_t::bilinearEnhanced;
            scaling.spriteScaling.factor = 3u;
            scaling.spriteScaling.mode = upscaling_mode_t::xbrzEnhanced;

            scaling.screenSmoothing = screen_smooth_mode_t::none;
            scaling.screenScaling.factor = 1u;
            scaling.screenScaling.mode = upscaling_mode_t::native;

            scaling.mdecFilter = mdec_filter_t::nnedi3;
            offscreen = offscreen_drawing_t::standard;

            display.internalRes.x = 4u;
            display.internalRes.y = 8u;
            display.ratioMode = screen_ratio_mode_t::aspect;
            display.ratioStretch = SCREEN_RATIO_STRETCH_Semi;
            display.ratioCrop = SCREEN_RATIO_CROP_Semi;

            effects.antiAliasing = antialiasing_t::smaa4;
            //...
            break;
        }
    }

    // common values
    display.isMirrored = false;
    display.isNtscRatioForced = false;
    display.blackBorders.x = 0u;
    display.blackBorders.y = 0u;
    display.curvature = screen_curvature_t::none;
    //...
    fixBits = 0u;
}
