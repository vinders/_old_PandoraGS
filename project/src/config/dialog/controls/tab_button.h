/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab button for tab control
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include <unordered_map>
#include "common.h"
#include "dialog.h"

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @namespace config.dialog.controls
        /// Dialog controls
        namespace controls
        {
            class TabControl;

            /// @struct bitmap_cache_t
            /// @brief Bitmap with icons - data cache
            struct bitmap_cache_t
            {
                #if _DIALOGAPI == DIALOGAPI_WIN32
                HBITMAP bitmapData; ///< Bitmap data
                bool isAlphaChannelSet; ///< Alpha channel prepared or not
                #endif
                int32_t iconSize; ///< Icon size
                int32_t maxIndex; ///< Max icon index used
            };


            /// @class TabButton
            /// @brief Tab button for tab control
            class TabButton
            {
            public:
                static const uint32_t tabInterval;
                static const uint32_t tabHeight;
                static const uint32_t firstTabOffsetY;

            private:
                static int32_t s_activeCount; ///< Number of tab buttons loaded
                static std::unordered_map<uint32_t, bitmap_cache_t> s_cache; ///< Icon bitmap cache
                #if _DIALOGAPI == DIALOGAPI_WIN32
                static HFONT s_tabFont; ///< Common font for tab buttons
                #endif

                library_instance_t m_instance;  ///< Current instance handle
                TabControl* m_pParentTabControl; ///< Parent tab control
                #if _DIALOGAPI == DIALOGAPI_WIN32
                static std::unordered_map<HWND, TabButton*> s_tabReferences; ///< Static references to tab button objects
                HWND m_tabButtonControl; ///< Tab button object in dialog
                WNDPROC m_defaultHandler;///< Default button event handler
                #endif
                uint32_t m_resourceId;  ///< Tab button resource ID
                uint32_t m_tabNumber;   ///< Tab number in tab control
                uint32_t m_topOffset;   ///< General top offset

                std::wstring& m_title;  ///< Button title text (reference)
                uint32_t m_bitmapId;    ///< Button icon bitmap
                uint32_t m_iconIndex;   ///< Button icon index in bitmap (0 = no icon)
                uint32_t m_width;       ///< Button width
                bool m_isActive; ///< Status - active or not

            public:
                /// @brief Create tab button
                /// @param[in] instance       Current instance handle
                /// @param[in] title          Tab name
                /// @param[in] resourceId     Tab button resource identifier
                /// @param[in] bmpResourceId  Bitmap with icons (resource ID)
                /// @param[in] icon           Icon index (0 = none)
                /// @param[in] iconSize       Icon size (pixels)
                TabButton(library_instance_t instance, std::wstring& title, const uint32_t resourceId,
                          const uint32_t bmpResourceId, const uint32_t icon, const uint32_t iconSize);
                /// @brief Destrot tab button
                ~TabButton();

                /// @brief Create control in dialog
                /// @param[in] parent         Parent tab control
                /// @param[in] window      Window handle
                /// @param[in] offset      Vertical offset for first tab button
                /// @param[in] width       Tab button width
                /// @returns Success
                bool create(TabControl& parent, window_handle_t window, const uint32_t offset, const uint32_t width);
                /// @brief Close control in dialog
                void close();

                /// @brief Set tab status (active tab or not)
                /// @param[in] isActive  Use as active tab or not
                void setActive(const bool isActive)
                {
                    m_isActive = isActive;
                }

                /// @brief Redraw control
                void invalidate();


            protected:
                #if _DIALOGAPI == DIALOGAPI_WIN32
                /// @brief Get current tab button non-static reference
                /// @param[in] hWindow  Window handle
                static TabButton* getCurrentTabButton(HWND hWindow);

                /// @brief Tab button event handler
                /// @param[in] hWindow  Window handle
                /// @param[in] msg      Event message
                /// @param[in] wParam   Command
                /// @param[in] lParam   Informations
                /// @returns Event result
                static INT_PTR CALLBACK tabButtonEventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);

                /// @brief Draw bitmap icon on tab button
                /// @param[in] hDC         Paint context
                /// @param[in] pTitleRect  Tab title text rectangle
                void TabButton::paintBitmap(HDC hDC, RECT* pTitleRect);

                /// @brief Prepare bitmap with alpha channel (set transparency)
                /// @param hDC     Device context handle
                /// @param img     Bitmap handle
                /// @param width   Bitmap width (pixels)
                /// @param height  Bitmap height (pixels)
                static void TabButton::setBitmapAlphaChannel(HDC hDC, HBITMAP img, LONG width, LONG height);
                #endif
            };
        }
    }
}