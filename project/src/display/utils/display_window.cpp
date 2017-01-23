/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : graphics output window management
*******************************************************************************/
#include "display_window.h"
using namespace display::utils;

/// @brief Display window on screen
void DisplayWindow::show()
{
    if (m_isVisible)
        return;
    
    //...
    
    m_isVisible = true;
}

/// @brief Hide existing window
void DisplayWindow::hide()
{
    if (m_isVisible == false)
        return;
    
    //...
    
    m_isVisible = false;
}

/// @brief Clear window content
void DisplayWindow::clear()
{
    if (m_isVisible == false)
        return;
    
    //...
}
