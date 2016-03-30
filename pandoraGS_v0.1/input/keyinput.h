/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   keyinput.h
    Description : keyboard input
*******************************************************************************/
#ifndef _KEYINPUT_H
#define _KEYINPUT_H

/* KEYBOARD INPUT ----------------------------------------------------------- */
class KeyInput
{
  private:
    #if _SYSTEM == _WINDOWS
        static WNDPROC _pWindowProcOriginal;
    #endif
    
  public:
    KeyInput();
    void startInput();
    void stopInput();
    //...
    
    #if _SYSTEM == _WINDOWS
        static LRESULT CALLBACK eventHandler(HWND, UINT, WPARAM, LPARAM);
    #endif
};

#endif
