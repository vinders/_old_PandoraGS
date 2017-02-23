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
#include <unordered_map>
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
/// @param[in] hWindow    Parent window handle
/// @param[in] offset     Page horizontal offset
/// @param[in] isSubPage  Page is a sub-page of another page
/// @returns Dialog result
bool TabPage::create(window_handle_t hWindow, const uint32_t offset, const bool isSubPage)
{
    #if _DIALOGAPI == DIALOGAPI_WIN32
    if (reinterpret_cast<HINSTANCE>(m_instance) == reinterpret_cast<HINSTANCE>(INVALID_HANDLE_VALUE)
     && (HINSTANCE)(m_instance = reinterpret_cast<library_instance_t>(GetModuleHandle(NULL))) == reinterpret_cast<HINSTANCE>(INVALID_HANDLE_VALUE))
        return false;
    if (reinterpret_cast<HWND>(hWindow) == reinterpret_cast<HWND>(INVALID_HANDLE_VALUE))
        return false;
    if (m_pParentDialog == nullptr)
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
    if (isSubPage == false)
    {
        RECT pageSize;
        GetClientRect(hWindow, &pageSize);
        pageSize.left += offset;
        pageSize.bottom -= DIALOG_BUTTON_BAR_HEIGHT;
        // set page position
        SetWindowPos(m_hPage, nullptr, pageSize.left, pageSize.top,
            pageSize.right - pageSize.left, pageSize.bottom - pageSize.top, 0);
    }
    #else
    //...
    #endif
    return true;
}


/// @brief Close tab page control
void TabPage::close()
{
    overridableClose();

    #if _DIALOGAPI == DIALOGAPI_WIN32
    if (m_hPage && reinterpret_cast<HWND>(m_hPage) != reinterpret_cast<HWND>(INVALID_HANDLE_VALUE))
        DestroyWindow(reinterpret_cast<HWND>(m_hPage));
    #endif
    m_hPage = 0;
}


// -- event management -- ------------------------------------------

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
                if (pPage->isRegisteredEvent(dialog_event_t::init)) // call handler
                    success = pPage->getEventHandler(dialog_event_t::init).handler(pPage, hWindow, wParam, lParam);
                pPage->setInitialized();
                return (INT_PTR)success; break;
            }
            case WM_PAINT: // background painting
            {
                if (pPage->isRegisteredEvent(dialog_event_t::paint)) // call handler
                {
                    return pPage->getEventHandler(dialog_event_t::paint).handler(pPage, hWindow, wParam, lParam);
                }
                else // default painting (white background)
                {
                    PAINTSTRUCT paint;
                    HDC hDC = BeginPaint(hWindow, &paint);
                    if (hDC)
                    {
                        RECT rect;
                        GetClientRect(hWindow, &rect);
                        FillRect(hDC, &rect, HBRUSH(GetStockObject(WHITE_BRUSH)));

                        EndPaint(hWindow, &paint);
                        ReleaseDC(hWindow, hDC);
                    }
                    return (INT_PTR)TRUE;
                }
                break;
            }
            case WM_CTLCOLORSTATIC: // label text color
            {
                HDC hdcStatic = (HDC)wParam;
                SetTextColor(hdcStatic, RGB(0,0,0));
                SetBkColor(hdcStatic, RGB(255,255,255));
                return (LRESULT)HBRUSH(GetStockObject(WHITE_BRUSH)); break;
            }
            case WM_DRAWITEM:
            {
                if (pPage->isRegisteredEvent(dialog_event_t::drawItem)) // call handler
                    return pPage->getEventHandler(dialog_event_t::drawItem).handler(pPage, hWindow, wParam, lParam);
                break;
            }
            case WM_NOTIFY:
            {
                if (pPage->isRegisteredEvent(dialog_event_t::notify)) // call handler
                    return pPage->getEventHandler(dialog_event_t::notify).handler(pPage, hWindow, wParam, lParam);
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
            // vertical scroll-bar
            case WM_VSCROLL:
            {
                if (pPage->isInitialized() && pPage->isRegisteredEvent(dialog_event_t::scrollY) // call handler
                    && pPage->getEventHandler(dialog_event_t::scrollY).handler(pPage, hWindow, wParam, lParam) == (INT_PTR)TRUE)
                    return (INT_PTR)TRUE;
                break;
            }
            // vertical mouse wheel
            case WM_MOUSEWHEEL:
            {
                if (pPage->isInitialized() && pPage->isRegisteredEvent(dialog_event_t::wheelY) // call handler
                    && pPage->getEventHandler(dialog_event_t::wheelY).handler(pPage, hWindow, wParam, lParam) == (INT_PTR)TRUE)
                    return (INT_PTR)TRUE;
                break;
            }
        }
    }
    return (INT_PTR)FALSE;
}
#endif
