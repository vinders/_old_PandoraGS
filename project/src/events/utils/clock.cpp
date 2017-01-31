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
#include <ctime>
#ifdef _WINDOWS
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#endif
#include "clock.h"
using namespace events::utils;


/// @brief Set frequency and change time mode
/// @param defaultFrequency Clock default frequency
/// @param preferedMode Prefered time mode (if available)
void Clock::setClockConfig(float frequency, events::timemode_t preferedMode)
{
    // set time mode (if available) and period duration
    #ifdef _WINDOWS
    if (preferedMode == events::timemode_t::highResCounter && QueryPerformanceFrequency(&m_cpuFreq))
    {
        m_timeMode = events::timemode_t::highResCounter;
    }
    else
    #endif
    {
        m_timeMode = events::timemode_t::multimediaClock;
    }
    setFrequency(frequency);
}

/// @brief Set frequency
void Clock::setFrequency(float frequency)
{
    // set time mode (if available) and period duration
    #ifdef _WINDOWS
    if (m_timeMode == events::timemode_t::highResCounter)
    {
        double duration = (double)(m_cpuFreq.LowPart) / (double)frequency;
        m_period = (float)duration;
    }
    else
    #endif
    {
        m_period = 1000.0 / frequency;
    }

    // calculate dropped decimals
    m_periodTruncated = static_cast<ticks_t>(m_periodTruncated);
    m_periodDrop = m_period - (float)m_periodTruncated;
    m_droppedTickParts = 0.0f;
}


/// @brief Reset time reference
void Clock::reset()
{
    #ifdef _WINDOWS
    if (m_timeMode == events::timemode_t::highResCounter)
    {
        QueryPerformanceCounter(&m_previousTime);
        m_freqCheckTime.HighPart = m_previousTime.HighPart;
        m_freqCheckTime.LowPart = m_previousTime.LowPart;
    }
    #endif
    m_previousTicks = (events::ticks_t)timeGetTime();
    m_freqCheckTicks = m_previousTicks;
    m_droppedTickParts = 0.0f;
    m_periodCount = 1;
    m_ticksToWait = m_periodTruncated; // wait normal period
}


/// @brief Wait for one period after time reference
/// @return Number of ticks that occured too late (after desired time reference)
events::ticks_t Clock::wait()
{
    events::ticks_t currentTicks, elapsedTicks, lateTicks;
    uint32_t waitLoopCount = 0uL;
    bool isTimeMaxReached = false;
    ++m_periodCount;

    #ifdef _WINDOWS
    // high resolution time sync (qpc)
    if (m_timeMode == events::timemode_t::highResCounter)
    {
        // wait until end of period
        LARGE_INTEGER currentTime;
        do
        {
            QueryPerformanceCounter(&currentTime); // get current time reference
            currentTicks = timeGetTime();
            elapsedTicks = currentTime.LowPart - m_previousTime.LowPart;

            // "more" than 0,5 sec elapsed (negative offset) -> use low-res timer instead
            if (elapsedTicks > (m_cpuFreq.LowPart >> 1))
            {
                if (currentTicks < m_previousTicks) // max integer value reached -> break
                    elapsedTicks = m_periodTruncated + m_ticksToWait + 1u;
                else
                    elapsedTicks = (m_cpuFreq.LowPart * (currentTicks - m_previousTicks)) / 1000;
            }
            ++waitLoopCount;
        } while (elapsedTicks <= m_ticksToWait && currentTime.LowPart >= m_previousTime.LowPart);

        if (currentTime.LowPart < m_previousTime.LowPart) // check if max value reached
            isTimeMaxReached = true;
        m_previousTime.HighPart = currentTime.HighPart; // copy time reference for next call
        m_previousTime.LowPart = currentTime.LowPart;
        m_previousTicks = currentTicks;
    }
    // low resolution time sync (system timer)
    else
    #endif
    {
        // wait until end of period
        do
        {
            currentTicks = timeGetTime(); // get current time reference
            elapsedTicks = currentTicks - m_previousTicks;
            ++waitLoopCount;
        } while (elapsedTicks <= m_ticksToWait && currentTicks >= m_previousTicks);

        if (currentTicks < m_previousTicks) // check if max value reached
            isTimeMaxReached = true;
        m_previousTicks = currentTicks; // copy time reference for next update
    }

    // set time to wait for next call
    if (waitLoopCount == 1uL) // time elapsed > period time : wait less than period duration
    {
        lateTicks = elapsedTicks - m_ticksToWait;
        if (lateTicks > m_periodTruncated) // more than an entire period too late -> no wait
        {
            m_ticksToWait = 0uL;
            m_droppedTickParts = 0.0f;
            if (isTimeMaxReached == false)
                lateTicks = 0u;
        }
        else
            m_ticksToWait = m_periodTruncated - lateTicks;
    }
    else // time elapsed == period time : normal period
    {
        m_ticksToWait = m_periodTruncated;
        lateTicks = 0u;
    }

    // higher precision fix -> count dropped decimals for each period
    m_droppedTickParts += m_periodDrop;
    if (m_droppedTickParts >= 1.0f)
    {
        elapsedTicks = static_cast<ticks_t>(m_droppedTickParts);
        m_ticksToWait += elapsedTicks;
        m_droppedTickParts -= (float)elapsedTicks;
    }
    return lateTicks; // number of ticks too late
}


/// @brief Calculate real number of periods per second
float Clock::checkFrequency()
{
    #ifdef _WINDOWS
    LARGE_INTEGER checkTime;
    #endif
    events::ticks_t checkTicks;
    float frequency;

    // high resolution counter
    #ifdef _WINDOWS
    if (m_timeMode == timemode_t::highResCounter)
    {
        // get current time reference
        QueryPerformanceCounter(&checkTime);
        checkTicks = timeGetTime();
        ticks_t elapsedTicks = checkTime.LowPart - m_freqCheckTime.LowPart;
        if (elapsedTicks > (m_cpuFreq.LowPart >> 1)) // max value exceeded -> use low resolution counter
            elapsedTicks = (m_cpuFreq.LowPart * (checkTicks - m_freqCheckTicks)) / 1000;
        if (elapsedTicks == 0uL) // avoid division by zero
            elapsedTicks = 1uL;

        // calculate frequency
        frequency = (float)m_periodCount * ((float)m_cpuFreq.LowPart / (float)elapsedTicks);

        m_freqCheckTime.HighPart = checkTime.HighPart;
        m_freqCheckTime.LowPart = checkTime.LowPart;
    }
    // low resolution counter
    else
    #endif
    {
        // get current time reference
        checkTicks = timeGetTime();
        ticks_t elapsedTicks = checkTicks - m_freqCheckTicks;
        if (elapsedTicks == 0uL) // avoid division by zero
            elapsedTicks = 1uL;

        // calculate frequency
        frequency = (float)m_periodCount * (1000.0f / (float)elapsedTicks);
    }

    m_freqCheckTicks = checkTicks;
    m_periodCount = 0;
    return frequency;
}


#ifndef _WINDOWS
/// @brief Linux-Unix compatible time system
unsigned long Clock::timeGetTime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (tv.tv_sec * 100000 + tv.tv_usec / 10);
}
#endif
