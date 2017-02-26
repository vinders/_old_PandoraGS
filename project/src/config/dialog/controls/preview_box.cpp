/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : 
*******************************************************************************/
#include "../../../globals.h"
#include <cstdint>
#include <string>
#include <vector>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#endif
#include "../../../res/resource.h"
#include "common.h"
#include "preview_box.h"
using namespace config::dialog;
using namespace config::dialog::controls;

#define PREVIEW_BORDER_COLOR         RGB(74,90,106)
#define PREVIEW_EMPTY_COLOR          RGB(70, 70, 70)
#define PREVIEW_CROPPED_COLOR        RGB(210,220,230)
#define PREVIEW_CROPPED_BORDER_COLOR RGB(170,180,190)


/// @brief Create preview box with background image
/// @param[in] instance  Current instance handle
/// @param[in] resourceId  Preview zone resource ID
/// @param[in] hWindow   Parent window handle
/// @param[in] width     Preview zone width
/// @param[in] height    Preview zone height
PreviewBox::PreviewBox(library_instance_t instance, window_handle_t hWindow, const int32_t resourceId, const uint32_t width, const uint32_t height,
    const int32_t backgroundResourceId, const uint32_t backWidth, const uint32_t backHeight)
    : m_hParent(hWindow), m_resourceId(resourceId), m_drawZoneWidth(width), m_drawZoneHeight(height), m_backgroundWidth(backWidth), m_backgroundHeight(backHeight)
{
    #if _DIALOGAPI == DIALOGAPI_WIN32
    if (instance == nullptr)
        instance = GetModuleHandle(NULL);
    if (instance)
    {
        m_backgroundBitmap = LoadBitmap(reinterpret_cast<HINSTANCE>(instance), MAKEINTRESOURCE(backgroundResourceId));
        if (m_backgroundBitmap == (HBITMAP)INVALID_HANDLE_VALUE)
            m_backgroundBitmap = 0;
    }
    #endif
}


/// @brief Draw screen ratio preview
/// @param[in] args        DrawItem event arguments
/// @param[in] maxVal      Max value for stretching and cropping
/// @param[in] stretching  Stretch background
/// @param[in] cropping    Crop background
/// @param[in] isMirrored  Mirrored drawing
void PreviewBox::drawScreenPreview(TabPage::event_args_t args, const uint32_t maxVal, uint32_t stretching, uint32_t cropping, const bool isMirrored)
{
    if (args.notifierData == 0 || m_backgroundBitmap == 0)
        return;

    #if _DIALOGAPI == DIALOGAPI_WIN32
    // prepare components
    LPDRAWITEMSTRUCT pDraw = (LPDRAWITEMSTRUCT)(args.notifierData);
    if (pDraw->CtlID != m_resourceId)
        return;
    HDC hDC = pDraw->hDC;
    HBRUSH brushBorder = CreateSolidBrush(PREVIEW_BORDER_COLOR);
    if (hDC == NULL || brushBorder == NULL)
        return;

    if (stretching > maxVal)
        stretching = maxVal;
    if (cropping > maxVal)
        cropping = maxVal;

    // set drawing zone
    int32_t maxOffsetY = (static_cast<int32_t>(m_drawZoneHeight) - static_cast<int32_t>(m_backgroundHeight)) / 2;
    RECT itemRect = pDraw->rcItem;
    RECT drawZone = { 0 };
    drawZone.top = itemRect.top + 2;
    drawZone.left = itemRect.left + 2;
    drawZone.right = drawZone.left + m_drawZoneWidth;
    drawZone.bottom = drawZone.top + m_drawZoneHeight;
    SetBkMode(hDC, TRANSPARENT);
    // reset page background
    FillRect(hDC, &itemRect, GetStockBrush(WHITE_BRUSH));

    // set preview settings - stretching
    int imgWidth = m_backgroundWidth + (((m_drawZoneWidth - m_backgroundWidth) / maxVal) * stretching);
    int imgHeight = m_backgroundHeight;
    // set preview settings - cropping
    if (cropping > 0u)
    {
        double factor = static_cast<double>(m_drawZoneWidth) / static_cast<double>(imgWidth);
        if (factor > 1.005) // > 1 (+ float rounding error) -> is cropped
        {
            if ((int)cropping < maxVal)
            {
                factor = ((static_cast<double>(cropping) / static_cast<double>(maxVal)) * (factor - 1.0)) + 1.0;
                double dWidth = factor * static_cast<double>(imgWidth);
                imgWidth = ((int)dWidth >> 1) << 1; // even
            }
            else
                imgWidth = m_drawZoneWidth;
            double dHeight = factor * (double)imgHeight;
            imgHeight = 1 + (((int)dHeight >> 1) << 1); // odd
        }
    }
    // set preview settings - position
    int offsetX = (m_drawZoneWidth - imgWidth) >> 1;
    int offsetY = (m_drawZoneHeight - imgHeight) >> 1;
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
            SelectObject(memDC, m_backgroundBitmap);
            HBITMAP dstBmp = CreateCompatibleBitmap(memDC, imgWidth, imgHeight); // create empty bitmap
            if (dstBmp)
            {
                // copy resized bitmap
                SelectObject(dstDC, dstBmp);
                StretchBlt(dstDC, (isMirrored) ? imgWidth : 0, 0, (isMirrored) ? -imgWidth - 1 : imgWidth, imgHeight, memDC, 0, 0, m_backgroundWidth, m_backgroundHeight, SRCCOPY);

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
    if (offsetY < maxOffsetY)
    {
        HBRUSH brushCrop = CreateSolidBrush(PREVIEW_CROPPED_COLOR);
        HBRUSH brushCropBorder = CreateSolidBrush(PREVIEW_CROPPED_BORDER_COLOR);
        if (brushCrop && brushCropBorder)
        {
            int cropHeight = 1 + ((imgHeight - m_backgroundHeight) >> 1);
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
            emptyZone.top = drawZone.top + maxOffsetY;
            emptyZone.bottom = drawZone.bottom - maxOffsetY;
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
    screenZone.top = drawZone.top + maxOffsetY;
    screenZone.left = drawZone.left;
    screenZone.right = drawZone.right;
    screenZone.bottom = drawZone.bottom - maxOffsetY;
    FrameRect(hDC, &screenZone, brushBorder); // border

    DeleteObject(brushBorder);
    ReleaseDC(args.window, hDC);
    #endif
}


/// @brief Repaint preview box
void PreviewBox::invalidate()
{
    #if _DIALOGAPI == DIALOGAPI_WIN32
    InvalidateRect(GetDlgItem(m_hParent, IDC_PROSTR_PICTUREBOX), NULL, TRUE);
    RedrawWindow(GetDlgItem(m_hParent, IDC_PROSTR_PICTUREBOX), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
    #endif
}
