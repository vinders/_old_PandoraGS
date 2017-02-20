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
#include "button.hpp"
#include "tab_control.h"
using namespace config::dialog::controls;

#define DIALOG_BOTTOM_BAR_HEIGHT  42
#define COLOR_BORDER              RGB(210,216,220)
#define COLOR_TAB_BORDER_MIN_R    217
#define COLOR_TAB_BORDER_MIN_G    222
#define COLOR_TAB_BORDER_MIN_B    226
#define COLOR_TAB_BORDER_MAX_R    191
#define COLOR_TAB_BORDER_MAX_G    202
#define COLOR_TAB_BORDER_MAX_B    215
const float TabControl::colorTop[3] { 235.0f, 240.0f, 245.0f };
const float TabControl::colorOffset[3] { -79.0f, -51.0f, -25.0f };
const float TabControl::colorTopBorder[3] { 227.0f, 232.0f, 236.0f };
const float TabControl::colorOffsetBorder[3] { -70.0f, -36.0f, -21.0f };


/// @brief Create tab control in dialog
/// @param[in] window  Window handle
/// @param[in] offset  Vertical offset for tab control
/// @param[in] width   Tab button width
/// @returns Success
bool TabControl::create(window_handle_t window, const uint32_t offset, const uint32_t width)
{
    if (m_activePageIndex != -1)
        close();

    // create tab pages
    m_activePageIndex = 0;
    m_offsetTabY = offset;
    m_width = width;
    for (uint32_t i = 0; i < m_pages.size(); ++i)
    {
        if (m_pages.at(i).page->create(window, width) == false)
            return false;
        m_pages.at(i).page->setVisible(i == 0);
    }

    // create tab buttons
    float posFactor;
    float lastIndex = static_cast<float>(m_pages.size() - 1u);
    for (uint32_t i = 0; i < m_pages.size(); ++i)
    {
        posFactor = static_cast<float>(i) / lastIndex;
        float red = COLOR_TAB_BORDER_MIN_R * (1.0f - posFactor) + COLOR_TAB_BORDER_MAX_R * (posFactor);
        float green = COLOR_TAB_BORDER_MIN_G * (1.0f - posFactor) + COLOR_TAB_BORDER_MAX_G * (posFactor);
        float blue = COLOR_TAB_BORDER_MIN_B * (1.0f - posFactor) + COLOR_TAB_BORDER_MAX_B * (posFactor);
        #if _DIALOGAPI == DIALOGAPI_WIN32
        COLORREF borders = RGB(static_cast<char>(red), static_cast<char>(green), static_cast<char>(blue));
        #else
        uint32_t borders = static_cast<uint32_t>(red) + (static_cast<uint32_t>(green) >> 8) + (static_cast<uint32_t>(blue) >> 16);
        #endif
        if (m_pages.at(i).button->create(*this, window, offset, width, (color_ref_t)borders) == false)
            return false;
    }
    m_pages.at(0).button->setActive(true);
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
    for (uint32_t i = 0; i < m_pages.size(); ++i)
    {
        // refresh tab buttons
        m_pages.at(i).button->invalidate();
        // translate components in pages
        m_pages.at(i).page->onLanguageChange(true);
    }
}


/// @brief Define colors for a tab button
/// @param[in]  tabNumber           Tab position
/// @param[in]  isHover             Mouse hover
/// @param[out] pColorTop           Vector (size = 3) to fill with top color
/// @param[out] pColorOffset        Vector (size = 3) to fill with color offset
/// @param[out] pColorTopBorder     Vector (size = 3) to fill with top border color
/// @param[out] pColorOffsetBorder  Vector (size = 3) to fill with border color offset
bool TabControl::getTabButtonColors(uint32_t tabNumber, bool isHover, float* pColorTop, float* pColorOffset, float* pColorTopBorder, float* pColorOffsetBorder)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    if (m_height != 0)
    {
        float percentHeight = static_cast<float>(TabButton::tabHeight) / static_cast<float>(m_height);
        uint32_t topCoord = TabButton::firstTabOffsetY + tabNumber * (TabButton::tabHeight + TabButton::tabInterval);
        float tabPercent = static_cast<float>(topCoord) / static_cast<float>(m_height);

        // colors
        pColorTop[0] = colorTop[0] + (tabPercent*colorOffset[0]);
        pColorTop[1] = colorTop[1] + (tabPercent*colorOffset[1]);
        pColorTop[2] = colorTop[2] + (tabPercent*colorOffset[2]);
        pColorTopBorder[0] = colorTopBorder[0] + (tabPercent*colorOffsetBorder[0]);
        pColorTopBorder[1] = colorTopBorder[1] + (tabPercent*colorOffsetBorder[1]);
        pColorTopBorder[2] = colorTopBorder[2] + (tabPercent*colorOffsetBorder[2]);

        // offsets
        pColorOffset[0] = percentHeight * colorOffset[0];
        pColorOffset[1] = percentHeight * colorOffset[1];
        pColorOffset[2] = percentHeight * colorOffset[2];
        pColorOffsetBorder[0] = percentHeight * colorOffsetBorder[0];
        pColorOffsetBorder[1] = percentHeight * colorOffsetBorder[1];
        pColorOffsetBorder[2] = percentHeight * colorOffsetBorder[2];

        // hover effect
        if (isHover)
        {
            pColorTop[0] *= 1.05f;
            if (pColorTop[0] > 255.0f)
                pColorTop[0] = 255.0f;
            pColorTop[1] *= 1.05f;
            if (pColorTop[1] > 255.0f)
                pColorTop[1] = 255.0f;
            pColorTop[2] *= 1.05f;
            if (pColorTop[2] > 255.0f)
                pColorTop[2] = 255.0f;
            pColorOffset[0] *= 0.8f;
            pColorOffset[1] *= 0.8f;
            pColorOffset[2] *= 0.8f;
        }
        return true;
    }
    return false;
}
#else
{
    //...
}
#endif


