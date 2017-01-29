/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration common tools
*******************************************************************************/
#pragma once

#include "../display/utils/i_window.h"
#include "../display/utils/display_window.h"
#include "../events/listener.h"
#include "../events/timer.h"
#include "../lang/i_lang.h"

/// @namespace config
/// Configuration management
namespace config
{
    // -- data types -- --------------------------------------------------------
    
    /// @enum framelimit_settings_t
    /// @brief Frame limit settings
    enum class framelimit_settings_t : uint32_t
    {
        disabled = 0u,  ///< No frame limit
        limit = 1u,     ///< Frame limit
        limitSkip = 2u, ///< Frame limit with frame skipping
    };
    
    /// @enum subprecision_settings_t
    /// @brief Geometry subprecision settings
    enum class subprecision_settings_t : uint32_t
    {
        disabled = 0u, ///< Use original integer coordinates
        standard = 1u, ///< Standard GTE subprecision (edgbla's)
        enhanced = 2u  ///< PGXP compatible GTE subprecision
    };
    
    
    // -- general settings -- --------------------------------------------------
    
    /// @struct config_display_t
    /// @brief Display configuration settings
    struct config_display_t
    {
        display::utils::window_mode_t windowMode; ///< Display mode (fullscreen / fixed window / resizable)
        display::coord_t fullscreenRes;                    ///< Fullscreen display resolution (X, Y)
        display::coord_t windowRes;                        ///< Window display resolution (X, Y)
        display::window_color_mode_t colorDepth;  ///< Color depth (16-bit / 32-bit)
        subprecision_settings_t subprecisionMode; ///< Geometry subprecision mode (integer / standard / enhanced)
    };
    
    /// @struct config_timer_t
    /// @brief Timer configuration settings
    struct config_timer_t
    {
        events::timemode_t timeMode;          ///< Timing mode (low-res / high-res)
        framelimit_settings_t frameLimitMode; ///< Frame limit mode (disabled / limit / limit and skip)
        float frameRateLimit;                 ///< Frame rate limit (0=auto / value=fixed)
        bool isFreqDisplay;                   ///< Show number of frames per second (on/off)
    };
    
    /// @struct config_events_t
    /// @brief Events configuration settings
    struct config_events_t
    {
        char pTriggerKeys[EVENT_KEYS_STRING_LENGTH]; ///< Event-triggering key bindings
        bool isNoScreenSaver;                        ///< Disable screen-saver (on/off)
        bool isDebugMode;                            ///< Debug mode (on/off)
    };
}
