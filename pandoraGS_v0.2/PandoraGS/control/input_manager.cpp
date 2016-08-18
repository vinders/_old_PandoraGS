/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   input_manager.h
Description : user input manager
*******************************************************************************/
#ifdef _WINDOWS
#include <Windows.h>
#endif
using namespace std;
#include "config.h"
#include "core_memory.h"
#include "log_utility.h"
#include "gui_main.h"
#include "input_manager.h"

// global data
#ifdef _WINDOWS
static WNDPROC  pEmulatorWindowHandler = 0;
static WNDPROC  pDisplayWindowHandler = 0;
#endif

bool InputManager::m_isListening = false; // state indicator (main)
bool InputManager::m_isListeningDisplay = false; // state indicator (display)

bool InputManager::m_isShowingMenu = false;
int32_t InputManager::m_menuIndex = 0;
bool InputManager::m_isProfileChangePending = false;

int32_t InputManager::m_frameSpeed = 0;
bool InputManager::m_isSizeChangePending = false;
bool InputManager::m_isStretchingChangePending = false;
bool InputManager::m_isWindowModeChangePending = false;


/// <summary>Start keyboard listener</summary>
void InputManager::initListener()
{
    if (m_isListening)
        return;
    // default values
    m_isShowingMenu = false;
    m_frameSpeed = 0;
    m_isSizeChangePending = false;
    m_isStretchingChangePending = false;
    m_isWindowModeChangePending = false;
    m_isProfileChangePending = false;

    #ifdef _WINDOWS
    // start event listener + save original listener
    if (!pEmulatorWindowHandler)
    {
        pEmulatorWindowHandler = (WNDPROC)GetWindowLong(CoreMemory::gen_hWindow, GWL_WNDPROC);
        SetWindowLong(CoreMemory::gen_hWindow, GWL_WNDPROC, (LONG)keyHandler);
    }
    #endif
    m_isListening = true;
    m_isListeningDisplay = (Config::dsp_isFullscreen == false);
}

/// <summary>Stop keyboard listener</summary>
void InputManager::stopListener()
{
    if (m_isListening)
    {
        #ifdef _WINDOWS
        // restore original event listener
        if (pEmulatorWindowHandler)
            SetWindowLong(CoreMemory::gen_hWindow, GWL_WNDPROC, (LONG)pEmulatorWindowHandler);
        pEmulatorWindowHandler = 0;
        #endif
        m_isListening = false;
    }
    m_isListeningDisplay = false;

    // default values
    m_isShowingMenu = false;
    m_frameSpeed = 0;
    m_isStretchingChangePending = false;
    m_isWindowModeChangePending = false;
    m_isProfileChangePending = false;
}

/// <summary>Update keyboard listener, based on settings</summary>
void InputManager::updateListener()
{
    m_isListeningDisplay = (Config::dsp_isFullscreen == false);
}

#ifdef _WINDOWS
/// <summary>Enable or disable screen saver</summary>
/// <param name="isEnabled">Enabled/disabled</param>
void InputManager::setScreensaver(bool isEnabled)
{
    // dynamic thread state system call
    EXECUTION_STATE(WINAPI *D_SetThreadExecutionState)(EXECUTION_STATE esFlags);
    HINSTANCE hKernel32 = NULL;

    // load the kernel32.dll library
    hKernel32 = LoadLibrary(L"kernel32.dll");
    if (hKernel32 != NULL)
    {
        D_SetThreadExecutionState = (EXECUTION_STATE(WINAPI *)(EXECUTION_STATE))GetProcAddress(hKernel32, "SetThreadExecutionState");
        if (D_SetThreadExecutionState != NULL)
        {
            if (isEnabled)
                D_SetThreadExecutionState(ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED); // enabled
            else
                D_SetThreadExecutionState(ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED | ES_CONTINUOUS); // disabled
        }
        FreeLibrary(hKernel32);
    }
}

