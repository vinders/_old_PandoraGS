/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   input_reader.h
Description : user input management
*******************************************************************************/
#ifndef _INPUT_READER_H
#define _INPUT_READER_H
#include <cstdint>
#include <map>

// data types
typedef int32_t menu_t;
enum ctrlkey_t : uint32_t // special keys
{
    Ctrlkey_menuToggle = 0,  // menu on/off
    Ctrlkey_menuPrev = 1,    // menu shows prev profile
    Ctrlkey_menuNext = 2,    // menu shows next profile
    Ctrlkey_menuDefault = 3, // menu shows default profile
    Ctrlkey_menuAccept = 4,  // apply selected profile
    Ctrlkey_ratioMode = 5,   // next stretching mode
    Ctrlkey_fastForward = 6, // faster while pressed
    Ctrlkey_slowMotion = 7,  // slow mode on/off
    Ctrlkey_debug = 8        // FPS display / next debug mode
};
#define CTRLKEYS_LENGTH 10          // 9 + NULL ending
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

// events
#define EVT_WINDOWMODE    0x1
#define EVT_WINDOWSIZE    0x2
#define EVT_WINDOWRATIO   0x4
#define EVT_PROFILECHANGE 0x8
#define EVT_DEBUGCHANGE   0x10
#define ST_SPEEDSLOW      0x20
#define ST_SPEEDFAST      0x30
#define ST_MENUVISIBLE    0x40


// User input management
class InputReader
{
private:
    static bool m_isActive;     // input reader active or not
    static bool m_isSizeActive; // window size reader active or not
    static bool m_isScnSvDisabled; // disable screen-saver messages
    static char m_pControlKeys[CTRLKEYS_LENGTH]; // key / control association array

    static uint32_t m_events;   // triggered events indicator
    static uint32_t m_status;   // special status indicator
    static menu_t m_menuIndex;  // current profile menu index
    static menu_t m_activeIndex;// profile currently in use
    static menu_t m_maxIndex;   // max profile index


public:
#ifdef _WINDOWS
    /// <summary>Start input reader</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="pCtrlKeys">Array with each control key ID</param>
    /// <param name="maxMenu">Maximum menu index</param>
    /// <param name="isFullscreen">Fullscreen / window mode</param>
    /// <param name="isScnSvDisabled">Screen-saver disabled or not</param>
    static void start(HWND hWindow, char* pCtrlKeys, menu_t maxMenu, bool isFullscreen, bool isScnSvDisabled);
#else
    static void start(char* pCtrlKeys, menu_t maxMenu, bool isFullscreen);
#endif
    /// <summary>Stop input reader</summary>
    static void stop();
    /// <summary>Update input reader, based on current settings</summary>
    /// <param name="isFullscreen">Fullscreen / window mode</param>
    static void refresh(bool isFullscreen);

#ifdef _WINDOWS
    /// <summary>Handle keyboard in Windows</summary>
    /// <param name="hWindow">Window handler</param>
    /// <param name="eventType">Event type</param>
    /// <param name="wpCode">Command code</param>
    /// <param name="lpInfo">Additional information</param>
    static LRESULT CALLBACK listen(HWND hWindow, UINT eventType, WPARAM wpCode, LPARAM lpInfo);
#else
    /// <summary>Handle keyboard in Linux</summary>
    /// <param name="keycode">Pressed key code</param>
    static void GPUkeypressed(int keycode);
#endif

    // -- GETTERS / SETTERS -- -------------------------------------------------

    /// <summary>Get active menu index (profile ID)</summary>
    /// <returns>Used profile ID</returns>
    static inline menu_t getActiveMenuIndex()
    {
        return m_activeIndex;
    }
    /// <summary>Get current menu index (profile ID)</summary>
    /// <returns>Menu index</returns>
    static inline menu_t getCurrentMenuIndex()
    {
        return m_menuIndex;
    }
    /// <summary>Set current menu index (profile ID)</summary>
    /// <param name="curIndex">Profile ID</param>
    static inline void setCurrentMenuIndex(menu_t curIndex)
    {
        m_menuIndex = m_activeIndex = curIndex;
    }

    /// <summary>Unset all events bits</summary>
    static inline void resetEvents()
    {
        m_events = 0u;
    }
    /// <summary>Activate specific events bit(s) in events buffer</summary>
    /// <param name="bits">Bits mask to set</param>
    static inline void setEvents(uint32_t bits)
    {
        m_events |= bits;
    }
    /// <summary>Remove specific events bit(s) in events buffer</summary>
    /// <param name="bits">Bits mask to remove</param>
    static inline void unsetEvents(uint32_t bits)
    {
        m_events &= ~bits;
    }

    /// <summary>Check all bit(s) from events buffer</summary>
    /// <returns>True if at least one bit is active</returns>
    static inline bool isEvent()
    {
        return (m_events != 0u);
    }
    /// <summary>Check specific events bit(s) from events buffer</summary>
    /// <param name="bits">Bits mask</param>
    /// <returns>True if at least one bit is active</returns>
    static inline bool getEvents(uint32_t bits)
    {
        return ((m_events & bits) != 0);
    }

    /// <summary>Unset all status bits</summary>
    static inline void resetStatus()
    {
        m_status = 0u;
    }
    /// <summary>Activate specific status bit(s) in status buffer</summary>
    /// <param name="bits">Bits mask to set</param>
    static inline void setStatus(uint32_t bits)
    {
        m_status |= bits;
    }
    /// <summary>Remove specific status bit(s) in status buffer</summary>
    /// <param name="bits">Bits mask to remove</param>
    static inline void unsetStatus(uint32_t bits)
    {
        m_status &= ~bits;
    }
    /// <summary>Invert specific status bit(s) in status buffer</summary>
    /// <param name="bits">Bits mask to set</param>
    static inline void toggleStatus(uint32_t bits)
    {
        m_status ^= bits;
    }

    /// <summary>Check all bit(s) from status buffer</summary>
    /// <returns>True if at least one bit is active</returns>
    static inline bool isStatus()
    {
        return (m_status != 0u);
    }
    /// <summary>Check specific status bit(s) from status buffer</summary>
    /// <param name="bits">Bits mask</param>
    /// <returns>True if at least one bit is active</returns>
    static inline bool getStatus(uint32_t bits)
    {
        return ((m_status & bits) != 0);
    }
};

#endif
