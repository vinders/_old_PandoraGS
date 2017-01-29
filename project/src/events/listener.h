/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : user & system input listener
*******************************************************************************/
#pragma once

#include <cstdint>
#ifdef _WINDOWS
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

// virtual keys
#define VK_NOKEY  0x07
#ifndef _WINDOWS
#define VK_INSERT 65379
#define VK_HOME   65360
#define VK_PRIOR  65365
#define VK_NEXT   65366
#define VK_END    65367
#define VK_DEL    65535
#define VK_F5     65474
#endif


/// @namespace events
/// Event management
namespace events
{
    /// @brief Menu index number
    typedef int32_t menu_index_t;

    /// @enum event_trigger_t
    /// @brief Menu/game triggered event
    enum class event_trigger_t : uint32_t
    {
        menuToggle = 0u,  ///< Menu on/off
        menuPrev = 1u,    ///< Menu shows prev profile
        menuNext = 2u,    ///< Menu shows next profile
        menuDefault = 3u, ///< Menu shows default profile
        menuAccept = 4u,  ///< Apply selected profile
        ratioMode = 5u,   ///< Next stretching mode
        fastForward = 6u, ///< Fast mode while pressed
        slowMotion = 7u,  ///< Slow mode on/off
        pause = 8u,       ///< Pause game on/off
        fps = 9u,         ///< FPS display on/off
        windowMode = 10u, ///< Fullscreen/window mode (no key configurable)
        windowSize = 11u  ///< Window resized (no key configurable)
    };
    #define EVENT_ARRAY_LENGTH       12 // all events: 0-11
    #define EVENT_KEYS_STRING_LENGTH 11 // configurable events: 0-9 + end character

    // @brief Event handler
    typedef void(*event_handler_t)(int32_t);


    /// @class Listener
    /// @brief User & system input listener
    class Listener
    {
    private:
        #ifdef _WINDOWS
        // default window event listener
        static HWND s_hWindow;             ///< Target window handle
        static WNDPROC s_pDefaultListener; ///< Default event listener
        #endif

        // listener settings
        static bool s_isActive;                                 ///< General event listener active or not
        static bool s_isScreenSaverDisabled;                    ///< Disable screen-saver messages
        static char s_pTriggerKeys[EVENT_KEYS_STRING_LENGTH];   ///< Key/trigger association array
        static event_handler_t s_pHandlers[EVENT_ARRAY_LENGTH]; ///< Handler/trigger association array

        // listener status
        static bool s_isLocked; ///< Listener locked (busy or temporarily disabled)
        static bool s_isPaused; ///< Game is currently paused


    public:
        #ifdef _WINDOWS
        /// @brief Start event listener
        /// @param hWindow Managed window handle
        /// @param isScnSvDisabled Screen-saver disabled or not
        /// @param pTriggerKeys Key/trigger association array
        static void start(HWND hWindow, bool isScnSvDisabled, char* pTriggerKeys);
        #else
        static void start(char* pTriggerKeys);
        #endif

        /// @brief Stop event listener
        static void stop();

        #ifdef _WINDOWS
        /// @brief Handle events in Windows
        /// @param hWindow Managed window handle
        /// @param eventType Event type
        /// @param wpCode Command code
        /// @param lpInfo Additional information
        static LRESULT CALLBACK listen(HWND hWindow, UINT eventType, WPARAM wpCode, LPARAM lpInfo);
        #else
        /// @brief Handle events in Linux
        /// @param keycode">Pressed key code
        static void GPUkeypressed(int keycode);
        #endif

        /// @brief Set handler for an event
        /// @param eventId Event type
        /// @param handler Pointer to event handler
        static void registerEvent(event_trigger_t eventId, event_handler_t handler)
        {
            s_pHandlers[static_cast<uint32_t>(eventId)] = handler;
        }

        /// @brief Disable handling of an event
        /// @param eventId Event type
        static void disableEvent(event_trigger_t eventId)
        {
            s_pHandlers[static_cast<uint32_t>(eventId)] = NULL;
        }


        // -- lock -- ----------------------------------------------------------

        /// @brief Lock listener
        static inline void lock()
        {
            s_isLocked = true;
        }

        /// @brief Unlock listener
        static inline void unlock()
        {
            s_isLocked = false;
        }

        /// @brief Get lock status
        /// @return Lock status
        static inline bool isLocked()
        {
            return s_isLocked;
        }

        /// @brief Get pause status
        /// @return Pause status
        static inline bool isPaused()
        {
            return s_isPaused;
        }
    };
}
