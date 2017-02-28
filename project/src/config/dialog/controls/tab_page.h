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
                /// @struct event_args_t
                /// @brief Dialog/page event arguments
                struct event_args_t
                {
                private:
                    const TabPage* pParent; ///< Parent page
                public:
                    const window_handle_t window; ///< Parent window handle
                    uint32_t eventData;   ///< Raw event data
                    int32_t notifierData; ///< Notification data

                    template<typename T> T& getParent() { return *((T*)pParent); } ///< Cast parent page
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    inline uint32_t actionType() noexcept { return static_cast<uint32_t>(HIWORD(eventData)); }      ///< Extract action type
                    inline uint32_t actionSignedType() noexcept { return static_cast<int32_t>(HIWORD(eventData)); } ///< Extract action type (signed)
                    inline int32_t  controlId() noexcept { return static_cast<int32_t>(LOWORD(eventData)); }        ///< Extract affected control ID
                    #else
                    inline uint32_t actionType() noexcept { return ((eventData >> 8) & 0x0FF); } ///< Extract action type
                    inline uint32_t actionSignedType() noexcept { return (eventData & 0x0FF); }  ///< Extract action type (signed)
                    inline int32_t  controlId() noexcept { return (eventData & 0x0FF); }         ///< Extract affected control ID
                    #endif

                    template <typename EventType>
                    bool isEventAction(EventType type) { return (actionType() == static_cast<uint32_t>(type)); } ///< Compare action with event type
                    template <typename EventType>
                    bool isEventSignedAction(EventType type) { return (actionSignedType() == static_cast<int32_t>(type)); } ///< Compare action with event type (signed)
                    template <typename EventType>
                    bool isEventSubAction(EventType type) { return (controlId() == static_cast<uint32_t>(type)); } ///< Compare sub-action with event type

                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    /// @brief Initialization constructor
                    event_args_t(TabPage* pPage, HWND hWindow, WPARAM wParam, LPARAM lParam)
                        : pParent(pPage), window(hWindow), eventData(static_cast<uint32_t>(wParam)), notifierData(static_cast<uint32_t>(lParam)) {}
                    /// @brief Copy constructor
                    event_args_t(const TabPage::event_args_t& copy)
                        : pParent(copy.pParent), window(copy.window), eventData(copy.eventData), notifierData(copy.notifierData) {}
                    #endif
                };
                /// @struct page_event_handler_t
                /// @brief Event handling function
                struct event_handler_t
                {
                    std::function<EVENT_RETURN(TabPage::event_args_t)> handler;
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
                /// @param[in] resourceId     Tab page description identifier
                TabPage(library_instance_t instance, Dialog* pParentDialog, const int32_t resourceId)
                    : m_instance(instance), m_pParentDialog(pParentDialog), m_pageResourceId(resourceId), m_isInitialized(false), m_hPage(0) {}
                /// @brief Destroy tab page
                virtual ~TabPage()
                {
                    close();
                }

                /// @brief Create tab page control
                /// @param[in] hWindow    Parent window handle
                /// @param[in] offset     Page horizontal offset
                /// @param[in] isSubPage  Page is a sub-page of another page
                /// @returns Dialog result
                bool create(window_handle_t hWindow, const uint32_t offset, const bool isSubPage = false);
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
            public:
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
                virtual bool onDialogConfirm(Dialog::event_args_t& args) = 0;
                /// @brief Language change event
                /// @param[in] isRecursive    Also translate controls in child pages or not
                virtual void onLanguageChange(const bool isRecursive) = 0;
                /// @brief Profile change event
                /// @param[in] isUpdate  Set to false to initialize controls
                virtual void onProfileChange(const bool isUpdate) {}
            };
        }
    }
}