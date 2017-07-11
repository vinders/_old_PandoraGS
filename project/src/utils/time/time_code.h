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
            TimeCode(const uint32_t roundedRate, const bool isDropFrame) noexcept 
                : m_hours(0), m_minutes(0), m_seconds(0), m_samples(0), m_roundedRate(roundedRate) 
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
                        throw std::invalid_argument("TimeCode: invalid rate value : drop-frame system requires multiple of 30 or of 24");
                }
            }
            /// @brief Create time code with initial value
            /// @param[in] roundedRate  Time code rate (rounded, if NTSC)
            /// @param[in] isDropFrame  Use drop-frame (if NTSC)
            /// @param[in] hours        Hours elapsed
            /// @param[in] minutes      Minutes elapsed in current hour
            /// @param[in] seconds      Seconds elapsed in current minute
            /// @param[in] samples      Samples elapsed in current second
            TimeCode(const uint32_t roundedRate, const bool isDropFrame, const int32_t hours, const int32_t minutes, const int32_t seconds, const int32_t samples) noexcept : m_roundedRate(roundedRate)
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
                    {
                        m_droppedFrames = roundedRate / 15u;
                        if (samples < m_droppedFrames && isDropMinute(minutes, seconds))
                            throw std::invalid_argument("TimeCode: invalid sample number for drop-frame system (N*30fps) : samples below (rate/15) are not allowed during minutes that aren't a multiple of 10");
                    }
                    else if ((roundedRate % 24) == 0)
                    {
                        m_droppedFrames = roundedRate / 12u;
                        if (samples < m_droppedFrames && isDropMinute(minutes, seconds))
                            throw std::invalid_argument("TimeCode: invalid sample number for drop-frame system (N*24fps) : samples below (rate/12) are not allowed during minutes with modulo 10 < 7, and if (minute+1) modulo 50 is 0");
                    }
                    else
                        throw std::invalid_argument("TimeCode: invalid rate value : drop-frame system requires a multiple of 30 or of 24");
                }
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
            static TimeCode fromSamples(const uint32_t roundedRate, const bool isDropFrame, const int64_t samples) noexcept 
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
            static TimeCode fromDuration(const uint32_t roundedRate, const bool isDropFrame, const utils::time::Duration& duration) noexcept 
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
                    
                    if (isDropMinute(duration.minutes(), seconds))
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
            static TimeCode fromMilliseconds(const uint32_t roundedRate, const bool isDropFrame, const int64_t milliseconds) noexcept 
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
                    
                    if (isDropMinute(minutes, seconds))
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
            static TimeCode fromString(const std::string tcStr)
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
            
            template <uint32_t RndRate = 30u>
            inline bool isDropMinute(const int32_t minutes, const int32_t seconds) const noexcept
            {
                // 30 fps -> drop 18 frames out of 300 (10 min)
                // 9 min / 10 : drop 2 frames
                //=(9*((60*30)-2) + 60*30)/600  =  29.97
                if (RndRate % 30u == 0)
                    return (seconds == 0 && (minutes % 10) != 0);
                // 24 fps -> drop 14.4 frames out of 240 (10 min) -> 14 frames out of 240 + 2 frames every 50 min
                // 7 min / 10 : drop 2 frames + 2 per 50 min
                //=(5*(7*((60*24)-2) + 3*60*24) - 2)/3000  =  29.976
                if (RndRate % 24u == 0)
                    return (seconds == 0 && ((minutes % 10) < 7 || ((minutes + 1) % 50) == 0));  
                // for multiples of 24 or 30, drop multiples of 2
                return false;
            }
            
            
            /// @brief Convert time code to string representation ('HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate', e.g.: '12:05:22:14@25' or '05:20:36;02@30')
            inline utils::time::Duration toDuration() const noexcept
            {
                
            }
            
            /// @brief Convert time code to string representation ('HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate', e.g.: '12:05:22:14@25' or '05:20:36;02@30')
            inline int64_t toMilliseconds() const noexcept
            {
                
            }
            
            /// @brief Convert time code to string representation ('HH:MM:SS:FF@rate' or 'HH:MM:SS;FF@rate', e.g.: '12:05:22:14@25' or '05:20:36;02@30')
            inline std::string toString() const noexcept
            {
                
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