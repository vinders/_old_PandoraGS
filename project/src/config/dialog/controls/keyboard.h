/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : keyboard information toolset
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
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
            /// @class Screen
            /// @brief Screen information toolset
            class Keyboard
            {
            private: 
                std::vector<std::wstring> m_keyNames; ///< Key names
                std::unordered_map<int32_t, int32_t> m_keyCodesToIndexes; ///< Code to index mapping
                std::vector<int32_t> m_indexesToKeyCodes;                 ///< Index to code mapping

            public:
                /// @brief Create lists of keys
                /// @param[in] isNameListFilled  Fill list of names or ignore it
                Keyboard(const bool isNameListFilled = true);

                /// @brief Get list of key names
                /// @returns List of names
                inline std::vector<std::wstring>& getKeyNames() noexcept
                {
                    return m_keyNames;
                }
                /// @brief Get mapped index based on key code
                /// @param[in] keyCode  Virtual key code
                /// @returns Index
                inline int32_t keyCodeToIndex(const int32_t keyCode) noexcept
                {
                    return (m_keyCodesToIndexes.count(keyCode)) ? m_keyCodesToIndexes[keyCode] : 0;
                }
                /// @brief Get mapped key code based on index
                /// @param[in] index  Index in list of keys
                /// @returns Key code
                inline int32_t indexToKeyCode(const int32_t index) noexcept
                {
                    return (index < static_cast<int32_t>(m_indexesToKeyCodes.size())) ? m_indexesToKeyCodes[index] : 0;
                }
            };
        }
    }
}