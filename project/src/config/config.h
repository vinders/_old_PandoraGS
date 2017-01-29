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

        /// @brief Check if config / profile is initialized
        /// @return Status (true = ready)
        static inline bool isConfigReady()
        {
            return s_isReady;
        }


        // -- profile management -- --------------------------------------------

        /// @brief Get specific profile
        /// @param index Profile index (0 based)
        /// @return Profile at the specified index (if available)
        static ConfigProfile* getProfile(uint32_t index);
        /// @brief Get current profile
        /// @return Current profile
        static inline ConfigProfile* getCurrentProfile()
        {
            return s_profiles[s_currentProfileId];
        }

        /// @brief Count available profiles
        /// @return Profile array length
        static uint32_t countProfiles()
        {
            return s_profiles.size();
        }


        // -- profile selection -- ---------------------------------------------

        /// @brief Set default profile as current
        static void useDefaultProfile();

        /// @brief Set specific profile as current (if available)
        /// @param index Profile index (0 based)
        /// @throw Memory allocation failure
        static void useProfile(uint32_t index);

        /// @brief Get current profile ID
        /// @return Current profile ID
        static inline uint32_t getCurrentProfileId()
        {
            return s_currentProfileId;
        }
        /// @brief Get ID of profile before specified ID
        /// @param currentId Specified ID
        /// @return Previous profile ID
        static uint32_t getPrevProfileId(uint32_t currentId);
        /// @brief Get ID of profile after specified ID
        /// @param currentId Specified ID
        /// @return Next profile ID
        static uint32_t getNextProfileId(uint32_t currentId);


        // -- fix bits management -- -------------------------------------------

        /// @brief Enable fix bits/summary>
        /// @param bits Fix bits mask
        static inline void setGlobalFix(uint32_t bits)
        {
            configFixBits |= bits;
        }
        /// @brief Disable fix bits
        /// @param bits Fix bits mask
        static inline void unsetGlobalFix(uint32_t bits)
        {
            configFixBits &= ~bits;
        }

        /// @brief Check if fix bits are active
        /// @param bits Bit(s) mask
        /// @return True if all the requested bits are active
        static inline bool getGlobalFix(uint32_t bits)
        {
            return ((configFixBits & bits) == bits);
        }
        /// @brief Check if at least one fix bit is active
        /// @param bits Bit(s) mask
        /// @return True if any of the requested bits is active
        static inline bool getAnyGlobalFix(uint32_t bits)
        {
            return ((configFixBits & bits) != 0);
        }
        /// @brief Check if fix bit is disabled
        /// @param bits Bit(s) mask (will return true if all are disabled)
        /// @return True if none of the requested bits are active
        static inline bool getNotGlobalFix(uint32_t bits)
        {
            return ((configFixBits & bits) == 0);
        }
    };
}
