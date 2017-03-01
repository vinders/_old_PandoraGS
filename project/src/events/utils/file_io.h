/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : file IO toolbox
*******************************************************************************/
#pragma once

#include <string>

/// @namespace events
/// Event management
namespace events
{
    /// @namespace event.utils
    /// Event management utilities
    namespace utils
    {
        /// @class FileIO
        /// @brief File IO toolbox
        class FileIO
        {
        public:
            /// @brief Get file path with write access (same as plugin or home path)
            /// @returns File path
            static std::string getWritableFilePath() noexcept;
            /// @brief Get file path with write access (same as plugin or home path)
            /// @returns File path as wide string
            static std::wstring getWritableFileWidePath() noexcept;
        };
    }
}
