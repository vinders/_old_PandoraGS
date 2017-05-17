/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : advanced file reader
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <ifstream>
#include "file_io.h"
#include "../memory/flag_set.hpp"

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.io
    /// Input/output utilities
    namespace io
    {
        /// @class FileReader
        /// @brief Advanced file reader - with encoder specialization
        /// @see FileIO
        template <FileIO::file_encoder_t Encoder, bool checkConcurrency = true>
        class FileReader : public FileIO
        {
        public:
            /// @enum file_mode_t
            /// @brief File opening mode
            enum class file_mode_t : uint32_t
            {
                start = 0u,
                atEnd = 1u
            };
            /// @enum flag_t
            /// @brief File reader flags - can be combined with |
            enum flag_t : uint32_t
            {
                FileReader_toUppercase = 0x20,  ///< Convert strings to uppercase (only in text mode)
                FileReader_toLowercase = 0x21,  ///< Convert strings to lowercase (only in text mode)
                FileReader_trimLeft = 0x22,     ///< Remove leading white-space characters for each operation (only in text mode)
                FileReader_trimRight = 0x24     ///< Remove trailing white-space characters for each operation (only in text mode)
            };
            
            
            /// @brief Create file reader instance (specialized)
            FileReader() noexcept : FileIO(), m_fileLength(0u), m_formatFlags(0u) {}
            /// @brief Move file reader instance
            /// @param[in] other  Other instance
            FileReader(FileReader&& other) : 
                FileIO(other), 
                m_fileStream(std::move(other.m_fileStream)), 
                m_fileLength(other.m_floatDecimalPrecision), 
                m_formatFlags(other.m_formatFlags), 
                m_errorBuffer(std::move(other.m_errorBuffer)) {}
            // no copy allowed
            FileReader(const FileReader& other) = delete;
            /// @brief Destroy file reader instance
            ~FileReader()
            {
                close();
            }
            
            /// @brief Assign moved file reader instance
            /// @param[in] other  Other instance
            /// @returns Moved instance
            FileReader& operator=(FileReader&& other) noexcept
            {
                FileIO::operator=(other);
                m_fileStream = std::move(other.m_fileStream); 
                m_fileLength = other.m_fileLength;
                m_formatFlags = other.m_formatFlags;
                m_errorBuffer = std::move(other.m_errorBuffer);
            }
            // no copy allowed
            FileReader& operator=(const FileReader& other) = delete;
            /// @brief Swap file reader instances
            /// @param[in] other  Other instance
            void swap(FileReader& other) noexcept
            {
                FileIO::swap(other);
                m_fileStream.swap(other.m_fileStream); 
                std::swap(m_fileLength, other.m_fileLength);
                m_formatFlags.swap(other.m_formatFlags);
                m_errorBuffer.swap(other.m_errorBuffer);
            }
        
        
            // -- File management --
            
            /// @brief Open output file
            /// @param[in] filePath     Source file path
            /// @param[in] openMode     File opening mode (optional)
            /// @param[in] formatFlags  Formatting flags, only for text mode (optional, see enum flag_t)
            /// @returns Success (use getLastErrorMessage() if false)
            bool open(const std::wstring filePath, const file_mode_t openMode = file_mode_t::start, const flag_t formatFlags = 0) noexcept;
            /// @brief Close input file
            void close();
            
            /// @brief Change reader position in current file
            /// @param[in] whence  Reference position to move from
            /// @param[in] offset  Offset, based on reference (bytes)
            /// @returns Current instance
            /// @throws failure  Seek failure
            inline FileReader& seek(const seek_reference_t whence, const int32_t offset)
            {
                lock<checkConcurrency>();
                seek_noLock(whence, offset);
                unlock<checkConcurrency>();
                return *this;
            }
            /// @brief Check if a file is open
            /// @returns File open (true) or not
            inline bool isOpen() const noexcept
            {
                return m_fileStream.is_open();
            }
            /// @brief Check if current position is at the end of the file
            /// @returns End of file reached (true) or not
            inline bool isEof() const noexcept
            {
                return m_fileStream.eof();
            }
            
            /// @brief Get most recent error message (if any)
            /// @returns Error message
            inline const std::string const& getLastErrorMessage() const noexcept
            {
                return m_errorBuffer;
            }
            /// @brief Get total file length (bytes)
            /// @returns File length
            inline uint64_t size() const noexcept
            {
                return m_fileLength;
            }
            /// @brief Check if current file is empty
            /// @returns Empty (true) or not
            inline bool isEmpty() const noexcept
            {
                return (size() == 0uLL);
            }
            
            
            // -- Reader settings --
            
            /// @brief Get configured file encoding
            /// @brief File encoding type
            inline FileIO::file_encoder_t getEncoding() const noexcept
            {
                return Encoder;
            }
            
            /// @brief Get formatting flags
            /// @brief Flag set
            inline flag_t getFormatFlags() const noexcept
            {
                return m_formatFlags.data();
            }
            /// @brief Set formatting flags
            /// @param[in] formatFlags  Flag set
            /// @returns Current instance
            inline FileReader& setFormatFlags(const flag_t formatFlags) noexcept
            {
                if (isOpen())
                {
                    lock<checkConcurrency>();
                    setFormatFlags_noLock(formatFlags);
                    unlock<checkConcurrency>();
                }
                return *this;
            }
            
            
            // -- Input operations --
            
            FileReader& get(bool val);
            FileReader& operator>>(bool val);
            FileReader& get(char val);
            FileReader& operator>>(char val);
            FileReader& get(uint16_t val);
            FileReader& operator>>(uint16_t val);
            FileReader& get(int16_t val);
            FileReader& operator>>(int16_t val);
            FileReader& get(uint32_t val);
            FileReader& operator>>(uint32_t val);
            FileReader& get(int32_t val);
            FileReader& operator>>(int32_t val);
            FileReader& get(uint64_t val);
            FileReader& operator>>(uint64_t val);
            FileReader& get(int64_t val);
            FileReader& operator>>(int64_t val);
            FileReader& get(float val);
            FileReader& operator>>(float val);
            FileReader& get(double val);
            FileReader& operator>>(double val);
            FileReader& get(long double val);
            FileReader& operator>>(long double val);
            
            FileReader& readStream(uint8_t* stream, size_t maxSize);
            
            template <FileIO::string_encoder_t DestEncoder = FileIO::string_encoder_t::ansi>
            FileReader& read(char* stream, size_t maxSize);
            template <FileIO::wstring_encoder_t DestEncoder = FileIO::wstring_encoder_t::utf16>
            FileReader& read(wchar_t* strVal, size_t maxSize);
            
            template <FileIO::string_encoder_t DestEncoder = FileIO::string_encoder_t::ansi>
            FileReader& read(std::string& strVal);
            template <FileIO::string_encoder_t DestEncoder = FileIO::string_encoder_t::ansi>
            FileReader& operator>>(std::string& val);
            template <FileIO::wstring_encoder_t DestEncoder = FileIO::wstring_encoder_t::utf16>
            FileReader& read(std::wstring& strVal);
            template <FileIO::wstring_encoder_t DestEncoder = FileIO::wstring_encoder_t::utf16>
            FileReader& operator>>(std::wstring& val);
            
            template <FileIO::string_encoder_t DestEncoder = FileIO::string_encoder_t::ansi>
            FileReader& readLine(char* strVal, size_t maxSize);
            template <FileIO::wstring_encoder_t DestEncoder = FileIO::wstring_encoder_t::utf16>
            FileReader& readLine(wchar_t* strVal, size_t maxSize);
            
            template <FileIO::string_encoder_t DestEncoder = FileIO::string_encoder_t::ansi>
            FileReader& readLine(std::string& strVal);
            template <FileIO::wstring_encoder_t DestEncoder = FileIO::wstring_encoder_t::utf16>
            FileReader& readLine(std::wstring& strVal);

            
            
        private:
            /// @brief Change reader position in current file
            /// @param[in] whence  Reference position to move from
            /// @param[in] offset  Offset, based on reference (bytes)
            /// @throws failure  Seek failure
            void seek_noLock(const FileIO::seek_reference_t whence, const int32_t offset)
            {
                switch (whence)
                {
                    case FileIO::seek_reference_t::begin:
                        m_fileStream.seekp((Encoder != FileIO::file_encoder_t::utf8_bom) ? offset : offset + 3, ios_base::beg);
                        break;
                    case FileIO::seek_reference_t::end:
                        m_fileStream.seekg(offset, ios_base::end); 
                        break;
                    case FileIO::seek_reference_t::cur
                    default: 
                        m_fileStream.seekg(offset, ios_base::cur); 
                        break;
                }
            }
            
            /// @brief Set formatting flags - no lock
            /// @param[in] formatFlags  Flag(s)
            inline void setFormatFlags_noLock(const flag_t formatFlags) noexcept
            {
                m_formatFlags = utils::memory::flag_set<flag_t>(formatFlags);
            }
            
            
        private:
            std::ifstream m_fileStream; ///< File input stream
            uint64_t m_fileLength;      ///< Total file length (bytes)
            std::string m_errorBuffer;  ///< Last opening error message
            
            utils::memory::flag_set<flag_t> m_formatFlags; ///< Formatting flags (text mode)
        };
    }
}
