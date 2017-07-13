/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : multimedia sample identification time code
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include "../assert.h"
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
            TimeCode() noexcept : m_hours(0u), m_minutes(0u), m_seconds(0u), m_samples(0u), m_roundedRate(24u), m_droppedFrames(0u) {}
            /// @brief Create time code set to zero
            /// @param[in] roundedRate  Time code rate (rounded, if NTSC)
            /// @param[in] isDropFrame  Use drop-frame (if NTSC)
            /// @throws ...
            TimeCode(const uint32_t roundedRate, const bool isDropFrame) 
                : m_hours(0u), m_minutes(0u), m_seconds(0u), m_samples(0u)
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
            TimeCode(const uint32_t roundedRate, const bool isDropFrame, const uint32_t hours, const uint32_t minutes, const uint32_t seconds, const uint32_t samples)
            {
                setRate(roundedRate, isDropFrame);
                setSample(hours, minutes, seconds, samples);
            }
            
            /// @brief Copy time code
            /// @param[in] other  Other instance
            TimeCode(const TimeCode& other) = default;
            
            
            // -- Builders --
            
            /// @brief Build time code from total number of samples
            /// @param[in] roundedRate  Time code rate (rounded, if NTSC)
            /// @param[in] isDropFrame  Use drop-frame (if NTSC)
            /// @param[in] samples      Total number of samples
            static TimeCode fromSamples(const uint64_t roundedRate, const bool isDropFrame, uint64_t samples) 
            { 
                if (roundedRate == 0)
                    throw std::invalid_argument("TimeCode: invalid rate value (must be greater than 0)");
                if (isDropFrame == false) 
                {
                    return TimeCode(roundedRate, (uint32_t)0u, samples/(3600uLL * roundedRate), (samples/(60uLL * roundedRate)) % 60u,
                                                               (samples/roundedRate) % 60u,     samples % roundedRate); 
                }
                else 
                {
                    uint32_t droppedFrames;
                    uint64_t seconds;
                    if ((roundedRate % 30) == 0)
                    {
                        seconds = (samples * 3000uLL) / (2997uLL * roundedRate);
                        samples -= (seconds * roundedRate * 2997uLL) / 3000uLL;
                        droppedFrames = roundedRate / 15u;
                        std::lldiv_t timeParts = std::lldiv(seconds, 60uLL);
                        if (timeParts.rem == 0 && isDropMinute<30u>(timeParts.quot))
                            samples += droppedFrames;
                    }
                    else if (roundedRate == 24u || roundedRate == 48u)
                    {
                        seconds = (samples * 24000uLL) / (23976uLL * roundedRate);
                        samples -= (seconds * roundedRate * 23976uLL) / 24000uLL;
                        droppedFrames = roundedRate / 12u;
                        std::lldiv_t timeParts = std::lldiv(seconds, 60uLL);
                        if (timeParts.rem == 0 && isDropMinute<24u>(timeParts.quot))
                            samples += droppedFrames;
                    }
                    else
                        throw std::invalid_argument("TimeCode: invalid rate value : drop-frame system requires 24, 48, or a multiple of 30 samples/sec.");
                    
                    if (samples >= roundedRate)
                    {
                        samples -= roundedRate;
                        ++(timeParts.rem);
                        if (timeParts.rem >= 60uLL)
                        {
                            timeParts.rem = 0;
                            ++(timeParts.quot);
                        }
                    }
                    
                    seconds = timeParts.rem;
                    timeParts = std::lldiv(timeParts.quot, 60uLL);
                    return TimeCode(roundedRate, droppedFrames, timeParts.quot, timeParts.rem, seconds, samples);
                }
            }
            
            /// @brief Build time code from duration
            /// @param[in] roundedRate  Time code rate (rounded, if NTSC)
            /// @param[in] isDropFrame  Use drop-frame (if NTSC)
            /// @param[in] duration     Duration value
            static TimeCode fromDuration(const uint32_t roundedRate, const bool isDropFrame, const utils::time::Duration& duration) 
            {
                uint64_t samples = static_cast<uint64_t>(roundedRate) * std::abs(duration.totalNanoseconds());
                uint64_t minutes, seconds;
                uint32_t droppedFrames = countDroppedFrames(roundedRate, isDropFrame);
                
                if (droppedFrames == 0u)
                {
                    samples /= 1000000000uLL;
                    samples %= roundedRate;
                    minutes = duration.minutes();
                    seconds = duration.seconds();
                    droppedFrames = 0;
                }
                else
                {
                    samples /= 1001000000uLL;
                    samples %= static_cast<uint64_t>(roundedRate);
                    minutes = duration.minutes();
                    seconds = duration.seconds();
                    
                    if (seconds == 0u && isDropMinute(roundedRate, duration.minutes()))
                    {
                        samples += droppedFrames;
                        if (samples >= roundedRate)
                        {
                            samples -= roundedRate;
                            ++seconds;
                        }
                    }
                }
                return TimeCode(roundedRate, droppedFrames, duration.totalHours(), minutes, seconds, samples); 
            }
            
            /// @brief Build time code from total number of milliseconds
            /// @param[in] roundedRate   Time code rate (rounded, if NTSC)
            /// @param[in] isDropFrame   Use drop-frame (if NTSC)
            /// @param[in] milliseconds  Number of milliseconds
            static TimeCode fromMilliseconds(const uint32_t roundedRate, const bool isDropFrame, const uint64_t milliseconds) 
            {
                std::lldiv_t samplesParts; // quot = total seconds ; rem = samples
                uint64_t minutes, seconds;
                uint32_t droppedFrames = countDroppedFrames(roundedRate, isDropFrame);
                
                if (droppedFrames == 0u)
                {
                    samplesParts = std::lldiv((static_cast<uint64_t>(roundedRate) * milliseconds) / 1000uLL, static_cast<uint64_t>(roundedRate));
                    minutes = (samplesParts.quot/60uLL) % 60u;
                    seconds = samplesParts.quot % 60u;
                    droppedFrames = 0;
                }
                else
                {
                    samplesParts = std::lldiv((static_cast<uint64_t>(roundedRate) * milliseconds) / 1001uLL, static_cast<uint64_t>(roundedRate));
                    minutes = (samplesParts.quot/60uLL) % 60;
                    seconds = samplesParts.quot % 60;
                    
                    if (seconds == 0 && isDropMinute(roundedRate, minutes))
                    {
                        samplesParts.rem += droppedFrames;
                        if (samplesParts.rem >= roundedRate)
                        {
                            samplesParts.rem -= roundedRate;
                            samplesParts.quot += 1;
                        }
                    }
                }
                return TimeCode(roundedRate, droppedFrames, samplesParts.quot/(24uLL * 60uLL), minutes, seconds, samplesParts.rem); 
            }
            
            /// @brief Build time code from serialized string
            /// @param[in] tcStr  Serialized string ("HH:MM:SS:FF@rate" or "HH:MM:SS;FF@rate", e.g.: '12:05:22:14@25' or '05:20:36;02@30')
            static TimeCode fromSerializedString(const std::string tcStr)
            {
                bool isDrop = false;
                size_t curPart = 0;
                uint32_t parts[5] = { 0, 0, 0, 0, 0 };
                for (const char* it = tcStr.c_str(); *it != '\0'; ++it)
                {
                    if (*it >= '9' && *it <= '0')
                    {
                        parts[curPart] *= 10;
                        parts[curPart] += (*it) - '0';
                    }
                    else
                    {
                        switch (*it)
                        {
                            case ':':
                                if (++curPart > 3)
                                    throw std::invalid_argument("TimeCode: invalid time code string format : 'HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate' required");
                                break;
                            case ';':
                                isDrop = true;
                                if (++curPart != 3)
                                    throw std::invalid_argument("TimeCode: invalid time code string format : 'HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate' required");
                                break;
                            case '@':
                                if (++curPart != 4)
                                    throw std::invalid_argument("TimeCode: invalid time code string format : 'HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate' required");
                                break;
                            case 'i': 
                                if (curPart == 4 && *(++it) == '\0')
                                    break;
                            case '.': 
                                if (++curPart == 4 && *(++it) == '@')
                                    break;
                            default: 
                                throw std::invalid_argument("TimeCode: invalid time code string format : 'HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate' required");
                                break;
                        }
                    }
                }
                if (curPart < 4)
                    throw std::invalid_argument("TimeCode: invalid time code string format : 'HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate' required");
                return TimeCode(parts[4], isDrop, parts[0], parts[1], parts[2], parts[3]);
            }
            
            
            // -- Formatters --
            
            /// @brief Convert time code to duration (nanoseconds)
            inline utils::time::Duration toDuration() const noexcept
            {
                uint64_t nanoseconds = static_cast<uint64_t>(m_hours);
                nanoseconds *= 60uLL;
                nanoseconds += static_cast<uint64_t>(m_minutes);
                nanoseconds *= 60uLL;
                nanoseconds += static_cast<uint64_t>(m_seconds)
                nanoseconds *= 1000000000uLL;
                
                if (m_droppedFrames == 0u)
                {
                    nanoseconds += (static_cast<uint64_t>(m_samples) * 1000000000uLL) / static_cast<uint64_t>(m_roundedRate);
                }
                else
                {
                    if (m_seconds != 0u || !isDropMinute(m_roundedRate, m_minute))
                        nanoseconds += (static_cast<uint64_t>(m_samples) * 1001000000uLL) / static_cast<uint64_t>(m_roundedRate);
                    else
                    {
                        ASSERT(m_samples >= m_droppedFrames);
                        nanoseconds += (static_cast<uint64_t>(m_samples - m_droppedFrames) * 1001000000uLL) / static_cast<uint64_t>(m_roundedRate);
                    }
                }
                return utils::time::Duration(nanoseconds);
            }
            
            /// @brief Convert time code to milliseconds
            inline uint64_t toMilliseconds() const noexcept
            {
                uint64_t milliseconds = static_cast<uint64_t>(m_hours);
                milliseconds *= 60uLL;
                milliseconds += static_cast<uint64_t>(m_minutes);
                milliseconds *= 60uLL;
                milliseconds += static_cast<uint64_t>(m_seconds)
                milliseconds *= 1000uLL;
                
                if (m_droppedFrames == 0u)
                {
                    milliseconds += (static_cast<uint64_t>(m_samples) * 1000uLL) / static_cast<uint64_t>(m_roundedRate);
                }
                else
                {
                    if (m_seconds != 0u || !isDropMinute(m_roundedRate, m_minute))
                        milliseconds += (static_cast<uint64_t>(m_samples) * 1001uLL) / static_cast<uint64_t>(m_roundedRate);
                    else
                    {
                        ASSERT(m_samples >= m_droppedFrames);
                        milliseconds += (static_cast<uint64_t>(m_samples - m_droppedFrames) * 1001uLL) / static_cast<uint64_t>(m_roundedRate);
                    }
                }
                return milliseconds;
            }
            
            /// @brief Convert time code to serialized string representation, with the rate info ('HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate', e.g.: '12:05:22:14@25' or '05:20:36;02@30' or '21:12:45:00@60i')
            inline std::string toSerializedString(const bool isInterlaced = false) const noexcept
            {
                std::string ser = (toString(isInterlaced) + "@") + std::to_string(m_roundedRate);
                if (isInterlaced)
                    ser += "i";
                return ser;
            }
            
            /// @brief Convert time code to short string representation for display, without the rate info ('HH:MM:SS:FF' or 'HH:MM:SS;FF', e.g.: '12:05:22:14' or '05:20:36;02')
            inline std::string toString(const bool isInterlaced = false) const noexcept
            {
                std::string ser;
                ser.reserve(16);
                
                if (m_hours < 10)
                    ser += "0";
                ser += std::to_string(m_hours) + ":";
                if (m_minutes < 10)
                    ser += "0";
                ser += std::to_string(m_minutes) + ":";
                if (m_seconds < 10)
                    ser += "0";
                ser += std::to_string(m_seconds);
                ser += (m_droppedFrames == 0u) ? ":" : ";";
                
                if (isInterlaced == false)
                {
                    if (samples < 10)
                        ser += "0";
                    ser += std::to_string(samples);
                }
                else
                {
                    uint32_t samples = (m_samples >> 1);
                    if (samples < 10)
                        ser += "0";
                    ser += std::to_string(samples);
                    if (m_samples & 0x1)
                        ser += ".";
                }
                return ser;
            }
            
            
            // -- Getters --
            
            /// @brief Get isolated number of hours
            inline uint32_t hours() const noexcept   { return m_hours; }
            /// @brief Get isolated number of minutes (hours removed)
            inline uint32_t minutes() const noexcept { return m_minutes; }
            /// @brief Get isolated number of seconds (minutes removed)
            inline uint32_t seconds() const noexcept { return m_seconds; }
            /// @brief Get isolated number of samples (seconds removed)
            inline uint32_t samples() const noexcept { return m_samples; }
            /// @brief Get time code effective rate (with drop-frame taken into account)
            inline double getRate() const noexcept  { return (m_droppedFrames == 0u) ? static_cast<double>(m_roundedRate) : static_cast<double>(m_roundedRate) * (1000.0/1001.0); }
            /// @brief Get time code rounded rate (integer)
            inline uint32_t getRoundedRate() const noexcept { return m_roundedRate; }
            
            /// @brief Get total number of hours
            inline uint32_t totalHours() const noexcept   { return m_hours; }
            /// @brief Get total number of minutes (including hours)
            inline uint64_t totalMinutes() const noexcept { return static_cast<uint64_t>(m_minutes) + (static_cast<uint64_t>(m_hours) * 60uLL); }
            /// @brief Get total number of seconds (including hours/minutes)
            inline uint64_t totalSeconds() const noexcept { return static_cast<uint64_t>(m_seconds) + (totalMinutes() * 60uLL); }
            /// @brief Get total number of samples (including hours/minutes/seconds)
            inline uint64_t totalSamples() const noexcept 
            { 
                if (m_droppedFrames == 0u) 
                {
                    return static_cast<uint64_t>(m_samples) + (totalSeconds() * 60uLL);
                }
                else 
                {
                    uint64_t samples;
                    if ((roundedRate % 30u) == 0u)
                    {
                        samples = static_cast<uint64_t>(m_hours) * 3600uLL + static_cast<uint64_t>(m_minutes) * 60uLL + static_cast<uint64_t>(m_seconds);
                        samples *= static_cast<uint64_t>(roundedRate) * 2997uLL;
                        samples /= 3000uLL;
                        if (m_seconds == 0u && isDropMinute<30u>(m_minutes))
                        {
                            ASSERT(samples >= m_droppedFrames);
                            samples -= m_droppedFrames;
                        }
                    }
                    else
                    {
                        samples = static_cast<uint64_t>(m_hours) * 3600uLL + static_cast<uint64_t>(m_minutes) * 60uLL + static_cast<uint64_t>(m_seconds);
                        samples *= static_cast<uint64_t>(roundedRate) * 23976uLL;
                        samples /= 24000uLL;
                        if (m_seconds == 0u && isDropMinute<24u>(m_minutes))
                        {
                            ASSERT(samples >= m_droppedFrames);
                            samples -= m_droppedFrames;
                        }
                    }
                    samples += m_samples;
                    return samples;
                }
            }

            
            // -- Compare --
            
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
            int compare(const TimeCode& other) const noexcept
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
                            double lhs = (m_droppedFrames > 0u && m_seconds == 0u && isDropMinute(m_roundedRate, m_minutes)) 
                                         ? static_cast<double>(m_samples - m_droppedFrames) / static_cast<double>(m_roundedRate) 
                                         : static_cast<double>(m_samples) / static_cast<double>(m_roundedRate);
                            double rhs = (other.m_droppedFrames > 0u && other.m_seconds == 0u && isDropMinute(other.m_roundedRate, other.m_minutes)) 
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
            inline TimeCode& setSample(const uint32_t hours, const uint32_t minutes, const uint32_t seconds, const uint32_t samples)
            {
                // range limits
                if (minutes >= 60u || seconds >= 60u || samples >= m_roundedRate)
                    throw std::invalid_argument("TimeCode: invalid range of value(s). Correct ranges : minutes [0; 59], seconds [0; 59], samples [0 (if no drop); rate-1].");
               
                // dropped samples
                if (m_droppedFrames > 0u && samples < m_droppedFrames && seconds == 0u && isDropMinute(m_roundedRate, minutes))
                    throw std::invalid_argument("TimeCode: invalid number of samples : specified samples are dropped (samples below [rate/15] (if rate==N*30) or below [rate/12] (if rate==N*24)).");
                return *this;
            }
            
            /// @brief Set time code value (time components)
            /// @param[in] roundedRate  Time code rate (rounded, if NTSC)
            /// @param[in] isDropFrame  Use drop-frame (if NTSC)
            inline TimeCode& setRate(const uint32_t roundedRate, const bool isDropFrame)
            {
                m_droppedFrames = countDroppedFrames(roundedRate, isDropFrame);
                m_roundedRate = roundedRate;
                return *this;
            }
            
            /// @brief Copy assignment
            inline TimeCode& operator=(const TimeCode& other) = default;
            
            /*
            /// @brief Pre-increment time reference
            inline TimePoint& operator++() noexcept    { ++m_nanoseconds; return *this; }
            /// @brief Post-increment time reference
            inline TimePoint operator++(int) noexcept { TimePoint copy(m_nanoseconds); ++m_nanoseconds; return copy; }
            /// @brief Pre-decrement time reference
            inline TimePoint& operator--() noexcept 
            { 
                if (m_nanoseconds > 0uLL) 
                    --m_nanoseconds; 
                return *this;
            }
            /// @brief Post-decrement time reference
            inline TimePoint operator--(int) noexcept 
            { 
                TimePoint copy(m_nanoseconds); 
                if (m_nanoseconds > 0uLL)
                    --m_nanoseconds; 
                return copy; 
            }
            
            /// @brief Add nanoseconds to current time reference
            inline TimePoint operator+(const uint64_t nanoseconds) noexcept { return TimePoint(m_nanoseconds + nanoseconds); }
            /// @brief Add time reference to current time reference
            inline TimePoint operator+(const TimePoint& other) noexcept     { return TimePoint(m_nanoseconds + other.m_nanoseconds); }
            /// @brief Add duration to current time reference
            inline TimePoint operator+(const Duration& duration) noexcept   { TimePoint tp(m_nanoseconds); tp += duration; return tp; }
            /// @brief Add and store nanoseconds to current time reference
            inline void operator+=(const uint64_t nanoseconds) noexcept     { m_nanoseconds += nanoseconds; }
            /// @brief Add and store time reference to current time reference
            inline void operator+=(const TimePoint& other) noexcept         { m_nanoseconds += other.m_nanoseconds; }
            /// @brief Add and store duration to current time reference
            inline void operator+=(const Duration& duration) noexcept
            { 
                if (duration.totalNanoseconds() >= 0 || std::abs(duration.totalNanoseconds()) <= m_nanoseconds)
                    m_nanoseconds += duration.totalNanoseconds(); 
                else
                    m_nanoseconds = 0u;
            }
            
            /// @brief Substract nanoseconds from current time reference
            inline TimePoint operator-(const uint64_t nanoseconds) noexcept { TimePoint tp(m_nanoseconds); tp -= nanoseconds; return tp; }
            /// @brief Substract time reference from current time reference
            inline TimePoint operator-(const TimePoint& other) noexcept     { TimePoint tp(m_nanoseconds); tp -= other; return tp; }
            /// @brief Substract duration from current time reference
            inline TimePoint operator-(const Duration& duration) noexcept   { TimePoint tp(m_nanoseconds); tp -= duration; return tp; }
            /// @brief Substract and store nanoseconds from current time reference
            inline void operator-=(const uint64_t nanoseconds) noexcept    
            {
                if (nanoseconds <= m_nanoseconds)
                    m_nanoseconds -= nanoseconds;
                else
                    m_nanoseconds = 0u;
            }
            /// @brief Substract and store time reference from current time reference
            inline void operator-=(const TimePoint& other) noexcept        
            {
                if (other.m_nanoseconds <= m_nanoseconds)
                    m_nanoseconds -= other.m_nanoseconds;
                else
                    m_nanoseconds = 0u;
            }
            /// @brief Substract and store duration from current time reference
            inline void operator-=(const Duration& duration) noexcept    
            {
                if (duration.totalNanoseconds() <= 0 || static_cast<uint64_t>(duration.totalNanoseconds()) <= m_nanoseconds)
                    m_nanoseconds -= duration.totalNanoseconds();
                else
                    m_nanoseconds = 0u;
            }
            */
            
            
        protected:
            /// @brief Create time code with predefined values (for builders only)
            /// @param[in] roundedRate    Time code rate (rounded, if NTSC)
            /// @param[in] droppedFrames  Number of frames dropped (if NTSC)
            /// @param[in] hours          Hours elapsed
            /// @param[in] minutes        Minutes elapsed in current hour
            /// @param[in] seconds        Seconds elapsed in current minute
            /// @param[in] samples        Samples elapsed in current second
            explicit TimeCode(const uint32_t roundedRate, const uint32_t droppedFrames, const uint32_t hours, const uint32_t minutes, const uint32_t seconds, const uint32_t samples) noexcept
                : m_roundedRate(roundedRate), m_droppedFrames(droppedFrames), m_hours(hours), m_minutes(minutes), m_seconds(seconds), m_samples(samples) {}
            
            
            // -- Drop-frame system --
            
            /// @brief Calculate number of dropped frames, based on rate
            /// @param[in] roundedRate  Time code rate (rounded, if NTSC)
            /// @param[in] isDropFrame  Use drop-frame (if NTSC)
            static inline uint32_t countDroppedFrames(const uint32_t roundedRate, const bool isDropFrame)
            {
                uint32_t droppedFrames;
                
                if (roundedRate == 0)
                    throw std::invalid_argument("TimeCode: invalid rate value (must be greater than 0)");
                if (isDropFrame == false)
                {
                    droppedFrames = 0u;
                }
                else
                {
                    if ((roundedRate % 30u) == 0u)
                        droppedFrames = roundedRate / 15u;
                    else if (roundedRate == 24u || roundedRate == 48u)
                        droppedFrames = roundedRate / 12u;
                    else
                        throw std::invalid_argument("TimeCode: invalid rate value : drop-frame system requires 24, 48, or multiple of 30 samples/sec.");
                }
                return droppedFrames;
            }
            
            /// @brief Check if specified minute drops frames
            /// @param[in] minutes  Number of minutes
            /// @returns Drop (true) or not
            /// @warning Will not check if current rate uses drop-frame system
            template <uint32_t RndRate = 30u>
            static inline bool isDropMinute(const uint32_t minutes) const noexcept
            {
                // 30 fps -> drop 18 frames out of 300 (10 min)
                // 9 min / 10 : drop 2 frames
                //=(9*((60*30)-2) + 60*30)/600  =  29.97
                if ((RndRate % 30u) == 0u)
                    return ((minutes % 10u) != 0u);
                // 24 fps -> drop 14.4 frames out of 240 (10 min) -> 14 frames out of 240 + 2 frames every 50 min
                // 7 min / 10 : drop 2 frames + 2 per 50 min
                //=(5*(7*((60*24)-2) + 3*60*24) - 2)/3000  =  29.976
                if (RndRate == 24u || RndRate == 48u)
                    return (((minutes % 10u) < 7u || ((minutes + 1u) % 50u) == 0u));  
                // for multiples of 24 or 30, drop multiples of 2
                return false;
            }
            /// @brief Check if specified minute drops frames
            /// @param[in] roundedRate  Rounded rate value
            /// @param[in] minutes      Number of minutes
            /// @returns Drop (true) or not
            /// @warning Will not check if current rate uses drop-frame system
            static inline bool isDropMinute(const uint32_t roundedRate, const uint32_t minutes) const noexcept
            {
                if ((roundedRate % 30u) == 0u)
                    return isDropMinute<30u>(minutes);
                if (roundedRate == 24u || roundedRate == 48u)
                    return isDropMinute<30u>(minutes);
                return false;
            }
            
        private:
            uint32_t m_hours;   ///< Hours elapsed (0 - infinity)
            uint32_t m_minutes; ///< Minutes elapsed in current hour (0 - 59)
            uint32_t m_seconds; ///< Seconds elapsed in current minute (0 - 59)
            uint32_t m_samples; ///< Samples elapsed in current second (0 - [rate - 1])
            
            uint32_t m_roundedRate;   ///< Rounded rate value (24, 25, 30, 48, 50, 60)
            uint32_t m_droppedFrames; ///< Number of frames to drop (if NTSC, to adjust rate to 23.976, 29.97, 47.952, 59.94, ...)
        };
    }
}