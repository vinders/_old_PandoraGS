/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : scrollable page for tab control
*******************************************************************************/
#include "../../../globals.h"
#include <string>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#endif
#include "../../../res/resource.h"
#include "mouse.h"
#include "common.h"
#include "dialog.h"
#include "scrollable_tab_page.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Create tab page - profile compatibility settings
/// @param[in] instance       Current instance handle
/// @param[in] pParentDialog  Parent dialog reference
/// @param[in] resourceId     Tab page description identifier
/// @param[in] maxRange       Max scroll range
ScrollableTabPage::ScrollableTabPage(library_instance_t instance, Dialog* pParentDialog, const int32_t resourceId, const uint32_t maxRange)
    : controls::TabPage(instance, pParentDialog, resourceId), m_maxRange(maxRange)
{
    // set event handlers
    TabPage::event_handler_t eventHandler;
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(PAGE_EVENT_HANDLER_ARGUMENTS)>(onInit);
    TabPage::registerEvent(dialog_event_t::init, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(PAGE_EVENT_HANDLER_ARGUMENTS)>(onVerticalScroll);
    TabPage::registerEvent(dialog_event_t::scrollY, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(PAGE_EVENT_HANDLER_ARGUMENTS)>(onMouseWheel);
    TabPage::registerEvent(dialog_event_t::wheelY, eventHandler);
}


// -- event handlers -- --------------------------------------------

/// @brief Initialization event handler
DIALOG_EVENT_RETURN ScrollableTabPage::onInit(PAGE_EVENT_HANDLER_ARGUMENTS)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    ScrollableTabPage& parent = getEventTargetPageReference(ScrollableTabPage);
    parent.m_hPage = getEventWindowHandle();

    // display vertical scrollbar
    EnableScrollBar(getEventWindowHandle(), SB_VERT, ESB_ENABLE_BOTH);
    ShowScrollBar(getEventWindowHandle(), SB_VERT, TRUE);

    // configure scrollbar
    RECT pageRect;
    GetClientRect(parent.m_hPage, &pageRect);
    int32_t pageSize = (pageRect.bottom - pageRect.top) / SCROLLABLETABPAGE_SCROLLUNIT;
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS | SIF_PAGE | SIF_RANGE;
    si.nMin = 0;
    si.nMax = parent.m_maxRange / SCROLLABLETABPAGE_SCROLLUNIT;
    si.nPage = pageSize;
    si.nPos = 0;
    SetScrollInfo(getEventWindowHandle(), SB_VERT, &si, FALSE);

    // mouse wheel manager
    parent.m_mouseManager = std::make_shared<Mouse>(pageSize);

    // custom init handler
    return parent.onInitOverridable(PAGE_EVENT_HANDLER_ARGUMENTS_VALUES);
}
#else
{
    //...
    return DIALOG_EVENT_RETURN_VALID;
}
#endif


/// @brief Vertical scroll event handler
DIALOG_EVENT_RETURN ScrollableTabPage::onVerticalScroll(PAGE_EVENT_HANDLER_ARGUMENTS)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    // get scrollbar previous state
    int32_t pos, prevPos;
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
    GetScrollInfo(getEventWindowHandle(), SB_VERT, &si);
    prevPos = si.nPos;

    // set new position
    int32_t action = getEventTargetControlId();
    switch (action)
    {
        case SB_TOP:           pos = si.nMin; break;
        case SB_BOTTOM:        pos = si.nMax; break;
        case SB_LINEUP:        pos = si.nPos - 1; break;
        case SB_LINEDOWN:      pos = si.nPos + 1; break;
        case SB_PAGEUP:        pos = si.nPos - si.nPage; break;
        case SB_PAGEDOWN:      pos = si.nPos + si.nPage; break;
        case SB_THUMBTRACK:    pos = si.nTrackPos; break;
        case SB_THUMBPOSITION: pos = si.nPos; break;
        default:               pos = si.nPos; break;
    }
    ScrollableTabPage::setScrollPosition(getEventWindowHandle(), pos, prevPos);
    return DIALOG_EVENT_RETURN_ERROR;
}
#else
{
    //...
    return DIALOG_EVENT_RETURN_VALID;
}
#endif


/// @brief Vertical mouse wheel event handler
DIALOG_EVENT_RETURN ScrollableTabPage::onMouseWheel(PAGE_EVENT_HANDLER_ARGUMENTS)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    int32_t delta = getEventSignedActionType();
    if (delta == 0)
        return DIALOG_EVENT_RETURN_ERROR;

    ScrollableTabPage& parent = getEventTargetPageReference(ScrollableTabPage);
    if (parent.m_mouseManager == nullptr)
        return DIALOG_EVENT_RETURN_ERROR;

    // get scrollbar previous state
    int32_t pos, prevPos;
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_PAGE | SIF_POS;
    GetScrollInfo(getEventWindowHandle(), SB_VERT, &si);
    prevPos = si.nPos;

    // set new position
    pos = prevPos + parent.m_mouseManager->getVerticalWheelScroll(delta, si.nPage);
    ScrollableTabPage::setScrollPosition(getEventWindowHandle(), pos, prevPos);
    return DIALOG_EVENT_RETURN_ERROR;
}
#else
{
    //...
    return DIALOG_EVENT_RETURN_VALID;
}
#endif
