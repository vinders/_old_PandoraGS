/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : high-resolution clock
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include "time_point.hpp"

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.events
    /// Event management utilities
    namespace events
    {
        /// @class Clock
        /// @brief High-resolution clock
        class Clock
        {
        public:
            /// @enum timemode_t
            /// @brief Timing modes
            enum class timemode_t : uint32_t
            {
                multimediaClock = 0u, ///< Low-resolution, steady
                highResCounter = 1u   ///< High-resolution, unstable
            };
            
            
            /// @brief Create default clock
            Clock() noexcept : 
                m_timeMode(Clock::timemode_t::multimediaClock), m_periodDuration(20000000uLL), m_runtimeDuration(20000000uLL), m_periodSubDuration(0.0f), m_droppedSubDurations(0.0f), m_periodCount(1) {}
            /// @brief Create initialized clock
            /// @param[in] freqNumerator     Clock frequency (desired periods per second) - numerator (e.g.: 60000)
            /// @param[in] freqDenominator   Clock frequency (desired periods per second) - denominator (e.g.: 1001)
            /// @param[in] preferedMode  Prefered time mode (if available)
            Clock(const uint32_t freqNumerator, const uint32_t freqDenominator, const Clock::timemode_t preferedMode) noexcept : m_periodCount(1)
            {
                setTimeMode(preferedMode);
                setFrequency(freqNumerator, freqDenominator);
                reset();
            }
            /// @brief Copy initialized clock
            /// @param[in] other  Other instance
            Clock(const Clock& other) noexcept :
                m_timeMode(other.m_timeMode), m_tickRate(other.m_tickRate), 
                m_periodDuration(other.m_periodDuration), m_periodSubDuration(other.m_periodSubDuration), 
                m_runtimeDuration(other.m_runtimeDuration), m_droppedSubDurations(other.m_droppedSubDurations)
                m_timeReference(other.m_timeReference), m_auxTimeReference(other.m_auxTimeReference), 
                m_freqCalcReference(other.m_freqCalcReference), m_auxFreqCalcReference(other.m_auxFreqCalcReference), m_periodCount(other.m_periodCount) {}
            
            
            // -- Utilities --
            
            /// @brief Reset time reference
            void reset() noexcept;

            /// @brief Wait for one period after time reference
            /// @returns Number of nanoseconds that occured too late (after desired time reference)
            uint64_t wait() noexcept;
            
            /// @brief Calculate real number of periods per second
            float calculateFrequency() noexcept;
            
            
            // -- Getters / Setters --
            
            /// @brief Set time mode and tick rate
            /// @param[in] preferedMode  Prefered time mode (if available)
            void setTimeMode(const Clock::timemode_t preferedMode) noexcept;
            /// @brief Set clock frequency
            /// @param[in] freqNumerator     Clock frequency (desired periods per second) - numerator (e.g.: 60000)
            /// @param[in] freqDenominator   Clock frequency (desired periods per second) - denominator (e.g.: 1001)
            void setFrequency(const uint32_t freqNumerator, const uint32_t freqDenominator) noexcept;
            
            /// @brief Extend current period duration
            /// @param[in] nanoseconds  Number of nanoseconds to add to duration
            inline void extendCurrentPeriodDuration(const uint64_t nanoseconds) noexcept
            {
                m_runtimeDuration += nanoseconds;
            }
            /// @brief Get period duration
            /// @returns Period duration (nanoseconds)
            inline uint64_t getPeriodDuration() const noexcept
            {
                return m_periodDuration;
            }
            
            
        private:
            // -- Time reference creation --
        
            /// @brief Create new time reference
            /// @returns Time reference (current time)
            static TimePoint now() const noexcept;
            /// @brief Create new time reference
            /// @param[out] timeRef     Time reference (current time)
            /// @param[out] timeRefAux  Auxiliary time reference (current time)
            static void now(TimePoint& timeRef, TimePoint& timeRefAux) const noexcept;
            
            
        private:
            // clock settings
            Clock::timemode_t m_timeMode;  ///< Timing mode
            uint64_t m_periodDuration;     ///< Duration between two checks (nanoseconds)
            float m_periodSubDuration;     ///< Sub-duration (less than 1 nanosecond) to add to duration
            #ifdef _WINDOWS
            uint64_t m_tickRate;           ///< Counter ticks per second
            #endif
            
            // time reference
            TimePoint m_timeReference;     ///< Time reference (latest check)
            TimePoint m_auxTimeReference;  ///< Auxiliary low-res time reference (in case of high-res inconsistencies)
            uint64_t m_runtimeDuration;    ///< Dynamic duration value (for current period only)
            float m_droppedSubDurations;   ///< Sum of sub-durations dropped (added when sum is more than 1 nanosecond)
            
            // frequency calculation
            TimePoint m_freqCalcReference; ///< Frequency calculation reference (latest frequency calculation)
            TimePoint m_auxFreqCalcReference; ///< Auxiliary low-res calculation reference (in case of high-res inconsistencies)
            uint32_t m_periodCount;        ///< Number of periods since last frequency calculation
        };
    }
}
