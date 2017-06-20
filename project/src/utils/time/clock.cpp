/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : high-resolution clock
*******************************************************************************/
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <chrono>
#ifdef _WINDOWS
#   define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#   include <windowsx.h>
#   include <mmsystem.h>
#endif
#include "time_point.hpp"
#include "clock.h"
using namespace utils::time;


// -- Getters / Setters --

/// @brief Set time mode and tick rate
/// @param[in] preferedMode  Prefered time mode (if available)
void Clock::setTimeMode(const Clock::time_mode_t preferedMode) noexcept
{
    #ifdef _WINDOWS
    LARGE_INTEGER cpuFreq;
    if (preferedMode == Clock::time_mode_t::high_resolution && QueryPerformanceFrequency(&cpuFreq))
    {
        m_tickRate = reinterpret_cast<uint64_t>(cpuFreq.QuadPart);
        m_timeMode = Clock::time_mode_t::high_resolution;
    }
    else
    {
        m_tickRate = 1000uLL;
        m_timeMode = Clock::time_mode_t::steady;
    }
    #else
    m_timeMode = preferedMode;
    #endif
}

/// @brief Set clock frequency
/// @param[in] frequency  Clock frequency (desired periods per second) - rate (e.g.: 60000/1001)
void Clock::setFrequency(const Rate& frequency) noexcept
{
    if (frequency.getCardinal() > 0u)
    {
        m_periodDuration = (static_cast<uint64_t>(frequency.getOrdinal()) * 1000000000uLL) / static_cast<uint64_t>(frequency.getCardinal());
        
        double subDuration = ((static_cast<double>(frequency.getOrdinal()) * 1000000000.0) / static_cast<double>(frequency.getCardinal())) - static_cast<double>(m_periodDuration);
        m_periodSubDuration = (subDuration > 0.0 && subDuration < 1.0) ? static_cast<float>(subDuration) : 0.0f;
    }
    else
    {
        m_periodDuration = 0uLL;
        m_periodSubDuration = 0.0f;
    }
    m_droppedSubDurations = 0.0f;
}



// -- Time reference creation --

/// @brief Create new time reference
/// @returns Time reference (current time)
TimePoint Clock::now() const noexcept
{
    #ifdef _WINDOWS
    if (m_timeMode == Clock::time_mode_t::high_resolution)
    {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        return TimePoint::fromTicks(reinterpret_cast<time::ticks_t>(currentTime.QuadPart), m_tickRate);
    }
    else
    {
        return TimePoint::fromMilliseconds(static_cast<uint64_t>(timeGetTime()));
    }
    #else
    auto time = (m_timeMode == Clock::time_mode_t::high_resolution) ? std::chrono::high_resolution_clock::now() : std::chrono::steady_clock::now();
    return TimePoint(std::chrono::duration_cast<std::chrono::nanoseconds>(time).count());
    #endif
}

/// @brief Create new time reference
/// @param[out] timeRef     Time reference (current time)
/// @param[out] timeRefAux  Auxiliary time reference (current time)
void Clock::now(TimePoint& timeRef, TimePoint& timeRefAux) const noexcept
{
    #ifdef _WINDOWS
    if (m_timeMode == Clock::time_mode_t::high_resolution)
    {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        uint64_t currentTimeAux = static_cast<uint64_t>(timeGetTime());
        
        timeRef = TimePoint::fromTicks(reinterpret_cast<time::ticks_t>(currentTime.QuadPart), m_tickRate);
        timeRefAux = TimePoint::fromMilliseconds(currentTimeAux);
    }
    else
    {
        uint64_t currentTime = static_cast<uint64_t>(timeGetTime());
        auto currentTimeAux = std::chrono::steady_clock::now();
        
        timeRef = TimePoint::fromMilliseconds(currentTime);
        timeRefAux = TimePoint(std::chrono::duration_cast<std::chrono::nanoseconds>(currentTimeAux).count());
    }
    #else // Linux / UNIX
    if (m_timeMode == Clock::time_mode_t::high_resolution)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto currentTimeAux = std::chrono::steady_clock::now();
        
        timeRef = TimePoint(std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime).count());
        timeRefAux = TimePoint(std::chrono::duration_cast<std::chrono::nanoseconds>(currentTimeAux).count());
    }
    else
    {
        auto currentTime =  std::chrono::system_clock::now();
        auto currentTimeAux = std::chrono::steady_clock::now();
        
        timeRef = TimePoint(std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime).count());
        timeRefAux = TimePoint(std::chrono::duration_cast<std::chrono::nanoseconds>(currentTimeAux).count());
    }
    #endif
}



