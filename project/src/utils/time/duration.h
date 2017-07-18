/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : high-resolution duration (can be negative)
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include "../vendor/windows_types.h"

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.time
    /// Time management utilities
    namespace time
    {
        #ifdef _WINDOWS
        /// @brief Time point ticks
        using ticks_t = LONGLONG;
        #else
        /// @brief Time point ticks
        using ticks_t = uint64_t;
        #endif
        using signed_ticks_t = int64_t;
        
        
        /// @class Duration
        /// @brief High-resolution duration
        class Duration
        {
        public:
            /// @brief Create duration set to zero
            Duration() noexcept : m_nanoseconds(0u) {}
            /// @brief Create duration
            /// @param[in] nanoseconds  Duration value (nanoseconds)
            Duration(const int64_t nanoseconds) noexcept : m_nanoseconds(nanoseconds) {}
            /// @brief Copy duration
            /// @param[in] other  Other instance
            Duration(const Duration& other) noexcept : m_nanoseconds(other.m_nanoseconds) {}
            /// @brief Move duration
            /// @param[in] other  Other instance
            Duration(Duration&& other) noexcept : m_nanoseconds(other.m_nanoseconds) {}
            
            /// @brief Create copy of current instance
            /// @returns Duration (copy)
            inline Duration clone() { return Duration(m_nanoseconds); }
            
            
            // -- Builders --
            
            /// @brief Build duration from total number of days
            /// @param[in] days  Number of days
            static Duration fromDays(const int64_t days) noexcept       { return Duration(days * (24LL * 3600LL * 1000000000LL)); }
            /// @brief Build duration from total number of hours
            /// @param[in] hours  Number of hours
            static Duration fromHours(const int64_t hours) noexcept     { return Duration(hours * (3600LL * 1000000000LL)); }
            /// @brief Build duration from total number of minutes
            /// @param[in] minutes  Number of minutes
            static Duration fromMinutes(const int64_t minutes) noexcept { return Duration(minutes * (60LL * 1000000000LL)); }
            /// @brief Build duration from total number of seconds
            /// @param[in] seconds  Number of seconds
            static Duration fromSeconds(const int64_t seconds) noexcept { return Duration(seconds * 1000000000LL); }
            /// @brief Build duration from total number of milliseconds
            /// @param[in] milliseconds  Number of milliseconds
            static Duration fromMilliseconds(const int64_t milliseconds) noexcept { return Duration(milliseconds * 1000000LL); }
            /// @brief Build duration from a number of ticks to convert
            /// @param[in] ticks  Number of ticks
            /// @param[in] rate   Tick rate (ticks per second) to use for conversion (if 0, defaults to 1000 ticks/sec)
            static Duration fromTicks(const signed_ticks_t ticks, const uint64_t rate)
            {
                if (rate > 0u) 
                {
                    int64_t nanoseconds = (static_cast<int64_t>(ticks) / rate) * 1000000000uLL;
                    nanoseconds += ((static_cast<int64_t>(ticks) % rate) * 1000000000uLL) / rate;
                    return Duration(nanoseconds); // two-part calculation, to avoid reaching max value
                }
                else
                    return Duration(static_cast<int64_t>(ticks) * 1000000uLL);
            }
            
            
            // -- Getters --
            
            /// @brief Get isolated number of days
            inline int64_t days() const noexcept         { return m_nanoseconds / (24LL * 3600LL * 1000000000LL); }
            /// @brief Get isolated number of hours (days removed)
            inline int64_t hours() const noexcept        { return (m_nanoseconds % (24uLL * 3600LL * 1000000000LL)) / (3600LL * 1000000000LL); }
            /// @brief Get isolated number of minutes (hours removed)
            inline int64_t minutes() const noexcept      { return (m_nanoseconds % (3600LL * 1000000000LL)) / (60uLL * 1000000000LL); }
            /// @brief Get isolated number of seconds (minutes removed)
            inline int64_t seconds() const noexcept      { return (m_nanoseconds % (60LL * 1000000000LL)) / 1000000000LL; }
            /// @brief Get isolated number of milliseconds (seconds removed)
            inline int64_t milliseconds() const noexcept { return (m_nanoseconds % 1000000000LL) / 1000000LL; }
            /// @brief Get isolated number of nanoseconds (milliseconds removed)
            inline int64_t nanoseconds() const noexcept  { return m_nanoseconds % 1000000LL; }
            
            /// @brief Get total number of days
            inline int64_t totalDays() const noexcept    { return m_nanoseconds / (24LL * 3600LL * 1000000000LL); }
            /// @brief Get total number of hours (including days)
            inline int64_t totalHours() const noexcept   { return m_nanoseconds / (3600LL * 1000000000LL); }
            /// @brief Get total number of minutes (including days/hours)
            inline int64_t totalMinutes() const noexcept { return m_nanoseconds / (60LL * 1000000000LL); }
            /// @brief Get total number of seconds (including days/hours/minutes)
            inline int64_t totalSeconds() const noexcept { return m_nanoseconds / 1000000000LL; }
            /// @brief Get total number of milliseconds (including days/hours/minutes/seconds)
            inline int64_t totalMilliseconds() const noexcept { return m_nanoseconds / 1000000LL; }
            /// @brief Get total number of nanoseconds (including days/hours/minutes/seconds/milliseconds)
            inline int64_t totalNanoseconds() const noexcept  { return m_nanoseconds; }
            
            /// @brief Convert time reference to ticks
            /// @param[in] rate  Tick rate to use for conversion (if 0, defaults to 1000 ticks/sec)
            inline signed_ticks_t toTicks(const uint64_t rate) const noexcept
            {
                int64_t ticks;
                if (rate > 0u) 
                {
                    // two-part calculation, to avoid reaching max value
                    ticks = (m_nanoseconds / 1000000000uLL) * rate;
                    ticks += ((m_nanoseconds % 1000000000uLL) * rate) / 1000000000uLL;
                }
                else
                    ticks = m_nanoseconds / 1000000uLL;
                return static_cast<signed_ticks_t>(ticks);
            }
            
            /// @brief Compare 2 instances
            inline bool equals(const Duration& other) const noexcept         { return (m_nanoseconds == other.m_nanoseconds); }
            /// @brief Compare instance to nanoseconds
            inline bool equals(const int64_t nanoseconds) const noexcept     { return (m_nanoseconds == nanoseconds); }
            /// @brief Compare 2 instances (equality)
            inline bool operator==(const Duration& other) const noexcept     { return (m_nanoseconds == other.m_nanoseconds); }
            /// @brief Compare instance to nanoseconds (equality)
            inline bool operator==(const int64_t nanoseconds) const noexcept { return (m_nanoseconds == nanoseconds); }
            /// @brief Compare 2 instances (difference)
            inline bool operator!=(const Duration& other) const noexcept     { return (m_nanoseconds != other.m_nanoseconds); }
            /// @brief Compare instance to nanoseconds (difference)
            inline bool operator!=(const int64_t nanoseconds) const noexcept { return (m_nanoseconds != nanoseconds); }

            /// @brief Compare 2 instances (lower)
            inline bool operator<(const Duration& other) const noexcept      { return (m_nanoseconds < other.m_nanoseconds); }
            /// @brief Compare instance to nanoseconds (lower)
            inline bool operator<(const int64_t nanoseconds) const noexcept  { return (m_nanoseconds < nanoseconds); }
            /// @brief Compare 2 instances (lower or equal)
            inline bool operator<=(const Duration& other) const noexcept     { return (m_nanoseconds <= other.m_nanoseconds); }
            /// @brief Compare instance to nanoseconds (lower or equal)
            inline bool operator<=(const int64_t nanoseconds) const noexcept { return (m_nanoseconds <= nanoseconds); }
            /// @brief Compare 2 instances (higher)
            inline bool operator>(const Duration& other) const noexcept      { return (m_nanoseconds > other.m_nanoseconds); }
            /// @brief Compare instance to nanoseconds (higher)
            inline bool operator>(const int64_t nanoseconds) const noexcept  { return (m_nanoseconds > nanoseconds); }
            /// @brief Compare 2 instances (higher or equal)
            inline bool operator>=(const Duration& other) const noexcept     { return (m_nanoseconds >= other.m_nanoseconds); }
            /// @brief Compare instance to nanoseconds (higher or equal)
            inline bool operator>=(const int64_t nanoseconds) const noexcept { return (m_nanoseconds >= nanoseconds); }
            
            
            // -- Setters --
            
            /// @brief Set duration value, based on total nanoseconds
            /// @param[in] nanoseconds  Number of nanoseconds (can be negative)
            inline void setTotalNanoseconds(const int64_t nanoseconds) noexcept
            {
                m_nanoseconds = nanoseconds;
            }
            
            /// @brief Copy assignment
            inline Duration& operator=(const Duration& other) noexcept     { m_nanoseconds = other.m_nanoseconds; return *this; }
            /// @brief Value assignment (total nanoseconds)
            inline Duration& operator=(const int64_t nanoseconds) noexcept { m_nanoseconds = nanoseconds; return *this; }
            
            /// @brief Pre-increment duration
            inline Duration& operator++() noexcept    { ++m_nanoseconds; return *this; }
            /// @brief Post-increment duration
            inline Duration operator++(int) noexcept { Duration copy(m_nanoseconds); ++m_nanoseconds; return copy; }
            /// @brief Pre-decrement duration
            inline Duration& operator--() noexcept    { --m_nanoseconds; return *this; }
            /// @brief Post-decrement duration
            inline Duration operator--(int) noexcept { Duration copy(m_nanoseconds); --m_nanoseconds; return copy; }
            
            /// @brief Add duration
            inline Duration operator+(const Duration& other) noexcept     { return Duration(m_nanoseconds + other.m_nanoseconds); }
            /// @brief Add nanoseconds
            inline Duration operator+(const int64_t nanoseconds) noexcept { return Duration(m_nanoseconds + nanoseconds); }
            /// @brief Add and store duration
            inline void operator+=(const Duration& other) noexcept        { m_nanoseconds += other.m_nanoseconds; }
            /// @brief Add and store nanoseconds
            inline void operator+=(const int64_t nanoseconds) noexcept    { m_nanoseconds += nanoseconds; }
            /// @brief Substract duration
            inline Duration operator-(const Duration& other) noexcept      { return Duration(m_nanoseconds - other.m_nanoseconds); }
            /// @brief Substract nanoseconds
            inline Duration operator-(const uint64_t nanoseconds) noexcept  { return Duration(m_nanoseconds - nanoseconds); }
            /// @brief Substract and store duration
            inline void operator-=(const Duration& other) noexcept        
            {
                if (other.m_nanoseconds <= m_nanoseconds)
                    m_nanoseconds -= other.m_nanoseconds;
                else
                    m_nanoseconds = 0u;
            }
            /// @brief Substract and store nanoseconds
            inline void operator-=(const int64_t nanoseconds) noexcept    
            {
                if (nanoseconds <= m_nanoseconds)
                    m_nanoseconds -= nanoseconds;
                else
                    m_nanoseconds = 0u;
            }
            
            /// @brief Multiply time reference
            inline Duration operator*(const int32_t times) noexcept  { return Duration(m_nanoseconds * static_cast<int64_t>(times)); }
            /// @brief Multiply and store time reference
            inline void operator*=(const int32_t times) noexcept     { m_nanoseconds *= static_cast<int64_t>(times); }
            /// @brief Divide time reference
            inline Duration operator/(const int32_t divider) noexcept { return Duration(m_nanoseconds / static_cast<int64_t>(divider)); }
            /// @brief Divide and store time reference
            inline void operator/=(const int32_t divider) noexcept    { m_nanoseconds /= static_cast<int64_t>(divider); }
            /// @brief Shift left
            inline Duration operator<<(const uint32_t factor) noexcept { return Duration(m_nanoseconds << factor); }
            /// @brief Shift left and store
            inline void operator<<=(const uint32_t factor) noexcept    { m_nanoseconds <<= factor; }
            /// @brief Shift right
            inline Duration operator>>(const uint32_t factor) noexcept { return Duration(m_nanoseconds >> factor); }
            /// @brief Shift right and store
            inline void operator>>=(const uint32_t factor) noexcept    { m_nanoseconds >>= factor; }
            
            
        private:
            int64_t m_nanoseconds; ///< Time duration (nanoseconds)
        };
    }
}
