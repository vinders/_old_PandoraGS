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
#ifdef _WINDOWS
#include <Windows.h>
#endif

/// @namespace events
/// Event management
namespace events
{
    #ifdef _WINDOWS
    /// @brief System clock ticks
    typedef DWORD ticks_t;
    /// @brief System clock frequency
    typedef LARGE_INTEGER tick_frequency_t;
    #else
    /// @brief System clock ticks
    typedef unsigned long ticks_t;
    /// @brief System clock frequency
    typedef unsigned long long tick_frequency_t;
    #endif

    /// @enum timemode_t
    /// @brief Timing modes
    enum class timemode_t : uint32_t
    {
        multimediaClock = 0u, // low res, steady
        highResCounter = 1u   // high res, unstable
    };

    /// @enum regionsync_t
    /// @brief Regional sync modes
    enum class regionsync_t : int32_t
    {
        undefined = -1,
        ntsc = 0,
        pal = 1
    };

    /// @enum speed_t
    /// @brief Speed modifiers
    enum class speed_t : int32_t
    {
        slow = -1,
        normal = 0,
        fast = 1
    };

    /// @enum skipmode_t
    /// @brief Skipping modes
    enum class skipmode_t : uint32_t
    {
        disabled = 0u,
        standard = 1u,
        alternate = 2u
    };


    /// @class Timer
    /// @brief Time management tool
    class Timer
    {
    private:
        // timing periods (ticks)
        static tick_frequency_t s_cpuFreq; ///< CPU ticks frequency
        static ticks_t s_period;           ///< CPU ticks per period
        static ticks_t s_periodHiRes;      ///< CPU high res ticks per period
        static ticks_t s_periodTimeout;    ///< Max ticks before skipping

        // timer settings
        static float s_targetFreq;         ///< Configured frequency
        static timemode_t s_timeMode;      ///< Timing mode
        static bool s_isInterlaced;        ///< Interlacing enabled
        static skipmode_t s_skipMode;      ///< Period skipping mode
        static speed_t s_speedMode;        ///< Current speed mode

        // timer execution
        static bool s_isResetPending;         ///< Time reference reset query
        static float s_measuredFreq;          ///< Current measured frequency
        static int32_t s_periodsSinceMeasure; ///< Number of periods since last check
        static int32_t s_periodsToSkip;       ///< Number of periods to skip (to stay in sync)
        static ticks_t s_lateTicks;           ///< Current lateness (compared to time reference)


    public:
        // -- timer settings -- ----------------------------------------------------

        /// @brief Set time management mode
        static void setTimeMode(timemode_t type);

        /// @brief Set timer frequency
        /// @param freqLimit">Frequency max limit
        /// @param regMode">Regional sync mode
        /// @param isInterlaced">Interlacing
        static void setFrequency(float freqLimit, regionsync_t regMode, bool isInterlaced);

        /// @brief Set period skipping mode
        /// @param isEnabled Enable skipping
        /// @param isHalfSkip Skip max one period out of two
        static void setSkippingMode(bool isEnabled, bool isHalfSkip)
        {
            if (isEnabled)
                s_skipMode = (isHalfSkip) ? skipmode_t::alternate : skipmode_t::standard;
            else
                s_skipMode = skipmode_t::disabled;
        }

        /// @brief Reset time reference
        static inline void resetTimeReference()
        {
            s_speedMode = speed_t::normal;
            if (s_periodsToSkip > 0 && s_isInterlaced)
                s_periodsToSkip--;
            else // current period is skipped -> if interlaced, skip next too
                s_periodsToSkip = (s_isInterlaced) ? 1 : 0;
            s_lateTicks = 0;
            s_isResetPending = true;
        }

        /// @brief Toggle slow motion mode - event handler
        static inline void toggleSlowMode(int32_t flag)
        {
            s_speedMode = (s_speedMode == speed_t::normal) ? speed_t::slow : speed_t::normal;
        }

        /// @brief Toggle fast mode - event handler
        static inline void setFastMode(int32_t flag)
        {
            s_speedMode = (flag != 0) ? speed_t::fast : speed_t::normal;
        }


        // -- time operations -- ---------------------------------------------------

        /// @brief Wait for one period (after previous time reference) + optional skipping
        /// @param isLimited Limit frequency
        /// @param isOddPeriod Odd period (if interlaced)
        static void wait(bool isLimited, bool isOddPeriod);

        /// @brief Set skipping for upcoming periods
        /// @param isOddPeriod Odd period (if interlaced)
        static void setSkipping(bool isOddPeriod);

        /// @brief Calculate current number of effective periods per second
        static void calcFrequency();
        /// @brief Get current number of effective periods per second
        /// @return Current periods per second
        static inline float getMeasuredFrequency()
        {
            return s_measuredFreq;
        }

        /// @brief Check if processed period should be skipped
        /// @return Skip indicator (boolean)
        static inline bool isPeriodSkipped()
        {
            return (s_periodsToSkip > 0);
        }

    #ifndef _WINDOWS
    private:
        /// @fn timeGetTime
        /// @brief Linux-Unix compatible time system
        unsigned long timeGetTime();
    #endif
    };
}
