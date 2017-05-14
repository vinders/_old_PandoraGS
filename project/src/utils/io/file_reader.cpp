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
    
    //...
    
    unlock<checkConcurrency>();
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
    
    unlock<checkConcurrency>();
}

//https://www.codeproject.com/Articles/38242/Reading-UTF-8-with-C-streams
