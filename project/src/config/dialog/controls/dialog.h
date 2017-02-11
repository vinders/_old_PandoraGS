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
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#endif
#include "common.h"

#if _DIALOGAPI == DIALOGAPI_WIN32
#define DIALOG_EVENT_HANDLER_ARGUMENTS Dialog* pDialog, HWND hWindow, WPARAM wParam, LPARAM lParam
#define DIALOG_EVENT_RETURN INT_PTR
#else
#define DIALOG_EVENT_HANDLER_ARGUMENTS void* pDialog
#define DIALOG_EVENT_RETURN void*
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
            class Dialog;

            /// @struct dialog_event_handler_t
            /// @brief Event handling function
            struct dialog_event_handler_t
            {
                std::function<DIALOG_EVENT_RETURN(DIALOG_EVENT_HANDLER_ARGUMENTS)> handler;
            };
            /// @enum dialog_result_t
            /// @brief Action performed when closing a dialog
            enum class dialog_result_t : int32_t
            {
                error = -1,
                cancel = 0,
                confirm = 1
            };
            /// @struct paint_event_args_t
            /// @brief Drawing event arguments
            struct dialog_runtime_data_t
            {
                dialog_result_t dialogResult; ///< Dialog end result
                std::map<dialog_event_t, dialog_event_handler_t> registeredHandlers; ///< Registered event handlers
            };


            /// @class Dialog
            /// @brief Dialog control
            class Dialog
            {
            protected:
                library_instance_t m_instance; ///< Library instance handle
            private:
                dialog_runtime_data_t m_dialogData; ///< Modal dialog data
                static Dialog* s_dialogRefBuffer; ///< Dialog reference buffer (pass non-static data to static handler)


            public:
                /// @brief Create dialog box
                /// @param[in] instance  Current instance handle
                /// @throws invalid_argument  Invalid instance
                Dialog(library_instance_t instance);
                /// @brief Destroy dialog box
                virtual ~Dialog() {}

                /// @brief Display modal dialog box
                /// @param[in] dialogId        Dialog description identifier
                /// @param[in] isStyleEnabled  Enable enhanced visual style
                /// @returns Dialog result
                /// @throws runtime_error  Dialog creation error or runtime error
                dialog_result_t showDialog(const int32_t resourceId, const bool isStyleEnabled = false);


            protected:
                /// @brief Register a handler for an event
                /// @param[in] eventType  Event type
                /// @param[in] handler    Event handler
                inline void registerEvent(const dialog_event_t eventType, dialog_event_handler_t& handler) noexcept
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


                /// @brief Get event handler (no check)
                /// @param[in] eventType  Event type
                inline dialog_event_handler_t& getEventHandler(const dialog_event_t eventType)
                {
                    return m_dialogData.registeredHandlers[eventType];
                }
                /// @brief Set dialog result
                /// @param[in] result  Result value
                inline void setDialogResult(const dialog_result_t result) noexcept
                {
                    m_dialogData.dialogResult = result;
                }


            private:
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