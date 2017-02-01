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
