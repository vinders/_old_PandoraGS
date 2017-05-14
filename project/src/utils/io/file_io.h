/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : file input/output and encoding - base
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <codecvt>
#include <locale>
#include <vector>
#include "../thread/thread_mutex.hpp"
#include "../string_encoder.h"

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
                begin = 0u,   ///< reference from beginning of the file
                current = 1u, ///< reference from current read/write position
                end = 2u      ///< reference from end of the file
            };
        
            /// @enum file_encoder_t
            /// @brief File encoding types
            enum class file_encoder_t : uint32_t
            {
                binary = 0u,   ///< binary mode (depends on endianness)
                ansi = 1u,     ///< text mode - ANSI Windows-1252 (Western European)
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
            
            /// @brief Get system home directory
            /// @returns Home directory path
            static std::wstring getHomeDirectoryPath() noexcept;
            /// @brief Get working directory
            /// @returns Working directory path
            static std::wstring getWorkingDirectoryPath() noexcept;

            
            
        protected:
            /// @brief Default constructor
            FileIO() {}
            /// @brief Move instance
            /// @param[in] other  Other instance
            FileIO(FileIO&& other) : m_fileLock(std::move(other.m_fileLock)) {}
            /// @brief Swap instances
            /// @param[in] other  Other instance
            void swap(FileWriter& other) noexcept
            {
                ThreadSpinLock tmp = std::move(other.m_fileLock);
                other.m_fileLock = std::move(m_fileLock);
                m_fileLock = std::move(tmp);
            }
            
            
            // -- Concurrency protection --
            
            /// @brief Lock file access
            template <bool checkConcurrency = true>
            void lock();
            /// @brief Try to lock file access - with timeout
            template <bool checkConcurrency = true>
            void lock(uint32_t timeout);
            /// @brief Try to lock file access - no wait
            template <bool checkConcurrency = true>
            void tryLock();
            /// @brief Unlock file access
            template <bool checkConcurrency = true>
            void unlock();

            
        private:
            utils::thread::ThreadMutex m_fileLock;  ///< Concurrency protection lock
        };
        
        
        
        // -- Concurrency protection --
        
        /// @brief Lock file access
        template <>
        inline void FileIO::lock<true>()
        {
            m_fileLock.lock();
        }
        /// @brief Try to lock file access - with timeout (milliseconds)
        template <>
        inline void FileIO::lock<true>(uint32_t timeout)
        {
            m_fileLock.lock(timeout);
        }
        /// @brief Try to lock file access - no wait
        template <>
        inline void FileIO::tryLock<true>()
        {
            m_fileLock.tryLock();
        }
        /// @brief Unlock file access
        template <>
        inline void FileIO::unlock<true>()
        {
            m_fileLock.unlock();
        }
        
        /// @brief Lock file access
        template <>
        inline void FileIO::lock<false>() { (void)0; }
        /// @brief Try to lock file access - with timeout
        template <>
        inline void FileIO::lock<false>(uint32_t timeout) { (void)0; }
        /// @brief Try to lock file access - no wait
        template <>
        inline void FileIO::tryLock<false>() { (void)0; }
        /// @brief Unlock file access
        template <>
        inline void FileIO::unlock<false>() { (void)0; }
    }
}
