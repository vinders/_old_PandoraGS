/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   keyinput_manager.cpp
    Description : keyinput management and singleton
*******************************************************************************/
using namespace std;
#include "../globals.h"
#include "keyinput_manager.h"

/* static variables */
KeyInput KeyInputManager::_keyInput;

/* define singleton target -------------------------------------------------- */
void KeyInputManager::setKeyInput()
{
    _keyInput.startInput();
}

/* remove singleton target -------------------------------------------------- */
void KeyInputManager::unsetKeyInput()
{
    _keyInput.stopInput();
}
