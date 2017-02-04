/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : time management tool
*******************************************************************************/
#include "../globals.h"
#include <cstdlib>
#include <thread>
#include "utils/clock.h"
#include "timer.h"
using namespace events;

#define MAX_SUCCESSIVE_SKIPPING 4


// -- timer settings -- ----------------------------------------------------

/// @brief Set time management mode
/// @param[in] timeMode  Prefered time mode
void Timer::setTimeMode(const timemode_t timeMode) noexcept
{
    m_isInterlaced = false;
    m_speedMode = speed_t::normal;
    m_skipMode = skipmode_t::disabled;
    m_clock.setClockConfig(59.94f, timeMode);

    m_measuredFreq = 0.0f;
    m_periodsToSkip = 0;
    m_lateTicks = static_cast<ticks_t>(0uL);
    m_isResetPending = true;
}

/// @brief Set timer frequency
/// @param[in] freqLimit     Frequency max limit
/// @param[in] regMode       Regional sync mode
/// @param[in] isInterlaced  Interlacing
void Timer::setFrequency(const float freqLimit, const regionsync_t regMode, const bool isInterlaced) noexcept
{
    // set fixed framerate limit
    m_targetFreq = 59.94f;
    if (freqLimit > 0.01f) // freqLimit > 0 (+ float error offset) = fixed
    {
        m_targetFreq = freqLimit;
    }
    // set auto framerate limit (official chip timing)
    else if (regMode != regionsync_t::undefined)
    {
        switch (regMode)
        {
            case regionsync_t::ntsc: // NTSC - chip timing
            {
                if (isInterlaced)
                    m_targetFreq = 33868800.0f / 565031.25f; // 59.94146
                else
                    m_targetFreq = 33868800.0f / 566107.50f; // 59.82750
                break;
            }
            case regionsync_t::pal: // PAL - chip timing
            {
                if (isInterlaced)
                    m_targetFreq = 33868800.0f / 677343.75f; // 50.00238
                else
                    m_targetFreq = 33868800.0f / 680595.00f; // 49.76351
                break;
            }
            case regionsync_t::ntscStd: // NTSC - standard rate
                m_targetFreq = (isInterlaced) ? 30000.0f / 1001.0f : 60000.0f / 1001.0f; 
                break;
            case regionsync_t::palStd: // PAL - standard rate
                m_targetFreq = (isInterlaced) ? 25.0f : 50.0f; 
                break;
        }
    }

    // set clock frequency + timeout
    m_isInterlaced = isInterlaced;
    m_clock.setFrequency(m_targetFreq);
    m_timeout = ((7uL * static_cast<ticks_t>(m_clock.getPeriodDuration())) >> 3);
    m_measuredFreq = m_targetFreq;

    // reset skipping
    m_periodsToSkip = 0;
    m_lateTicks = static_cast<ticks_t>(0uL);
    m_isResetPending = true;
}

/// @brief Set period skipping mode
/// @param[in] isEnabled   Enable skipping
/// @param[in] isHalfSkip  Skip max one period out of two
void Timer::setSkippingMode(const bool isEnabled, const bool isAlternatedSkip) noexcept
{
    if (isEnabled)
        m_skipMode = (isAlternatedSkip) ? skipmode_t::alternate : skipmode_t::standard;
    else
        m_skipMode = skipmode_t::disabled;
}


// -- time operations -- ---------------------------------------------------

/// @brief Wait for one period (after previous time reference) + optional skipping
/// @param[in] isLimited    Limit frequency
/// @param[in] isOddPeriod  Odd period (if interlaced)
void Timer::wait(const bool isWaiting, const bool isOddPeriod) noexcept
{
    // reset time reference
    if (m_isResetPending)
    {
        m_isResetPending = false;
        m_clock.reset();
        return;
    }

    // speed modifiers
    if (m_speedMode != speed_t::normal)
    {
        if (processSpeedAlteration() == false)
            return; // no wait
    }

    // apply frequency limit
    if (isWaiting)
    {
        m_lateTicks = m_clock.wait();
    }
    else
        m_clock.skip();

    // skipping
    if (m_periodsToSkip > 0) // if current period was already skipped, don't check time
        m_periodsToSkip--;
    else
    {
        if (m_skipMode != skipmode_t::disabled)
            setSkipping(isOddPeriod);
    }
}


