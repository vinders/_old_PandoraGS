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
#include <thread>
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
        static std::vector<ConfigProfile*> s_profiles;   ///< Config profiles (vector)
        static std::vector<std::wstring> s_profileNames; ///< Names of the profiles (for in-game menu -> no need to load every profile)
        static uint32_t s_currentProfileId;              ///< Active profile ID
        static bool s_isInitialized;                     ///< Initialization status
        static bool s_isReady;                           ///< Fast mutual exclusion

    public:
        static lang::langcode_t langCode; ///< Used language identifier
        static std::wstring langFilePath; ///< External language file path

        static config_display_t display; ///< Display settings
        static config_timer_t timer;     ///< Timer sync settings
        static config_events_t events;   ///< Event handling settings

        static uint32_t configFixBits;   ///< Configured fixes
        static uint32_t runtimeFixBits;  ///< Fixes set by emulator


    public:
        /// @brief Create config container (default values + default profile)
        static void init();

        /// @brief Close config container and profiles
        static void close();

        /// @brief Set default config values
        /// @param[in] isKeyBindingReset  Also reset event-trigger key bindings
        static void reset(const bool isKeyBindingReset) noexcept;
        /// @brief Reset event-trigger key bindings
        static void resetKeyBindings() noexcept;

        /// @brief Check if config / profile is initialized
        /// @returns Status (true = ready)
        static inline bool isConfigReady() noexcept
        {
            return s_isReady;
        }
        
    private:
        /// @brief Set status to "not ready"
        static inline void lock() noexcept
        {
            s_isReady = false;
        }
        /// @brief Wait while status is "not ready"
        static inline void waitLock() noexcept
        {
            while (s_isReady == false)
                std::this_thread::yield();
        }
        /// @brief Set status to "ready"
        static inline void unlock() noexcept
        {
            s_isReady = true;
        }

    public:
        // -- profile management -- --------------------------------------------

        /// @brief Get specific profile
        /// @param[in] index  Profile index (0 based)
        /// @returns Profile at the specified index (if available)
        static ConfigProfile* getProfile(const uint32_t index);
        /// @brief Get current profile
        /// @returns Current profile
        static inline ConfigProfile* getCurrentProfile() noexcept
        {
            if (s_isInitialized == false)
                return nullptr;
            return s_profiles[s_currentProfileId];
        }

        /// @brief Count available profiles
        /// @returns Profile array length
        static inline uint32_t countProfiles() noexcept
        {
            return s_profiles.size();
        }


        // -- profile selection -- ---------------------------------------------

        /// @brief Set default profile as current
        static void useDefaultProfile();

        /// @brief Set specific profile as current (if available)
        /// @param[in] index  Profile index (0 based)
        static void useProfile(const uint32_t index);

        /// @brief Get current profile ID
        /// @returns Current profile ID
        static inline uint32_t getCurrentProfileId() noexcept
        {
            return s_currentProfileId;
        }
        /// @brief Get ID of profile before specified ID
        /// @param[in] currentId  Specified ID
        /// @returns Previous profile ID
        static uint32_t getPrevProfileId(const uint32_t currentId) noexcept;
        /// @brief Get ID of profile after specified ID
        /// @param[in] currentId  Specified ID
        /// @returns Next profile ID
        static uint32_t getNextProfileId(const uint32_t currentId) noexcept;

        /// @brief Get specific profile name
        /// @param[in] index  Profile index (0 based)
        /// @returns Name of profile at the specified index (if available)
        static std::wstring* getProfileName(const uint32_t index) noexcept;
        /// @brief Get list of profile names (only meant for dialogs)
        /// @returns List of profile names
        static std::vector<std::wstring>& getAllProfileNames() noexcept
        {
            return s_profileNames;
        }


        // -- fix bits management -- -------------------------------------------

        /// @brief Enable fix bits/summary>
        /// @param[in] bits  Fix bits mask
        static inline void setGlobalFix(const uint32_t bits) noexcept
        {
            configFixBits |= bits;
        }
        /// @brief Disable fix bits
        /// @param[in] bits  Fix bits mask
        static inline void unsetGlobalFix(const uint32_t bits) noexcept
        {
            configFixBits &= ~bits;
        }

        /// @brief Check if fix bits are active
        /// @param[in] bits  Bit(s) mask
        /// @returns True if all the requested bits are active
        static inline bool getGlobalFix(const uint32_t bits) noexcept
        {
            return ((configFixBits & bits) == bits);
        }
        /// @brief Check if at least one fix bit is active
        /// @param[in] bits  Bit(s) mask
        /// @returns True if any of the requested bits is active
        static inline bool getAnyGlobalFix(const uint32_t bits) noexcept
        {
            return ((configFixBits & bits) != 0);
        }
        /// @brief Check if fix bit is disabled
        /// @param[in] bits  Bit(s) mask (will return true if all are disabled)
        /// @returns True if none of the requested bits are active
        static inline bool getNotGlobalFix(const uint32_t bits) noexcept
        {
            return ((configFixBits & bits) == 0);
        }
    };
}
