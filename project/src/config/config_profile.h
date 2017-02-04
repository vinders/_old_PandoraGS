/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration profile container
*******************************************************************************/
#pragma once

#include <cstdint>
#include <string>
#include "config_common.h"

// custom fixes
#define CFG_FIX_FRAMEBUFFER_FF7        0x1 // FF7 battle cursor fix - adjusted framebuffer access
#define CFG_FIX_FRAMEBUFFER_DIRECT     0x2 // speed frame upload fix - direct framebuffer updates
#define CFG_FIX_IGNORE_BLACK           0x4 // black screens & Lunar fix - ignore black brightness
#define CFG_FIX_SWAP_FRONTBACK         0x8 // Speed Freaks & Killer Loop fix - swap front/back detection
#define CFG_FIX_NO_COORD_CHECK        0x10 // coord compatibility mode
#define CFG_FIX_REMOVE_BLUE           0x20 // Legacy of Dragoon fix - remove blue glitches (needs alpha multipass)
#define CFG_FIX_STANDARD_RATE         0x40 // use standard frame rate instead of chip timing
//#define CFG_FIX_8                   0x80 
#define CFG_FIX_HALF_SKIPPING        0x100 // skip one frame out of two
#define CFG_FIX_POLYGON_CACHE_FF9    0x200 // FF9 battle rect fix - G4 polygon cache
#define CFG_FIX_IGNORE_SMALL_MOVE    0x400 //+ FF7 & 8 smooth fix - ignore small framebuffer moves
#define CFG_FIX_LAZY_UPLOAD          0x800 // Dynasty Warriors 7 fix - lazy upload detection
#define CFG_FIX_STATUS_INTERLACE    0x1000 // Chronocross fix - switch during status read instead of update lace
#define CFG_FIX_EXPAND_SCREEN       0x2000 // Capcom fix - fix screen width to show full area
#define CFG_FIX_FILTERING_BLACK     0x4000 // old filtering fix - removes black areas
#define CFG_FIX_SPECIAL_UPLOAD      0x8000 // splash screens fix - special upload detection
#define CFG_FIX_FAKE_LOWCOMP_READ  0x10000 //+ LOD & RPGmaker fix - fake low compatibility frame read
#define CFG_FIX_FAKE_GPU_BUSY      0x20000 // fake busy emulation hack
//#define CFG_FIX_19               0x40000
//#define CFG_FIX_20               0x80000


/// @namespace config
/// Configuration management
namespace config
{
    /// @enum config_preset_t
    /// @brief Configuration profile presets
    enum class config_preset_t : uint32_t
    {
        fastest = 0u,
        standard = 1u,
        enhanced2D = 2u,
        enhanced3D = 3u
    };
    #define CONFIG_PRESET_LENGTH 4


    /// @class ConfigProfile
    /// @brief Configuration profile container
    class ConfigProfile
    {
    private:
        uint32_t m_profileId;        ///< Profile unique identifier
        std::wstring m_profileName;  ///< Profile displayed name

    public:
        bool isExternalShaders;       ///< Use external shaders (bypass built-in effects)
        std::string shadersPath;      ///< External shaders path

        config_screen_t display;       ///< Screen / display settings
        config_scaling_t scaling;      ///< Smoothing / upscaling settings
        offscreen_drawing_t offscreen; ///< Off-screen drawing mode
        //...
        config_shading_t effects;      ///< Shader effects settings
        //...
        uint32_t fixBits;              ///< Custom fixes


    public:
        /// @brief Create profile container (with preset values)
        /// @param[in] id  Profile unique identifier
        ConfigProfile(const uint32_t id) noexcept;
        /// @brief Create profile container (with preset values)
        /// @param[in] id    Profile unique identifier
        /// @param[in] name  Profile name
        ConfigProfile(const uint32_t id, const std::wstring name) noexcept;
        /// @brief Copy profile container
        /// @param[in] copy  Profile container to copy
        ConfigProfile(const ConfigProfile& copy) noexcept;

        /// @brief Set profile preset values
        /// @param[in] preset  Default values to use
        void setPresetValues(const config_preset_t preset) noexcept;


        // -- getters -- -------------------------------------------------------

        /// @brief Get profile identifier
        /// @returns Profile ID
        inline uint32_t getProfileId() const noexcept
        {
            return m_profileId;
        }
        /// @brief Get profile name
        /// @returns Reference to profile name
        inline std::wstring& getProfileName() noexcept
        {
            return m_profileName;
        }


        // -- fix bits management -- -------------------------------------------

        /// @brief Enable fix bits/summary>
        /// @param[in] bits  Fix bits mask
        inline void setFix(const uint32_t bits) noexcept
        {
            fixBits |= bits;
        }
        /// @brief Disable fix bits
        /// @param[in] bits  Fix bits mask
        inline void unsetFix(const uint32_t bits) noexcept
        {
            fixBits &= ~bits;
        }

        /// @brief Check if fix bits are active
        /// @param[in] bits  Bit(s) mask
        /// @returns True if all requested bits are active
        inline bool getFix(const uint32_t bits) const noexcept
        {
            return ((fixBits & bits) == bits);
        }
        /// @brief Check if at least one fix bit is active
        /// @param[in] bits  Bit(s) mask
        /// @returns True if any of the requested bits is active
        inline bool getAnyFix(const uint32_t bits) const noexcept
        {
            return ((fixBits & bits) != 0);
        }
        /// @brief Check if fix bit is disabled
        /// @param[in] bits  Bit(s) mask (will return true if all are disabled)
        /// @returns True if none of the requested bits are active
        inline bool getNotFix(const uint32_t bits) const noexcept
        {
            return ((fixBits & bits) == 0);
        }
    };
}
