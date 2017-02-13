/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : page for tab control - abstract class
*******************************************************************************/
#include "../../../globals.h"
#include <string>
#include <map>
#include <stdexcept>
#include <thread>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#endif
#include "../../../res/resource.h"
#include "common.h"
#include "dialog.h"
#include "tab_page.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;

TabPage* TabPage::s_pageRefBuffer = nullptr; ///< Page reference buffer (pass non-static data to static handler)


/// @brief Create tab page control (non-virtual)
/// @param[in] hWindow  Parent window handle
/// @param[in] offset   Page horizontal offset
/// @returns Dialog result
bool TabPage::createPage(window_handle_t hWindow, const uint32_t offset)
{
    #if _DIALOGAPI == DIALOGAPI_WIN32
    if (reinterpret_cast<HINSTANCE>(m_instance) == reinterpret_cast<HINSTANCE>(INVALID_HANDLE_VALUE)
     && (HINSTANCE)(m_instance = reinterpret_cast<library_instance_t>(GetModuleHandle(NULL))) == reinterpret_cast<HINSTANCE>(INVALID_HANDLE_VALUE))
        return false;
    if (reinterpret_cast<HWND>(hWindow) == reinterpret_cast<HWND>(INVALID_HANDLE_VALUE))
        return false;
    #endif

    // wait until previous page has emptied buffer
    while (s_pageRefBuffer != nullptr)
        std::this_thread::yield();
    s_pageRefBuffer = this;

    #if _DIALOGAPI == DIALOGAPI_WIN32
    // create page
    m_hPage = CreateDialog(m_instance, MAKEINTRESOURCE(m_pageResourceId), hWindow, (DLGPROC)pageEventHandler);
    if (!m_hPage || reinterpret_cast<HWND>(m_hPage) == reinterpret_cast<HWND>(INVALID_HANDLE_VALUE))
        return false;
    
    // define page size
    RECT pageSize;
    GetClientRect(hWindow, &pageSize);
    pageSize.left += offset;
    pageSize.bottom -= DIALOG_BUTTON_BAR_HEIGHT;
    // set page position
    SetWindowPos(m_hPage, nullptr, pageSize.left, pageSize.top,
                 pageSize.right - pageSize.left, pageSize.bottom - pageSize.top, 0);
    #else
    //...
    #endif
    return true;
}


/// @brief Close tab page control
void TabPage::close()
{
    #if _DIALOGAPI == DIALOGAPI_WIN32
    if (m_hPage && reinterpret_cast<HWND>(m_hPage) != reinterpret_cast<HWND>(INVALID_HANDLE_VALUE))
        DestroyWindow(reinterpret_cast<HWND>(m_hPage));
    #endif
    m_hPage = 0;
}


// -- event management -- ------------------------------------------

/// @brief Trigger an event (send message to the object's own static handler)
/// @param[in] eventType   Type of event to trigger
/// @param[in] eventParam  Event-specific flag
void TabPage::triggerEvent(dialog_event_t eventType, uint32_t eventParam)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    //...sendmessage to resourceId
}
#else
{

}
#endif


#if _DIALOGAPI == DIALOGAPI_WIN32
/// @brief Get current dialog non-static reference
/// @param[in] hWindow  Window handle
TabPage* TabPage::getCurrentPage(HWND hWindow)
{
    TabPage* pPage = (TabPage*)GetWindowLongPtr(hWindow, GWLP_USERDATA); // read reference to dialog (if existing)
    if (!pPage)
    {
        // store reference to non-static dialog
        pPage = nullptr;
        if (s_pageRefBuffer != nullptr)
        {
            pPage = s_pageRefBuffer;
            s_pageRefBuffer = nullptr;
            SetWindowLongPtr(hWindow, GWLP_USERDATA, (LONG_PTR)(pPage));
        }
    }
    return pPage;
}


/// @brief Tab page event handler
/// @param[in] hWindow  Window handle
/// @param[in] msg      Event message
/// @param[in] wParam   Command
/// @param[in] lParam   Informations
/// @returns Event result
INT_PTR CALLBACK TabPage::pageEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    TabPage* pPage = getCurrentPage(hWindow);
    if (pPage != nullptr)
    {
        switch (msg)
        {
            // page settings
            case WM_INITDIALOG:
            {
                int32_t success = TRUE;
                pPage->setInitialized();
                if (pPage->isRegisteredEvent(dialog_event_t::init)) // call handler
                    success = pPage->getEventHandler(dialog_event_t::init).handler(pPage, hWindow, wParam, lParam);
                return (INT_PTR)success; break;
            }
            case WM_PAINT:
            {
                if (pPage->isRegisteredEvent(dialog_event_t::paint)) // call handler
                    return pPage->getEventHandler(dialog_event_t::paint).handler(pPage, hWindow, wParam, lParam);
                break;
            }
            case WM_DRAWITEM:
            {
                if (pPage->isRegisteredEvent(dialog_event_t::drawItem)) // call handler
                    return pPage->getEventHandler(dialog_event_t::drawItem).handler(pPage, hWindow, wParam, lParam);
                break;
            }

            // page controls
            case WM_COMMAND:
            {
                if (pPage->isInitialized() && pPage->isRegisteredEvent(dialog_event_t::command) // call handler
                    && pPage->getEventHandler(dialog_event_t::command).handler(pPage, hWindow, wParam, lParam) == (INT_PTR)TRUE)
                    return (INT_PTR)TRUE;
                break;
            }
            // horizontal scroll-bar / track-bar
            case WM_HSCROLL:
            {
                if (pPage->isInitialized() && pPage->isRegisteredEvent(dialog_event_t::scrollX) // call handler
                    && pPage->getEventHandler(dialog_event_t::scrollX).handler(pPage, hWindow, wParam, lParam) == (INT_PTR)TRUE)
                    return (INT_PTR)TRUE;
                break;
            }
            // vertical scroll-bar
            case WM_VSCROLL:
            {
                if (pPage->isInitialized() && pPage->isRegisteredEvent(dialog_event_t::scrollY) // call handler
                    && pPage->getEventHandler(dialog_event_t::scrollY).handler(pPage, hWindow, wParam, lParam) == (INT_PTR)TRUE)
                    return (INT_PTR)TRUE;
                break;
            }
        }
    }
    return (INT_PTR)FALSE;
}
#endif
