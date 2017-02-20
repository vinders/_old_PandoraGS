/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab button for tab control
*******************************************************************************/
#include "../../../globals.h"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#endif
#include "../../../res/resource.h"
#include "tab_control.h"
#include "tab_button.h"
using namespace config::dialog::controls;
using namespace std::literals::string_literals;

#define TAB_TEXT_OFFSET   64
#if _DIALOGAPI == DIALOGAPI_WIN32
#define COLOR_MENU_TEXT        RGB(67,82,97)
#define COLOR_MENU_TEXT_ACTIVE RGB(88,100,116)
#endif
const uint32_t TabButton::tabInterval = 6u;
const uint32_t TabButton::tabHeight = 90u;
const uint32_t TabButton::firstTabOffsetY = 2u;


int32_t TabButton::s_activeCount = 0; ///< Number of tab buttons loaded
std::unordered_map<uint32_t, bitmap_cache_t> TabButton::s_cache; ///< Icon bitmap cache
#if _DIALOGAPI == DIALOGAPI_WIN32
HFONT TabButton::s_tabFont = NULL; ///< Common font for tab buttons
std::unordered_map<HWND, TabButton*> TabButton::s_tabReferences; ///< Static references to tab button objects
#endif


/// @brief Create tab button
/// @param[in] instance       Current instance handle
/// @param[in] title          Tab name
/// @param[in] resourceId     Tab button resource identifier
/// @param[in] bmpResourceId  Bitmap with icons (resource ID)
/// @param[in] icon           Icon index (0 = none)
/// @param[in] iconSize       Icon size (pixels)
TabButton::TabButton(library_instance_t instance, std::wstring& title, const uint32_t resourceId,
                     const uint32_t bmpResourceId, const uint32_t icon, const uint32_t iconSize = 48u)
    : m_instance(instance), m_title(title), m_resourceId(resourceId), m_bitmapId(bmpResourceId), m_tabButtonControl(0), m_pParentTabControl(nullptr), m_width(0)
{
    m_tabNumber = s_activeCount;
    ++s_activeCount;
    m_iconIndex = icon;

    // load bitmap with icon
    if (!s_cache.count(bmpResourceId))
    {
        s_cache[bmpResourceId] = bitmap_cache_t();
        s_cache[bmpResourceId].iconSize = iconSize;
        s_cache[bmpResourceId].maxIndex = icon;
        s_cache[bmpResourceId].isAlphaChannelSet = false;
        s_cache[bmpResourceId].bitmapData = reinterpret_cast<HBITMAP>(LoadBitmap(instance, MAKEINTRESOURCE(IDB_CONFIG_ICONS)));
        if (s_cache[bmpResourceId].bitmapData == nullptr)
        {
            s_cache.erase(bmpResourceId);
            m_iconIndex = 0;
        }
    }
    // bitmap already loaded -> update max index
    else if (static_cast<int32_t>(icon) > s_cache[bmpResourceId].maxIndex)
    {
        s_cache[bmpResourceId].maxIndex = icon;
    }
}

/// @brief Destrot tab button
TabButton::~TabButton()
{
    // free resources
    --s_activeCount;
    close();

    if (s_activeCount <= 0u)
    {
        // clear cache if no tab button left
        s_activeCount = 0u;
        for (auto it = s_cache.begin(); it != s_cache.end(); ++it)
        {
            #if _DIALOGAPI == DIALOGAPI_WIN32
            if (it->second.bitmapData != nullptr) 
                DeleteObject(it->second.bitmapData);
            #endif
        }
        s_cache.clear();

        // remove font
        #if _DIALOGAPI == DIALOGAPI_WIN32
        if (TabButton::s_tabFont)
        {
            DeleteObject(TabButton::s_tabFont);
            TabButton::s_tabFont = NULL;
        }
        #endif
    }
}