/// <summary>Handle keyboard in Windows</summary>
/// <param name="hWindow">Window handler</param>
/// <param name="eventType">Event type</param>
/// <param name="wpCode">Command code</param>
/// <param name="lpInfo">Additional information</param>
LRESULT CALLBACK InputManager::keyHandler(HWND hWindow, UINT eventType, WPARAM wpCode, LPARAM lpInfo)
{
    switch (eventType)
    {
        // system commands
        case WM_SYSCOMMAND:
            if (wpCode == SC_SCREENSAVE && Config::misc_isScreensaverDisabled) // screensaver
                return 0L; // disable
            break;

        // key pressed
        case WM_KEYDOWN:
            if (wpCode == (WPARAM)(Config::misc_gpuKeys[(int)GpuKeys_FastForward])) // fast-forward
            {
                m_frameSpeed = FrameSpeed_FastForward;
            }
            break;

        // key released
        case WM_SYSKEYUP:
            if (wpCode == VK_RETURN // window mode
            && m_isWindowModeChangePending == false) // not busy
                m_isWindowModeChangePending = true;
            break;
        case WM_KEYUP:
        {
            // speed modifications
            if (wpCode == (WPARAM)(Config::misc_gpuKeys[(int)GpuKeys_FastForward]))
            {
                m_frameSpeed = FrameSpeed_Normal;
                break;
            }
            if (wpCode == (WPARAM)(Config::misc_gpuKeys[(int)GpuKeys_SlowMotion]))
            {
                if (m_frameSpeed != FrameSpeed_Normal)
                    m_frameSpeed = FrameSpeed_Normal;
                else
                    m_frameSpeed = FrameSpeed_SlowMotion;
                break;
            }

            // profile management
            if (wpCode == (WPARAM)(Config::misc_gpuKeys[(int)GpuKeys_ProfileMenu])) // show/hide menu
            {
                if (m_isShowingMenu == false)
                {
                    m_menuIndex = Config::getCurrentProfileId();
                    m_isShowingMenu = true;
                }
                else
                {
                    m_isShowingMenu = false;
                }
                break;
            }
            if (wpCode == (WPARAM)(Config::misc_gpuKeys[(int)GpuKeys_ProfileConfirm])) // change profile
            {
                if (m_isShowingMenu)
                {
                    m_isProfileChangePending = (m_menuIndex != Config::getCurrentProfileId());
                }
                break;
            }
            if (wpCode == (WPARAM)(Config::misc_gpuKeys[(int)GpuKeys_ProfileDefault]))
            {
                if (m_isShowingMenu)
                {
                    Config::getPrevProfileId(1uL);
                    m_menuIndex = 0;
                }
                break;
            }
            if (wpCode == (WPARAM)(Config::misc_gpuKeys[(int)GpuKeys_ProfilePrev]))
            {
                if (m_isShowingMenu)
                {
                    unsigned int newId = Config::getPrevProfileId(m_menuIndex);
                    m_menuIndex = newId;
                }
                break;
            }
            if (wpCode == (WPARAM)(Config::misc_gpuKeys[(int)GpuKeys_ProfileNext]))
            {
                if (m_isShowingMenu)
                {
                    unsigned int newId = Config::getNextProfileId(m_menuIndex);
                    m_menuIndex = newId;
                }
                break;
            }

            // miscellaneous
            if (wpCode == (WPARAM)(Config::misc_gpuKeys[(int)GpuKeys_Debug]))
            {
                if (Config::rnd_debugMode == (uint32_t)DebugMode_None) // not debugging -> toggle FPS display
                {
                    Config::rnd_isFpsDisplayed = !(Config::rnd_isFpsDisplayed);
                }
                else // debug mode -> next mode
                {
                    if (Config::rnd_debugMode < (uint32_t)DebugMode_TexturedMode)
                        Config::rnd_debugMode += 1;
                    else
                        Config::rnd_debugMode = DebugMode_Normal;
                }
                break;
            }
            if (wpCode == (WPARAM)(Config::misc_gpuKeys[(int)GpuKeys_Stretching])) // screen stretching mode
            {
                Config::getCurrentProfile()->dsp_ratioType++;
                if (Config::getCurrentProfile()->dsp_ratioType >= CFG_ScreenRatio_LENGTH)
                    Config::getCurrentProfile()->dsp_ratioType = 0;
                m_isStretchingChangePending = true;
                m_isWindowModeChangePending = true;
                break;
            }
        }
        // window position/size/state changed
        case WM_WINDOWPOSCHANGED:
        {
            // only in window mode (and no change pending)
            if (m_isListeningDisplay &&
                m_isWindowModeChangePending == false && Config::dsp_isFullscreen == false)
            {
                m_isListeningDisplay = false;
                if (wpCode == SIZE_MAXIMIZED || wpCode == SIZE_RESTORED) // not minimized
                {
                    static WPARAM prevState = wpCode;
                    if (prevState != wpCode) // different window state
                    {
                        prevState = wpCode;
                        m_isStretchingChangePending = true;
                        m_isWindowModeChangePending = true;
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
                            m_isSizeChangePending = true;
                            m_isStretchingChangePending = true;
                            m_isWindowModeChangePending = true;
                            return 0L;
                        }
                    }
                }
            }
            break;
        }
    }
    // transmit event to emulator
    return pEmulatorWindowHandler(hWindow, eventType, wpCode, lpInfo);
}


#else
#define VK_INSERT      65379
#define VK_HOME        65360
#define VK_PRIOR       65365
#define VK_NEXT        65366
#define VK_END         65367
#define VK_DEL         65535
#define VK_F5          65474

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
