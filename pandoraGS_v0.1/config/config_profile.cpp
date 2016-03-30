/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   config_profile.cpp
    Description : config profile container and toolbox
*******************************************************************************/
using namespace std;
#include "../globals.h"
#include "config_profile.h"

ConfigProfile::ConfigProfile(const int profile)
{
    // set default values
    pNext = NULL;
    profileId = profile;
    loadPreset(CFG_PRESET_STD);
    
    //!read and replace values
}

void ConfigProfile::loadPreset(const int preset)
{
    switch (preset)
    {
        case CFG_PRESET_FAST:   // fastest
        {
            textureSmoothing = CFG_TXSM_NONE;
            textureUpscaling = CFG_TXUS_NONE;
            spriteSmoothing  = CFG_2DSM_NONE;
            spriteUpscaling  = CFG_2DUS_NONE;
            screenSmoothing  = CFG_SCSM_NONE;
            bMdecFilter = false;
            
            antiAliasing    = -1;
            bMotionBlur     = false;
            bHdrLumaSharpen = false;
            bHdrBloom       = false;
            hdrLumaType     = CFG_LUMA_NORMAL;
            hdrBloomType    = CFG_BLOOM_SCREEN;
            
            internalResX     = CFG_INTRES_SCR;
            internalResY     = CFG_INTRES_SCR;
            screenStretching = CFG_STRETCH_MULT;
            
            bGammaCorrection = false;
            bTonemap  = false;
            bContrast = false;
            bColorVibrance    = false;
            subpixelDithering = CFG_DITHER_ORDER;
            break;
        }
        case CFG_PRESET_STD:    // standard
        {
            textureSmoothing = CFG_TXSM_BILINEAR;
            textureUpscaling = CFG_TXUS_2XSEAGLE;
            spriteSmoothing  = CFG_2DSM_BILINEAR;
            spriteUpscaling  = CFG_2DUS_2XSAI;
            screenSmoothing  = CFG_SCSM_NONE;
            bMdecFilter = false;
            
            antiAliasing    = CFG_AA_FXAA;
            bMotionBlur     = false;
            bHdrLumaSharpen = false;
            bHdrBloom       = false;
            hdrLumaType     = CFG_LUMA_HQ;
            hdrBloomType    = CFG_BLOOM_SCREEN;
            
            internalResX     = CFG_INTRES_2X;
            internalResY     = CFG_INTRES_4X;
            screenStretching = CFG_STRETCH_FULL;
            
            bGammaCorrection = false;
            bTonemap  = false;
            bContrast = false;
            bColorVibrance    = false;
            subpixelDithering = CFG_DITHER_ORDER;
            break;
        }
        case CFG_PRESET_EN2D:   // enhanced 2D
        {
            textureSmoothing = CFG_TXSM_NONE;
            textureUpscaling = CFG_TXUS_NONE;
            spriteSmoothing  = CFG_2DSM_NONE;
            spriteUpscaling  = CFG_2DUS_NONE;
            screenSmoothing  = CFG_SCSM_4XBRZBLUR;
            bMdecFilter = true;
            
            antiAliasing    = CFG_AA_NFAA4;
            bMotionBlur     = false;
            bHdrLumaSharpen = false;
            bHdrBloom       = false;
            hdrLumaType     = CFG_LUMA_HQ;
            hdrBloomType    = CFG_BLOOM_SCREEN;
            
            internalResX     = CFG_INTRES_4X;
            internalResY     = CFG_INTRES_4X;
            screenStretching = CFG_STRETCH_RATIO;
            
            bGammaCorrection = true;
            bTonemap  = false;
            bContrast = false;
            bColorVibrance    = false;
            subpixelDithering = CFG_DITHER_ORDER;
            break;
        }
        case CFG_PRESET_EN3D:   // enhanced 3D
        {
            textureSmoothing = CFG_TXSM_ENHANCED;
            textureUpscaling = CFG_TXUS_2XSEAGLE;
            spriteSmoothing  = CFG_2DSM_NONE;
            spriteUpscaling  = CFG_2DUS_3XBRZ;
            screenSmoothing  = CFG_SCSM_NONE;
            bMdecFilter = true;
            
            antiAliasing    = CFG_AA_SMAA4;
            bMotionBlur     = false;
            bHdrLumaSharpen = false;
            bHdrBloom       = false;
            hdrLumaType     = CFG_LUMA_HQ;
            hdrBloomType    = CFG_BLOOM_SCREEN;
            
            internalResX     = CFG_INTRES_4X;
            internalResY     = CFG_INTRES_4X;
            screenStretching = CFG_STRETCH_CLOSE;
            
            bGammaCorrection = true;
            bTonemap  = false;
            bContrast = false;
            bColorVibrance    = true;
            subpixelDithering = CFG_DITHER_ORDER;
            break;
        }
    }
    
    // common default values
    bBumpMapping = false;
    specialFilter   = -1;
    specialLevel    = 2;
    bumpAngle   = 30; // degrees
    bumpLevel   = 1;
    motionLevel = 1;
    
    hdrLumaLevel      = 0.40f;
    hdrLumaClamp      = 0.015f;
    hdrLumaOffsetBias = 4.0f;
    hdrBloomLevel     = 0.3f;
    hdrBloomSpread    = 1.0f;
    customShaderVision = -1;
    customShaderAspect = -1;
    customShaderOverlay= -1;
    bCustomShaderBlend = false;
    customShaderRed    = 128;
    customShaderGreen  = 128;
    customShaderBlue   = 128;
    customShaderAlpha  = 0.5f;
    
    bMirrorScreen    = false;
    bMirrorScrSprites= false;
    bExtraRendering  = false;
    blackBorder      = 0;
    rotateScreen     = -1;
    
    bTvScanlines   = false;
    scanlinesLevel = 1;
    scanlinesType  = CFG_SCANLN_HORIZ;
    scanlinesRed   = 40;
    scanlinesGreen = 40;
    scanlinesBlue  = 40;
    gunCursor      = -1;
    gunCursorColor = CFG_GUNCOLOR_GREEN;
    gunCursorAlpha = 0.7f;
    
    gammaCorrectionLumin = 1.0f;
    gammaCorrectionScale = 1.0f;
    gammaCorrectionValue = 2.2f;
    tonemapBlack = 0.038f;
    tonemapWhite = 1.0f;
    tonemapLuminance = 100.0f;
    tonemapExposure = 100.0f;
    contrastLevel = 0.33f;
    colorVibranceLevel = 0.02f;
    
    // disable special fixes
    bOddEvenBit  = false;
    bLowResTimer = false;
    bExpandWidth = false;
}