/// @brief Create control in dialog
/// @param[in] parent       Parent tab control
/// @param[in] window       Window handle
/// @param[in] offset       Vertical offset for first tab button
/// @param[in] width        Tab button width
/// @param[in] borderColor  Tab border color
/// @returns Success
bool TabButton::create(TabControl& parent, window_handle_t window, const uint32_t offset, const uint32_t width, const color_ref_t borderColor)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    // create tab button
    m_pParentTabControl = &parent;
    m_isActive = m_isHover = m_isTrackingMouse = false;
    m_width = width;
    m_borderColor = borderColor;
    std::wstring controlName = L"Tab "s + std::to_wstring(m_tabNumber);
    m_tabButtonControl = CreateWindow(L"button", controlName.c_str(), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_BITMAP | BS_BOTTOM | BS_OWNERDRAW,
                                      0, offset + firstTabOffsetY + (m_tabNumber * (tabHeight + tabInterval)), width, tabHeight,
                                      reinterpret_cast<HWND>(window), reinterpret_cast<HMENU>(m_resourceId), reinterpret_cast<HINSTANCE>(m_instance), NULL);
    if (!m_tabButtonControl || (HWND)m_tabButtonControl == (HWND)INVALID_HANDLE_VALUE)
        return false;

    s_tabReferences[m_tabButtonControl] = this;
    HDC hDC = GetDC(reinterpret_cast<HWND>(window));
    if (hDC)
    {
        // set icon bitmap transparency
        if (m_iconIndex > 0u && s_cache[m_bitmapId].isAlphaChannelSet == false)
        {
            s_cache[m_bitmapId].isAlphaChannelSet = true;
            setBitmapAlphaChannel(hDC, s_cache[m_bitmapId].bitmapData, s_cache[m_bitmapId].maxIndex * s_cache[m_bitmapId].iconSize, s_cache[m_bitmapId].iconSize);
        }

        // load font
        if (!TabButton::s_tabFont)
        {
            LOGFONT logFont = { 0 };
            logFont.lfHeight = -MulDiv(10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
            logFont.lfWeight = FW_NORMAL;
            //logFont.lfQuality = CLEARTYPE_QUALITY;
            wcscpy_s(logFont.lfFaceName, L"Tahoma");
            TabButton::s_tabFont = CreateFontIndirect(&logFont);
        }
        ReleaseDC(reinterpret_cast<HWND>(window), hDC);
        // set font
        SendMessage(m_tabButtonControl, WM_SETFONT, (WPARAM)TabButton::s_tabFont, (LPARAM)MAKELONG(TRUE, 0));
    }

    // set custom drawing
    m_defaultHandler = (WNDPROC)SetWindowLong(m_tabButtonControl, GWL_WNDPROC, (LONG)tabButtonEventHandler);
    return true;
}
#else
{
    //...
    return true;
}
#endif


/// @brief Close control in dialog
void TabButton::close()
{
#if _DIALOGAPI == DIALOGAPI_WIN32
    if (m_defaultHandler)
        (WNDPROC)SetWindowLong(m_tabButtonControl, GWL_WNDPROC, (LONG)m_defaultHandler);
    if (s_tabReferences.count(m_tabButtonControl))
        s_tabReferences.erase(m_tabButtonControl);
    if (m_tabButtonControl && reinterpret_cast<HWND>(m_tabButtonControl) != reinterpret_cast<HWND>(INVALID_HANDLE_VALUE))
        DestroyWindow(reinterpret_cast<HWND>(m_tabButtonControl));
#endif
    m_tabButtonControl = 0;
}


/// @brief Redraw control
void TabButton::invalidate()
{
#if _DIALOGAPI == DIALOGAPI_WIN32
    SendMessage(m_tabButtonControl, WM_SETFONT, (WPARAM)TabButton::s_tabFont, (LPARAM)MAKELONG(TRUE, 0));
#endif
}



#if _DIALOGAPI == DIALOGAPI_WIN32
/// @brief Get current tab button non-static reference
/// @param[in] hWindow  Window handle
TabButton* TabButton::getCurrentTabButton(HWND hWindow)
{
    TabButton* ptab = nullptr;
    if (s_tabReferences.count(hWindow))
    {
        ptab = s_tabReferences[hWindow];
    }
    return ptab;
}


