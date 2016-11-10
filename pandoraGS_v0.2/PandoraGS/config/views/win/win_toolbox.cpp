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
#define PREVIEW_BORDER_CROP_COLOR RGB(176,79,79)

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

/// <summary>Screen stretching preview drawing</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="hDrawItem">Drawing destination box</param>
/// <param name="stretching">Stretching value</param>
/// <param name="cropping">Cropping value</param>
/// <param name="isMirrored">Mirror indicator</param>
void WinToolbox::drawScreenStretchingPreview(HWND hWindow, HWND hDrawItem, uint32_t stretching, uint32_t cropping, bool isMirrored)
{
    static bool isBusy = false;
    if (isBusy || !hDrawItem || stretching > CFG_RATIO_MAX || cropping > CFG_RATIO_MAX)
        return;
    // load bitmap
    if (!s_hPreviewStretchingBitmap)
    {
        HINSTANCE hInst = PandoraGS::getInstance();
        if (hInst == NULL)
            hInst = GetModuleHandle(NULL);
        if (hInst == NULL)
            return;
        HBITMAP s_hPreviewStretchingBitmap = (HBITMAP)LoadImage(NULL, MAKEINTRESOURCE(IDB_RATIO), 0, 180, 135, LR_DEFAULTCOLOR);
        if (!s_hPreviewStretchingBitmap)
            return;
    }
    isBusy = true;

    // set preview settings - stretching
    int imgWidth = 140 + 5 * (int)stretching;
    int imgHeight = 105;
    // set preview settings - cropping
    if (cropping > 0u)
    {
        double factor = 180.0 / (double)imgWidth;
        if (factor > 0.01) // not fully stretched
        {
            if ((int)cropping < CFG_RATIO_MAX)
                factor = (((double)cropping / 8.0f) * (factor - 1.0f)) + 1.0f;
            double dWidth = factor * (double)imgWidth;;
            double dHeight = factor * (double)imgHeight;;
            imgWidth = (int)dWidth;
            imgHeight = (int)dHeight;
            imgWidth = (imgWidth >> 1) << 1; // even
            imgHeight = 1 + ((imgHeight >> 1) << 1); // odd
        }
    }
    // set preview settings - mirroring
    if (isMirrored)
        imgWidth *= -1;
    // set preview settings - position
    int offsetX = (180 - imgWidth) >> 1;
    int offsetY = (135 - imgHeight) >> 1;

    // drawing - settings
    RECT drawZone;
    GetClientRect(hDrawItem, &drawZone);
    // drawing - resources
    HDC hDC = GetDC(hWindow);
    // drawing bitmap
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = imgWidth;
    bmi.bmiHeader.biHeight = imgHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 3;
    VOID* pvBits = NULL;
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hBmp = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
    SelectObject(hMemDC, hBmp);
    if (StretchBlt(hMemDC, 0, 0, imgWidth, imgHeight, hDC, 0, 0, 180, 135, 0u) == FALSE)
    {
        DeleteObject(hBmp);
        DeleteDC(hMemDC);
        ReleaseDC(hWindow, hDC);
        isBusy = false; return;
    }
    BITMAP bm = { 0 };
    GetObject(hBmp, sizeof(bm), &bm);
    LPBYTE pBitData = (LPBYTE)LocalAlloc(LPTR, bm.bmWidth * bm.bmHeight * sizeof(DWORD));
    if (pBitData != NULL)
    {
        LPBYTE pData = pBitData;
        GetDIBits(hDC, hBmp, 0, bm.bmHeight, pData, &bmi, DIB_RGB_COLORS); // copy image to array
        int maxTop = 15 - offsetY;
        int minBottom = bm.bmHeight - offsetY;
        for (int x = 0; x < bm.bmWidth; x++)
        {
            for (int y = 0; y < maxTop; y++)
            {
                pData[0] = (BYTE)(((DWORD)pData[0] + 255) / 2);
                pData[1] = (BYTE)(((DWORD)pData[1] + 255) / 2);
                pData[2] = (BYTE)(((DWORD)pData[2] + 255) / 2);
                pData += 3;
            }
            for (int y = minBottom; y < bm.bmHeight; y++)
            {
                pData[0] = (BYTE)(((DWORD)pData[0] + 255) / 2);
                pData[1] = (BYTE)(((DWORD)pData[1] + 255) / 2);
                pData[2] = (BYTE)(((DWORD)pData[2] + 255) / 2);
                pData += 3;
            }
        }
        SetDIBits(hDC, hBmp, 0, bm.bmHeight, pBitData, &bmi, DIB_RGB_COLORS); // copy array to image
        LocalFree(pBitData);
        SendDlgItemMessage(hWindow, IDC_PROSTR_PICTUREBOX, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
    }
    DeleteObject(hBmp);
    DeleteDC(hMemDC);

    if (offsetX > 0) // gray background
    {
        RECT emptyZone;
        emptyZone.top = drawZone.top + 15;
        emptyZone.bottom = drawZone.bottom - 15;
        HBRUSH brushBack = CreateSolidBrush(RGB(40, 40, 40));
        emptyZone.left = drawZone.left;
        emptyZone.right = drawZone.left + offsetX;
        FillRect(hDC, &emptyZone, brushBack);
        emptyZone.right = drawZone.right;
        emptyZone.left = drawZone.right - offsetX;
        FillRect(hDC, &emptyZone, brushBack);
        DeleteObject(brushBack);
    }

    RECT line; // side borders
    line.top = drawZone.top + 16;
    line.bottom = drawZone.top - 16;
    HBRUSH brushBorder = CreateSolidBrush(PREVIEW_BORDER_COLOR);
    line.left = drawZone.left;
    line.right = line.left + 1;
    FillRect(hDC, &line, brushBorder);
    line.right = drawZone.right;
    line.left = line.right - 1;
    FillRect(hDC, &line, brushBorder);

    line.left = drawZone.left; // top/bottom borders
    line.right = drawZone.right;
    line.top = drawZone.top + 15; 
    line.bottom = line.top + 1;
    if (cropping == 0)
    {
        FillRect(hDC, &line, brushBorder);
        line.bottom = drawZone.bottom - 15;
        line.top = line.bottom - 1;
        FillRect(hDC, &line, brushBorder);
    }
    else
    {
        HBRUSH brushBorderCrop = CreateSolidBrush(PREVIEW_BORDER_CROP_COLOR);
        FillRect(hDC, &line, brushBorderCrop);
        line.bottom = drawZone.bottom - 15;
        line.top = line.bottom - 1;
        FillRect(hDC, &line, brushBorderCrop);
        DeleteObject(brushBorderCrop);
    }
    DeleteObject(brushBorder);

    InvalidateRect(hDrawItem, NULL, FALSE);
    UpdateWindow(hDrawItem);
    ReleaseDC(hWindow, hDC);
    isBusy = false;
}

#endif
