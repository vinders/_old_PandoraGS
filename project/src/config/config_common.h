/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration common tools
*******************************************************************************/
#pragma once

#include <cstdint>
#include "../display/utils/i_window.h"
#include "../display/utils/display_window.h"
#include "../events/listener.h"
#include "../events/timer.h"
#include "../lang/i_lang.h"

/// @namespace config
/// Configuration management
namespace config
{
    // -- data types - general settings -- -------------------------------------

    /// @enum framelimit_settings_t
    /// @brief Frame limit settings
    enum class framelimit_settings_t : uint32_t
    {
        disabled = 0u,  ///< No frame limit
        limit = 1u,     ///< Frame limit
        limitSkip = 2u, ///< Frame limit with frame skipping
    };
    #define FRAMELIMIT_SETTINGS_LENGTH 3
    
    /// @enum subprecision_settings_t
    /// @brief Geometry subprecision settings
    enum class subprecision_settings_t : uint32_t
    {
        disabled = 0u, ///< Use original integer coordinates
        standard = 1u, ///< Standard GTE subprecision (edgbla's)
        enhanced = 2u  ///< PGXP compatible GTE subprecision
    };
    #define SUBPRECISION_SETTINGS_LENGTH 3

    
    // -- data types - profile settings -- -------------------------------------

    /// @enum offscreen_drawing_t
    /// @brief Offscreen drawing modes
    enum class offscreen_drawing_t : uint32_t
    {
        none = 0u,
        standard = 1u,
        full = 2u
    };
    #define OFFSCREEN_DRAWING_LENGTH 3

    /// @enum interpolation_mode_t
    /// @brief Interpolation modes
    enum class interpolation_mode_t : uint32_t
    {
        nearest = 0u,
        bilinear = 1u,
        bilinearEnhanced = 2u,
        bicubic = 3u,
        gauss = 4u
    };
    #define INTERPOLATION_MODE_LENGTH 5

    /// @enum upscaling_mode_t
    /// @brief Upscaling modes
    enum class upscaling_mode_t : uint32_t
    {
        native = 0u,      // 1x
        sai = 1u,             // 2x
        xbr = 2u,      //LV2  // 2x, 3x, 4x
        xbrz = 3u,            // 2x, 3x, 4x, 5x
        xbrzEnhanced = 4u,    //     3x, 4x, 5x // deposterize pre-filter -> utiliser option indépendante du filtre ?
        superXbr = 5u,        // 2x,     4x,     8x
        superXbrFastBlt = 6u, // 2x,     4x,     8x  // FB = fast-bilateral
        nnedi3 = 7u           // 2x,     4x,     8x
    };
    #define UPSCALING_MODE_LENGTH 8

    /// @enum mdec_filter_t
    /// @brief MDEF filter types
    enum class mdec_filter_t : uint32_t
    {
        none = 0u,
        standard = 1u,
        superxbr = 2u,
        nnedi3 = 3u
    };
    #define MDEC_FILTER_LENGTH 4

    /// @enum screen_smooth_mode_t
    /// @brief Screen smoothing modes
    enum class screen_smooth_mode_t : uint32_t
    {
        none = 0u,
        slight = 1u,
        blur = 2u,
        noise = 3u
    };
    #define SCREEN_SMOOTH_MODE_LENGTH 4

    /// @enum antialiasing_t
    /// @brief Anti-aliasing modes
    enum class antialiasing_t : uint32_t
    {
        none = 0u,
        fxaa = 1u, // fast
        nfaa = 2u, // good with 2D
        smaa4 = 3u,
        smaa8 = 4u,
        msaa4 = 5u,
        msaa8 = 6u
    };
    #define ANTIALIASING_LENGTH 7

    /// @enum screen_curvature_t
    /// @brief Screen curvature modes
    enum class screen_curvature_t : uint32_t
    {
        none = 0u,
        fisheye = 1u,
        curvedShape = 2u
    };
    #define SCREEN_CURVATURE_LENGTH 3

    /// @enum screen_ratio_mode_t
    /// @brief Screen ratio modes
    enum class screen_ratio_mode_t : uint32_t
    {
        aspect = 0u, // 15:10 - NTSC US/J + some PAL ports / 4:3 - PAL standard
        pixel = 1u
    };
    #define SCREEN_RATIO_MODE_LENGTH 2

    // screen stretching presets
    #define SCREEN_RATIO_MAX_VAL  8
    #define SCREEN_RATIO_STRETCH_FullWindow  SCREEN_RATIO_MAX_VAL // full window stretch - best if emulator uses widescreen hack
    #define SCREEN_RATIO_CROP_FullWindow     0
    #define SCREEN_RATIO_STRETCH_Keep        0                    // keep ratio - best with 2D
    #define SCREEN_RATIO_CROP_Keep           0
    #define SCREEN_RATIO_STRETCH_KeepFill    0                    // keep ratio & crop - strong top/bottom cut
    #define SCREEN_RATIO_CROP_KeepFill       SCREEN_RATIO_MAX_VAL
    #define SCREEN_RATIO_STRETCH_Semi  (SCREEN_RATIO_MAX_VAL / 2) // half stretched & slight cropping - best with 3D
    #define SCREEN_RATIO_CROP_Semi     (SCREEN_RATIO_MAX_VAL / 2)

    
    // -- general settings -- --------------------------------------------------
    
