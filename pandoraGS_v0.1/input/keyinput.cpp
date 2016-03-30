/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   keyinput.cpp
    Description : keyboard input
*******************************************************************************/
using namespace std;
#include "../globals.h"
#include "keyinput.h"

#if _SYSTEM == _WINDOWS
    extern HWND gpu_window;
    WNDPROC KeyInput::_pWindowProcOriginal = 0;
#endif

/* keyboard input constructor ----------------------------------------------- */
KeyInput::KeyInput()
{
}

/* keyboard input handler init ---------------------------------------------- */
void KeyInput::startInput()
{
    #if _SYSTEM == _WINDOWS
        if (_pWindowProcOriginal == 0)
        {      
            _pWindowProcOriginal = (WNDPROC)GetWindowLong(gpu_window, GWL_WNDPROC);
            SetWindowLong(gpu_window, GWL_WNDPROC, (long)eventHandler);
        }
    #endif
}

/* keyboard input handler destruction --------------------------------------- */
void KeyInput::stopInput()
{
    #if _SYSTEM == _WINDOWS
        if (_pWindowProcOriginal)
            SetWindowLong(gpu_window, GWL_WNDPROC, (long)_pWindowProcOriginal);
        _pWindowProcOriginal = 0;                                      
    #endif
}

#if _SYSTEM == _WINDOWS
/* keyboard events handler -------------------------------------------------- */
LRESULT CALLBACK KeyInput::eventHandler(HWND hWindow, UINT uEvent, WPARAM wParam, LPARAM lParam)
{
    // check plugin defined keys
    switch(uEvent)
    {
        case WM_SYSCOMMAND: break;
        case WM_KEYDOWN:    break;
        case WM_SYSKEYUP:   break;
        case WM_KEYUP:      break;
    }
    
    // if other key, use standard handler
    return _pWindowProcOriginal(hWindow, uEvent, wParam, lParam);
}
#endif