/// @brief Trigger control drawing
DIALOG_EVENT_RETURN TabControl::onPaint(DIALOG_EVENT_HANDLER_ARGUMENTS)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    PAINTSTRUCT paint;
    HDC hDC = nullptr;
    hDC = BeginPaint(getEventWindowHandle(), &paint);
    if (hDC == nullptr)
        return DIALOG_EVENT_RETURN_ERROR;

    RECT windowRect;
    GetClientRect(getEventWindowHandle(), &windowRect); 

    // tab control bottom border
    HBRUSH borderBrush = CreateSolidBrush(COLOR_BORDER);
    if (borderBrush)
    {
        RECT borderRect;
        borderRect.left = windowRect.left;
        borderRect.right = windowRect.right;
        borderRect.bottom = windowRect.bottom - DIALOG_BOTTOM_BAR_HEIGHT;
        borderRect.top = borderRect.bottom - 1;
        FillRect(hDC, &borderRect, HBRUSH(GetStockObject(WHITE_BRUSH)));

        borderRect.top += 1;
        borderRect.bottom += 1;
        FillRect(hDC, &borderRect, borderBrush);

        DeleteObject(borderBrush);
        borderBrush = nullptr;
    }

    // set background gradient zone
    float curPercent;
    RECT gradientLine, gardientBorder;
    HBRUSH gradientBrush;
    windowRect.top += m_offsetTabY; // skip top offset
    windowRect.bottom -= (DIALOG_BOTTOM_BAR_HEIGHT + 1); // skip bottom bar + border
    m_height = windowRect.bottom - windowRect.top;

    // draw background gradient
    gradientLine.left = windowRect.left;
    gradientLine.right = gardientBorder.left = windowRect.left + m_width - 1;
    gardientBorder.right = gardientBorder.left + 1;

    for (int px = 1; px < m_height; px += 2) // 2 lines at once (px and px-1)
    {
        curPercent = static_cast<float>(px) / static_cast<float>(m_height);

        gradientLine.top = windowRect.top + px - 1;
        gradientLine.bottom = gradientLine.top + 2;
        gradientBrush = CreateSolidBrush(RGB(colorTop[0] + (curPercent*colorOffset[0]),
                                                colorTop[1] + (curPercent*colorOffset[1]),
                                                colorTop[2] + (curPercent*colorOffset[2])));
        FillRect(hDC, &gradientLine, gradientBrush); // background
        DeleteObject(gradientBrush);

        gardientBorder.top = gradientLine.top;
        gardientBorder.bottom = gradientLine.bottom ;
        borderBrush = CreateSolidBrush(RGB(colorTopBorder[0] + (curPercent*colorOffsetBorder[0]),
                                            colorTopBorder[1] + (curPercent*colorOffsetBorder[1]),
                                            colorTopBorder[2] + (curPercent*colorOffsetBorder[2])));
        FillRect(hDC, &gardientBorder, borderBrush); // border
        DeleteObject(borderBrush);
    }

    EndPaint(hWindow, &paint);
    ReleaseDC(hWindow, hDC);
    return DIALOG_EVENT_RETURN_VALID;
}
#else
{
    //...
    return DIALOG_EVENT_RETURN_VALID;
}
#endif


/// @brief Tab button command event handler
DIALOG_EVENT_RETURN TabControl::onCommand(DIALOG_EVENT_HANDLER_ARGUMENTS)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    // check if a tab button was clicked (and not already active)
    int32_t resourceId = getEventTargetControlId();
    if (m_resourceIdMap.count(resourceId) && eventActionEquals(Button::event_t::clicked) && m_resourceIdMap[resourceId] != m_activePageIndex)
    {
        // deselect previous tab button
        int32_t previousIndex = m_activePageIndex;
        m_pages.at(m_activePageIndex).button->setActive(false);
        m_pages.at(m_activePageIndex).button->invalidate();
        // select new tab button
        m_activePageIndex = m_resourceIdMap[resourceId];
        m_pages.at(m_activePageIndex).button->setActive(true);
        m_pages.at(m_activePageIndex).button->invalidate();

        // change tab page
        m_pages.at(m_activePageIndex).page->setVisible(true);
        m_pages.at(previousIndex).page->setVisible(false);
        return DIALOG_EVENT_RETURN_VALID;
    }
    return DIALOG_EVENT_RETURN_ERROR;
}
#else
{
//...
return return DIALOG_EVENT_RETURN_ERROR;
}
#endif
