/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : advanced tab control
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <unordered_map>
#include "common.h"
#include "dialog.h"
#include "tab_button.h"
#include "tab_page.h"

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @namespace config.dialog.controls
        /// Dialog controls
        namespace controls
        {
            /// @struct tab_association_t
            /// @brief Tab button/page association
            struct tab_association_t
            {
                std::shared_ptr<TabButton> button; ///< Tab selection button
                std::shared_ptr<TabPage> page;     ///< Page associated with tab

                tab_association_t() {}
                tab_association_t(const tab_association_t& copy)
                {
                    button = copy.button;
                    page = copy.page;
                }
            };
            
            
            /// @class TabControl
            /// @brief Advanced tab control
            class TabControl
            {
            public:
                static const float colorTop[3];
                static const float colorOffset[3];
                static const float colorTopBorder[3];
                static const float colorOffsetBorder[3];

            protected:
                std::unordered_map<int32_t, int32_t> m_resourceIdMap; ///< Map of resource ID for each tab ([ID] = page index)
                std::vector<tab_association_t> m_pages; ///< Collection of tab pages (buttons/pages)
                int32_t m_activePageIndex; ///< Index of active page
            private:
                int32_t m_offsetTabY; ///< Vertical offset for first tab
                int32_t m_width;  ///< Tab control width
                int32_t m_height; ///< Tab control height
                
            public:
                /// @brief Create tab control
                TabControl() : m_activePageIndex(-1), m_offsetTabY(2), m_width(0), m_height(0) {}

                /// @brief Remove all tabs
                void clear() noexcept
                {
                    m_activePageIndex = -1;
                    m_resourceIdMap.clear();
                    m_pages.clear();
                }
                /// @brief Insert new tab in tab control
                /// @param[in] data        Tab data (button / page)
                /// @param[in] tabButtonId  Tab button identifier
                void addTab(const tab_association_t& data, const int32_t tabButtonId) noexcept
                {
                    m_resourceIdMap[tabButtonId] = m_pages.size();
                    m_pages.push_back(tab_association_t(data));
                }
                /// @brief Check if active tab button has specified identifier
                /// @param[in] tabButtonId  Tab button identifier
                inline bool isActiveTab(const int32_t tabButtonId) noexcept
                {
                    return (m_resourceIdMap.count(tabButtonId) && m_resourceIdMap[tabButtonId] == m_activePageIndex);
                }


                /// @brief Create tab control in dialog
                /// @param[in] window  Window handle
                /// @param[in] offset  Vertical offset for tab control
                /// @param[in] width   Tab button width
                /// @returns Success
                bool create(window_handle_t window, const uint32_t offset, const uint32_t width);
                /// @brief Close control in dialog
                void close();

                /// @brief Define colors for a tab button
                /// @param[in]  tabNumber           Tab position
                /// @param[in]  isHover             Mouse hover
                /// @param[out] pColorTop           Vector (size = 3) to fill with top color
                /// @param[out] pColorOffset        Vector (size = 3) to fill with color offset
                /// @param[out] pColorTopBorder     Vector (size = 3) to fill with top border color
                /// @param[out] pColorOffsetBorder  Vector (size = 3) to fill with border color offset
                bool getTabButtonColors(uint32_t tabNumber, bool isHover, float* pColorTop, float* pColorOffset, float* pColorTopBorder, float* pColorOffsetBorder);


                // -- event handlers -- --------------------------------------------

                /// @brief Language change event
                /// @returns Validity
                bool onDialogConfirm(Dialog::event_args_t& args);
                /// @brief Language change event
                /// @param[in] instance  Library instance handle
                void onLanguageChange(Dialog::event_args_t& args);

                /// @brief Trigger control drawing
                EVENT_RETURN onPaint(Dialog::event_args_t& args);
                /// @brief Tab button command event handler
                EVENT_RETURN onCommand(Dialog::event_args_t& args);
            };
        }
    }
}