// -- Utilities --

/// @brief Reset time reference
void Clock::reset() noexcept
{
    Clock::now(m_timeReference, m_auxTimeReference);
    m_runtimeDuration = m_periodDuration;
    m_droppedSubDurations = 0.0f;
    
    m_freqCalcReference = m_timeReference;
    m_auxFreqCalcReference = m_auxTimeReference;
    m_periodCount = 1u;
}

/// @brief Wait for one period after time reference
/// @returns Number of nanoseconds that occured too late (after desired time reference)
uint64_t Clock::wait() noexcept
{
    TimePoint currentReference, currentAuxReference;
    int64_t elapsedTime;
    uint64_t lateness;
    
    // wait until end of period
    uint32_t waitLoopCount = 0u;
    do
    {
        Clock::now(currentReference, currentAuxReference);
        elapsedTime = static_cast<int64_t>(currentReference.totalNanoseconds()) - static_cast<int64_t>(m_timeReference.totalNanoseconds());
        
        // invalid time : negative difference or huge number -> use low-res timer instead
        if (elapsedTime <= 0LL || elapsedTime > 500000000LL)
        {
            elapsedTime = static_cast<int64_t>(currentAuxReference.totalNanoseconds()) - static_cast<int64_t>(m_auxTimeReference.totalNanoseconds());
            if (elapsedTime < 0LL) // both negative -> max integer value reached
                elapsedTime = m_runtimeDuration; // break
        }
        ++waitLoopCount;
    } while (elapsedTime < m_runtimeDuration);
    
    ++m_periodCount;
    m_timeReference = currentReference;
    m_auxTimeReference = currentAuxReference;
    
    // set period runtime duration, for next call
    if (waitLoopCount == 1u) // only one loop turn -> lateness -> wait less than period duration to compensate
    {
        lateness = elapsedTime - m_runtimeDuration;
        if (lateness > m_periodDuration) // more than an entire period too late
        {
            m_runtimeDuration = (m_periodDuration >> 1); // half a period
            m_droppedSubDurations = 0.0f;
            lateness = m_periodDuration;
        }
        else
        {
            m_runtimeDuration = m_periodDuration - lateness; // compensation
        }
    }
    else
    {
        m_runtimeDuration = m_periodDuration;
        lateness = 0u;
    }
    // restore dropped sub-durations, if sum is above 1 nanosecond
    m_droppedSubDurations += m_periodSubDuration;
    if (m_droppedSubDurations >= 1.0f) 
    {
        ++m_runtimeDuration;
        m_droppedSubDurations -= 1.0f;
    }
    
    return lateness; // number of ticks too late
}


/// @brief Calculate actual number of periods per second
float Clock::checkFrequency() noexcept
{
    TimePoint currentReference, currentAuxReference;
    int64_t elapsedTime;
    
    // read current time
    Clock::now(currentReference, currentAuxReference);
    elapsedTime = static_cast<int64_t>(currentReference.totalNanoseconds()) - static_cast<int64_t>(m_freqCalcReference.totalNanoseconds());
    
    if (elapsedTime <= 0LL || elapsedTime > 500000000LL) // invalid time -> use low-res timer instead
    {
        elapsedTime = static_cast<int64_t>(currentAuxReference.totalNanoseconds()) - static_cast<int64_t>(m_auxFreqCalcReference.totalNanoseconds());
        if (elapsedTime <= 0LL) // both negative (max integer value reached) or zero (avoid division by zero)
        {
            elapsedTime = (elapsedTime == 0LL) 1LL : m_periodDuration;
        }
    }
    
    m_freqCalcReference = currentReference;
    m_auxFreqCalcReference = currentAuxReference;
    
    // calculate real frequency
    float frequency = static_cast<float>(m_periodCount) * (1000000000.0f / elapsedTime);
    
    m_periodCount = 0u;
    return frequency;
}
