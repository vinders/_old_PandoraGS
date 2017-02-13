/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : dialog control
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include <map>
#include <stack>
#include <functional>
#include "common.h"

#if _DIALOGAPI == DIALOGAPI_WIN32
#define DIALOG_EVENT_HANDLER_ARGUMENTS Dialog* pDialog, HWND hWindow, WPARAM wParam, LPARAM lParam
#define DIALOG_EVENT_HANDLER_ARGUMENTS_VALUES pDialog,hWindow,wParam,lParam
#define DIALOG_EVENT_RETURN INT_PTR
#define DIALOG_EVENT_RETURN_VALID (INT_PTR)TRUE
#define DIALOG_EVENT_RETURN_ERROR (INT_PTR)FALSE
#define getEventTargetControlId() LOWORD(wParam)
#define getEventActionType() static_cast<uint32_t>(HIWORD(wParam))
#else
#define DIALOG_EVENT_HANDLER_ARGUMENTS void* pDialog, int hWindow, int type
#define DIALOG_EVENT_HANDLER_ARGUMENTS_VALUES pDialog,hWindow,type
#define DIALOG_EVENT_RETURN int32_t
#define DIALOG_EVENT_RETURN_VALID 0
#define DIALOG_EVENT_RETURN_ERROR -1
#define getEventTargetControlId() (type&0x0FF)
#define getEventActionType() ((type>>8)&0x0FF)
#endif
#define getEventTargetDialogReference(TYPE) *((TYPE*)(pDialog))
#define getEventWindowHandle() reinterpret_cast<window_handle_t>(hWindow)
#define eventActionEquals(VAL) getEventActionType()==static_cast<uint32_t>(VAL)
#define DIALOG_USE_BASE_WINDOW INVALID_HANDLE_VALUE

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
            /// @class Dialog
            /// @brief Dialog control
            class Dialog
            {
            public:
                /// @struct dialog_event_handler_t
                /// @brief Event handling function
                struct event_handler_t
                {
                    std::function<DIALOG_EVENT_RETURN(DIALOG_EVENT_HANDLER_ARGUMENTS)> handler;
                };
                /// @enum result_t
                /// @brief Action performed when closing a dialog
                enum class result_t : int32_t
                {
                    error = -1,
                    cancel = 0,
                    confirm = 1
                };
                /// @struct runtime_data_t
                /// @brief Dialog execution data
                struct runtime_data_t
                {
                    bool isInitialized;
                    Dialog::result_t dialogResult; ///< Dialog end result
                    std::map<dialog_event_t, Dialog::event_handler_t> registeredHandlers; ///< Registered event handlers
                };


            protected:
                library_instance_t m_instance; ///< Library instance handle
            private:
                Dialog::runtime_data_t m_dialogData; ///< Modal dialog data
                static Dialog* s_dialogRefBuffer; ///< Dialog reference buffer (pass non-static data to static handler)


            public:
                /// @brief Create dialog box
                /// @param[in] instance  Current instance handle
                /// @throws invalid_argument  Invalid instance
                Dialog(library_instance_t instance);
                /// @brief Destroy dialog box
                virtual ~Dialog() {}

                /// @brief Display modal dialog box
                /// @param[in] resourceId      Dialog description identifier
                /// @param[in] hParentWindow   Parent window handle
                /// @param[in] isStyleEnabled  Enable enhanced visual style
                /// @returns Dialog result
                /// @throws runtime_error  Dialog creation error or runtime error
                Dialog::result_t showDialog(const int32_t resourceId, window_handle_t hParentWindow = reinterpret_cast<window_handle_t>(DIALOG_USE_BASE_WINDOW),
                                            const bool isStyleEnabled = false);


                /// @brief Register a handler for an event
                /// @param[in] eventType  Event type
                /// @param[in] handler    Event handler
                inline void registerEvent(const dialog_event_t eventType, Dialog::event_handler_t& handler) noexcept
                {
                    if (handler.handler != nullptr)
                        m_dialogData.registeredHandlers[eventType] = handler;
                }
                /// @brief Unregister handlers for an event
                /// @param[in] eventType  Event type
                inline void unregisterEvent(const dialog_event_t eventType) noexcept
                {
                    if (m_dialogData.registeredHandlers.count(eventType))
                        m_dialogData.registeredHandlers.erase(eventType);
                }
                /// @brief Check if handler exists for an event
                /// @param[in] eventType  Event type
                inline bool isRegisteredEvent(const dialog_event_t eventType) noexcept
                {
                    return (m_dialogData.registeredHandlers.count(eventType) && m_dialogData.registeredHandlers[eventType].handler != nullptr);
                }


            private:
                /// @brief Get event handler (no check)
                /// @param[in] eventType  Event type
                inline Dialog::event_handler_t& getEventHandler(const dialog_event_t eventType)
                {
                    return m_dialogData.registeredHandlers[eventType];
                }
                /// @brief Set dialog result
                /// @param[in] result  Result value
                inline void setDialogResult(const Dialog::result_t result) noexcept
                {
                    m_dialogData.dialogResult = result;
                }


                #if _DIALOGAPI == DIALOGAPI_WIN32
                /// @brief Get current dialog non-static reference
                /// @param[in] hWindow  Window handle
                static Dialog* getCurrentDialog(HWND hWindow);

                /// @brief Main dialog event handler
                /// @param[in] hWindow  Window handle
                /// @param[in] msg      Event message
                /// @param[in] wParam   Command
                /// @param[in] lParam   Informations
                /// @returns Event result
                static INT_PTR CALLBACK dialogEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
                #endif
            };
        }
    }
}