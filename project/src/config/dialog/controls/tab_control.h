/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : advanced tab control
*******************************************************************************/
#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "common.h"
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
            protected:
                std::vector<tab_association_t> m_pages; ///< Collection of tab pages (buttons/pages)
                int32_t m_activePageIndex; ///< Index of active page
                
            public:
                /// @brief Create tab control
                TabControl() : m_activePageIndex(-1) {}

                /// @brief Remove all tabs
                void clear() noexcept
                {
                    m_activePageIndex = -1;
                    m_pages.clear();
                }
                /// @brief Insert new tab in tab control
                /// @param[in] data  Tab data (button / page)
                void addTab(const tab_association_t& data) noexcept
                {
                    m_pages.push_back(tab_association_t(data));
                }


                /// @brief Create tab control in dialog
                /// @param[in] window      Window handle
                /// @param[in] offset      Vertical offset for first tab button
                /// @param[in] width       Tab button width
                void create(window_handle_t window, const uint32_t offset, const uint32_t width);

                /// @brief Trigger control drawing
                /// @param[in] args  Event arguments
                void paint(paint_event_args_t args);
            };
        }
    }
}