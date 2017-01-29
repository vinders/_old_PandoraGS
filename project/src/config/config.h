/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration container
*******************************************************************************/
#pragma once

#include <string>
#include <vector>
#include "config_common.h"
#include "config_profile.h"

// config constants
#define RESOLUTION_AUTODETECT 0u
#define FRAMELIMIT_AUTODETECT 0.0f

// general fixes
#define GEN_FIX_MIXED_SOFT_FRAMEBUFFER 0x1 // speed framebuffer access fix for ATI
#define GEN_FIX_FAKE_SUB_BLENDING      0x2 // for buggy drivers - fake substractive blending
#define GEN_FIX_RELOAD_CFG_AFTER_OPEN  0x4 // don't keep same profile after disc change


/// @namespace config
/// Configuration management
namespace config
{
    /// @class Config
    /// @brief Configuration container
    class Config
    {
    private:
        static std::vector<ConfigProfile*> s_profiles; ///< Config profiles (vector)
        static uint32_t s_currentProfileId;            ///< Active profile ID
        static bool s_isReady;                         ///< Fast mutual exclusion

    public:
        static lang::langcode_t langCode; ///< Used language identifier
        static std::wstring langFilePath; ///< External language file path

        static config_display_t display; ///< Display settings
        static config_timer_t timer;     ///< Timer sync settings
        static config_events_t events;   ///< Events handling settings

        static uint32_t configFixBits;   ///< Configured fixes
        static uint32_t runtimeFixBits;  ///< Fixes set by emulator


    public:
        /// @brief Create config container (default values, no profile)
        static void init();

        /// @brief Close config container and profiles
        static void close();

        /// @brief Set default config values
        /// @param isKeyBindingReset Also reset event-trigger key bindings
        static void reset(bool isKeyBindingReset);

        /// @brief Reset event-trigger key bindings
        static void resetKeyBindings();


        // -- profile management -- --------------------------------------------

        //...
    };
}
