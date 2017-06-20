/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : high-resolution stopwatch
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include <chrono>
#include "duration.h"

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.time
    /// Time management utilities
    namespace time
    {
        /// @class Stopwatch
        /// @brief High-resolution stopwatch
        class Stopwatch
        {
        public:
            /// @brief Create default stopwatch
            Stopwatch() noexcept : m_isRunning(false), m_prevElapsedNanoseconds(0), m_curElapsedNanoseconds(0) {}

            /// @brief Copy initialized stopwatch
            /// @param[in] other  Other instance
            Stopwatch(const Stopwatch& other) noexcept 
                : m_isRunning(other.m_isRunning), m_timeReference(other.m_timeReference), 
                  m_prevElapsedNanoseconds(other.m_prevElapsedNanoseconds), m_curElapsedNanoseconds(other.m_curElapsedNanoseconds) {}
                  
            /// @brief Copy assignment
            inline Stopwatch& operator=(const Stopwatch& other) noexcept 
            { 
                m_timeReference = other.m_timeReference;
                m_prevElapsedNanoseconds = other.m_prevElapsedNanoseconds;
                m_curElapsedNanoseconds = other.m_curElapsedNanoseconds;
                m_isRunning = other.m_isRunning;
            }
            
            
            // -- Utilities --
            
            /// @brief Stop time interval measurement and reset elapsed time to zero
            inline void reset()
            {
                m_isRunning = false;
                m_prevElapsedNanoseconds = m_curElapsedNanoseconds = m_timeReference = 0uLL;
            }

            /// @brief Start/resume measuring elapsed time
            inline void start()
            {
                if (m_isRunning)
                {
                    countElapsedTime();
                    m_prevElapsedNanoseconds += m_curElapsedNanoseconds;
                }
                auto curTime = std::chrono::high_resolution_clock::now()
                m_timeReference = std::chrono::duration_cast<std::chrono::nanoseconds>(curTime).count();
                m_isRunning = true;
            }
            /// @brief Reset elapsed time to zero and start measuring elapsed time
            inline void restart()
            {
                reset();
                start();
            }
            
            /// @brief Stop measuring elapsed time
            inline void stop()
            {
                if (m_isRunning)
                {
                    countElapsedTime();
                    m_isRunning = false;
                    m_prevElapsedNanoseconds += m_curElapsedNanoseconds;
                    m_curElapsedNanoseconds = 0uLL;
                }
            }
            
            
            // -- Getters / Setters --
            
            /// @brief Get current status
            /// @returns Running (ture) or idle (false)
            inline bool isRunning() const noexcept
            {
                return m_isRunning;
            }
            
            /// @brief Get elapsed time (duration)
            /// @returns Time duration (nanoseconds)
            inline Duration getElapsedTime() const
            {
                if (m_isRunning)
                    countElapsedTime();
                return Duration(static_cast<int64_t>(m_prevElapsedNanoseconds + m_curElapsedNanoseconds));
            }
            /// @brief Get elapsed time (milliseconds)
            /// @returns Time (milliseconds)
            inline uint64_t getElapsedMilliseconds() const
            {
                if (m_isRunning)
                    countElapsedTime();
                return (m_prevElapsedNanoseconds + m_curElapsedNanoseconds) / 1000000uLL;
            }
            /// @brief Get elapsed time (microseconds)
            /// @returns Time (microseconds)
            inline uint64_t getElapsedMicroseconds() const
            {
                if (m_isRunning)
                    countElapsedTime();
                return (m_prevElapsedNanoseconds + m_curElapsedNanoseconds) / 1000uLL;
            }
            
        protected:
            /// @brief Measure elapsed time since time reference
            /// @returns Time duration (nanoseconds)
            inline void countElapsedTime()
            {
                auto curTime = std::chrono::high_resolution_clock::now()
                uint64_t curNanosec = std::chrono::duration_cast<std::chrono::nanoseconds>(curTime).count();
                m_curElapsedNanoseconds = curNanosec - m_timeReference;
            }
            
            
        private:
            bool m_isRunning;         ///< Stopwatch status : running / idle
            uint64_t m_timeReference; ///< Time reference for current measurement
            uint64_t m_prevElapsedNanoseconds; ///< Elapsed nanoseconds (during previous measurements)
            uint64_t m_curElapsedNanoseconds;  ///< Elapsed nanoseconds (current measurement)
        };
    }
}
