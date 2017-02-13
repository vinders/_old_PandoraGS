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
#include <map>
#include "common.h"

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
            private:
                static int32_t s_activeCount; ///< Number of tab buttons loaded
                static std::map<uint32_t, bitmap_cache_t> s_cache; ///< Icon bitmap cache
                #if _DIALOGAPI == DIALOGAPI_WIN32
                static HFONT s_tabFont; ///< Common font for tab buttons
                #endif

                library_instance_t m_instance;  ///< Current instance handle
                #if _DIALOGAPI == DIALOGAPI_WIN32
                HWND m_tabButtonControl; ///< Tab button object in dialog
                #endif
                uint32_t m_resourceId;  ///< Tab button resource ID
                uint32_t m_tabNumber;   ///< Tab number in tab control
                std::wstring& m_title;  ///< Button title text (reference)
                uint32_t m_bitmapId;    ///< Button icon bitmap
                uint32_t m_iconIndex;   ///< Button icon index in bitmap (0 = no icon)

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
                /// @param[in] window      Window handle
                /// @param[in] offset      Vertical offset for first tab button
                /// @param[in] width       Tab button width
                /// @returns Success
                bool create(window_handle_t window, const uint32_t offset, const uint32_t width);
                /// @brief Close control in dialog
                void close();

                /// @brief Trigger control drawing
                /// @param[in] args  Event arguments
                void paint(paint_control_event_args_t& args);


            private:
                #if _DIALOGAPI == DIALOGAPI_WIN32
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