/// @brief Tab button event handler
/// @param[in] hWindow  Window handle
/// @param[in] msg      Event message
/// @param[in] wParam   Command
/// @param[in] lParam   Informations
/// @returns Event result
INT_PTR CALLBACK TabButton::tabButtonEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    TabButton* pTabButton = getCurrentTabButton(hWindow);
    if (pTabButton != nullptr)
    {
        switch (msg)
        {
            // paint tab button
            case WM_PAINT:
            {
                PAINTSTRUCT paint;
                HDC hDC = nullptr;
                hDC = BeginPaint(hWindow, &paint);
                if (hDC == nullptr)
                    break;

                RECT buttonRect;
                GetClientRect(hWindow, &buttonRect);
                SetBkMode(hDC, TRANSPARENT);

                // active -> white background
                if (pTabButton->m_isActive)
                {
                    RECT borderRect;
                    HBRUSH borderBrush;
                    // white background
                    FillRect(hDC, &buttonRect, HBRUSH(GetStockBrush(WHITE_BRUSH)));
                    // top/bottom borders
                    borderBrush = CreateSolidBrush((COLORREF)pTabButton->m_borderColor);
                    borderRect.top = buttonRect.top;
                    borderRect.bottom = buttonRect.top + 1;
                    borderRect.left = buttonRect.left;
                    borderRect.right = buttonRect.right;
                    FillRect(hDC, &borderRect, borderBrush);
                    borderRect.top = buttonRect.bottom - 1;
                    borderRect.bottom = buttonRect.bottom;
                    FillRect(hDC, &borderRect, borderBrush);
                }
                // inactive -> background gradient
                else if (pTabButton->m_pParentTabControl != nullptr)
                {
                    // set background gradient zone
                    float curPercent;
                    RECT gradientLine, gardientBorder;
                    HBRUSH gradientBrush, borderBrush;

                    // set background gradient colors
                    float colorTop[3];
                    float colorOffset[3];
                    float colorTopBorder[3];
                    float colorOffsetBorder[3];
                    if (pTabButton->m_pParentTabControl->getTabButtonColors(pTabButton->m_tabNumber, pTabButton->m_isHover, colorTop, colorOffset, colorTopBorder, colorOffsetBorder))
                    {
                        // draw background gradient
                        gradientLine.left = buttonRect.left;
                        gradientLine.right = gardientBorder.left = buttonRect.right - 1;
                        gardientBorder.right = buttonRect.right;
                        for (int px = 0; px < tabHeight; ++px)
                        {
                            if (px % 2 == 1) // 2 lines at once
                            {
                                curPercent = static_cast<float>(px) / static_cast<float>(tabHeight);

                                gradientLine.top = buttonRect.top + px - 1;
                                gradientLine.bottom = gradientLine.top + 2;
                                gradientBrush = CreateSolidBrush(RGB(colorTop[0] + (curPercent*colorOffset[0]),
                                                                     colorTop[1] + (curPercent*colorOffset[1]),
                                                                     colorTop[2] + (curPercent*colorOffset[2])));
                                FillRect(hDC, &gradientLine, gradientBrush); // background
                                DeleteObject(gradientBrush);

                                gardientBorder.top = gradientLine.top;
                                gardientBorder.bottom = gradientLine.bottom;
                                borderBrush = CreateSolidBrush(RGB(colorTopBorder[0] + (curPercent*colorOffsetBorder[0]),
                                                                   colorTopBorder[1] + (curPercent*colorOffsetBorder[1]),
                                                                   colorTopBorder[2] + (curPercent*colorOffsetBorder[2])));
                                FillRect(hDC, &gardientBorder, borderBrush); // border
                                DeleteObject(borderBrush);
                            }
                        }
                    }
                }

                // draw tab text
                RECT titleRect;
                titleRect.left = buttonRect.left;
                titleRect.right = buttonRect.right;
                titleRect.top = buttonRect.top + TAB_TEXT_OFFSET;
                titleRect.bottom = buttonRect.bottom;
                SelectObject(hDC, s_tabFont);
                SetTextColor(hDC, (pTabButton->m_isActive) ? COLOR_MENU_TEXT_ACTIVE : COLOR_MENU_TEXT);
                DrawText(hDC, pTabButton->m_title.c_str(), -1, &titleRect, DT_CENTER);

                // draw tab icon
                pTabButton->paintBitmap(hDC, &titleRect);

                EndPaint(hWindow, &paint);
                ReleaseDC(hWindow, hDC);
                break;
            }

            // track mouse movements
            case WM_MOUSEMOVE:
            {
                if (pTabButton->m_isTrackingMouse == false)
                {
                    TRACKMOUSEEVENT tme = { sizeof(tme) };
                    tme.dwFlags = TME_HOVER;
                    tme.hwndTrack = hWindow;
                    tme.dwHoverTime = 12;
                    ::TrackMouseEvent(&tme);
                    tme.dwFlags = TME_LEAVE;
                    tme.dwHoverTime = 1;
                    ::TrackMouseEvent(&tme);
                    pTabButton->m_isTrackingMouse = true;
                }
                break;
            }
            case WM_MOUSEHOVER:
            {
                if (pTabButton->m_isHover == false)
                {
                    pTabButton->m_isHover = true;
                    pTabButton->invalidate();
                }
                break;
            }
            case WM_MOUSELEAVE:
            {
                if (pTabButton->m_isHover)
                {
                    pTabButton->m_isHover = false;
                    pTabButton->invalidate();
                }
                pTabButton->m_isTrackingMouse = false;
                break;
            }
        }
        if (pTabButton->m_defaultHandler)
            return CallWindowProc(pTabButton->m_defaultHandler, hWindow, msg, wParam, lParam);
    }
    return (INT_PTR)FALSE;
}


