/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : scrollable page for tab control
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include <memory>
#include "common.h"
#include "mouse.h"
#include "tab_page.h"

#define SCROLLABLETABPAGE_SCROLLUNIT 16

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
            /// @class ScrollableTabPage
            /// @brief Scrollable page for tab control
            class ScrollableTabPage : public TabPage
            {
            private:
                std::shared_ptr<Mouse> m_mouseManager; ///< Mouse wheel manager
                uint32_t m_maxRange; ///< Max scrolling range

            public:
                /// @brief Create tab page - profile compatibility settings
                /// @param[in] instance       Current instance handle
                /// @param[in] pParentDialog  Parent dialog reference
                /// @param[in] resourceId     Tab page description identifier
                /// @param[in] maxRange       Max scroll range
                ScrollableTabPage(controls::library_instance_t instance, controls::Dialog* pParentDialog, const int32_t resourceId, const uint32_t maxRange);
                // @brief Destroy tab page - profile compatibility settings
                virtual ~ScrollableTabPage() {}


            private:
                /// @brief Create tab page - profile compatibility settings
                /// @param[in] hWindow  Page handle
                /// @param[in] pos      Desired position
                /// @param[in] prevPos  Previous position
                static inline void setScrollPosition(window_handle_t hWindow, int32_t pos, const int32_t prevPos)
                {
                    if (pos != prevPos)
                    {
                        #if _DIALOGAPI == DIALOGAPI_WIN32
                        // set scrollbar
                        SetScrollPos(reinterpret_cast<HWND>(hWindow), SB_VERT, pos, TRUE);
                        pos = GetScrollPos(reinterpret_cast<HWND>(hWindow), SB_VERT); // get real position
                        // scroll page
                        ScrollWindowEx(reinterpret_cast<HWND>(hWindow), 0, (prevPos - pos) * SCROLLABLETABPAGE_SCROLLUNIT, 
                                       NULL, NULL, NULL, NULL, SW_ERASE | SW_INVALIDATE | SW_SCROLLCHILDREN);
                        #endif
                    }
                }


                // -- event handlers -- --------------------------------------------

                /// @brief Initialization event handler
                static DIALOG_EVENT_RETURN onInit(PAGE_EVENT_HANDLER_ARGUMENTS);
                /// @brief Initialization extendable event handler
                virtual DIALOG_EVENT_RETURN onInitOverridable(PAGE_EVENT_HANDLER_ARGUMENTS) = 0;

                /// @brief Vertical scroll event handler
                static DIALOG_EVENT_RETURN onVerticalScroll(PAGE_EVENT_HANDLER_ARGUMENTS);
                /// @brief Vertical mouse wheel event handler
                static DIALOG_EVENT_RETURN onMouseWheel(PAGE_EVENT_HANDLER_ARGUMENTS);
            };
        }
    }
}