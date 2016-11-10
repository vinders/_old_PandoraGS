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
#ifndef _WIN_TOOLBOX_HPP
#define _WIN_TOOLBOX_HPP
#include <stdint.h>
#include <string>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#define COMBOBOX_USE_PREVIOUS_INDEX -1

// Configuration dialog page - view
class WinToolbox
{
private: 
    static HBRUSH s_hBackgroundBrush;
    static HBITMAP s_hPreviewStretchingBitmap;
public:
    /// <summary>Show/hide page</summary>
    /// <param name="itemId">Item associated with tooltip (ID)</param>
    /// <param name="hWindow">Parent dialog handle</param>
    /// <param name="phInst">Window instance</param>
    /// <param name="pText">Tooltip text</param>
    /// <returns>Tooltip handle</returns>
    static HWND createToolTip(int itemId, HWND hWindow, HINSTANCE* phInst, PTSTR pText);
    /// <summary>Show/hide page</summary>
    /// <param name="hTooltip">Tooltip handle</param>
    /// <param name="itemId">Item associated with tooltip (ID)</param>
    /// <param name="hWindow">Parent dialog handle</param>
    /// <param name="pText">Tooltip text</param>
    static void updateToolTip(HWND hTooltip, int itemId, HWND hWindow, PTSTR pText);

    /// <summary>Fill combobox and select an item</summary>
    /// <param name="hCombobox">Combobox handle</param>
    /// <param name="selectedIndex">Index to select (or -1 to keep previous index)</param>
    /// <param name="pListItems">List of combobox entries</param>
    /// <param name="itemsNb">Length of the list</param>
    /// <returns>Success</returns>
    static inline bool setComboboxValues(HWND hCombobox, int selectedIndex, std::wstring* pListItems, int itemsNb)
    {
        if (!hCombobox) // check if combobox exists
            return false;

        // get previous index
        if (selectedIndex == COMBOBOX_USE_PREVIOUS_INDEX)
        {
            selectedIndex = ComboBox_GetCurSel(hCombobox);
            if (selectedIndex < 0)
                selectedIndex = 1;
        }
        // add list items
        ComboBox_ResetContent(hCombobox);
        for (int i = 0; i < itemsNb; ++i)
            ComboBox_AddString(hCombobox, (LPCTSTR)pListItems[i].c_str());
        // select item
        ComboBox_SetCurSel(hCombobox, selectedIndex);
        return true;
    }

    /// <summary>Set upscaling factors list</summary>
    /// <param name="hCombobox">Combobox handle</param>
    /// <param name="selection">Selected index</param>
    /// <param name="defaultSelection">Default index to select (if no selection)</param>
    /// <param name="maxFactor">Max factor value</param>
    /// <returns>Selected index</returns>
    static inline uint32_t setUpscalingFactors(HWND hCombobox, uint32_t selection, uint32_t defaultSelection, uint32_t maxFactor)
    {
        if (hCombobox)
        {
            if (selection < 1 || selection > maxFactor || selection == 6 || selection == 7)
                selection = defaultSelection;
            ComboBox_AddString(hCombobox, (LPCTSTR)L"1x");
            ComboBox_AddString(hCombobox, (LPCTSTR)L"2x");
            ComboBox_AddString(hCombobox, (LPCTSTR)L"3x");
            ComboBox_AddString(hCombobox, (LPCTSTR)L"4x");
            if (maxFactor >= 5u)
            {
                ComboBox_AddString(hCombobox, (LPCTSTR)L"5x");
                if (maxFactor >= 8u)
                    ComboBox_AddString(hCombobox, (LPCTSTR)L"8x");
            }
            ComboBox_SetCurSel(hCombobox, (selection == 8) ? 5 : (selection - 1));
        }
        return selection;
    }

    /// <summary>Window event handler</summary>
    /// <param name="hDC">Context handle</param>
    /// <param name="img">Bitmap handle</param>
    /// <param name="width">Bitmap width (px)</param>
    /// <param name="height">Bitmap height (px)</param>
    static inline void prepareBitmapAlphaChannel(HDC hDC, HBITMAP img, LONG width, LONG height)
    {
        if (img)
        {
            // prepare icons - set information
            BITMAPINFO bmi;
            ZeroMemory(&bmi, sizeof(BITMAPINFO));
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = width;
            bmi.bmiHeader.biHeight = height;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4;
            // prepare icons - pre-multiply alpha channel
            BITMAP bm = { 0 };
            GetObject(img, sizeof(bm), &bm);
            GetDIBits(hDC, img, 0, bm.bmHeight, NULL, &bmi, DIB_RGB_COLORS); // pass effective dimensions to bmi
            LPBYTE pBitData = (LPBYTE)LocalAlloc(LPTR, bm.bmWidth * bm.bmHeight * sizeof(DWORD));
            if (pBitData != NULL)
            {
                LPBYTE pData = pBitData;
                GetDIBits(hDC, img, 0, bm.bmHeight, pData, &bmi, DIB_RGB_COLORS); // copy image to array
                for (int y = 0; y < bm.bmHeight; y++)
                {
                    for (int x = 0; x < bm.bmWidth; x++)
                    {
                        pData[0] = (BYTE)((DWORD)pData[0] * pData[3] / 255); // for AlphaBlend
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

    /// <summary>Page drawing event handler</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="msg">Event message</param>
    /// <param name="wParam">Command</param>
    /// <param name="lParam">Informations</param>
    /// <param name="color">Background color (global to all pages)</param>
    /// <returns>Action code</returns>
    static inline INT_PTR pageDrawingEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam, COLORREF color)
    {
        switch (msg)
        {
            case WM_PAINT: // page background color
            {
                PAINTSTRUCT paint;
                HDC hDC = BeginPaint(hWindow, &paint);
                RECT rect;
                if (!s_hBackgroundBrush)
                    s_hBackgroundBrush = CreateSolidBrush(color);
                if (s_hBackgroundBrush)
                {
                    GetClientRect(hWindow, &rect);
                    FillRect(hDC, &rect, s_hBackgroundBrush);
                }
                EndPaint(hWindow, &paint);
                if (hDC)
                    ReleaseDC(hWindow, hDC);
                return (INT_PTR)TRUE; break;
            }
            case WM_CTLCOLORSTATIC: // controls text color
            {
                HDC hdcStatic = (HDC)wParam;
                SetTextColor(hdcStatic, RGB(0, 0, 0));
                SetBkColor(hdcStatic, color);
                if (!s_hBackgroundBrush)
                    s_hBackgroundBrush = CreateSolidBrush(color);
                return (LRESULT)s_hBackgroundBrush; break;
            }
            default: return DefWindowProc(hWindow, msg, wParam, lParam); break;
        }
        return (INT_PTR)FALSE;
    }
    /// <summary>Delete page background brush</summary>
    static void destroyPageBackgroundBrush();

    /// <summary>Screen stretching preview drawing</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="hDrawItem">Drawing destination box</param>
    /// <param name="stretching">Stretching value</param>
    /// <param name="cropping">Cropping value</param>
    /// <param name="isMirrored">Mirror indicator</param>
    static void drawScreenStretchingPreview(HWND hWindow, HWND hDrawItem, uint32_t stretching, uint32_t cropping, bool isMirrored);
};

#endif
#endif
