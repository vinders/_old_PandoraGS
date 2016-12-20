/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   input_reader.cpp
Description : user input management
*******************************************************************************/
#ifdef _WINDOWS
#include <Windows.h>
#endif
using namespace std;
#include "input_reader.h"

#ifdef _WINDOWS
// default window event handlers
static HWND g_hEmuWindow;
static WNDPROC  g_pWindowHandler = 0;
#endif
bool InputReader::s_isActive = false;     // input reader active or not
bool InputReader::s_isSizeActive = false; // window size reader active or not
bool InputReader::s_isLocked = false; // input lock
bool InputReader::s_isScnSvDisabled = false; // disable screen-saver messages
char InputReader::s_pControlKeys[CTRLKEYS_LENGTH]; // key / control association array

uint32_t InputReader::s_events = 0u; // triggered events indicator
uint32_t InputReader::s_status = 0u; // special status indicator
speed_t InputReader::s_speedStatus = Speed_normal; // speed status indicator
menu_t InputReader::s_menuIndex = 0; // current profile menu index
menu_t InputReader::s_activeIndex = 0;// profile currently in use
menu_t InputReader::s_maxIndex = 0;   // max profile index

#ifdef _WINDOWS
/// <summary>Start input reader</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="pCtrlKeys">Array with each control key ID</param>
/// <param name="maxMenu">Maximum menu index</param>
/// <param name="isFullscreen">Fullscreen / window mode</param>
/// <param name="isScnSvDisabled">Screen-saver disabled or not</param>
void InputReader::start(HWND hWindow, char* pCtrlKeys, menu_t maxMenu, bool isFullscreen, bool isScnSvDisabled)
{
    if (s_isActive)
        return;

    // key / control association
    for (int i = 0; i < CTRLKEYS_LENGTH; ++i)
        s_pControlKeys[i] = pCtrlKeys[i];

    // start event listener + save original handler
    if (!g_pWindowHandler)
    {
        g_hEmuWindow = hWindow;
        g_pWindowHandler = (WNDPROC)GetWindowLong(hWindow, GWL_WNDPROC);
        SetWindowLong(hWindow, GWL_WNDPROC, (LONG)listen);
    }
    resetEvents();
    resetStatus();
    s_speedStatus = Speed_normal;
    s_maxIndex = maxMenu;
    s_isActive = true;
    s_isSizeActive = (isFullscreen == false);
    s_isScnSvDisabled = isScnSvDisabled;
    s_isLocked = false;
}
#else
void InputReader::start(char* pCtrlKeys, menu_t maxMenu, bool isFullscreen)
{
    if (m_isActive)
        return;

    // key / control association
    for (int i = 0; i < CTRLKEYS_LENGTH; ++i)
        s_pControlKeys[i] = pCtrlKeys[i];

    // initialize event listener
    resetEvents();
    resetStatus();
    s_maxIndex = maxMenu;
    s_isActive = true;
    s_isSizeActive = (isFullscreen == false);
}
#endif

/// <summary>Stop input reader</summary>
void InputReader::stop()
{
    if (s_isActive)
    {
        #ifdef _WINDOWS
        // restore original event listener
        if (g_pWindowHandler)
            SetWindowLong(g_hEmuWindow, GWL_WNDPROC, (LONG)g_pWindowHandler);
        g_pWindowHandler = 0;
        #endif
        s_isActive = false;
    }
    s_isSizeActive = false;

    // default values
    resetEvents();
    resetStatus();
    s_speedStatus = Speed_normal;
}

