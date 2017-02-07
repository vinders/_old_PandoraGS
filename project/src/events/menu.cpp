/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : game menu
*******************************************************************************/
#include "../globals.h"
#include <cstdlib>
#include <cstdint>
#include <string>
#include <thread>
using namespace std::literals::string_literals;
#include "../config/config.h"
#include "menu.h"
using namespace events;


/// @brief Set status to "busy"
void Menu::lock()
{
    while (m_isBusy)
        std::this_thread::yield();
    m_isBusy = true;
}

/// @brief Set status to "available"
inline void Menu::unlock()
{
    m_isBusy = false;
}


/// @brief Set selected profile index and update profile name
/// @param[in] index  Index to select
void Menu::setSelectedIndex(const uint32_t index)
{
    lock();
    m_selectedIndex = index;
    std::wstring* pName = config::Config::getProfileName(m_selectedIndex);
    m_selectionName = (pName != nullptr) ? *pName : L"<undefined>"s;
    unlock();
}


// -- menu events management -- ----------------------------------------

/// @brief Show menu
/// @param[in] flag  Event flag (unused)
void Menu::show(int32_t flag)
{
    setSelectedIndex(config::Config::getCurrentProfileId());
    m_isVisible = true;
}

/// @brief Hide menu
/// @param[in] flag  Event flag (unused)
void Menu::hide(int32_t flag)
{
    m_isVisible = false;
}


/// @brief Change selection in menu
/// @param[in] flag  Type of change: default index (0), previous (< 0) or next (> 0)
void Menu::onIndexChange(int32_t flag)
{
    if (isVisible())
    {
        if (flag == 0)
            setSelectedIndex(0u);
        else
        {
            if (flag < 0)
                setSelectedIndex(config::Config::getPrevProfileId(m_selectedIndex));
            else
                setSelectedIndex(config::Config::getNextProfileId(m_selectedIndex));
        }
    }
}

/// @brief Confirm menu selection and apply
/// @param[in] flag  Event flag (unused)
void Menu::onAcceptChange(int32_t flag)
{
    if (isVisible())
        config::Config::useProfile(m_selectedIndex);
}
