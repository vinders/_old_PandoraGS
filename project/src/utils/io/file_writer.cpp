/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : advanced file writer
*******************************************************************************/
#include <cstddef>
#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <ofstream>
#include "file_writer.h"
using namespace utils::io;

#define LOCK_TIMEOUT 2000


/// @brief Open output file
/// @param[in] filePath     Destination file path
/// @param[in] openMode     File opening mode (optional)
/// @param[in] formatFlags  Formatting flags (optional, see enum flag_t)
/// @returns Success (use getLastErrorMessage() if false)
bool FileWriter::open(const std::wstring filePath, const file_mode_t openMode, const flag_t formatFlags) noexcept
{
    lock<checkConcurrency>(LOCK_TIMEOUT);
    if (isOpen())
    {
        close(); // lock per thread -> no deadlock if close() locks it too
    }
    
    // open file
    ios::openmode mode;
    if (Encoder != FileIO::file_encoder_t::binary)
        mode = ios::out;
    else
        mode = ios::out | ios::binary;
    if (openMode == file_mode_t::append)
        mode |= ios::app;
    
    #ifdef _WINDOWS
    m_fileStream.open(filePath.c_str(), mode);
    #else
    std::string pathUtf8 = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(filePath);
    m_fileStream.open(pathUtf8.c_str(), mode);
    #endif
    
    bool isSuccess = isOpen();
    if (isSuccess)
    {
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

/// @brief Close output file and flush buffer
void FileWriter::close()
{
    lock<checkConcurrency>(LOCK_TIMEOUT);
    if (!isOpen())
    {
        unlock<checkConcurrency>();
        return;
    }
    
    flush_noLock();
    m_fileStream.close();
    unlock<checkConcurrency>();
}


//-- Writer settings --

/// @brief Set formatting flags - no lock
/// @param[in] formatFlags  Flag(s)
void FileWriter::setFormatFlags_noLock(const flag_t formatFlags) noexcept
{
    m_formatFlags = utils::memory::flag_set<flag_t>(formatFlags);
    
    // binary + text
    if (formatFlags.isAnyFlag(FileWriter_flushBufferOnWrite)) // auto-flush on each operation
        m_fileStream.setf(std::ios_base::unitbuf);
    else
        m_fileStream.unsetf(std::ios_base::unitbuf);
    
    // text only
    if (Encoder != FileIO::file_encoder_t::binary) 
    {
        if (formatFlags.isAnyFlag(FileWriter_boolAsAlpha)) // booleans as alpha strings
            m_fileStream.setf(std::ios_base::boolalpha);
        else
            m_fileStream.unsetf(std::ios_base::boolalpha);
        if (formatFlags.isAnyFlag(FileWriter_showPositiveSign)) // all integers with sign
            m_fileStream.setf(std::ios_base::showpos);
        else
            m_fileStream.unsetf(std::ios_base::showpos);
        if (formatFlags.isAnyFlag(FileWriter_intToHex)) // integer representation
            m_fileStream.setf(std::ios::hex, std::ios::basefield);
        else if (formatFlags.isAnyFlag(FileWriter_intToOctal))
            m_fileStream.setf(std::ios::oct, std::ios::basefield);
        else
            m_fileStream.setf(std::ios::dec, std::ios::basefield);
        if (formatFlags.isAnyFlag(FileWriter_floatFixed)) // float representation
            m_fileStream.setf(std::ios::fixed, std::ios::floatfield);
        else if (formatFlags.isAnyFlag(FileWriter_floatScientific))
            m_fileStream.setf(std::ios::scientific, std::ios::floatfield);
        else
            m_fileStream.unsetf(std::ios::floatfield);
    }
}

//https://www.codeproject.com/Articles/38242/Reading-UTF-8-with-C-streams


/*
#include <fstream>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
void prepare_file()
{
    // UTF-8 data
    char utf8[] = {'\x7a',                       // latin small letter 'z' U+007a
                   '\xe6','\xb0','\xb4',         // CJK ideograph "water"  U+6c34
                   '\xf0','\x9d','\x84','\x8b'}; // musical sign segno U+1d10b
    std::ofstream fout("text.txt");
    fout.write(utf8, sizeof utf8);
}
void test_file_utf16()
{
    std::wifstream fin("text.txt");
    fin.imbue(std::locale(fin.getloc(), new std::codecvt_utf8_utf16<wchar_t>));
    std::cout << "Read from file using UTF-8/UTF-16 codecvt\n";
    for(wchar_t c; fin >> c; )
        std::cout << std::hex << std::showbase << c << '\n';
}
void test_file_ucs2()
{
    std::wifstream fin("text.txt");
    fin.imbue(std::locale(fin.getloc(), new std::codecvt_utf8<wchar_t>));
    std::cout << "Read from file using UTF-8/UCS2 codecvt\n";
    for(wchar_t c; fin >> c; )
        std::cout << std::hex << std::showbase << c << '\n';
}
int main()
{
    prepare_file();
    test_file_utf16();
    test_file_ucs2();
}


unsigned char bom[] = { 0xEF,0xBB,0xBF };
    f.write((char*)bom, sizeof(bom));
    
    
// ---
https://github.com/kajzur/TextFilters/blob/master/include/stxutif.h
    
Add stxutif.h to your project from sources above

Open the file in ANSI mode and add the BOM to the start of a file, like this:

std::ofstream fs;
fs.open(filepath, std::ios::out|std::ios::binary);

unsigned char smarker[3];
smarker[0] = 0xEF;
smarker[1] = 0xBB;
smarker[2] = 0xBF;

fs << smarker;
fs.close();

Then open the file as UTF and write your content there:

std::wofstream fs;
fs.open(filepath, std::ios::out|std::ios::app);

std::locale utf8_locale(std::locale(), new utf8cvt<false>);
fs.imbue(utf8_locale); 

fs << .. // Write anything you want...



*/