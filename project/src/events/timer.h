/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : time management tool
*******************************************************************************/
#pragma once

#include <cstdint>
#include "utils/clock.h"

/// @namespace events
/// Event management
namespace events
{
    /// @enum regionsync_t
    /// @brief Regional sync modes
    enum class regionsync_t : int32_t
    {
        undefined = -1,
        ntsc = 0,    ///< NTSC - chip timing
        pal = 1,     ///< PAL - chip timing
        ntscStd = 2, ///< NTSC - standard rate
        palStd = 3   ///< PAL - standard rate
    };

    /// @enum skipmode_t
    /// @brief Skipping modes
    enum class skipmode_t : uint32_t
    {
        disabled = 0u, ///< No skipping
        standard = 1u, ///< Standard skipping
        alternate = 2u ///< Alternate skipping (skip one period out of two)
    };

    /// @enum speed_t
    /// @brief Speed modifiers
    enum class speed_t : int32_t
    {
        pause = -2, ///< Freeze game
        slow = -1,  ///< Slow motion
        normal = 0, ///< Normal speed
        fast = 1    ///< Fast-forward
    };


    /// @class Timer
    /// @brief Time management tool
    class Timer
    {
    private:
        // timer settings
        float m_targetFreq;      ///< Configured frequency
        bool m_isInterlaced;     ///< Interlacing enabled
        speed_t m_speedMode;     ///< Current speed mode
        skipmode_t m_skipMode;   ///< Period skipping mode
        ticks_t m_timeout;       ///< Max ticks before skipping

        // timer execution
        utils::Clock m_clock;    ///< Time counter and synchronizer
        int32_t m_speedIteration;///< Speed change iteration (period count)
        bool m_isResetPending;   ///< Time reference reset query
        float m_measuredFreq;    ///< Current measured frequency
        int32_t m_periodsToSkip; ///< Number of periods to skip (to stay in sync)
        ticks_t m_lateTicks;     ///< Current lateness (compared to time reference)


    public:
        /// @brief Create default timer
        Timer() noexcept : m_targetFreq(50.0f), m_isInterlaced(false), m_speedMode(speed_t::normal), m_skipMode(skipmode_t::disabled),
                           m_timeout((ticks_t)42uL), m_measuredFreq(0.0f), m_periodsToSkip(0), m_lateTicks((ticks_t)0uL), m_isResetPending(true) {}
        /// @brief Initialize timer
        /// @param timeMode Prefered time mode
        Timer(timemode_t timeMode) noexcept : m_targetFreq(59.94f), m_isInterlaced(false), m_speedMode(speed_t::normal), m_skipMode(skipmode_t::disabled),
                                              m_timeout((ticks_t)52uL), m_measuredFreq(0.0f), m_periodsToSkip(0), m_lateTicks((ticks_t)0uL), m_isResetPending(false)
        {
            m_clock.setClockConfig(59.94f, timeMode);
        }

        // -- timer settings -- ----------------------------------------------------

        /// @brief Set time management mode
        /// @param[in] timeMode  Prefered time mode
        void setTimeMode(const timemode_t timeMode) noexcept;
        /// @brief Set timer frequency
        /// @param[in] freqLimit     Frequency max limit
        /// @param[in] regMode       Regional sync mode
        /// @param[in] isInterlaced  Interlacing
        void setFrequency(const float freqLimit, const regionsync_t regMode, const bool isInterlaced) noexcept;
        /// @brief Set period skipping mode
        /// @param[in] isEnabled   Enable skipping
        /// @param[in] isHalfSkip  Skip max one period out of two
        void setSkippingMode(const bool isEnabled, const bool isAlternatedSkip) noexcept;

        /// @brief Reset time reference
        inline void resetTimeReference() noexcept
        {
            m_speedMode = speed_t::normal;
            if (m_periodsToSkip > 0 && m_isInterlaced)
                m_periodsToSkip--;
            else // current period is skipped -> if interlaced, skip next too
                m_periodsToSkip = (m_isInterlaced) ? 1 : 0;
            m_lateTicks = 0;
            m_isResetPending = true;
        }


        // -- time operations -- ---------------------------------------------------

        /// @brief Wait for one period (after previous time reference) + optional skipping
        /// @param[in] isWaiting    Limit frequency
        /// @param[in] isOddPeriod  Odd period (if interlaced)
        void wait(const bool isWaiting, const bool isOddPeriod) noexcept;

        /// @brief Check if current period should be skipped
        /// @returns Skip indicator (boolean)
        inline bool isPeriodSkipped() const noexcept
        {
            return (m_periodsToSkip > 0);
        }

    private:
        /// @brief Set skipping for upcoming periods
        /// @param[in] isOddPeriod  Odd period (if interlaced)
        void setSkipping(const bool isOddPeriod) noexcept;

        /// @brief Process temporary speed modification
        /// @returns Continue time calculation (true) or exit function (false)
        bool processSpeedAlteration() noexcept;

    public:
        // -- time measures -- -----------------------------------------------------

        /// @brief Calculate current number of effective periods per second
        inline void checkFrequency() noexcept
        {
            m_measuredFreq = m_clock.checkFrequency();
        }
        /// @brief Get current number of effective periods per second
        /// @returns Current periods per second
        inline float getMeasuredFrequency() const noexcept
        {
            return m_measuredFreq;
        }


        // -- events -- ------------------------------------------------------------

        /// @brief Toggle slow motion mode - event handler
        /// @param[in] flag  Unused
        void toggleSlowMode(int32_t flag) noexcept
        {
            m_speedMode = (m_speedMode == speed_t::normal) ? speed_t::slow : speed_t::normal;
        }
        /// @brief Toggle fast mode - event handler
        /// @param[in] speedStatus  Desired fast-speed status boolean (on / off)
        void setFastMode(int32_t speedStatus) noexcept
        {
            m_speedIteration = 0;
            m_speedMode = (speedStatus != 0) ? speed_t::fast : speed_t::normal;
        }
        /// @brief Toggle pause mode - event handler
        /// @param[in] pauseStatus  Desired pause status boolean (on / off)
        void setPauseMode(int32_t pauseStatus) noexcept
        {
            m_speedMode = (pauseStatus != 0) ? speed_t::pause : speed_t::normal;
        }
    };
}
