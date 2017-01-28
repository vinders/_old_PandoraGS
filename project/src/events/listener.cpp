/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : user & system input listener
*******************************************************************************/
#ifdef _WINDOWS
#include <Windows.h>
#endif
#include "listener.h"
using namespace events;

#ifdef _WINDOWS
// default window event listener
HWND Listener::s_hWindow;                 ///< Target window handle
WNDPROC Listener::s_pDefaultListener = 0; ///< Default event listener
#endif

// listener settings
bool Listener::s_isActive;                                 ///< General event listener active or not
bool Listener::s_isScreenSaverDisabled;                    ///< Disable screen-saver messages
char Listener::s_pTriggerKeys[EVENT_KEYS_STRING_LENGTH];   ///< Key/trigger association array
event_handler_t Listener::s_pHandlers[EVENT_ARRAY_LENGTH]; ///< Handler/trigger association array

// listener status
bool Listener::s_isLocked = false; ///< Listener locked (busy or temporarily disabled)
bool Listener::s_isPaused = false; ///< Game is currently paused


#ifdef _WINDOWS
/// @brief Start event listener
/// @param hWindow Managed window handle
/// @param isScnSvDisabled Screen-saver disabled or not
/// @param pTriggerKeys Key/trigger association array
void Listener::start(HWND hWindow, bool isScnSvDisabled, char* pTriggerKeys)
{
    if (s_isActive)
        return;

    // set key / control association
    memcpy(s_pTriggerKeys, pTriggerKeys, EVENT_KEYS_STRING_LENGTH);
    memset(s_pHandlers, 0x0, EVENT_ARRAY_LENGTH);

    // start event listener + save original handler
    if (!s_pDefaultListener)
    {
        s_hWindow = hWindow;
        s_pDefaultListener = (WNDPROC)GetWindowLong(hWindow, GWL_WNDPROC);
        SetWindowLong(hWindow, GWL_WNDPROC, (LONG)listen);
    }

    // set listener status
    s_isScreenSaverDisabled = isScnSvDisabled;
    s_isLocked = false;
    s_isPaused = false;
    s_isActive = true;
}
#else
void InputReader::start(char* pTriggerKeys)
{
    if (m_isActive)
        return;

    // set key / control association
    memcpy(s_pTriggerKeys, pTriggerKeys, EVENT_ARRAY_LENGTH);

    // set listener status
    s_isScreenSaverDisabled = isScnSvDisabled;
    s_isLocked = false;
    s_isPaused = false;
    s_isActive = true;
}
#endif

/// @brief Stop event listener
void Listener::stop()
{
    if (s_isActive)
    {
        #ifdef _WINDOWS
        // restore original event listener
        if (s_pDefaultListener)
            SetWindowLong(s_hWindow, GWL_WNDPROC, (LONG)s_pDefaultListener);
        s_pDefaultListener = 0;
        #endif
        s_isActive = false;
        s_isPaused = false;
        s_isLocked = false;
    }
}

