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


// User input manager
class InputManager
{
private:
    static bool m_isListening; // state indicator
public:
    static bool m_isShowingMenu;
    static int  m_menuIndex;
    static bool m_isProfileChangePending;

    static bool m_isFastForward;
    static bool m_isSlowMotion;
    static bool m_isStretchingChangePending;
    static bool m_isWindowModeChangePending;

public:
    /// <summary>Start keyboard listener</summary>
    static void initListener();
    /// <summary>Stop keyboard listener</summary>
    static void stopListener();

    #ifdef _WINDOWS
    /// <summary>Enable or disable screen saver</summary>
    /// <param name="isEnabled">Enabled/disabled</param>
    static void setScreensaver(bool isEnabled);
    #endif
};

#ifdef _WINDOWS
/// <summary>Handle keyboard in Linux</summary>
/// <param name="hWindow">Window handler</param>
/// <param name="eventType">Event type</param>
/// <param name="wpCode">Command code</param>
/// <param name="lpInfo">Additional information</param>
LRESULT CALLBACK keyHandler(HWND hWindow, UINT eventType, WPARAM wpCode, LPARAM lpInfo);
#else
/// <summary>Handle keyboard in Linux</summary>
/// <param name="keycode">Pressed key code</param>
void GPUkeypressed(int keycode);
#endif

#endif
