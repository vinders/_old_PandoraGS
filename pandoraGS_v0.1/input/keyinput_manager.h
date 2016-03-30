/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   keyinput_manager.h
    Description : keyinput management and singleton
*******************************************************************************/
#ifndef _KEYINPUT_MANAGER_H
#define _KEYINPUT_MANAGER_H
#include "keyinput.h"

/* KEYINPUT MANAGER SINGLETON ----------------------------------------------- */
class KeyInputManager
{
  protected:
    static KeyInput _keyInput;
    
  public:
    static KeyInput* getInstance() // singleton pattern
    {
        return &_keyInput;
    }
    
    // properties
    static void setKeyInput();
    static void unsetKeyInput();
};

#endif
