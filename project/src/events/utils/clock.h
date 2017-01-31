/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : high-resolution clock
*******************************************************************************/
#pragma once

#include <cstdlib>
#include <cstdint>
#ifdef _WINDOWS
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
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
    #define TIMEMODE_LENGTH 2


    /// @namespace event.utils
    /// Event management utilities
    namespace utils
    {
        /// @class Clock
        /// @brief Time counter and synchronizer
        class Clock
        {
        private:
            // clock settings
            events::timemode_t m_timeMode; ///< Timing mode
            tick_frequency_t m_cpuFreq;    ///< CPU ticks frequency
            float m_period;                ///< CPU ticks per period
            ticks_t m_periodTruncated;     ///< CPU ticks per period (integer value, truncated decimals)
            float m_periodDrop;            ///< Period dropped decimals (if period is not an integer value)

            // time references
            #ifdef _WINDOWS
            LARGE_INTEGER m_previousTime; ///< Last time reference (high-res)
            #endif
            ticks_t m_previousTicks;      ///< Last ticks reference (low-res)
            ticks_t m_ticksToWait;        ///< Number of ticks required before next period
            float m_droppedTickParts;     ///< Ticks decimal part dropped in calculation (if period is not an integer)

            // measures
            int32_t m_periodCount;  ///< Number of periods since last check
            #ifdef _WINDOWS
            LARGE_INTEGER m_freqCheckTime; ///< Last frequency check time reference (high-res)
            #endif
            ticks_t m_freqCheckTicks;      ///< Last frequency check ticks reference (low-res)


        public:
            /// @brief Create default clock
            Clock() : m_period(20.0f), m_periodTruncated((ticks_t)20uL), m_periodDrop(0.0f), m_ticksToWait((ticks_t)0uL),
                      m_timeMode(events::timemode_t::multimediaClock), m_droppedTickParts(0.0f), m_periodCount(1) {}
            /// @brief Initialize clock
            /// @param defaultFrequency Clock default frequency
            /// @param preferedMode Prefered time mode (if available)
            Clock(float defaultFrequency, events::timemode_t preferedMode)
            {
                setClockConfig(defaultFrequency, preferedMode);
                reset();
            }

            /// @brief Reset time reference
            void reset();

            /// @brief Wait for one period after time reference
            /// @return Number of ticks that occured too late (after desired time reference)
            events::ticks_t wait();
            /// @brief Skip one period (no wait)
            void skip()
            {
                ++m_periodCount;
            }

            /// @brief Calculate real number of periods per second
            float checkFrequency();


            // -- getters / setters -- -----------------------------------------

            /// @brief Set frequency and change time mode
            /// @param frequency Clock frequency
            /// @param preferedMode Prefered time mode (if available)
            void setClockConfig(float frequency, events::timemode_t preferedMode);
            /// @brief Set frequency
            /// @param frequency Clock frequency
            void setFrequency(float frequency);

            /// @brief Get period duration
            /// @return Period value (ticks)
            float getPeriodDuration() const
            {
                return m_period;
            }
            /// @brief Add custom duration for current period
            /// @return Period value (ticks)
            void addCustomDuration(events::ticks_t duration)
            {
                m_ticksToWait += duration;
            }


            #ifndef _WINDOWS
            /// @brief Linux-Unix compatible time system
            static unsigned long timeGetTime();
            #endif
        };
    }
}
