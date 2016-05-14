/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   input_manager.h
Description : user input manager
*******************************************************************************/
#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H
#include "globals.h"

#include <map>
#include "config.h"

// speed modifiers
enum FrameSpeed
{
    FrameSpeed_SlowMotion = -1,
    FrameSpeed_Normal = 0,
    FrameSpeed_FastForward = 1
};


// User input manager
class InputManager
{
private:
    static bool m_isListening; // state indicator (main)
    static bool m_isListeningDisplay; // state indicator (display)
public:
    static bool m_isShowingMenu;
    static int  m_menuIndex;
    static bool m_isProfileChangePending;

    static int  m_frameSpeed;
    static bool m_isSizeChangePending;
    static bool m_isStretchingChangePending;
    static bool m_isWindowModeChangePending;

public:
    /// <summary>Start keyboard listener</summary>
    static void initListener();
    /// <summary>Set display window listener</summary>
    /// <param name="isEnabled">Enabled/disabled</param>
    static void setDisplayListener(bool isEnabled);
    /// <summary>Stop keyboard listener</summary>
    static void stopListener();

    #ifdef _WINDOWS
    /// <summary>Enable or disable screen saver</summary>
    /// <param name="isEnabled">Enabled/disabled</param>
    static void setScreensaver(bool isEnabled);
    #endif
};

#ifdef _WINDOWS
/// <summary>Handle keyboard in Windows</summary>
/// <param name="hWindow">Window handler</param>
/// <param name="eventType">Event type</param>
/// <param name="wpCode">Command code</param>
/// <param name="lpInfo">Additional information</param>
LRESULT CALLBACK keyHandler(HWND hWindow, UINT eventType, WPARAM wpCode, LPARAM lpInfo);
/// <summary>Handle window size in Windows</summary>
/// <param name="hWindow">Window handler</param>
/// <param name="eventType">Event type</param>
/// <param name="wpCode">Command code</param>
/// <param name="lpInfo">Additional information</param>
LRESULT CALLBACK sizeHandler(HWND hWindow, UINT eventType, WPARAM wpCode, LPARAM lpInfo);
#else
/// <summary>Handle keyboard in Linux</summary>
/// <param name="keycode">Pressed key code</param>
void GPUkeypressed(int keycode);
#endif

#endif
