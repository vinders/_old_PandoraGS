/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : multimedia sample identification time code
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include "duration.h"

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.time
    /// Time management utilities
    namespace time
    {
        /// @class TimeCode
        /// @brief Multimedia sample identification time code (HH:mm:ss:ff)
        class TimeCode
        {
        public:
            /// @brief Create 24p time code set to zero
            TimeCode() noexcept : m_hours(0), m_minutes(0), m_seconds(0), m_samples(0), m_roundedRate(24u), m_droppedFrames(0u) {}
            /// @brief Create time code set to zero
            /// @param[in] roundedRate  Time code rate (rounded, if NTSC)
            /// @param[in] isDropFrame  Use drop-frame (if NTSC)
            TimeCode(const uint32_t roundedRate, const bool isDropFrame) 
                : m_hours(0), m_minutes(0), m_seconds(0), m_samples(0)
            {
                setRate(roundedRate, isDropFrame);
            }
            /// @brief Create time code with initial value
            /// @param[in] roundedRate  Time code rate (rounded, if NTSC)
            /// @param[in] isDropFrame  Use drop-frame (if NTSC)
            /// @param[in] hours        Hours elapsed
            /// @param[in] minutes      Minutes elapsed in current hour
            /// @param[in] seconds      Seconds elapsed in current minute
            /// @param[in] samples      Samples elapsed in current second
            TimeCode(const uint32_t roundedRate, const bool isDropFrame, const int32_t hours, const int32_t minutes, const int32_t seconds, const int32_t samples)
            {
                setRate(roundedRate, isDropFrame);
                setSample(hours, minutes, seconds, samples);
            }
            
            /// @brief Copy time code
            /// @param[in] other  Other instance
            TimeCode(const TimeCode& other) noexcept 
                : m_hours(other.m_hours), m_minutes(other.m_minutes), m_seconds(other.m_seconds), m_samples(other.m_samples), m_roundedRate(other.m_roundedRate), m_droppedFrames(other.m_droppedFrames) {}
            /// @brief Move time code
            /// @param[in] other  Other instance
            TimeCode(TimeCode&& other) noexcept 
                : m_hours(other.m_hours), m_minutes(other.m_minutes), m_seconds(other.m_seconds), m_samples(other.m_samples), m_roundedRate(other.m_roundedRate), m_droppedFrames(other.m_droppedFrames) {}
            
            
            // -- Builders --
            
            /// @brief Build time code from total number of samples
            /// @param[in] roundedRate  Time code rate (rounded, if NTSC)
            /// @param[in] isDropFrame  Use drop-frame (if NTSC)
            /// @param[in] samples      Total number of samples
            static TimeCode fromSamples(const uint32_t roundedRate, const bool isDropFrame, const int64_t samples) 
            { 
                if (roundedRate == 0)
                    throw std::invalid_argument("TimeCode: invalid rate value (must be greater than 0)");                  
                if (isDropFrame == false) 
                {
                    return TimeCode(roundedRate, isDropFrame, 
                                    samples/(3600LL * static_cast<int64_t>(roundedRate)), (samples/(60LL * static_cast<int64_t>(roundedRate))) % 60,
                                    (samples/static_cast<int64_t>(roundedRate)) % 60,     samples % static_cast<int64_t>(roundedRate)); 
                }
                else 
                {
                    if ((roundedRate % 30) == 0)
                    {
                        int64_t droppedSamples = (samples * 18) / (600 * 30); // drop 18 codes per 600 seconds
                        // puis boucler sur chaque minute -> +2 si pas %10
                        //...
                    }
                    else if ((roundedRate % 24) == 0)
                    {
                        int64_t droppedSamples = (samples * 18) / (600 * 24); // drop 14 codes per 600 seconds
                        droppedSamples += (samples * 2) / (3000 * 24);        // drop 2 more codes per 3000 seconds
                        // puis boucler sur chaque minute
                        //...
                    }
                    else
                        throw std::invalid_argument("TimeCode: invalid rate value : drop-frame system requires a multiple of 30 or of 24");
                }
            }
            
            /// @brief Build time code from duration
            /// @param[in] roundedRate  Time code rate (rounded, if NTSC)
            /// @param[in] isDropFrame  Use drop-frame (if NTSC)
            /// @param[in] duration     Duration value
            static TimeCode fromDuration(const uint32_t roundedRate, const bool isDropFrame, const utils::time::Duration& duration) 
            {
                int64_t samples = static_cast<int64_t>(roundedRate) * duration.totalNanoseconds();
                int64_t minutes, seconds;
                if (isDropFrame == false)
                {
                    samples /= 1000000000LL;
                    samples %= roundedRate;
                    minutes = duration.minutes();
                    seconds = duration.seconds();
                }
                else
                {
                    samples /= 1001000000LL;
                    samples %= static_cast<int64_t>(roundedRate);
                    minutes = duration.minutes();
                    seconds = duration.seconds();
                    
                    if (seconds == 0 && isDropMinute(duration.minutes()))
                    {
                        samples += (roundedRate % 30 == 0) ? static_cast<int64_t>(roundedRate) / 15LL : static_cast<int64_t>(roundedRate) / 12LL;
                        if (samples >= roundedRate)
                        {
                            samples -= roundedRate;
                            seconds += 1;
                        }
                    }
                }
                return TimeCode(roundedRate, isDropFrame, duration.totalHours(), minutes, seconds, samples); 
            }
            
            /// @brief Build time code from total number of milliseconds
            /// @param[in] roundedRate   Time code rate (rounded, if NTSC)
            /// @param[in] isDropFrame   Use drop-frame (if NTSC)
            /// @param[in] milliseconds  Number of milliseconds
            static TimeCode fromMilliseconds(const uint32_t roundedRate, const bool isDropFrame, const int64_t milliseconds) 
            {
                std::lldiv_t samplesParts; // quot = total seconds ; rem = samples
                int64_t minutes, seconds;
                if (isDropFrame == false)
                {
                    samplesParts = std::lldiv((static_cast<int64_t>(roundedRate) * milliseconds) / 1000LL, static_cast<int64_t>(roundedRate));
                    minutes = (samplesParts.quot/60LL) % 60;
                    seconds = samplesParts.quot % 60;
                }
                else
                {
                    samplesParts = std::lldiv((static_cast<int64_t>(roundedRate) * milliseconds) / 1001LL, static_cast<int64_t>(roundedRate));
                    minutes = (samplesParts.quot/60LL) % 60;
                    seconds = samplesParts.quot % 60;
                    
                    if (seconds == 0 && isDropMinute(minutes))
                    {
                        samplesParts.rem += (roundedRate % 30 == 0) ? static_cast<int64_t>(roundedRate) / 15LL : static_cast<int64_t>(roundedRate) / 12LL;
                        if (samplesParts.rem >= roundedRate)
                        {
                            samplesParts.rem -= roundedRate;
                            samplesParts.quot += 1;
                        }
                    }
                }
                return TimeCode(roundedRate, isDropFrame, samplesParts.quot/(24LL * 60LL), minutes, seconds, samplesParts.rem); 
            }
            
            /// @brief Build time point from a number of ticks to convert
            /// @param[in] tcStr  Serialized string ("HH:MM:SS:FF@rate" or "HH:MM:SS;FF@rate", e.g.: '12:05:22:14@25' or '05:20:36;02@30')
            static TimeCode fromCodeString(const std::string tcStr)
            {
                bool isDrop = false;
                bool isNegative = false;
                size_t curPart = 0;
                uint32_t parts[5] = { 0, 0, 0, 0, 0 };
                for (const char* it = tcStr.c_str(); *it != '\0'; ++it)
                {
                    if (*it >= '9' && *it <= '0')
                    {
                        parts[curPart] *= 10;
                        parts[curPart] += (*it) - '0';
                    }
                    else if (*it == ':')
                    {
                        if (++curPart > 3)
                            throw std::invalid_argument("TimeCode: invalid time code string format : 'HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate' required");
                    }
                    else if (*it == ';')
                    {
                        isDrop = true;
                        if (++curPart != 3)
                            throw std::invalid_argument("TimeCode: invalid time code string format : 'HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate' required");
                    }
                    else if (*it == '@')
                    {
                        if (++curPart != 4)
                            throw std::invalid_argument("TimeCode: invalid time code string format : 'HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate' required");
                    }
                    else if (*it == '-')
                    {
                        isNegative = true;
                        if (curPart > 0 || parts[0] != 0)
                            throw std::invalid_argument("TimeCode: invalid time code string format : minus sign can only be the first character");
                    }
                    else
                        throw std::invalid_argument("TimeCode: invalid time code string format : 'HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate' required");
                }
                
                if (curPart < 4)
                    throw std::invalid_argument("TimeCode: invalid time code string format : 'HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate' required");
                if (isNegative)
                {
                    if (parts[0] != 0)
                        parts[0] = -parts[0];
                    else if (parts[1] != 0)
                        parts[1] = -parts[1];
                    else if (parts[2] != 0)
                        parts[2] = -parts[2];
                    else
                        parts[3] = -parts[3];
                }
                return TimeCode(parts[4], isDrop, parts[0], parts[1], parts[2], parts[3]);
            }
            
            
            // -- Formatters --
            
            /// @brief Convert time code to string representation ('HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate', e.g.: '12:05:22:14@25' or '05:20:36;02@30')
            inline utils::time::Duration toDuration() const noexcept
            {
                
            }
            
            /// @brief Convert time code to string representation ('HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate', e.g.: '12:05:22:14@25' or '05:20:36;02@30')
            inline int64_t toMilliseconds() const noexcept
            {
                
            }
            
            /// @brief Convert time code to full string representation, with the rate info ('HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate', e.g.: '12:05:22:14@25' or '05:20:36;02@30')
            inline std::string toCodeString() const noexcept
            {
                
            }
            
            /// @brief Convert time code to short string representation for display, without the rate info ('HH:MM:SS:FF' or 'HH:MM:SS;FF', e.g.: '12:05:22:14' or '05:20:36;02')
            inline std::string toString() const noexcept
            {
                
            }
            
            
            // -- Getters --
            
            /// @brief Get isolated number of hours
            inline int32_t hours() const noexcept   { return m_hours; }
            /// @brief Get isolated number of minutes (hours removed)
            inline int32_t minutes() const noexcept { return m_minutes; }
            /// @brief Get isolated number of seconds (minutes removed)
            inline int32_t seconds() const noexcept { return m_seconds; }
            /// @brief Get isolated number of samples (seconds removed)
            inline int32_t samples() const noexcept { return m_samples; }
            /// @brief Get time code effective rate (with drop-frame taken into account)
            inline double getRate() const noexcept  { return (m_droppedFrames == 0u) ? static_cast<double>(m_roundedRate) : static_cast<double>(m_roundedRate) * (1000.0/1001.0); }
            /// @brief Get time code rounded rate (integer)
            inline uint32_t getRoundedRate() const noexcept { return m_roundedRate; }
            
            /// @brief Get total number of hours
            inline int32_t totalHours() const noexcept   { return m_hours; }
            /// @brief Get total number of minutes (including hours)
            inline int64_t totalMinutes() const noexcept { return static_cast<int64_t>(m_minutes) + (static_cast<int64_t>(m_hours) * 60LL); }
            /// @brief Get total number of seconds (including hours/minutes)
            inline int64_t totalSeconds() const noexcept { return static_cast<int64_t>(m_seconds) + (totalMinutes() * 60LL); }
            /// @brief Get total number of samples (including hours/minutes/seconds)
            inline int64_t totalSamples() const noexcept { return static_cast<int64_t>(m_samples) + (totalSeconds() * 60LL); }
            
            /// @brief Check if specified minute drops frames
            /// @param[in] minutes  Number of minutes
            /// @returns Drop (true) or not
            /// @warning Will not check if current rate uses drop-frame system
            template <uint32_t RndRate = 30u>
            static inline bool isDropMinute(const int32_t minutes) const noexcept
            {
                // 30 fps -> drop 18 frames out of 300 (10 min)
                // 9 min / 10 : drop 2 frames
                //=(9*((60*30)-2) + 60*30)/600  =  29.97
                if (RndRate % 30u == 0)
                    return ((minutes % 10) != 0);
                // 24 fps -> drop 14.4 frames out of 240 (10 min) -> 14 frames out of 240 + 2 frames every 50 min
                // 7 min / 10 : drop 2 frames + 2 per 50 min
                //=(5*(7*((60*24)-2) + 3*60*24) - 2)/3000  =  29.976
                if (RndRate % 24u == 0)
                    return (((minutes % 10) < 7 || ((minutes + 1) % 50) == 0));  
                // for multiples of 24 or 30, drop multiples of 2
                return false;
            }
            
            /// @brief Compare 2 instances (all members)
            inline bool equals(const TimeCode& other) const noexcept
            { 
                return (m_roundedRate == other.m_roundedRate && m_droppedFrames == other.m_droppedFrames 
                     && m_hours == other.m_hours && m_minutes == other.m_minutes 
                     && m_seconds == other.m_seconds && m_samples == other.m_samples; 
            }
            /// @brief Compare 2 instances (equality)
            inline bool operator==(const TimeCode& other) const noexcept { return equals(other); }
            /// @brief Compare 2 instances (difference)
            inline bool operator!=(const TimeCode& other) const noexcept { return !equals(other); }

            /// @brief Compare 2 instances (sample value)
            inline int32_t compare(const TimeCode& other) const noexcept
            { 
                if (m_hours < other.m_hours)
                    return -1; // smaller
                if (m_hours == other.m_hours)
                {
                    if (m_minutes < other.m_minutes)
                        return -1; // smaller
                    if (m_minutes == other.m_minutes)
                    {
                        if (m_seconds < other.m_seconds)
                            return -1; // smaller
                        if (m_seconds == other.m_seconds)
                        {
                            // same rate -> simple sample comparison
                            if (m_roundedRate == other.m_roundedRate && m_droppedFrames == other.m_droppedFrames)
                            {
                                if (m_samples == other.m_samples)
                                    return 0;
                                return (m_samples < other.m_samples) ? -1 : 1;
                            }
                            
                            // different rate -> calculation
                            double lhs = (m_droppedFrames > 0 && isDropMinute(m_minutes)) 
                                         ? static_cast<double>(m_samples - m_droppedFrames) / static_cast<double>(m_roundedRate) 
                                         : static_cast<double>(m_samples) / static_cast<double>(m_roundedRate);
                            double rhs = (other.m_droppedFrames > 0 && isDropMinute(other.m_minutes)) 
                                         ? static_cast<double>(other.m_samples - other.m_droppedFrames) / static_cast<double>(other.m_roundedRate) 
                                         : static_cast<double>(other.m_samples) / static_cast<double>(other.m_roundedRate);
                            double epsilon = (1.0 / ((static_cast<double>(m_roundedRate) + static_cast<double>(other.m_roundedRate)) * 0.5)) * 0.1;
                            if (lhs >= rhs - epsilon && lhs <= rhs + epsilon)
                                return 0;
                            return (lhs < rhs) ? -1 : 1;
                        }
                    }
                }
                return 1; // greater
            }
            /// @brief Compare 2 instances (lower)
            inline bool operator<(const TimeCode& other) const noexcept  { return (compare(other) < 0); }
            /// @brief Compare 2 instances (lower or equal)
            inline bool operator<=(const TimeCode& other) const noexcept { return (compare(other) <= 0); }
            /// @brief Compare 2 instances (higher)
            inline bool operator>(const TimeCode& other) const noexcept  { return (compare(other) > 0); }
            /// @brief Compare 2 instances (higher or equal)
            inline bool operator>=(const TimeCode& other) const noexcept { return (compare(other) >= 0); }
            
            
            // -- Setters --
            
            /// @brief Set time code value (time components)
            /// @param[in] hours    Hours elapsed
            /// @param[in] minutes  Minutes elapsed in current hour
            /// @param[in] seconds  Seconds elapsed in current minute
            /// @param[in] samples  Samples elapsed in current second
            inline TimeCode& setSample(const int32_t hours, const int32_t minutes, const int32_t seconds, const int32_t samples)
            {
                // range limits
                if (minutes >= 60 || seconds >= 60 || samples >= m_roundedRate)
                    throw std::invalid_argument("TimeCode: invalid range of value(s). Correct ranges : minutes [0; 59], seconds [0; 59], samples [0 (if no drop); rate-1].");
                if (minutes < 0 || seconds < 0 || samples < 0)
                {
                    if (hours == 0 && (minutes < 0 || (minutes == 0 && seconds <= 0)) )
                    {
                        if (minutes <= -60 || seconds <= -60 || samples <= -m_roundedRate)
                            throw std::invalid_argument("TimeCode: invalid range of value(s). Correct ranges : minutes [-59; 59], seconds [-59; 59], samples [-rate+1; rate-1].");
                    }
                    else
                        throw std::invalid_argument("TimeCode: invalid sign. Only biggest non-zero item can be negative.");
                }
               
                // dropped samples
                if (m_droppedFrames > 0 && samples < m_droppedFrames && seconds == 0 && isDropMinute(minutes))
                    throw std::invalid_argument("TimeCode: invalid number of samples : specified samples are dropped (samples below [rate/15] (if rate==N*30) or below [rate/12] (if rate==N*24)).");
                return *this;
            }
            
            /// @brief Set time code value (time components)
            /// @param[in] roundedRate  Time code rate (rounded, if NTSC)
            /// @param[in] isDropFrame  Use drop-frame (if NTSC)
            inline TimeCode& setRate(const uint32_t roundedRate, const bool isDropFrame)
            {
                if (roundedRate == 0)
                    throw std::invalid_argument("TimeCode: invalid rate value (must be greater than 0)");
                
                if (isDropFrame == false)
                {
                    m_droppedFrames = 0u;
                }
                else
                {
                    if ((roundedRate % 30) == 0)
                        m_droppedFrames = roundedRate / 15u;
                    else if ((roundedRate % 24) == 0)
                        m_droppedFrames = roundedRate / 12u;
                    else
                        throw std::invalid_argument("TimeCode: invalid rate value : drop-frame system requires multiple of 30 or of 24 samples/sec.");
                }
                m_roundedRate = roundedRate;
                return *this;
            }
            
            /// @brief Copy assignment
            inline TimeCode& operator=(const TimeCode& other) noexcept 
            { 
                m_roundedRate = other.m_roundedRate; 
                m_droppedFrames = other.m_droppedFrames; 
                m_hours = other.m_hours; 
                m_minutes = other.m_minutes; 
                m_seconds = other.m_seconds; 
                m_samples = other.m_samples; 
                return *this; 
            }
            
            
        private:
            int32_t m_hours;   ///< Hours elapsed (0 - infinity)
            int32_t m_minutes; ///< Minutes elapsed in current hour (0 - 59)
            int32_t m_seconds; ///< Seconds elapsed in current minute (0 - 59)
            int32_t m_samples; ///< Samples elapsed in current second (0 - [rate - 1])
            
            uint32_t m_roundedRate;   ///< Rounded rate value (24, 25, 30, 48, 50, 60)
            uint32_t m_droppedFrames; ///< Number of frames to drop (if NTSC, to adjust rate to 23.976, 29.97, 47.952, 59.94, ...)
        };
    }
}