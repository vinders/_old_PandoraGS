/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : keyboard information toolset
*******************************************************************************/
#include "../../../globals.h"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#endif
#include "common.h"
#include "keyboard.h"
#include "../../../events/listener.h"
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Create lists of keys
/// @param[in] isNameListFilled  Fill list of names or ignore it
Keyboard::Keyboard(const bool isNameListFilled)
{
    if (isNameListFilled)
    {
        m_keyNames = { L"<none>"s, L"F9"s, L"F10"s, L"F11"s, L"F12"s,
            L"Insert"s, L"Delete"s, L"Home"s, L"End"s, L"Page Up"s, L"Page Down"s,
            L"Ctrl Left"s, L"Ctrl Right"s, L"Shift Left"s, L"Shift Right"s,
            L"Numpad 0"s, L"Numpad 1"s, L"Numpad 2"s, L"Numpad 3"s, L"Numpad 4"s, L"Numpad 5"s, L"Numpad 6"s, L"Numpad 7"s, L"Numpad 8"s, L"Numpad 9"s,
            L"Arrow Up"s, L"Arrow Left"s, L"Arrow Right"s, L"Arrow Down"s };
    }

    m_indexesToKeyCodes.push_back(VK_NOKEY);
    m_indexesToKeyCodes.push_back(VK_F9);
    m_indexesToKeyCodes.push_back(VK_F10);
    m_indexesToKeyCodes.push_back(VK_F11);
    m_indexesToKeyCodes.push_back(VK_F12);
    m_indexesToKeyCodes.push_back(VK_INSERT);
    m_indexesToKeyCodes.push_back(VK_DELETE);
    m_indexesToKeyCodes.push_back(VK_HOME);
    m_indexesToKeyCodes.push_back(VK_END);
    m_indexesToKeyCodes.push_back(VK_PRIOR);
    m_indexesToKeyCodes.push_back(VK_NEXT);
    m_indexesToKeyCodes.push_back(VK_LCONTROL);
    m_indexesToKeyCodes.push_back(VK_RCONTROL);
    m_indexesToKeyCodes.push_back(VK_LSHIFT);
    m_indexesToKeyCodes.push_back(VK_RSHIFT);
    m_indexesToKeyCodes.push_back(VK_NUMPAD0);
    m_indexesToKeyCodes.push_back(VK_NUMPAD1);
    m_indexesToKeyCodes.push_back(VK_NUMPAD2);
    m_indexesToKeyCodes.push_back(VK_NUMPAD3);
    m_indexesToKeyCodes.push_back(VK_NUMPAD4);
    m_indexesToKeyCodes.push_back(VK_NUMPAD5);
    m_indexesToKeyCodes.push_back(VK_NUMPAD6);
    m_indexesToKeyCodes.push_back(VK_NUMPAD7);
    m_indexesToKeyCodes.push_back(VK_NUMPAD8);
    m_indexesToKeyCodes.push_back(VK_NUMPAD9);
    m_indexesToKeyCodes.push_back(VK_UP);
    m_indexesToKeyCodes.push_back(VK_LEFT);
    m_indexesToKeyCodes.push_back(VK_RIGHT);
    m_indexesToKeyCodes.push_back(VK_DOWN);
    
    for (int32_t i = 0; i < m_indexesToKeyCodes.size(); ++i)
        m_keyCodesToIndexes[m_indexesToKeyCodes.at(i)] = i;
}
