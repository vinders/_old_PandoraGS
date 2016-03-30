/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   config_profile.h
    Description : config profile container and toolbox
*******************************************************************************/
#ifndef _CONFIG_PROFILE_H
#define _CONFIG_PROFILE_H

#include "config_profile_values.h"


/* CONFIG PROFILE TOOLBOX --------------------------------------------------- */
class ConfigProfile
{
  private:
    //...
    
  public:
    ConfigProfile* pNext; // chained list pointer
    ConfigProfile(const int);
    void loadPreset(const int);
  
    // public config variables ---------------------------------------
    
    unsigned int profileId;
    
    // smooth/scale filters
    unsigned int textureSmoothing;
    unsigned int textureUpscaling;
    unsigned int spriteSmoothing;
    unsigned int spriteUpscaling;
    unsigned int screenSmoothing;
    bool bMdecFilter;
    
    // visual effects filters
    int  antiAliasing;
    bool bBumpMapping;
    bool bHdrLumaSharpen;
    bool bHdrBloom;
    bool bMotionBlur;
    int specialFilter;
    unsigned int specialLevel;
    // visual effects filters - advanced
    unsigned int bumpAngle;         //-90 to 90
    unsigned int bumpLevel;          //0 to 2
    unsigned int motionLevel;        //0 to 2
    unsigned int hdrLumaType;        //0 to 4
    float hdrLumaLevel;              //0.1 to 3.0
    float hdrLumaClamp;              //0.0 to 1.0
    float hdrLumaOffsetBias;         //0.0 to 6.0
    unsigned int hdrBloomType;       //0 to 2
    float hdrBloomLevel;             //0.1 to 0.9
    float hdrBloomSpread;            //0.0 to 4.0
    // visual effects filters - custom shader
    int  customShaderVision;
    int  customShaderAspect;
    int  customShaderOverlay;
    bool bCustomShaderBlend;
    unsigned int customShaderRed;    //0 to 255
    unsigned int customShaderGreen;  //0 to 255
    unsigned int customShaderBlue;   //0 to 255
    float customShaderAlpha;//0.0 to 1.0
    
    // screen adjustment
    unsigned int internalResX;
    unsigned int internalResY;
    unsigned int screenStretching;
    bool bMirrorScreen;
    bool bMirrorScrSprites;
    bool bExtraRendering;
    int  blackBorder;                //0 to 240px
    // screen adjustment - advanced
    int  rotateScreen;
    
    // misc settings
    bool bTvScanlines;
    int  gunCursor;
    //misc settings - advanced
    unsigned int scanlinesLevel; //0 to 2
    unsigned int scanlinesType;
    unsigned int scanlinesRed;
    unsigned int scanlinesGreen;
    unsigned int scanlinesBlue;
    unsigned int gunCursorColor;
    float gunCursorAlpha;

    //unsigned int frameBufferMode;
    //unsigned int offscreenDrawing;
    //bool bColorDithering;
    //bool bAlphaMultiPass;
    //bool bAdvancedBlending;
    
    // color correction
    bool  bGammaCorrection;
    float gammaCorrectionLumin;      //0.25 to 4.0
    float gammaCorrectionScale;      //0.25 to 4.0
    float gammaCorrectionValue;      //0.01 to 6.0
    bool  bTonemap;
    float tonemapBlack;              //0.01 to 1.0 - use 0.01 increments
    float tonemapWhite;              //0.01 to 1.0 - use 0.01 increments
    float tonemapLuminance;          //0.0 to 100.0
    float tonemapExposure;           //0.0 to 100.0
    bool  bContrast;                 //-1.0 to 1.0
    float contrastLevel;
    bool  bColorVibrance;
    float colorVibranceLevel;        //-1.0 to 1.0
    int   subpixelDithering;
    
    // special fixes
    bool bOddEvenBit;   //FIX_ODDEVENBIT
    bool bLowResTimer;  //FIX_NOFPSCOUNTER
    bool bExpandWidth;  //FIX_DISPWIDTHS
};

#endif
