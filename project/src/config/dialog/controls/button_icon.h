/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : button icon - background image for button
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
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
            /// @class ButtonIcon
            /// @brief Button icon (background image for button)
            class ButtonIcon
            {
            private:
                #if _DIALOGAPI == DIALOGAPI_WIN32
                HANDLE m_hIcon; ///< Icon handle
                #else
                uint32_t m_hIcon; ///< Icon handle
                #endif


            public:
                /// @brief Create icon for buttons
                /// @param[in] instance         Current instance handle
                /// @param[in] imageResourceId  Image resource identifier
                /// @param[in] iconWidth        Icon width
                /// @param[in] iconHeight       Icon height
                ButtonIcon(library_instance_t instance, const int32_t imageResourceId, const uint32_t iconWidth, const uint32_t iconHeight);
                /// @brief Destroy button icon
                ~ButtonIcon();

                /// @brief Assign icon to a button
                /// @param[in] hWindow           Parent container
                /// @param[in] buttonResourceId  Identifier of assigned button
                /// @param[in] placeHolder       Place-holder text, if icon assignment fails
                void addIconToButton(window_handle_t hWindow, const int32_t buttonResourceId, std::wstring placeHolder);
            };
        }
    }
}