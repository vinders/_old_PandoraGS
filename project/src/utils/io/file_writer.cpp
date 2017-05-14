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
    
    //...
    
    m_defaultPrecision = m_fileStream.precision();
    if (m_floatDecimalPrecision != 0u)
        m_fileStream.precision(m_floatDecimalPrecision);
    
    unlock<checkConcurrency>();
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