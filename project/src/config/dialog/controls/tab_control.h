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
                    m_pages.clear();
                }
                /// @brief Insert new tab in tab control
                /// @param[in] data  Tab data (button / page)
                void addTab(const tab_association_t& data) noexcept
                {
                    m_pages.push_back(tab_association_t(data));
                }


                /// @brief Create tab control in dialog
                /// @param[in] window  Window handle
                /// @param[in] offset  Vertical offset for tab control
                /// @param[in] width   Tab button width
                /// @returns Success
                bool create(window_handle_t window, const uint32_t offset, const uint32_t width);
                /// @brief Close control in dialog
                void close();


                // -- event handlers -- --------------------------------------------

                /// @brief Language change event
                /// @returns Validity
                bool onDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS);
                /// @brief Language change event
                /// @param[in] instance  Library instance handle
                void onLanguageChange(DIALOG_EVENT_HANDLER_ARGUMENTS);

                /// @brief Trigger control drawing
                DIALOG_EVENT_RETURN onPaint(DIALOG_EVENT_HANDLER_ARGUMENTS);
            };
        }
    }
}