    /// @struct config_display_t
    /// @brief Display configuration settings
    struct config_display_t
    {
        display::utils::window_mode_t windowMode;       ///< Display mode (fullscreen / fixed window / resizable)
        display::coord_t              fullscreenRes;    ///< Fullscreen display resolution (X, Y)
        display::coord_t              windowRes;        ///< Window display resolution (X, Y)
        display::window_color_mode_t  colorDepth;       ///< Color depth (16-bit / 32-bit)
        subprecision_settings_t       subprecisionMode; ///< Geometry subprecision mode (integer / standard / enhanced)
    };
    
    /// @struct config_timer_t
    /// @brief Timer configuration settings
    struct config_timer_t
    {
        events::timemode_t    timeMode;       ///< Timing mode (low-res / high-res)
        framelimit_settings_t frameLimitMode; ///< Frame limit mode (disabled / limit / limit and skip)
        float                 frameRateLimit; ///< Frame rate limit (0=auto / value=fixed)
        bool                  isFreqDisplay;  ///< Show number of frames per second (on/off)
    };
    
    /// @struct config_events_t
    /// @brief Events configuration settings
    struct config_events_t
    {
        char pTriggerKeys[EVENT_KEYS_STRING_LENGTH]; ///< Event-triggering key bindings
        bool isNoScreenSaver;                        ///< Disable screen-saver (on/off)
        bool isDebugMode;                            ///< Debug mode (on/off)
    };


    // -- profile settings -- --------------------------------------------------

    /// @struct config_upscaling_t
    /// @brief Upscaling configuration settings
    struct config_upscaling_t
    {
        uint32_t         factor; ///< Upscaling factor
        upscaling_mode_t mode;   ///< Upscaling type
    };

    /// @struct config_scaling_t
    /// @brief Scaling / smoothing configuration settings
    struct config_scaling_t
    {
        interpolation_mode_t textureSmoothing; ///< Texture/3D interpolation type
        config_upscaling_t   textureScaling;   ///< Texture/3D upscaling (factor 1-5x, type)
        interpolation_mode_t spriteSmoothing;  ///< Sprite/2D interpolation type
        config_upscaling_t   spriteScaling;    ///< Sprite/2D upscaling (factor 1-5x, type)
        screen_smooth_mode_t screenSmoothing;  ///< Screen interpolation type
        config_upscaling_t   screenScaling;    ///< Screen upscaling (factor 1-8x, type)
        mdec_filter_t        mdecFilter;       ///< MDEC video filter type
    };

    //smooth texture transitions ('texture splatting' avec textures de polygones voisins)
        //-> essayer de ne le faire qu'une fois par association de textures (tableau avec ID textures ?)
        //-> paramètre étendue : léger dégradé / moyen dégradé / moyen distribué / fort dégradé / fort distribué
        //-> distribution pattern (au lieu de dégradé, baser transition sur masque 
                                //(selon une image, ou selon nuages de plus en plus petits/transparents)
        //-> étendue dépend de taille des polygones (petits polys = petites transitions) -> évite fond flou 
                                //(! prévoir étendue max assez petite, pour éviter skyboxes complètement floues)

    /// @struct config_screen_t
    /// @brief Screen adjustment settings
    struct config_screen_t
    {
        display::coord_t    internalRes;       ///< Internal resolution (X, Y)
        screen_ratio_mode_t ratioMode;         ///< Screen ratio mode (aspect ratio, pixel ratio)
        uint32_t            ratioStretch;      ///< Screen stretching - "keep ratio" (0) / "full stretch" (8)
        uint32_t            ratioCrop;         ///< Screen cropping - "with black sides" (0) / "cropped" (8)
        bool                isNtscRatioForced; ///< Force 15:10 NTSC ratio for PAL games
        bool                isMirrored;        ///< Screen mirroring (mirrored / normal)
        display::coord_t    blackBorders;      ///< Add black borders (X, Y) (0 = none)
        screen_curvature_t  curvature;         ///< Curved CRT screen simulation
    };

    /// @struct config_shading_t
    /// @brief Shader effects settings
    struct config_shading_t
    {
        antialiasing_t antiAliasing;    ///< Anti-aliasing mode
        //bump mapping
        //motion blur
        //HDR bloom
        //HDR luma sharpen
        //HDR misc : tonemapping pass, light attenuation, pixel vibrance, subpixel dithering
        //effects (cel-shading V1 - 4, kirsch-negative, storybook, incrustations, ...) + strength
        //color effects (color-blind mode, natural vision, CRT, greenish, blueish, copper-desaturated, grayscale) + strength
    };

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
}
