/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   win_toolbox.cpp
Description : winAPI toolbox
*******************************************************************************/
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
using namespace std;
#include "win_toolbox.hpp"
#include "config_profile.h"
#include "pandoraGS.h"

#define PREVIEW_BORDER_COLOR      RGB(74,90,106)
#define PREVIEW_EMPTY_COLOR       RGB(70, 70, 70)
#define PREVIEW_CROPPED_COLOR     RGB(210,220,230)
#define PREVIEW_CROPPED_BORDER_COLOR RGB(170,180,190)

HBRUSH WinToolbox::s_hBackgroundBrush = NULL;
HBITMAP WinToolbox::s_hPreviewStretchingBitmap = NULL;

/// <summary>Show/hide page</summary>
/// <param name="itemId">Item associated with tooltip (ID)</param>
/// <param name="hWindow">Parent dialog handle</param>
/// <param name="phInst">Window instance</param>
/// <param name="pText">Tooltip text</param>
/// <returns>Tooltip handle</returns>
HWND WinToolbox::createToolTip(int itemId, HWND hWindow, HINSTANCE* phInst, PTSTR pText)
{
    // check arguments
    if (!itemId || !hWindow || !pText)
        return (HWND)NULL;
    HWND hItem = GetDlgItem(hWindow, itemId);
    if (!hItem)
        return (HWND)NULL;

    // tooltip creation
    HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWindow, NULL, *phInst, NULL);
    if (!hwndTip)
        return (HWND)NULL;

    // tooltip association with item
    TOOLINFO ti = { 0 };
    ti.cbSize = sizeof(ti);
    ti.hwnd = hWindow;
    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    ti.uId = (UINT_PTR)hItem;
    ti.lpszText = pText;
    SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
    return hwndTip;
}

/// <summary>Show/hide page</summary>
/// <param name="hTooltip">Tooltip handle</param>
/// <param name="itemId">Item associated with tooltip (ID)</param>
/// <param name="hWindow">Parent dialog handle</param>
/// <param name="pText">Tooltip text</param>
void WinToolbox::updateToolTip(HWND hTooltip, int itemId, HWND hWindow, PTSTR pText)
{
    // check arguments
    if (!itemId || !hTooltip || !pText)
        return;
    HWND hItem = GetDlgItem(hWindow, itemId);
    if (!hItem)
        return;

    // update tooltip definition
    TOOLINFO ti = { 0 };
    ti.cbSize = sizeof(ti);
    ti.hwnd = hWindow;
    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    ti.uId = (UINT_PTR)hItem;
    ti.lpszText = pText;
    SendMessage(hTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&ti);
}

/// <summary>Delete page background brush</summary>
void WinToolbox::destroyPageBackgroundBrush()
{
    if (s_hBackgroundBrush)
        DeleteObject(s_hBackgroundBrush);
    s_hBackgroundBrush = NULL;
    if (s_hPreviewStretchingBitmap)
        DeleteObject(s_hPreviewStretchingBitmap);
    s_hPreviewStretchingBitmap = NULL;
}

