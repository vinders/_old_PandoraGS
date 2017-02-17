/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : combo box toolset
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include <string>
#include <vector>
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
            /// @class ComboBox
            /// @brief Combo box toolset
            class ComboBox
            {
            public:
                /// @enum event_t
                /// @brief Combo box event types
                enum class event_t : uint32_t
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    selectionChanged = CBN_SELCHANGE
                    #else
                    selectionChanged = 1//...
                    #endif
                };

                /// @brief Initialize list of values in combo box
                /// @param[in] hWindow        Parent window handle
                /// @param[in] resourceId     Combo box resource identifier
                /// @param[in] values         Values inserted in combo box
                /// @param[in] selectedIndex  Default index to select
                /// @returns Success
                static bool initValues(window_handle_t hWindow, const int32_t resourceId, const std::vector<std::wstring>& values, int32_t selectedIndex);
                /// @brief Change list of values in combo box
                /// @param[in] hWindow        Parent window handle
                /// @param[in] resourceId     Combo box resource identifier
                /// @param[in] values         Values inserted in combo box
                /// @param[in] selectedIndex  Default index to select (optional : if not set, keeps same index as previous)
                /// @returns Success
                static bool setValues(window_handle_t hWindow, const int32_t resourceId, const std::vector<std::wstring>& values, int32_t selectedIndex = -1);

                /// @brief Get selected index in combo box
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Combo box resource identifier
                /// @param[out] outIndex   Selected index (output)
                /// @returns Success
                static bool getSelectedIndex(window_handle_t hWindow, const int32_t resourceId, int32_t& outIndex);
                /// @brief Set selected index in combo box
                /// @param[in] hWindow        Parent window handle
                /// @param[in] resourceId     Combo box resource identifier
                /// @param[in] selectedIndex  Selected index
                /// @returns Success
                static bool setSelectedIndex(window_handle_t hWindow, const int32_t resourceId, const int32_t selectedIndex);
            
                /// @brief Get text content of an item
                /// @param[in] hWindow     Parent window handle
                /// @param[in] resourceId  Combo box resource identifier
                /// @param[in] index       Index of item
                /// @returns Text value
                static std::wstring getItemText(window_handle_t hWindow, const int32_t resourceId, const int32_t index);
            };
        }
    }
}