/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : preview picture box
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include "tab_page.h"
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
            /// @class PreviewBox
            /// @brief Previex picture box
            class PreviewBox
            {
            private:
                window_handle_t m_hParent;  ///< Parent window handle
                #if _DIALOGAPI == DIALOGAPI_WIN32
                HBITMAP m_backgroundBitmap; ///< Background image (centered)
                #endif
                uint32_t m_resourceId;       ///< Preview box resource identifier
                uint32_t m_drawZoneWidth;    ///< Draw zone width
                uint32_t m_drawZoneHeight;   ///< Draw zone height
                uint32_t m_backgroundWidth;  ///< Background image width
                uint32_t m_backgroundHeight; ///< Background image height


            public:
                /// @brief Create simple preview box
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Preview zone resource ID
                /// @param[in] width       Preview zone width
                /// @param[in] height      Preview zone height
                PreviewBox(window_handle_t hWindow, const int32_t resourceId, const uint32_t width, const uint32_t height)
                    : m_hParent(hWindow), m_resourceId(resourceId), m_drawZoneWidth(width), m_drawZoneHeight(height), m_backgroundWidth(0), m_backgroundHeight(0)
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    m_backgroundBitmap = 0;
                    #endif
                }
                /// @brief Create preview box with background image
                /// @param[in] instance  Current instance handle
                /// @param[in] resourceId  Preview zone resource ID
                /// @param[in] hWindow   Parent window handle
                /// @param[in] width     Preview zone width
                /// @param[in] height    Preview zone height
                PreviewBox(library_instance_t instance, window_handle_t hWindow, const int32_t resourceId, const uint32_t width, const uint32_t height,
                    const int32_t backgroundResourceId, const uint32_t backWidth, const uint32_t backHeight);
                /// @brief Destroy preview box
                ~PreviewBox()
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    if (m_backgroundBitmap)
                        DeleteObject(m_backgroundBitmap);
                    #endif
                }


                /// @brief Draw screen ratio preview
                /// @param[in] args        DrawItem event arguments
                /// @param[in] maxVal      Max value for stretching and cropping
                /// @param[in] stretching  Stretch background
                /// @param[in] cropping    Crop background
                /// @param[in] isMirrored  Mirrored drawing
                void drawScreenPreview(TabPage::event_args_t args, uint32_t maxVal, uint32_t stretching, uint32_t cropping, bool isMirrored);
                /// @brief Repaint preview box
                void invalidate();
            };
        }
    }
}