/// <summary>Screen stretching page drawing</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="color">Background color (global to all pages)</param>
/// <param name="lParam">Informations</param>
/// <param name="stretching">Stretching value</param>
/// <param name="cropping">Cropping value</param>
/// <param name="isMirrored">Mirror indicator</param>
/// <returns>Action code</returns>
INT_PTR WinToolbox::drawScreenStretchingPage(HWND hWindow, COLORREF color, LPARAM lParam, uint32_t stretching, uint32_t cropping, bool isMirrored)
{
    LPDRAWITEMSTRUCT pDraw = (LPDRAWITEMSTRUCT)lParam;
    if (!lParam || pDraw->CtlID != IDC_PROSTR_PICTUREBOX || stretching > CFG_RATIO_MAX || cropping > CFG_RATIO_MAX)
        return (INT_PTR)FALSE;

    // load preview bitmap
    if (!s_hPreviewStretchingBitmap)
    {
        HINSTANCE hInst = PandoraGS::getInstance();
        if (hInst == NULL) hInst = GetModuleHandle(NULL);
        if (hInst)
            s_hPreviewStretchingBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RATIO));
        if (!s_hPreviewStretchingBitmap)
            return (INT_PTR)TRUE;
    }

    // prepare draw components
    HDC hDC = pDraw->hDC;
    HBRUSH brushBorder = CreateSolidBrush(PREVIEW_BORDER_COLOR);
    if (hDC == NULL || brushBorder == NULL)
        return (INT_PTR)TRUE;
    RECT itemRect = pDraw->rcItem;
    RECT drawZone = { 0 };
    drawZone.top = itemRect.top + 2;
    drawZone.left = itemRect.left + 2;
    drawZone.right = drawZone.left + 180;
    drawZone.bottom = drawZone.top + 135;
    SetBkMode(hDC, TRANSPARENT);
    // draw page background
    if (s_hBackgroundBrush)
        FillRect(hDC, &itemRect, s_hBackgroundBrush);

    // set preview settings - stretching
    int imgWidth = 140 + (5 * stretching);
    int imgHeight = 105;
    // set preview settings - cropping
    if (cropping > 0u)
    {
        double factor = 180.0 / (double)imgWidth;
        if (factor > 1.005) // > 1 (+ rounding error)
        {
            if ((int)cropping < CFG_RATIO_MAX)
            {
                factor = (((double)cropping / (double)CFG_RATIO_MAX) * (factor - 1.0)) + 1.0;
                double dWidth = factor * (double)imgWidth;
                imgWidth = ((int)dWidth >> 1) << 1; // even
            }
            else
                imgWidth = 180;
            double dHeight = factor * (double)imgHeight;
            imgHeight = 1 + (((int)dHeight >> 1) << 1); // odd
        }
    }
    // set preview settings - position
    int offsetX = (180 - imgWidth) >> 1;
    int offsetY = (135 - imgHeight) >> 1;
    RECT imgRect = { 0 };
    imgRect.left = drawZone.left + offsetX;
    imgRect.right = drawZone.right - offsetX;
    imgRect.top = drawZone.top + offsetY;
    imgRect.bottom = drawZone.bottom - offsetY;

    // draw preview image
    HDC dstDC = CreateCompatibleDC(hDC);
    if (dstDC)
    {
        // display resized screen image
        HDC memDC = CreateCompatibleDC(hDC);
        if (memDC)
        {
            SelectObject(memDC, s_hPreviewStretchingBitmap);
            HBITMAP dstBmp = CreateCompatibleBitmap(memDC, imgWidth, imgHeight); // create empty bitmap
            if (dstBmp)
            {
                // copy resized bitmap
                SelectObject(dstDC, dstBmp);
                StretchBlt(dstDC, (isMirrored) ? imgWidth : 0, 0, (isMirrored) ? -imgWidth - 1 : imgWidth, imgHeight, memDC, 0, 0, 140, 105, SRCCOPY);

                // create image-pattern brush
                HBRUSH ratioImgBrush = CreatePatternBrush(dstBmp);
                if (ratioImgBrush)
                {
                    SetBrushOrgEx(hDC, imgRect.left, imgRect.top, NULL);
                    FillRect(hDC, &imgRect, ratioImgBrush);
                    DeleteObject(ratioImgBrush);
                }
                DeleteObject(dstBmp);
            }
            DeleteDC(memDC);
        }
        DeleteDC(dstDC);
    }

    // cropped zones
    if (offsetY < 15)
    {
        HBRUSH brushCrop = CreateSolidBrush(PREVIEW_CROPPED_COLOR);
        HBRUSH brushCropBorder = CreateSolidBrush(PREVIEW_CROPPED_BORDER_COLOR);
        if (brushCrop && brushCropBorder)
        {
            int cropHeight = 1 + ((imgHeight - 105) >> 1);
            RECT cropRect = { 0 };
            cropRect.left = imgRect.left;
            cropRect.right = imgRect.right;
            // top cropping
            cropRect.top = imgRect.top;
            cropRect.bottom = cropRect.top + cropHeight;
            FillRect(hDC, &cropRect, brushCrop);
            FrameRect(hDC, &cropRect, brushCropBorder);
            // bottom cropping
            cropRect.bottom = imgRect.bottom;
            cropRect.top = imgRect.bottom - cropHeight;
            FillRect(hDC, &cropRect, brushCrop);
            FrameRect(hDC, &cropRect, brushCropBorder);
            DeleteObject(brushCrop);
            DeleteObject(brushCropBorder);
        }
        else
        {
            if (brushCrop) DeleteObject(brushCrop);
            if (brushCropBorder) DeleteObject(brushCropBorder);
        }
    }

    // draw preview empty background
    if (offsetX > 0)
    {
        HBRUSH brushBack = CreateSolidBrush(PREVIEW_EMPTY_COLOR);
        if (brushBack)
        {
            RECT emptyZone;
            emptyZone.top = drawZone.top + 15;
            emptyZone.bottom = drawZone.bottom - 15;
            // left empty space
            emptyZone.left = drawZone.left;
            emptyZone.right = drawZone.left + offsetX;
            FillRect(hDC, &emptyZone, brushBack);
            // right empty space
            emptyZone.right = drawZone.right;
            emptyZone.left = drawZone.right - offsetX;
            FillRect(hDC, &emptyZone, brushBack);
            DeleteObject(brushBack);
        }
    }

    // draw preview rectangle
    RECT screenZone = { 0 };
    screenZone.top = drawZone.top + 15;
    screenZone.left = drawZone.left;
    screenZone.right = drawZone.right;
    screenZone.bottom = drawZone.bottom - 15;
    FrameRect(hDC, &screenZone, brushBorder); // border

    // end painting
    DeleteObject(brushBorder);
    ReleaseDC(hWindow, hDC);
    return (INT_PTR)TRUE;
}

#endif