#ifdef _WINDOWS
/// <summary>Handle keyboard in Windows</summary>
/// <param name="hWindow">Window handler</param>
/// <param name="eventType">Event type</param>
/// <param name="wpCode">Command code</param>
/// <param name="lpInfo">Additional information</param>
LRESULT CALLBACK InputReader::listen(HWND hWindow, UINT eventType, WPARAM wpCode, LPARAM lpInfo)
{
    if (s_isLocked == false)
    {
        switch (eventType)
        {
            // system commands
            case WM_SYSCOMMAND:
            {
                if (wpCode == SC_SCREENSAVE && s_isScnSvDisabled) // screensaver
                    return 0L; // disable
                break;
            }
            // key pressed
            case WM_KEYDOWN:
            {
                if (wpCode == (WPARAM)(s_pControlKeys[Ctrlkey_fastForward])) // fast-forward
                {
                    s_speedStatus = Speed_fast;
                }
                break;
            }
            // key released
            case WM_SYSKEYUP:
            {
                if (wpCode == VK_RETURN && getEvents(EVT_WINDOWMODE) == false) // window mode
                    setEvents(EVT_WINDOWMODE | EVT_WINDOWSIZE);
                break;
            }
            case WM_KEYUP:
            {
                // speed modifications
                if (wpCode == (WPARAM)(s_pControlKeys[Ctrlkey_fastForward]))
                {
                    s_speedStatus = Speed_normal;
                    break;
                }
                if (wpCode == (WPARAM)(s_pControlKeys[Ctrlkey_slowMotion]))
                {
                    s_speedStatus = (s_speedStatus == Speed_normal) ? Speed_slow : Speed_normal;
                    break;
                }

                // profile management
                if (wpCode == (WPARAM)(s_pControlKeys[Ctrlkey_menuToggle])) // show/hide menu
                {
                    if (getStatus(ST_PAUSE) == false)
                    {
                        toggleStatus(ST_MENUVISIBLE);
                        if (getStatus(ST_MENUVISIBLE) == false)
                            s_menuIndex = s_activeIndex;
                    }
                    break;
                }
                if (wpCode == (WPARAM)(s_pControlKeys[Ctrlkey_menuAccept])) // change profile
                {
                    if (getStatus(ST_MENUVISIBLE) && s_menuIndex != s_activeIndex)
                    {
                        s_activeIndex = s_menuIndex;
                        setEvents(EVT_PROFILECHANGE);
                    }
                    break;
                }
                if (wpCode == (WPARAM)(s_pControlKeys[Ctrlkey_menuDefault]))
                {
                    if (getStatus(ST_MENUVISIBLE))
                        s_menuIndex = 0;
                    break;
                }
                if (wpCode == (WPARAM)(s_pControlKeys[Ctrlkey_menuPrev]))
                {
                    if (getStatus(ST_MENUVISIBLE))
                    {
                        menu_t newIndex = (s_menuIndex > 0) ? s_menuIndex - 1 : s_maxIndex;
                        s_menuIndex = newIndex;
                    }
                    break;
                }
                if (wpCode == (WPARAM)(s_pControlKeys[Ctrlkey_menuNext]))
                {
                    if (getStatus(ST_MENUVISIBLE))
                    {
                        menu_t newIndex = (s_menuIndex < s_maxIndex) ? s_menuIndex + 1 : 0;
                        s_menuIndex = newIndex;
                    }
                    break;
                }

                // miscellaneous
                if (wpCode == (WPARAM)(s_pControlKeys[Ctrlkey_fps])) // fps display
                {
                    if (getStatus(ST_MENUVISIBLE) == false)
                        setEvents(EVT_FPSCHANGE);
                    break;
                }
                if (wpCode == (WPARAM)(s_pControlKeys[Ctrlkey_pause])) // pause
                {
                    if (getStatus(ST_MENUVISIBLE) == false)
                        toggleStatus(ST_PAUSE);
                    break;
                }
                if (wpCode == (WPARAM)(s_pControlKeys[Ctrlkey_ratioMode])) // screen ratio mode
                {
                    setEvents(EVT_WINDOWRATIO);
                    break;
                }
            }
            // window position/size/state changed
            case WM_WINDOWPOSCHANGED:
            {
                // only in window mode (and no change pending)
                if (s_isSizeActive && isEvent() == false)
                {
                    s_isSizeActive = false; // busy
                    if (wpCode == SIZE_MAXIMIZED || wpCode == SIZE_RESTORED) // not minimized
                    {
                        static WPARAM prevState = wpCode;
                        if (prevState != wpCode) // different window state
                        {
                            prevState = wpCode;
                            setEvents(EVT_WINDOWSIZE);
                            return 0L;
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
                                setEvents(EVT_WINDOWSIZE);
                                return 0L;
                            }
                        }
                    }
                    s_isSizeActive = true; // free
                }
                break;
            }
        }
    }
    // default: transmit event to emulator
    return g_pWindowHandler(hWindow, eventType, wpCode, lpInfo); 
}

#else
/// <summary>Handle keyboard in Linux</summary>
/// <param name="keycode">Pressed key code</param>
void InputManager::GPUkeypressed(int keycode)
{
    switch (keycode)
    {
        default: //...
            break;
    }
}
#endif
