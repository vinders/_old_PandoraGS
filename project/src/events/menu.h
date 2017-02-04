/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : game menu
*******************************************************************************/
#pragma once

#include <cstdint>
#include <string>

/// @namespace events
/// Event management
namespace events
{
    /// @class Menu
    /// @brief Game menu
    class Menu
    {
    private:
        bool m_isVisible;             ///< Menu displayed or hidden
        uint32_t m_selectedIndex;     ///< Profile selection index in menu
        std::wstring m_selectionName; ///< Name to display for current selection
        bool m_isBusy;


    private:
        /// @brief Set status to "busy"
        void lock();
        /// @brief Set status to "available"
        inline void unlock();

    public:
        /// @brief Create game menu instance
        Menu() : m_isVisible(false), m_selectedIndex(0u) {}
        
        /// @brief Set selected profile index and update profile name
        /// @param[in] index  Index to select
        void setSelectedIndex(const uint32_t index);
        /// @brief Get displayed name for selected index
        /// @param[out] outName  Formatted profile name
        inline void getSelectedIndex(std::wstring& outName)
        {
            lock();
            outName = std::to_wstring(m_selectedIndex) + L" - "s + m_selectionName;
            unlock();
        }
        /// @brief Get menu display status
        /// @returns Visible (true) or hidden (false)
        inline bool isVisible() const
        {
            return m_isVisible;
        }

        
        // -- menu events management -- ----------------------------------------
        
        /// @brief Show menu
        /// @param[in] flag  Event flag (unused)
        void show(int32_t flag = 0);
        /// @brief Hide menu
        /// @param[in] flag  Event flag (unused)
        void hide(int32_t flag = 0);
        
        /// @brief Change selection in menu
        /// @param[in] flag  Type of change: default index (0), previous (< 0) or next (> 0)
        void onIndexChange(int32_t flag);
        /// @brief Confirm menu selection and apply
        /// @param[in] flag  Event flag (unused)
        void onAcceptChange(int32_t flag);
    };
}
