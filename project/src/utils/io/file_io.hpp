/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : file input/output - base
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include "../memory/thread_spin_lock.hpp"

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.io
    /// Input/output utilities
    namespace io
    {
        /// @class FileReader
        /// @brief Advanced file reader
        class FileIO
        {
        public:
            /// @enum seek_reference_t
            /// @brief Reference point for seek operations
            enum class seek_reference_t : uint32_t
            {
                start = 0u,   ///< reference from beginning of the file
                current = 1u, ///< reference from current read/write position
                end = 2u      ///< reference from end of the file
            };
        
            /// @enum file_encoder_t
            /// @brief File encoding types
            enum class file_encoder_t : uint32_t
            {
                binary = 0u,   ///< binary mode (depends on endianness)
                ansi = 1u,     ///< text mode - ASCII/ANSI
                utf8 = 2u,     ///< text mode - UTF-8 (standard)
                utf8_bom = 3u, ///< text mode - UTF-8 with byte order mark
                utf16_le = 4u, ///< text mode - UTF-16 little endian (standard)
                utf16_be = 5u  ///< text mode - UTF-16 big endian
            };
            /// @enum string_encoder_t
            /// @brief String encoding types
            enum class string_encoder_t : uint32_t
            {
                ansi = 1u,
                utf8 = 2u
            };
            /// @enum wstring_encoder_t
            /// @brief Wide string encoding types
            enum class wstring_encoder_t : uint32_t
            {
                utf16 = 4u,
                ucs2 = 6u
            };
            
            /// @brief Default constructor
            FileIO() {}
            
        protected:
            ThreadSpinLock m_fileLock;  ///< File single-access protection lock
        };
    }
}