/// @brief Draw bitmap icon on tab button
/// @param[in] hDC  Paint context
/// @param[in] pTitleRect  Tab title text rectangle
void TabButton::paintBitmap(HDC hDC, RECT* pTitleRect)
{
    // get cached bitmap
    if (!s_cache.count(m_bitmapId) || m_iconIndex == 0u)
        return;
    bitmap_cache_t& bitmapData = s_cache[m_bitmapId];
    if (m_iconIndex > static_cast<uint32_t>(bitmapData.maxIndex))
        m_iconIndex = 1u;

    // prepare bitmap information
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bitmapData.maxIndex * bitmapData.iconSize;
    bmi.bmiHeader.biHeight = bitmapData.iconSize;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4;
    // copy bitmap to DIB
    VOID* pvBits = NULL;
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hBmp = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
    SelectObject(hMemDC, hBmp);
    GetDIBits(hDC, bitmapData.bitmapData, 0, bitmapData.iconSize, pvBits, &bmi, DIB_RGB_COLORS);

    // blend alpha
    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 0xFF; // opaque
    bf.AlphaFormat = AC_SRC_ALPHA; // bitmap alpha
    AlphaBlend(hDC, pTitleRect->left + ((m_width / 2) - (bitmapData.iconSize / 2)), pTitleRect->top - (4 + bitmapData.iconSize),
               bitmapData.iconSize, bitmapData.iconSize, hMemDC, 
               (m_iconIndex - 1u) * bitmapData.iconSize, 0, 
               bitmapData.iconSize, bitmapData.iconSize, bf);

    DeleteObject(hBmp);
    DeleteDC(hMemDC);
}



/// @brief Prepare bitmap with alpha channel (set transparency)
/// @param hDC     Device context handle
/// @param img     Bitmap handle
/// @param width   Bitmap width (pixels)
/// @param height  Bitmap height (pixels)
void TabButton::setBitmapAlphaChannel(HDC hDC, HBITMAP img, LONG width, LONG height)
{
    if (img)
    {
        // set bitmap meta-data
        BITMAPINFO bmi;
        ZeroMemory(&bmi, sizeof(BITMAPINFO));
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4;

        // multiply alpha channel
        BITMAP bm = { 0 };
        GetObject(img, sizeof(bm), &bm);
        GetDIBits(hDC, img, 0, bm.bmHeight, NULL, &bmi, DIB_RGB_COLORS); // add real dimensions to bmi
        LPBYTE pBitData = (LPBYTE)LocalAlloc(LPTR, bm.bmWidth * bm.bmHeight * sizeof(DWORD));
        if (pBitData != NULL)
        {
            LPBYTE pData = pBitData;
            GetDIBits(hDC, img, 0, bm.bmHeight, pData, &bmi, DIB_RGB_COLORS); // copy image to array
            for (int y = 0; y < bm.bmHeight; y++)
            {
                for (int x = 0; x < bm.bmWidth; x++)
                {
                    pData[0] = (BYTE)((DWORD)pData[0] * pData[3] / 255); // fix for AlphaBlend
                    pData[1] = (BYTE)((DWORD)pData[1] * pData[3] / 255);
                    pData[2] = (BYTE)((DWORD)pData[2] * pData[3] / 255);
                    pData += 4;
                }
            }
            SetDIBits(hDC, img, 0, bm.bmHeight, pBitData, &bmi, DIB_RGB_COLORS); // copy array to image
            LocalFree(pBitData);
        }
    }
}
#endif
