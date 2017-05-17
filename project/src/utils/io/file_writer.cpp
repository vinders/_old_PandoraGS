/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : advanced file writer
*******************************************************************************/
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <ofstream>
#include "string_encoder.h"
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
        // byte-order-mark
        if (Encoder == FileIO::file_encoder_t::utf8_bom && openMode == file_mode_t::truncate)
        {
            unsigned char bom[] = { 0xEF,0xBB,0xBF };
            m_fileStream.write((char*)bom, sizeof(bom));
        }
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
    if (formatFlags == 0)
        return;
    
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



// -- Output operations - general --
            
/// @brief Write encoded string value - use in text mode
/// @param[in] strVal  String value
/// @returns Current object
template <FileIO::string_encoder_t SrcEncoder>
FileWriter& FileWriter::write(const char* strVal)
{
    std::string buffer;
    if (SrcEncoder == FileIO::string_encoder_t::utf8)
    {
        switch (Encoder)
        {
            case FileIO::file_encoder_t::ansi :
                buffer = StringEncoder::utf8ToAnsi1252(strVal); 
                break;
            case FileIO::file_encoder_t::utf8 :
            case FileIO::file_encoder_t::utf8_bom : 
                return writeStream(strVal, strlen(strVal)); 
                break;
            case FileIO::file_encoder_t::utf16_le :
                buffer = StringEncoder::wideStringToBigEndianBytes(StringEncoder::utf8ToUtf16(strVal).c_str()); 
                break;
            case FileIO::file_encoder_t::utf16_be : 
                buffer = StringEncoder::wideStringToLittleEndianBytes(StringEncoder::utf8ToUtf16(strVal).c_str()); 
                break;
            case FileIO::file_encoder_t::ucs2 : 
                buffer = StringEncoder::wideStringToLittleEndianBytes(StringEncoder::utf8ToUcs2(strVal).c_str()); 
                break;
            default: 
                return *this; break;
        }
    }
    else // ansi
    {
        switch (Encoder)
        {
            case FileIO::file_encoder_t::ansi : 
                return writeStream(strVal, strlen(strVal)); 
                break;
            case FileIO::file_encoder_t::utf8 :
            case FileIO::file_encoder_t::utf8_bom : 
                buffer = ansi1252ToUtf8(strVal); 
                break;
            case FileIO::file_encoder_t::utf16_le :
                buffer = StringEncoder::wideStringToBigEndianBytes(StringEncoder::ansi1252ToUtf16(strVal).c_str()); 
                break;
            case FileIO::file_encoder_t::utf16_be : 
                buffer = StringEncoder::wideStringToLittleEndianBytes(StringEncoder::ansi1252ToUtf16(strVal).c_str()); 
                break;
            case FileIO::file_encoder_t::ucs2 : 
                buffer = StringEncoder::wideStringToLittleEndianBytes(StringEncoder::ansi1252ToUcs2(strVal).c_str()); 
                break;
            default: 
                return *this; break;
        }

    }
    return writeStream(buffer.c_str(), buffer.size()); 
}

/// @brief Write encoded wide-string value - use in text mode
/// @param[in] strVal  String value
/// @returns Current object
template <FileIO::wstring_encoder_t SrcEncoder = FileIO::wstring_encoder_t::utf16>
FileWriter& FileWriter::write(const wchar_t* strVal)
{
    std::string buffer;
    if (SrcEncoder == FileIO::string_encoder_t::utf16)
    {
        switch (Encoder)
        {
            case FileIO::file_encoder_t::ansi :
                buffer = StringEncoder::utf16ToAnsi1252(strVal); 
                break;
            case FileIO::file_encoder_t::utf8 :
            case FileIO::file_encoder_t::utf8_bom : 
                buffer = StringEncoder::utf16ToUtf8(strVal); 
                break;
            case FileIO::file_encoder_t::utf16_le :
                buffer = StringEncoder::wideStringToBigEndianBytes(strval); 
                break;
            case FileIO::file_encoder_t::utf16_be : 
                buffer = StringEncoder::wideStringToLittleEndianBytes(strval); 
                break;
            case FileIO::file_encoder_t::ucs2 : 
                buffer = StringEncoder::wideStringToLittleEndianBytes(StringEncoder::utf16ToUcs2(strVal).c_str()); 
                break;
            default: 
                return *this; break;
        }
    }
    else // ucs2
    {
        switch (Encoder)
        {
            case FileIO::file_encoder_t::ansi :
                buffer = StringEncoder::ucs2ToAnsi1252(strVal); 
                break;
            case FileIO::file_encoder_t::utf8 :
            case FileIO::file_encoder_t::utf8_bom : 
                buffer = StringEncoder::ucs2ToUtf8(strVal); 
                break;
            case FileIO::file_encoder_t::utf16_le :
                buffer = StringEncoder::wideStringToBigEndianBytes(strval); 
                break;
            case FileIO::file_encoder_t::utf16_be : 
                buffer = StringEncoder::wideStringToLittleEndianBytes(strval); 
                break;
            case FileIO::file_encoder_t::ucs2 : 
                buffer = StringEncoder::wideStringToLittleEndianBytes(strVal); 
                break;
            default: 
                return *this; break;
        }

    }
    return writeStream(buffer.c_str(), buffer.size()); 
}



