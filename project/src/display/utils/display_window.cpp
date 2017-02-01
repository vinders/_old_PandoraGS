/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : graphics output window management
*******************************************************************************/
#include "../../globals.h"
#include <cstdlib>
#include <cstring>
#ifdef _WINDOWS
#include <Windows.h>
#include <tchar.h>
#else
#include <unistd.h>
#include <sys/types.h>
#endif
#include <stdexcept>
#include "display_window.h"
using namespace display::utils;


/// @brief Display window on screen
/// @param width Window width (ignored if fullscreen)
/// @param height Window height (ignored if fullscreen)
/// @param windowMode Window mode : fullscreen, fixed window, resizable window
void DisplayWindow::show(uint32_t width, uint32_t height, int32_t windowMode)
{
    if (m_isVisible)
        return;
    
    // set window visual style
    setWindowStyle(windowMode);
    //if (windowMode == static_cast<int32_t>(window_mode_t::fullscreen))
        //...ChangeDisplaySettings ...

    // display new window
    #ifdef _WINDOWS
    if (windowMode == static_cast<int32_t>(window_mode_t::fullscreen)) // fullscreen mode
    {
        ShowWindow(m_window, SW_SHOWMAXIMIZED);
    }
    else // window mode
    {
        uint32_t screenWidth, screenHeight;
        readScreenSize(screenWidth, screenHeight);

        // show centered window
        ShowWindow(m_window, SW_SHOWNORMAL);
        MoveWindow(m_window,
            screenWidth / 2 - width / 2,
            screenHeight / 2 - height / 2,
            width + GetSystemMetrics(SM_CXFIXEDFRAME) + 3,
            height + GetSystemMetrics(SM_CYFIXEDFRAME) + 3 + GetSystemMetrics(SM_CYCAPTION),
            TRUE);
        UpdateWindow(m_window);
    }
    #else
    // Linux/UNIX version
    //...
    #endif

    m_isVisible = true;
}

/// @brief Hide existing window
void DisplayWindow::hide()
{
    if (m_isVisible == false)
        return;
    
    //if (m_windowMode == window_mode_t::fullscreen)
        //...ChangeDisplaySettings ...
    restoreWindowStyle();
    m_isVisible = false;
}

/// @brief Clear window content
/// @param context Current device context
/// @param clearedZone Cleared zone (rectangle)
void DisplayWindow::clear(display::device_handle_t context, display::rect_t& clearedZone)
{
    if (m_isVisible == false)
        return;
    
    #ifdef _WINDOWS
    FillRect(context, &clearedZone, (HBRUSH)GetStockObject(BLACK_BRUSH));
    #else
    // Linux/UNIX version
    //...
    #endif
}


// -- getters / setters -- ---------------------------------------------

/// @brief Prepare visual style for display window
/// @param outWidth Width destination variable
void DisplayWindow::setWindowStyle(int32_t windowMode)
{
    // set new window style
    #ifdef _WINDOWS
    GetWindowRect(m_window, &m_sourceSize);
    DWORD dwStyle = GetWindowLong(m_window, GWL_STYLE);
    m_sourceStyle = dwStyle; // save source window state, to be able to restore it
    switch (windowMode)
    {
        case static_cast<int32_t>(window_mode_t::fullscreen): 
            m_windowMode = window_mode_t::fullscreen;
            dwStyle = CS_OWNDC; break;

        case static_cast<int32_t>(window_mode_t::resizable): 
            m_windowMode = window_mode_t::resizable;
            dwStyle |= (WS_BORDER | WS_CAPTION | CS_OWNDC); break;

        default: // window_mode_t::fixed
            m_windowMode = window_mode_t::fixed;
            dwStyle &= ~WS_THICKFRAME;
            dwStyle |= (WS_BORDER | WS_CAPTION | CS_OWNDC); break;
    }
    SetWindowLong(m_window, GWL_STYLE, dwStyle);
    #else
    // Linux/UNIX version
    //...
    #endif

    // hide source window menu
    #ifdef _WINDOWS
    m_sourceMenu = GetMenu(m_window);
    if (m_sourceMenu)
        SetMenu(m_window, NULL);
    #else
    // Linux/UNIX version
    //...
    #endif
}

