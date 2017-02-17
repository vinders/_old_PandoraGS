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
#include "common.h"
#include "dialog.h"
#include "tab_button.h"
#include "tab_page.h"
#include "tab_control.h"
using namespace config::dialog::controls;


/// @brief Create tab control in dialog
/// @param[in] window  Window handle
/// @param[in] offset  Vertical offset for first tab button
/// @param[in] width   Tab button width
/// @returns Success
bool TabControl::create(window_handle_t window, const uint32_t offset, const uint32_t width)
{
    if (m_activePageIndex != -1)
        close();

    // create tab pages
    m_activePageIndex = 0;
    for (uint32_t i = 0; i < m_pages.size(); ++i)
    {
        if (m_pages.at(i).page->create(window, width) == false)
            return false;
        m_pages.at(i).page->setVisible(i == 0);
    }

    // create tab buttons
    for (uint32_t i = 0; i < m_pages.size(); ++i)
    {
        if (m_pages.at(i).button->create(window, offset, width) == false)
            return false;
    }
    return true;
}


/// @brief Close control in dialog
void TabControl::close()
{
    if (m_activePageIndex != -1 && m_pages.size() > 0)
    {
        for (uint32_t i = 0; i < m_pages.size(); ++i)
        {
            m_pages.at(i).page->close();
            m_pages.at(i).button->close();
        }
    }
    m_activePageIndex = -1;
}



// -- event handlers -- --------------------------------------------

/// @brief Language change event
/// @returns Validity
bool TabControl::onDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    // check settings validity in pages
    for (uint32_t i = 0; i < m_pages.size(); ++i)
    {
        if (m_pages.at(i).page->onDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS_VALUES) == false)
            return false;
    }
    return true;
}

/// @brief Language change event
/// @param[in] instance  Library instance handle
void TabControl::onLanguageChange(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    // refresh tab buttons
    //...repaint tab buttons
    //...

    // translate components in pages
    for (uint32_t i = 0; i < m_pages.size(); ++i)
    {
        m_pages.at(i).page->onLanguageChange(true);
    }
}


/// @brief Trigger control drawing
/// @param[in] args  Event arguments
void TabControl::onPaint(paint_event_args_t args)
#if _DIALOGAPI == DIALOGAPI_WIN32
{

}
#else
{

}
#endif