// -- Output operations - base types --

/// @brief Write boolean value
FileWriter& FileWriter::put(const bool val)
{
    if (Encoder == FileIO::file_encoder_t::binary) // binary mode
    {
        return writeStream(reinterpret_cast<char*>(&val), sizeof(val));
    }
    else // text mode
    {
        if (Encoder == FileIO::file_encoder_t::utf16_le || Encoder == FileIO::file_encoder_t::utf16_be || Encoder == FileIO::file_encoder_t::ucs2)
        {
            if (m_formatFlags.isAnyFlag(FileWriter_boolAsAlpha)) 
            {
                if ((m_formatFlags.isAnyFlag(FileWriter_stringToUppercase))
                    return (val) ? writeWideString(L"TRUE") : writeWideString(L"FALSE");
                else
                    return (val) ? writeWideString(L"true") : writeWideString(L"false");
            }
            else
            {
                return (val) ? writeWideString(L"1") : writeWideString(L"0");
            }
        }
        else
        {
            if (m_formatFlags.isAnyFlag(FileWriter_boolAsAlpha)) 
            {
                if ((m_formatFlags.isAnyFlag(FileWriter_stringToUppercase))
                    return (val) ? writeStream("TRUE", 4u) : writeStream("FALSE", 5u);
                else
                    return (val) ? writeStream("true", 4u) : writeStream("false", 5u);
            }
            else
            {
                return (val) ? writeStream("1", 1u) : writeStream("0", 1u);
            }
        }
    }
}

/// @brief Write single character
FileWriter& FileWriter::put(const char val)
{
    if (Encoder == FileIO::file_encoder_t::binary) // binary mode
    {
        return writeStream(reinterpret_cast<char*>(&val), 1u);
    }
    else // text mode
    {
        if (val >= 0) // ascii
        {
            if (Encoder == FileIO::file_encoder_t::utf16_le || Encoder == FileIO::file_encoder_t::ucs2)
            {
                char buffer[3];
                buffer[1] = val;
                buffer[0] = buffer[2] = '\0';
                return writeStream(buffer);
            }
            else if (Encoder == FileIO::file_encoder_t::utf16_be)
            {
                char buffer[3];
                buffer[0] = val;
                buffer[1] = buffer[2] = '\0';
                return writeStream(buffer);
            }
            else
            {
                return writeStream(val, 1u);
            }
        }
        else // extended
        {
            switch (Encoder)
            {
                case FileIO::file_encoder_t::ansi : 
                    return writeStream(val, 2u); break;
                case FileIO::file_encoder_t::utf8 :
                case FileIO::file_encoder_t::utf8_bom : 
                {
                    std::string data = StringEncoder::ansi1252ToUtf8(val); 
                    return writeStream(data.c_str(), data.size()); break;
                }
                case FileIO::file_encoder_t::utf16_le :
                case FileIO::file_encoder_t::utf16_be : 
                    return writeWideString(StringEncoder::ansi1252ToUtf16(val).c_str()); break;
                case FileIO::file_encoder_t::ucs2 : 
                    return writeWideString(StringEncoder::ansi1252ToUcs2(val).c_str()); break;
                default: 
                    return *this; break;
            }
        }
    }
}


/// @brief Write unsigned short integer value
FileWriter& FileWriter::put(const uint16_t val)
{
    return *this;
}

/// @brief Write short integer value
FileWriter& FileWriter::put(const int16_t val)
{
    return *this;
}

/// @brief Write unsigned integer value
FileWriter& FileWriter::put(const uint32_t val)
{
    return *this;
}

/// @brief Write integer value
FileWriter& FileWriter::put(const int32_t val)
{
    return *this;
}

/// @brief Write unsigned 64-bit integer value
FileWriter& FileWriter::put(const uint64_t val)
{
    return *this;
}

/// @brief Write s64-bit integer value
FileWriter& FileWriter::put(const int64_t val)
{
    return *this;
}


/// @brief Write floating-point value
FileWriter& FileWriter::put(const float val)
{
    return *this;
}

/// @brief Write double-precision floating-point value
FileWriter& FileWriter::put(const double val)
{
    return *this;
}

/// @brief Write long double-precision floating-point value
FileWriter& FileWriter::put(const long double val)
{
    return *this;
}






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