/// @brief Restore source visual style
void DisplayWindow::restoreWindowStyle()
{
    // restore window style
    #ifdef _WINDOWS
    SetWindowLong(m_window, GWL_STYLE, m_sourceStyle);
    MoveWindow(m_window, m_sourceSize.left, m_sourceSize.top, m_sourceSize.right - m_sourceSize.left, m_sourceSize.bottom - m_sourceSize.top, TRUE);
    m_sourceStyle = 0;
    #else
    // Linux/UNIX version
    //...
    #endif

    // restore source window menu
    #ifdef _WINDOWS
    if (m_sourceMenu)
        SetMenu(m_window, m_sourceMenu);
    m_sourceMenu = NULL;
    #else
    // Linux/UNIX version
    //...
    #endif
}

/// @brief Get current window size
/// @param outWidth Width destination variable
/// @param outHeight Height destination variable
bool DisplayWindow::getWindowSize(uint32_t& outWidth, uint32_t& outHeight)
{
    #ifdef _WINDOWS
    RECT clientRect;
    if (GetClientRect(m_window, &clientRect))
    {
        outWidth = clientRect.right;
        outHeight = clientRect.bottom;
        return true;
    }
    #endif
    return false;
}

/// @brief Initialize window pixel format and fill window
/// @param resolution Display resolution (X, Y)
/// @param colorMode Display color mode
void DisplayWindow::setPixelFormat(display::coord_t resolution, display::window_color_mode_t colorMode)
{
    #ifdef _WINDOWS
    display::device_handle_t deviceContext = GetDC(m_window);
    #else
    // Linux/UNIX version
    display::device_handle_t deviceContext = 0/*...*/;
    #endif

    // pixel format config
    #ifdef _WINDOWS
    PIXELFORMATDESCRIPTOR pxFd = { sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // support OpenGL + double buffering
        PFD_TYPE_RGBA, 16, // output color depth
        0, 0, 0, 0, 0, 0,  // color bits ignored
        0, 0,              // no alpha buffer + shift bit ignored
        0, 0, 0, 0, 0,     // no accumulation buffer + accumulation bits (R,G,B,A) ignored
        0,                 // no z-buffer
        0, 0,              // no stencil buffer + no auxillary buffer
        PFD_MAIN_PLANE, 0, // main layer
        0, 0, 0            // layer masks, ignored
    };
    if (colorMode == display::window_color_mode_t::rgb_32bit)
        pxFd.cColorBits = 32;
    //...if (zbuffer)
        pxFd.cDepthBits = 16;

    // apply pixel format config
    unsigned int pxFormat = ChoosePixelFormat(deviceContext, &pxFd);
    if (!pxFormat || !SetPixelFormat(deviceContext, pxFormat, &pxFd))
    {
        ReleaseDC(m_window, deviceContext);
        throw std::runtime_error("RenderApi: failed to set pixel format");
    }
    #else
    // Linux/UNIX version
    //...
    #endif

    // fill screen
    display::rect_t fillZone;
    fillZone.left = 0;
    fillZone.top = 0;
    fillZone.right = resolution.x;
    fillZone.bottom = resolution.y;
    clear(deviceContext, fillZone);

    #ifdef _WINDOWS
    ReleaseDC(m_window, deviceContext);
    #else
    // Linux/UNIX version
    //...
    #endif
}

/// @brief Read screen display size
/// @param outWidth Width destination variable
/// @param outHeight Height destination variable
void DisplayWindow::readScreenSize(uint32_t& outWidth, uint32_t& outHeight)
{
    #ifdef _WINDOWS
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    if (screenWidth < 640)
        screenWidth = 800;
    outWidth = screenWidth;

    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    if (screenHeight < 480)
        screenHeight = 600;
    outHeight = screenHeight;
    #else
    // Linux/UNIX version
    //...
    #endif
}

/// @brief Enable or disable screen saver
/// @param isEnabled Enabled/disabled
void DisplayWindow::setScreenSaver(bool isEnabled)
{
    #ifdef _WINDOWS
    // dynamic thread state system call
    EXECUTION_STATE(WINAPI *D_SetThreadExecutionState)(EXECUTION_STATE esFlags);
    HINSTANCE hKernel32 = NULL;

    // load the kernel32 library
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
    #endif
}
