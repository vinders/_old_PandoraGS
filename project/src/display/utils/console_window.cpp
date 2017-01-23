/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : console output window management
*******************************************************************************/
#include "console_window.h"
using namespace display::utils;

/// @brief Display window on screen
void ConsoleWindow::show()
{
    if (m_isVisible)
        return;
    
    //...
    
    m_isVisible = true;
}

/// @brief Hide existing window
void ConsoleWindow::hide()
{
    if (m_isVisible == false)
        return;
    
    //...
    
    m_isVisible = false;
}

/// @brief Clear window content
void ConsoleWindow::clear()
{
    if (m_isVisible == false)
        return;
    
    //...
}
