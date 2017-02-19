/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : page for tab control - abstract class
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include <unordered_map>
#include "common.h"
#include "dialog.h"

#define DIALOG_BUTTON_BAR_HEIGHT 43 // OK/Cancel bar size

#if _DIALOGAPI == DIALOGAPI_WIN32
#define PAGE_EVENT_HANDLER_ARGUMENTS TabPage* pPage, HWND hWindow, WPARAM wParam, LPARAM lParam
#define getEventTargetPageReference(TYPE) *(static_cast<TYPE*>(pPage))
#else
#define PAGE_EVENT_HANDLER_ARGUMENTS void* pPage
#define getEventTargetPageReference(TYPE) *((TYPE*)pPage)
#endif

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
            /// @class TabPage
            /// @brief Page for tab control - abstract class
            class TabPage
            {
            public:
                /// @struct page_event_handler_t
                /// @brief Event handling function
                struct event_handler_t
                {
                    std::function<DIALOG_EVENT_RETURN(PAGE_EVENT_HANDLER_ARGUMENTS)> handler;
                };


            protected:
                library_instance_t m_instance; ///< Library instance handle
                int32_t m_pageResourceId; ///< Page resource identifier
                window_handle_t m_hPage;  ///< Page control handle
            private:
                Dialog* m_pParentDialog;  ///< Parent dialog reference
                bool m_isInitialized;
                std::unordered_map<dialog_event_t, TabPage::event_handler_t> m_registeredHandlers; ///< Registered event handlers
                static TabPage* s_pageRefBuffer; ///< Page reference buffer (pass non-static data to static handler)


            public:
                /// @brief Create tab page
                /// @param[in] instance       Current instance handle
                /// @param[in] pParentDialog  Parent dialog reference
                /// @param[in] resourceId     Dialog description identifier
                TabPage(library_instance_t instance, Dialog* pParentDialog, const int32_t resourceId)
                    : m_instance(instance), m_pParentDialog(pParentDialog), m_pageResourceId(resourceId), m_isInitialized(false), m_hPage(0) {}
                /// @brief Destroy tab page
                virtual ~TabPage()
                {
                    close();
                }

                /// @brief Create tab page control
                /// @param[in] hWindow  Parent window handle
                /// @param[in] offset   Page horizontal offset
                /// @returns Dialog result
                bool create(window_handle_t hWindow, const uint32_t offset);
                /// @brief Close tab page control
                void close();
                /// @brief Close tab page control - overridable method
                virtual void overridableClose() {}


            public:
                /// @brief Set label visibility
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Label resource identifier
                /// @param[in] isVisible   Visible or hidden
                inline void setVisible(bool isVisible)
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    if (m_hPage && reinterpret_cast<HWND>(m_hPage) != reinterpret_cast<HWND>(INVALID_HANDLE_VALUE))
                    {
                        ShowWindow(m_hPage, (isVisible) ? SW_SHOW : SW_HIDE);
                        EnableWindow(m_hPage, (isVisible) ? TRUE : FALSE);
                    }
                    #else
                    //...
                    #endif
                }


            protected:
                /// @brief Register a handler for an event
                /// @param[in] eventType  Event type
                /// @param[in] handler    Event handler
                inline void registerEvent(const dialog_event_t eventType, TabPage::event_handler_t& handler) noexcept
                {
                    if (handler.handler != nullptr)
                        m_registeredHandlers[eventType] = handler;
                }
                /// @brief Unregister handlers for an event
                /// @param[in] eventType  Event type
                inline void unregisterEvent(const dialog_event_t eventType) noexcept
                {
                    if (m_registeredHandlers.count(eventType))
                        m_registeredHandlers.erase(eventType);
                }
                /// @brief Check if handler exists for an event
                /// @param[in] eventType  Event type
                inline bool isRegisteredEvent(const dialog_event_t eventType) noexcept
                {
                    return (m_registeredHandlers.count(eventType) && m_registeredHandlers[eventType].handler != nullptr);
                }


            private:
                /// @brief Get event handler (no check)
                /// @param[in] eventType  Event type
                inline TabPage::event_handler_t& getEventHandler(const dialog_event_t eventType)
                {
                    return m_registeredHandlers[eventType];
                }
                /// @brief Mark page as initialized
                void setInitialized() noexcept
                {
                    m_isInitialized = true;
                }
                /// @brief Check if page is initialized
                /// @returns Init status
                bool isInitialized() const noexcept
                {
                    return m_isInitialized;
                }
            protected:
                /// @brief Get page handle
                /// @returns Page handle
                template<typename Subclass>
                Subclass* getParentDialog() noexcept
                {
                    return (Subclass*)m_pParentDialog;
                }
                /// @brief Get page handle
                /// @returns Page handle
                inline window_handle_t getPageHandle() noexcept
                {
                    return m_hPage;
                }


                // -- event management -- ------------------------------------------

            private:
                #if _DIALOGAPI == DIALOGAPI_WIN32
                /// @brief Get current dialog non-static reference
                /// @param[in] hWindow  Window handle
                static TabPage* TabPage::getCurrentPage(HWND hWindow);

                /// @brief Tab page event handler
                /// @param[in] hWindow  Window handle
                /// @param[in] msg      Event message
                /// @param[in] wParam   Command
                /// @param[in] lParam   Informations
                /// @returns Event result
                static INT_PTR CALLBACK pageEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
                #endif

            public:
                // -- specialized handlers -- --------------------------------------

                /// @brief Language change event
                /// @returns Validity
                virtual bool onDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS) = 0;
                /// @brief Language change event
                /// @param[in] isRecursive    Also translate controls in child pages or not
                virtual void onLanguageChange(const bool isRecursive) = 0;
            };
        }
    }
}