/// @brief Set skipping for upcoming periods
/// @param[in] isOddPeriod  Odd period (if interlaced)
void Timer::setSkipping(const bool isOddPeriod) noexcept
{
    // standard skipping mode
    if (m_skipMode == skipmode_t::standard)
    {
        if (m_lateTicks >= m_timeout)
        {
            float periodDuration = m_clock.getPeriodDuration();
            if (periodDuration == 0.0f)
                periodDuration = 1000.0f / 59.94f;
            float latePeriods = static_cast<float>(m_lateTicks) / periodDuration;

            if (m_isInterlaced) // interlacing
            {
                if (isOddPeriod == false) // always skip 'even' period first
                {
                    // set skipping
                    m_periodsToSkip = static_cast<int32_t>(latePeriods);
                    if (m_periodsToSkip > MAX_SUCCESSIVE_SKIPPING) // keep at least 2 periods out of 6
                        m_periodsToSkip = MAX_SUCCESSIVE_SKIPPING;
                    else if (m_periodsToSkip % 2 != 0) // skip by groups of 2
                        ++m_periodsToSkip;
                }
            }
            else // not interlaced
            {
                latePeriods += 0.15f;
                m_periodsToSkip = static_cast<int32_t>(latePeriods);
                if (m_periodsToSkip >= MAX_SUCCESSIVE_SKIPPING) // keep at least 1 period out of 4
                    m_periodsToSkip = MAX_SUCCESSIVE_SKIPPING - 1;
            }
        }
    }
    // alternate mode - skip max one period out of two
    else if (m_skipMode == skipmode_t::alternate)
    {
        if (m_isInterlaced) // interlacing -> keep 2, skip 2
        {
            if (m_lateTicks > (m_timeout / 8.0f) && isOddPeriod == false)
                m_periodsToSkip = 2;
        }
        else if (m_lateTicks > (m_timeout / 16.0f)) // keep 1, skip 1
            m_periodsToSkip = 1;
    }
    m_lateTicks = static_cast<ticks_t>(0uL);
}

/// @brief Process temporary speed modification
/// @returns Continue time calculation (true) or exit function (false)
bool Timer::processSpeedAlteration() noexcept
{
    switch (m_speedMode)
    {
        // fast
        case speed_t::fast:
        {
            // skip 3 frames out of 4
            int nextPeriodsToSkip = 0;
            if (++m_speedIteration < 4)
                nextPeriodsToSkip = 1;
            else
                m_speedIteration = 0;

            // set period skipping
            if (m_periodsToSkip > 0) // current was skipped -> skip waiting too (don't limit frequency)
            {
                m_periodsToSkip = nextPeriodsToSkip; // set skipping flag
                return false;
            }
            else
                m_periodsToSkip = 1 + nextPeriodsToSkip; // +1 to bypass skipping calculations (will decrement)
            break;
        }
        // slow motion
        case speed_t::slow:
        {
            // double period duration
            m_clock.addCustomDuration(static_cast<ticks_t>(m_clock.getPeriodDuration()));
            m_periodsToSkip = 1; // +1 to bypass skipping calculations (will decrement)
            break;
        }
        // pause
        case speed_t::pause:
        {
            while (m_speedMode == speed_t::pause) // polling until resume event occurs
                std::this_thread::yield();
            m_clock.reset(); // avoid super-speed or skipping after resuming
            return false; break;
        }
        // unknown type -> restore default
        default: m_speedMode = speed_t::normal; break;
    }
    return true;
}
