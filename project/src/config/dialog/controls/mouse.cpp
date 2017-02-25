/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : mouse manager
*******************************************************************************/
#include "../../../globals.h"
#include <cstdint>
#include <thread>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#endif
#include "common.h"
#include "mouse.h"
using namespace config::dialog;
using namespace config::dialog::controls;


/// @brief Create mouse manager
/// @param[in] nPage  Current scrolling page
Mouse::Mouse(const uint32_t nPage) : m_isReady(true), m_accumulator(0), m_previousTime(0)
{
    #if _DIALOGAPI == DIALOGAPI_WIN32
    // get scrolling speed
    if (!SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &m_linesPerWheelDelta, 0))
        m_linesPerWheelDelta = 3;  // default

    // adjust speed
    if (m_linesPerWheelDelta == WHEEL_PAGESCROLL || m_linesPerWheelDelta > nPage)
        m_linesPerWheelDelta = nPage;
    #endif
}


/// @brief Get vertical scrolling value for a mouse wheel event
/// @param[in] delta    Mouse wheel delta value
/// @param[in] nPage    Current scrolling page
/// @returns Scroll value
int32_t Mouse::getVerticalWheelScroll(const int32_t delta, uint32_t nPage)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    if (m_linesPerWheelDelta == 0) // exit if scrolling disabled on system
        return 0;
    if (nPage < 1) // always scroll a little, even if less than a scrolling page
        nPage = 1;

    int32_t scrollValue = 0; // lines to scroll

    // get current time
    DWORD currentTime = GetTickCount();

    int32_t timeout = 2000;
    while (m_isReady == false && --timeout > 0)
        std::this_thread::yield();
    if (timeout <= 0)
        return 0;
    m_isReady = false; // lock

    if (currentTime - m_previousTime > GetDoubleClickTime() * 2 // long period of inactivity -> reset accumulator
     || (m_accumulator > 0) == (delta < 0))                     // scrolling direction reversed -> reset accumulator
    {
        m_accumulator = 0; 
    }

    // accumulate + scroll if enough
    m_accumulator += delta;
    scrollValue = (m_accumulator * static_cast<int32_t>(m_linesPerWheelDelta)) / WHEEL_DELTA;
    m_accumulator -= (scrollValue * WHEEL_DELTA) / static_cast<int32_t>(m_linesPerWheelDelta);
    // save time for next call
    m_previousTime = currentTime;

    m_isReady = true; // unlock
    return -scrollValue;
}
#else
{
    return 0;
}
#endif
