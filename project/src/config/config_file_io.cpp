/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration file or registry input/output
*******************************************************************************/
#include "../globals.h"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#ifdef _WINDOWS
#include <Windows.h>
#include <tchar.h>
#endif
#include "config_file_io.h"
using namespace config;


// -- registry key -- ------------------------------------------------------

#ifdef _WINDOWS
/// @brief Destroy config key
/// @param path Key path (without name)
/// @param fileName Key name
bool ConfigFileIO<registry_io_mode_t>::remove(std::wstring path, std::wstring fileName)
{

}


/// @brief Read float value
/// @param[in]  key Registry key item
/// @param[out] outFloatVal Output value
inline void ConfigFileIO<registry_io_mode_t>::read(const wchar_t* key, float& outFloatVal)
{

}

/// @brief Read char string value
/// @param[in]  key Registry key item
/// @param[out] outStringVal Output value
void ConfigFileIO<registry_io_mode_t>::read(const wchar_t* key, char* outStringVal, size_t length)
{

}

/// @brief  Read standard string value
/// @param[in]  key Registry key item
/// @param[out] outStringVal Output value
void ConfigFileIO<registry_io_mode_t>::read(const wchar_t* key, std::string& outStringVal)
{

}

/// @brief Read wide string value
/// @param[in]  key Registry key item
/// @param[out] outWideStringVal Output value
void ConfigFileIO<registry_io_mode_t>::read(const wchar_t* key, std::wstring& outWideStringVal)
{

}


/// @brief Write float value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<registry_io_mode_t>::writeFloat(const wchar_t* key, float val)
{

}

/// @brief Write char string value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<registry_io_mode_t>::writeString(const wchar_t* key, char* val, size_t length)
{

}

/// @brief Write standard string value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<registry_io_mode_t>::writeString(const wchar_t* key, std::string& val)
{

}

/// @brief Write wide string value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<registry_io_mode_t>::writeWideString(const wchar_t* key, std::wstring& val)
{

}
#endif


// -- external config file -- ----------------------------------------------

/// @brief Open data source
/// @param path File path
/// @param fileMode IO mode
bool ConfigFileIO<file_io_mode_t>::open(std::wstring path, file_io_mode_t fileMode)
{
    // open file
    if (fileMode == file_io_mode_t::read) // open / read
    {
        m_file.open(path, std::ios_base::in);
        createContentIndex();
    }
    else if (fileMode == file_io_mode_t::write) // replace / write
    {
        m_file.open(path, std::ios_base::out | std::ios_base::trunc);
    }
    // check success
    if (m_file.is_open())
    {
        m_openMode = fileMode;
        return true;
    }
    return false;
}

/// @brief Close data source
void ConfigFileIO<file_io_mode_t>::close()
{
    if (m_openMode != file_io_mode_t::none)
    {
        m_file.close();
        m_openMode = file_io_mode_t::none;
    }
}

/// @brief Read and index all values
void ConfigFileIO<file_io_mode_t>::createContentIndex()
{
    //... lire chaque ligne
        //... si pas vide, séparer clé de valeur
            //... si clé pas vide, insérer dans map
}

/// @brief Destroy config file
/// @param path File path (without name)
/// @param fileName File name
bool ConfigFileIO<file_io_mode_t>::remove(std::wstring path, std::wstring fileName)
{
    //...
}


/// @brief Read float value
/// @param[in]  key Registry key item
/// @param[out] outFloatVal Output value
void ConfigFileIO<file_io_mode_t>::read(const wchar_t* key, float& outFloatVal)
{
    if (m_openMode != file_io_mode_t::read || m_fileData.find(key) == m_fileData.end())
        return;
    outFloatVal = static_cast<float>(std::stoi(m_fileData[key]));
    if (outFloatVal != 0.0f)
        outFloatVal /= 1000.0f;
}

/// @brief Read char string value
/// @param[in]  key Registry key item
/// @param[out] outStringVal Output value
void ConfigFileIO<file_io_mode_t>::read(const wchar_t* key, char* outStringVal, size_t length)
{
    if (m_openMode != file_io_mode_t::read || m_fileData.find(key) == m_fileData.end())
        return;
    std::string buffer(m_fileData[key].begin(), m_fileData[key].end());
    strcpy(outStringVal, buffer.c_str());
}

/// @brief  Read standard string value
/// @param[in]  key Registry key item
/// @param[out] outStringVal Output value
void ConfigFileIO<file_io_mode_t>::read(const wchar_t* key, std::string& outStringVal)
{
    if (m_openMode != file_io_mode_t::read || m_fileData.find(key) == m_fileData.end())
        return;
    outStringVal = std::string(m_fileData[key].begin(), m_fileData[key].end());
}

/// @brief Read wide string value
/// @param[in]  key Registry key item
/// @param[out] outWideStringVal Output value
void ConfigFileIO<file_io_mode_t>::read(const wchar_t* key, std::wstring& outWideStringVal)
{
    if (m_openMode != file_io_mode_t::read || m_fileData.find(key) == m_fileData.end())
        return;
    outWideStringVal = m_fileData[key];
}


/// @brief Write boolean value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<file_io_mode_t>::writeBool(const wchar_t* key, bool val)
{
    if (m_openMode != file_io_mode_t::write)
        return;
    std::wstring valStr = std::wstring(key) + std::wstring(L";");
    valStr += (val) ? L"true" : L"false";
    valStr += L"\n";
    m_file.write(valStr.c_str(), valStr.size());
}

/// @brief Write integer value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<file_io_mode_t>::writeInt(const wchar_t* key, uint32_t val)
{
    if (m_openMode != file_io_mode_t::write)
        return;
    std::wstring valStr = std::wstring(key) + std::wstring(L";") + std::to_wstring(val) + std::wstring(L"\n");
    m_file.write(valStr.c_str(), valStr.size());
}

/// @brief Write float value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<file_io_mode_t>::writeFloat(const wchar_t* key, float val)
{
    if (m_openMode != file_io_mode_t::write)
        return;
    if (val != 0.0f)
        val *= 1000.0f;
    std::wstring valStr = std::wstring(key) + std::wstring(L";") + std::to_wstring(static_cast<int32_t>(val))+std::wstring(L"\n");
    m_file.write(valStr.c_str(), valStr.size());
}

/// @brief Write char string value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<file_io_mode_t>::writeString(const wchar_t* key, char* val, size_t length)
{
    if (m_openMode != file_io_mode_t::write)
        return;
    std::string buffer(val);
    std::wstring valStr = std::wstring(key) + std::wstring(L";") + std::wstring(buffer.begin(), buffer.end()) + std::wstring(L"\n");
    m_file.write(valStr.c_str(), valStr.size());
}

/// @brief Write standard string value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<file_io_mode_t>::writeString(const wchar_t* key, std::string& val)
{
    if (m_openMode != file_io_mode_t::write)
        return;
    std::wstring valStr = std::wstring(key) + std::wstring(L";") + std::wstring(val.begin(), val.end()) + std::wstring(L"\n");
    m_file.write(valStr.c_str(), valStr.size());
}

/// @brief Write wide string value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<file_io_mode_t>::writeWideString(const wchar_t* key, std::wstring& val)
{
    if (m_openMode != file_io_mode_t::write)
        return;
    std::wstring valStr = std::wstring(key) + std::wstring(L";") + val + std::wstring(L"\n");
    m_file.write(valStr.c_str(), valStr.size());
}
