/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : high-resolution time reference point
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#ifdef _WINDOWS
#   include <windef.h>
#endif

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.events
    /// Event management utilities
    namespace events
    {
        #ifdef _WINDOWS
        /// @brief Time point ticks
        typedef LONGLONG ticks_t;
        #else
        /// @brief Time point ticks
        typedef uint64_t ticks_t;
        #endif
        
        
        /// @class TimePoint
        /// @brief High-resolution time reference point
        class TimePoint
        {
        public:
            /// @brief Create time point set to zero
            TimePoint() noexcept : m_nanoseconds(0u) {}
            /// @brief Create time point
            /// @param[in] nanoseconds  Time reference (nanoseconds)
            TimePoint(const uint64_t nanoseconds) noexcept : m_nanoseconds(nanoseconds) {}
            /// @brief Copy time point 
            /// @param[in] other  Other instance
            TimePoint(const TimePoint& other) noexcept : m_nanoseconds(other.nanoseconds) {}
            /// @brief Move time point
            /// @param[in] other  Other instance
            TimePoint(TimePoint&& other) noexcept : m_nanoseconds(other.nanoseconds) {}
            
            
            // -- Builders --
            
            /// @brief Build time point from total number of days
            /// @param[in] ticks  Number of days
            static TimePoint fromDays(const uint64_t days) noexcept       { return TimePoint(days * (24uLL * 3600uLL * 1000000000uLL)); }
            /// @brief Build time point from total number of hours
            /// @param[in] ticks  Number of hours
            static TimePoint fromHours(const uint64_t hours) noexcept     { return TimePoint(hours * (3600uLL * 1000000000uLL)); }
            /// @brief Build time point from total number of minutes
            /// @param[in] ticks  Number of minutes
            static TimePoint fromMinutes(const uint64_t minutes) noexcept { return TimePoint(minutes * (60uLL * 1000000000uLL)); }
            /// @brief Build time point from total number of seconds
            /// @param[in] ticks  Number of seconds
            static TimePoint fromSeconds(const uint64_t seconds) noexcept { return TimePoint(seconds * 1000000000uLL); }
            /// @brief Build time point from total number of milliseconds
            /// @param[in] ticks  Number of milliseconds
            static TimePoint fromMilliseconds(const uint64_t milliseconds) noexcept { return TimePoint(milliseconds * 1000000uLL); }
            /// @brief Build time point from a number of ticks to convert
            /// @param[in] ticks  Number of ticks
            /// @param[in] rate   Tick rate (ticks per second) to use for conversion (if 0, defaults to 1000 ticks/sec)
            static TimePoint fromTicks(const ticks_t ticks, const uint64_t rate)
            {
                if (rate > 0u) 
                {
                    uint64_t nanoseconds = (static_cast<uint64_t>(ticks) / rate) * 1000000000uLL;
                    nanoseconds += ((static_cast<uint64_t>(ticks) % rate) * 1000000000uLL) / rate;
                    return TimePoint(nanoseconds); // two-part calculation, to avoid reaching max value
                }
                else
                    return TimePoint(static_cast<uint64_t>(ticks) * 1000000uLL);
            }
            
            
            // -- Getters --
            
            /// @brief Get isolated number of days
            inline uint64_t days() const noexcept         { return m_nanoseconds / (24uLL * 3600uLL * 1000000000uLL); }
            /// @brief Get isolated number of hours (days removed)
            inline uint64_t hours() const noexcept        { return (m_nanoseconds % (24uLL * 3600uLL * 1000000000uLL)) / (3600uLL * 1000000000uLL); }
            /// @brief Get isolated number of minutes (hours removed)
            inline uint64_t minutes() const noexcept      { return (m_nanoseconds % (3600uLL * 1000000000uLL)) / (60uLL * 1000000000uLL); }
            /// @brief Get isolated number of seconds (minutes removed)
            inline uint64_t seconds() const noexcept      { return (m_nanoseconds % (60uLL * 1000000000uLL)) / 1000000000uLL; }
            /// @brief Get isolated number of milliseconds (seconds removed)
            inline uint64_t milliseconds() const noexcept { return (m_nanoseconds % 1000000000uLL) / 1000000uLL; }
            /// @brief Get isolated number of nanoseconds (milliseconds removed)
            inline uint64_t nanoseconds() const noexcept  { return m_nanoseconds % 1000000uLL; }
            
            /// @brief Get total number of days
            inline uint64_t totalDays() const noexcept    { return m_nanoseconds / (24uLL * 3600uLL * 1000000000uLL); }
            /// @brief Get total number of hours (including days)
            inline uint64_t totalHours() const noexcept   { return m_nanoseconds / (3600uLL * 1000000000uLL); }
            /// @brief Get total number of minutes (including days/hours)
            inline uint64_t totalMinutes() const noexcept { return m_nanoseconds / (60uLL * 1000000000uLL); }
            /// @brief Get total number of seconds (including days/hours/minutes)
            inline uint64_t totalSeconds() const noexcept { return m_nanoseconds / 1000000000uLL; }
            /// @brief Get total number of milliseconds (including days/hours/minutes/seconds)
            inline uint64_t totalMilliseconds() const noexcept { return m_nanoseconds / 1000000uLL; }
            /// @brief Get total number of nanoseconds (including days/hours/minutes/seconds/milliseconds)
            inline uint64_t totalNanoseconds() const noexcept  { return m_nanoseconds; }
            
            /// @brief Convert time reference to ticks
            /// @param[in] rate  Tick rate to use for conversion (if 0, defaults to 1000 ticks/sec)
            inline ticks_t toTicks(const uint64_t rate) const noexcept
            {
                uint64_t ticks;
                if (rate > 0u) 
                {
                    // two-part calculation, to avoid reaching max value
                    ticks = (m_nanoseconds / 1000000000uLL) * rate;
                    ticks += ((m_nanoseconds % 1000000000uLL) * rate) / 1000000000uLL;
                }
                else
                    ticks = m_nanoseconds / 1000000uLL;
                return static_cast<ticks_t>(ticks);
            }
            
            /// @brief Compare 2 instances
            inline bool equals(const TimePoint& other) const noexcept         { return (m_nanoseconds == other.m_nanoseconds); }
            /// @brief Compare instance to nanoseconds
            inline bool equals(const uint64_t nanoseconds) const noexcept     { return (m_nanoseconds == nanoseconds); }
            /// @brief Compare 2 instances (equality)
            inline bool operator==(const TimePoint& other) const noexcept     { return (m_nanoseconds == other.m_nanoseconds); }
            /// @brief Compare instance to nanoseconds (equality)
            inline bool operator==(const uint64_t nanoseconds) const noexcept { return (m_nanoseconds == nanoseconds); }
            /// @brief Compare 2 instances (difference)
            inline bool operator!=(const TimePoint& other) const noexcept     { return (m_nanoseconds != other.m_nanoseconds); }
            /// @brief Compare instance to nanoseconds (difference)
            inline bool operator!=(const uint64_t nanoseconds) const noexcept { return (m_nanoseconds != nanoseconds); }

            /// @brief Compare 2 instances (lower)
            inline bool operator<(const TimePoint& other) const noexcept      { return (m_nanoseconds < other.m_nanoseconds); }
            /// @brief Compare instance to nanoseconds (lower)
            inline bool operator<(const uint64_t nanoseconds) const noexcept  { return (m_nanoseconds < nanoseconds); }
            /// @brief Compare 2 instances (lower or equal)
            inline bool operator<=(const TimePoint& other) const noexcept     { return (m_nanoseconds <= other.m_nanoseconds); }
            /// @brief Compare instance to nanoseconds (lower or equal)
            inline bool operator<=(const uint64_t nanoseconds) const noexcept { return (m_nanoseconds <= nanoseconds); }
            /// @brief Compare 2 instances (higher)
            inline bool operator>(const TimePoint& other) const noexcept      { return (m_nanoseconds > other.m_nanoseconds); }
            /// @brief Compare instance to nanoseconds (higher)
            inline bool operator>(const uint64_t nanoseconds) const noexcept  { return (m_nanoseconds > nanoseconds); }
            /// @brief Compare 2 instances (higher or equal)
            inline bool operator>=(const TimePoint& other) const noexcept     { return (m_nanoseconds >= other.m_nanoseconds); }
            /// @brief Compare instance to nanoseconds (higher or equal)
            inline bool operator>=(const uint64_t nanoseconds) const noexcept { return (m_nanoseconds >= nanoseconds); }
            
            
            // -- Setters --
            
            /// @brief Set time point value, based on total nanoseconds
            /// @param[in] nanoseconds  Number of nanoseconds
            inline void setTotalNanoseconds(const uint64_t nanoseconds) noexcept
            {
                m_nanoseconds = nanoseconds;
            }
            
            /// @brief Copy assignment
            inline TimePoint& operator=(const TimePoint& other) noexcept     { m_nanoseconds = other.m_nanoseconds; return *this; }
            /// @brief Value assignment (total nanoseconds)
            inline TimePoint& operator=(const uint64_t nanoseconds) noexcept { m_nanoseconds = nanoseconds; return *this; }
            
            /// @brief Pre-increment time reference
            inline uint64_t operator++() noexcept    { return ++m_nanoseconds; }
            /// @brief Post-increment time reference
            inline uint64_t operator++(int) noexcept { uint64_t copy = m_nanoseconds; ++m_nanoseconds; return copy; }
            /// @brief Pre-decrement time reference
            inline uint64_t operator--() noexcept    { return --m_nanoseconds; }
            /// @brief Post-decrement time reference
            inline uint64_t operator--(int) noexcept { uint64_t copy = m_nanoseconds; --m_nanoseconds; return copy; }
            
            /// @brief Add time reference to current time reference
            inline uint64_t operator+(const TimePoint& other) noexcept     { return m_nanoseconds + other.m_nanoseconds; }
            /// @brief Add nanoseconds to current time reference
            inline uint64_t operator+(const uint64_t nanoseconds) noexcept { return m_nanoseconds + nanoseconds; }
            /// @brief Add and store time reference to current time reference
            inline void operator+=(const TimePoint& other) noexcept        { m_nanoseconds += other.m_nanoseconds; }
            /// @brief Add and store time nanoseconds to current time reference
            inline void operator+=(const uint64_t nanoseconds) noexcept    { m_nanoseconds += nanoseconds; }
            /// @brief Substract time reference from current time reference
            inline int64_t operator-(const TimePoint& other) noexcept      { return static_cast<int64_t>(m_nanoseconds) - static_cast<int64_t>(other.m_nanoseconds); }
            /// @brief Substract nanoseconds from current time reference
            inline int64_t operator-(const uint64_t nanoseconds) noexcept  { return static_cast<int64_t>(m_nanoseconds) - static_cast<int64_t>(nanoseconds); }
            /// @brief Substract and store time reference from current time reference
            inline void operator-=(const TimePoint& other) noexcept        
            {
                if (other.m_nanoseconds <= m_nanoseconds)
                    m_nanoseconds -= other.m_nanoseconds;
                else
                    m_nanoseconds = 0u;
            }
            /// @brief Substract and store nanoseconds from current time reference
            inline void operator-=(const uint64_t nanoseconds) noexcept    
            {
                if (nanoseconds <= m_nanoseconds)
                    m_nanoseconds -= nanoseconds;
                else
                    m_nanoseconds = 0u;
            }
            
            /// @brief Multiply time reference
            inline uint64_t operator*(const uint32_t times) noexcept  { return m_nanoseconds * static_cast<uint64_t>(times); }
            /// @brief Multiply and store time reference
            inline void operator*=(const uint32_t times) noexcept     { m_nanoseconds *= static_cast<uint64_t>(times); }
            /// @brief Divide time reference
            inline uint64_t operator/(const uint32_t divider) noexcept { return m_nanoseconds / static_cast<uint64_t>(divider); }
            /// @brief Divide and store time reference
            inline void operator/=(const uint32_t divider) noexcept    { m_nanoseconds /= static_cast<uint64_t>(divider); }
            /// @brief Shift left
            inline uint64_t operator<<(const uint32_t factor) noexcept { return (m_nanoseconds << factor); }
            /// @brief Shift left and store
            inline void operator<<=(const uint32_t factor) noexcept    { m_nanoseconds <<= factor; }
            /// @brief Shift right
            inline uint64_t operator>>(const uint32_t factor) noexcept { return (m_nanoseconds >> factor); }
            /// @brief Shift right and store
            inline void operator>>=(const uint32_t factor) noexcept    { m_nanoseconds >>= factor; }
            
            
        private:
            uint64_t m_nanoseconds; ///< Time reference (nanoseconds)
        };
    }
}