#ifdef _WINDOWS
/// @brief Handle events in Windows
/// @param hWindow Managed window handle
/// @param eventType Event type
/// @param wpCode Command code
/// @param lpInfo Additional information
LRESULT CALLBACK Listener::listen(HWND hWindow, UINT eventType, WPARAM wpCode, LPARAM lpInfo)
{
    if (s_isLocked == false)
    {
        switch (eventType)
        {
            // system commands
            case WM_SYSCOMMAND:
            {
                if (wpCode == SC_SCREENSAVE && s_isScreenSaverDisabled) // screensaver
                    return 0L; // disable
                break;
            }
            // key pressed
            case WM_KEYDOWN:
            {
                if (wpCode == (WPARAM)(s_pTriggerKeys[static_cast<uint32_t>(event_trigger_t::fastForward)])) // fast-forward
                {
                    if (s_pHandlers[static_cast<uint32_t>(event_trigger_t::fastForward)] && !s_isPaused)
                        s_pHandlers[static_cast<uint32_t>(event_trigger_t::fastForward)](1);
                }
                break;
            }
            // key released
            case WM_SYSKEYUP:
            {
                if (wpCode == VK_RETURN) // window mode
                {
                    if (s_pHandlers[static_cast<uint32_t>(event_trigger_t::windowMode)] && !s_isPaused)
                        s_pHandlers[static_cast<uint32_t>(event_trigger_t::windowMode)](0);
                }
                break;
            }
            case WM_KEYUP:
            {
                // pause
                if (wpCode == (WPARAM)(s_pTriggerKeys[static_cast<uint32_t>(event_trigger_t::pause)])) // pause
                {
                    s_isPaused = (s_isPaused == false);
                    if (s_pHandlers[static_cast<uint32_t>(event_trigger_t::pause)])
                        s_pHandlers[static_cast<uint32_t>(event_trigger_t::pause)]((int32_t)s_isPaused);
                    break;
                }
                if (s_isPaused) // if paused, ignore other commands
                    break;

                // speed modifications
                if (wpCode == (WPARAM)(s_pTriggerKeys[static_cast<uint32_t>(event_trigger_t::fastForward)]))
                {
                    if (s_pHandlers[static_cast<uint32_t>(event_trigger_t::fastForward)])
                        s_pHandlers[static_cast<uint32_t>(event_trigger_t::fastForward)](0);
                    break;
                }
                if (wpCode == (WPARAM)(s_pTriggerKeys[static_cast<uint32_t>(event_trigger_t::slowMotion)]))
                {
                    if (s_pHandlers[static_cast<uint32_t>(event_trigger_t::slowMotion)])
                        s_pHandlers[static_cast<uint32_t>(event_trigger_t::slowMotion)](0);
                    break;
                }

                // profile management
                if (wpCode == (WPARAM)(s_pTriggerKeys[static_cast<uint32_t>(event_trigger_t::menuToggle)])) // show/hide menu
                {
                    if (s_pHandlers[static_cast<uint32_t>(event_trigger_t::menuToggle)])
                        s_pHandlers[static_cast<uint32_t>(event_trigger_t::menuToggle)](0);
                    break;
                }
                if (wpCode == (WPARAM)(s_pTriggerKeys[static_cast<uint32_t>(event_trigger_t::menuAccept)])) // change profile
                {
                    if (s_pHandlers[static_cast<uint32_t>(event_trigger_t::menuAccept)])
                        s_pHandlers[static_cast<uint32_t>(event_trigger_t::menuAccept)](2);
                    break;
                }
                if (wpCode == (WPARAM)(s_pTriggerKeys[static_cast<uint32_t>(event_trigger_t::menuDefault)]))
                {
                    if (s_pHandlers[static_cast<uint32_t>(event_trigger_t::menuDefault)])
                        s_pHandlers[static_cast<uint32_t>(event_trigger_t::menuDefault)](0);
                    break;
                }
                if (wpCode == (WPARAM)(s_pTriggerKeys[static_cast<uint32_t>(event_trigger_t::menuPrev)]))
                {
                    if (s_pHandlers[static_cast<uint32_t>(event_trigger_t::menuPrev)])
                        s_pHandlers[static_cast<uint32_t>(event_trigger_t::menuPrev)](-1);
                    break;
                }
                if (wpCode == (WPARAM)(s_pTriggerKeys[static_cast<uint32_t>(event_trigger_t::menuNext)]))
                {
                    if (s_pHandlers[static_cast<uint32_t>(event_trigger_t::menuNext)])
                        s_pHandlers[static_cast<uint32_t>(event_trigger_t::menuNext)](1);
                    break;
                }

                // miscellaneous
                if (wpCode == (WPARAM)(s_pTriggerKeys[static_cast<uint32_t>(event_trigger_t::fps)])) // fps display
                {
                    if (s_pHandlers[static_cast<uint32_t>(event_trigger_t::fps)])
                        s_pHandlers[static_cast<uint32_t>(event_trigger_t::fps)](0);
                    break;
                }
                if (wpCode == (WPARAM)(s_pTriggerKeys[static_cast<uint32_t>(event_trigger_t::ratioMode)])) // screen ratio mode
                {
                    if (s_pHandlers[static_cast<uint32_t>(event_trigger_t::ratioMode)])
                        s_pHandlers[static_cast<uint32_t>(event_trigger_t::ratioMode)](0);
                    break;
                }
            }
            // window position/size/state changed
            case WM_WINDOWPOSCHANGED:
            {
                // only in window mode
                if (s_pHandlers[static_cast<uint32_t>(event_trigger_t::windowSize)])
                {
                    lock();
                    if (wpCode == SIZE_MAXIMIZED || wpCode == SIZE_RESTORED) // not minimized
                    {
                        static WPARAM prevState = wpCode;
                        if (prevState != wpCode) // different window state
                        {
                            prevState = wpCode;
                            s_pHandlers[static_cast<uint32_t>(event_trigger_t::windowSize)](0);
                        }
                        else // window resized
                        {
                            WINDOWPOS* winpos = (WINDOWPOS*)lpInfo;
                            static int prevCx = winpos->cx;
                            static int prevCy = winpos->cy;
                            if (prevCx != winpos->cx || prevCy != winpos->cy) // different size
                            {
                                prevCx = winpos->cx;
                                prevCy = winpos->cy;
                                s_pHandlers[static_cast<uint32_t>(event_trigger_t::windowSize)](prevCx + (prevCy << 16));
                            }
                        }
                    }
                    unlock();
                }
                break;
            }
        }
    }
    // default: transmit event to emulator
    return s_pDefaultListener(hWindow, eventType, wpCode, lpInfo);
}

#else
/// @brief Handle events in Linux
/// @param keycode">Pressed key code
void InputManager::GPUkeypressed(int keycode)
{
    switch (keycode)
    {
    default: //...
        break;
    }
}
#endif
