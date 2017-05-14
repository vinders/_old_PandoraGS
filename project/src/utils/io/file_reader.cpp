/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : advanced file reader
*******************************************************************************/
#include <cstddef>
#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <ifstream>
#include "file_reader.h"
using namespace utils::io;

#define LOCK_TIMEOUT 2000


/// @brief Open output file
/// @param[in] filePath     Source file path
/// @param[in] openMode     File opening mode (optional)
/// @param[in] formatFlags  Formatting flags, only for text mode (optional, see enum flag_t)
/// @returns Success (use getLastErrorMessage() if false)
bool FileReader::open(const std::wstring filePath, const file_mode_t openMode, const flag_t formatFlags) noexcept
{
    lock<checkConcurrency>(LOCK_TIMEOUT);
    if (isOpen())
    {
        close(); // lock per thread -> no deadlock if close() locks it too
    }
    
    // open file
    ios::openmode mode;
    if (Encoder != FileIO::file_encoder_t::binary)
        mode = ios::in | ios::ate;
    else
        mode = ios::in | ios::binary | ios::ate;
    
    #ifdef _WINDOWS
    m_fileStream.open(filePath.c_str(), mode);
    #else
    std::string pathUtf8 = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(filePath);
    m_fileStream.open(pathUtf8.c_str(), mode);
    #endif
    
    bool isSuccess = isOpen();
    if (isSuccess)
    {
        // get file size + set position
        int64_t length = m_fileStream.tellg();
        m_fileLength = (length >= 0) ? static_cast<uint64_t>(length) : m_fileLength = 0uLL;
        if (openMode == file_mode_t::start)
            seek_noLock(FileIO::seek_reference_t::begin, 0);
        
        // set formatting flags
        setFormatFlags_noLock(formatFlags);
        // set float precision
        m_defaultPrecision = m_fileStream.precision();
        if (m_floatDecimalPrecision != 0u)
            m_fileStream.precision(m_floatDecimalPrecision);
    }
    unlock<checkConcurrency>();
    return isSuccess;
}

/// @brief Close input file
void FileReader::close()
{
    lock<checkConcurrency>(LOCK_TIMEOUT);
    if (!isOpen())
    {
        unlock<checkConcurrency>();
        return;
    }
    
    m_fileStream.close();
    m_fileLength = 0uLL;
    unlock<checkConcurrency>();
}

//https://www.codeproject.com/Articles/38242/Reading-UTF-8-with-C-streams
