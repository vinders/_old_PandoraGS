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
        ntsc = 0,
        pal = 1
    };

    /// @enum skipmode_t
    /// @brief Skipping modes
    enum class skipmode_t : uint32_t
    {
        disabled = 0u,
        standard = 1u,
        alternate = 2u
    };

    /// @enum speed_t
    /// @brief Speed modifiers
    enum class speed_t : int32_t
    {
        pause = -2,
        slow = -1,
        normal = 0,
        fast = 1
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
        Timer() : m_targetFreq(50.0f), m_isInterlaced(false), m_speedMode(speed_t::normal), m_skipMode(skipmode_t::disabled),
                  m_timeout((ticks_t)42uL), m_measuredFreq(0.0f), m_periodsToSkip(0), m_lateTicks((ticks_t)0uL), m_isResetPending(true) {}
        /// @brief Initialize timer
        /// @param timeMode Prefered time mode
        Timer(timemode_t timeMode) : m_targetFreq(59.94f), m_isInterlaced(false), m_speedMode(speed_t::normal), m_skipMode(skipmode_t::disabled),
                                     m_timeout((ticks_t)52uL), m_measuredFreq(0.0f), m_periodsToSkip(0), m_lateTicks((ticks_t)0uL), m_isResetPending(false)
        {
            m_clock.setClockConfig(59.94f, timeMode);
        }

        // -- timer settings -- ----------------------------------------------------

        /// @brief Set time management mode
        /// @param timeMode Prefered time mode
        void setTimeMode(timemode_t timeMode);
        /// @brief Set timer frequency
        /// @param freqLimit Frequency max limit
        /// @param regMode Regional sync mode
        /// @param isInterlaced Interlacing
        void setFrequency(float freqLimit, regionsync_t regMode, bool isInterlaced);
        /// @brief Set period skipping mode
        /// @param isEnabled Enable skipping
        /// @param isHalfSkip Skip max one period out of two
        void setSkippingMode(bool isEnabled, bool isAlternatedSkip);

        /// @brief Reset time reference
        inline void resetTimeReference()
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
        /// @param isWaiting Limit frequency
        /// @param isOddPeriod Odd period (if interlaced)
        void wait(bool isWaiting, bool isOddPeriod);

        /// @brief Check if current period should be skipped
        /// @return Skip indicator (boolean)
        inline bool isPeriodSkipped()
        {
            return (m_periodsToSkip > 0);
        }

    private:
        /// @brief Set skipping for upcoming periods
        /// @param isOddPeriod Odd period (if interlaced)
        void setSkipping(bool isOddPeriod);

        /// @brief Process temporary speed modification
        /// @return Continue time calculation (true) or exit function (false)
        bool processSpeedAlteration();

    public:
        // -- time measures -- -----------------------------------------------------

        /// @brief Calculate current number of effective periods per second
        void checkFrequency()
        {
            m_measuredFreq = m_clock.checkFrequency();
        }
        /// @brief Get current number of effective periods per second
        /// @return Current periods per second
        inline float getMeasuredFrequency()
        {
            return m_measuredFreq;
        }


        // -- events -- ------------------------------------------------------------

        /// @brief Toggle slow motion mode - event handler
        /// @param flag Unused
        void toggleSlowMode(int32_t flag)
        {
            m_speedMode = (m_speedMode == speed_t::normal) ? speed_t::slow : speed_t::normal;
        }
        /// @brief Toggle fast mode - event handler
        /// @param speedStatus Desired fast-speed status boolean (on / off)
        void setFastMode(int32_t speedStatus)
        {
            m_speedIteration = 0;
            m_speedMode = (speedStatus != 0) ? speed_t::fast : speed_t::normal;
        }
        /// @brief Toggle pause mode - event handler
        /// @param pauseStatus Desired pause status boolean (on / off)
        void setPauseMode(int32_t pauseStatus)
        {
            m_speedMode = (pauseStatus != 0) ? speed_t::pause : speed_t::normal;
        }
    };
}
