/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : advanced tab control
*******************************************************************************/
#include "../../../globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#endif
#include "../../../res/resource.h"
#include "dialog.h"
#include "tab_button.h"
#include "tab_page.h"
#include "tab_control.h"
using namespace config::dialog::controls;


/// @brief Create tab control in dialog
/// @param[in] window      Window handle
/// @param[in] offset      Vertical offset for first tab button
/// @param[in] width       Tab button width
/// @returns Success
bool TabControl::create(window_handle_t window, const uint32_t offset, const uint32_t width)
{
    // create tab pages
    //...

    // create tab buttons
    m_activePageIndex = 0;
    for (uint32_t i = 0; i < m_pages.size(); ++i)
    {
        if (m_pages.at(i).button->create(window, offset, width) == false)
            return false;
    }
    return true;
}


/// @brief Trigger control drawing
/// @param[in] args  Event arguments
void TabControl::paint(paint_event_args_t args)
#if _DIALOGAPI == DIALOGAPI_WIN32
{

}
#else
{

}
#endif
