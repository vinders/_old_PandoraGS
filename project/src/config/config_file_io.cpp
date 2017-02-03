/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration file or registry input/output
*******************************************************************************/
#include "../globals.h"
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <io.h>
#include <codecvt>
#include <cctype>
#include <algorithm>
#include <functional>
#include <map>
#ifdef _WINDOWS
#include <Windows.h>
#include <tchar.h>
#include <winerror.h>
#endif
#include "../events/utils/file_io.h"
#include "config_file_io.h"
using namespace config;


// -- registry key -- ------------------------------------------------------

#ifdef _WINDOWS
/// @brief Destroy config key
/// @param path Key path (without name)
/// @param fileName Key name
/// @return Success
bool ConfigFileIO<registry_io_mode_t>::remove(std::wstring path, std::wstring& keyName)
{
    HKEY baseKey;
    if (::RegOpenKeyEx(HKEY_CURRENT_USER, (LPCWSTR)(path.c_str()), 0, KEY_ALL_ACCESS, &baseKey) == ERROR_SUCCESS)
    {
        ::RegDeleteKeyEx(baseKey, (LPCWSTR)(keyName.c_str()), KEY_WOW64_32KEY, 0);
        ::RegDeleteKeyEx(baseKey, (LPCWSTR)(keyName.c_str()), KEY_WOW64_64KEY, 0);
        RegCloseKey(baseKey);
        return true;
    }
    return false;
}


/// @brief Read integer value
/// @param[in]  key File entry
/// @param[out] outIntVal Output value
void ConfigFileIO<registry_io_mode_t>::read(std::string& key, uint32_t& outIntVal)
{
    std::wstring wideKey = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(key);
    read(wideKey.c_str(), outIntVal);
}

/// @brief Read float value
/// @param[in]  key Registry key item
/// @param[out] outFloatVal Output value
void ConfigFileIO<registry_io_mode_t>::read(const wchar_t* key, float& outFloatVal)
{
    char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    m_keySize = sizeof(buffer) - 1;
    if (::RegQueryValueEx(m_regKey, (LPCWSTR)key, 0, &m_keyStatus, (PBYTE)buffer, &m_keySize) == ERROR_SUCCESS)
    {
        outFloatVal = static_cast<float>(atof(buffer));
    }
}

/// @brief Read char string value
/// @param[in]  key Registry key item
/// @param[out] outStringVal Output value
void ConfigFileIO<registry_io_mode_t>::read(const wchar_t* key, char* outStringVal, size_t length)
{
    char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    m_keySize = sizeof(buffer) - 1;
    if (::RegQueryValueEx(m_regKey, (LPCWSTR)key, 0, &m_keyStatus, (PBYTE)buffer, &m_keySize) == ERROR_SUCCESS)
    {
        std::string val(buffer);
        strncpy_s(outStringVal, length, val.c_str(), val.size());
    }
}

/// @brief  Read standard string value
/// @param[in]  key Registry key item
/// @param[out] outStringVal Output value
void ConfigFileIO<registry_io_mode_t>::read(const wchar_t* key, std::string& outStringVal)
{
    WCHAR buffer[MAX_PATH];
    memset(buffer, 0, sizeof(buffer));
    m_keySize = sizeof(buffer) - 1;
    if (::RegQueryValueEx(m_regKey, (LPCWSTR)key, 0, &m_keyStatus, (LPBYTE)buffer, &m_keySize) == ERROR_SUCCESS)
    {
        outStringVal = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(buffer);
    }
}

/// @brief Read wide string value
/// @param[in]  key Registry key item
/// @param[out] outWideStringVal Output value
void ConfigFileIO<registry_io_mode_t>::read(const wchar_t* key, std::wstring& outWideStringVal)
{
    WCHAR buffer[MAX_PATH];
    memset(buffer, 0, sizeof(buffer));
    m_keySize = sizeof(buffer) - 1;
    if (::RegQueryValueEx(m_regKey, (LPCWSTR)key, 0, &m_keyStatus, (LPBYTE)buffer, &m_keySize) == ERROR_SUCCESS)
    {
        outWideStringVal = buffer;
    }
}


/// @brief Read all available values
/// @param[out] outData Output values
void ConfigFileIO<registry_io_mode_t>::readAll(std::map<std::string, uint32_t>& outData)
{
    std::map<std::wstring, std::wstring> values;
    mapAllValues(values);
    for (auto it = values.begin(); it != values.end(); ++it)
    {
        std::string key = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(it->first);
        outData[key] = std::stoi(it->second);
    }
}

/// @brief Read all available values
/// @param[out] outData Output values
void ConfigFileIO<registry_io_mode_t>::readAll(std::map<std::wstring, std::wstring>& outData)
{
    mapAllValues(outData);
}

/// @brief Create hash-map with all available values
/// @param[out] outData Output map
void ConfigFileIO<registry_io_mode_t>::mapAllValues(std::map<std::wstring, std::wstring>& outData)
{
    TCHAR    classNameBuffer[MAX_PATH] = TEXT("");
    DWORD    classNameSize = sizeof(classNameBuffer);
    DWORD    subkeysNb = 0, subkeysMaxSize, maxClassData;
    DWORD    valuesNb = 0, valuesMaxSize, maxValueData;
    DWORD    securityDescriptorSize;
    FILETIME lastWriteTime;

    // get key information
    if (RegQueryInfoKey(m_regKey, classNameBuffer, &classNameSize, NULL, &subkeysNb, &subkeysMaxSize, &maxClassData,
        &valuesNb, &valuesMaxSize, &maxValueData, &securityDescriptorSize, &lastWriteTime) == ERROR_SUCCESS)
    {
        // enumerate the key values 
        std::wstring mapKey;
        std::wstring mapVal;
        WCHAR  valueIdentifier[64];
        DWORD valueMaxSize;
        for (uint32_t i = 0u; i < valuesNb; i++)
        {
            valueMaxSize = sizeof(valueIdentifier);
            memset(valueIdentifier, 0x0, valueMaxSize);
            if (RegEnumValue(m_regKey, static_cast<DWORD>(i), valueIdentifier, &valueMaxSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
            {
                // read entry
                mapKey = valueIdentifier;
                read(mapKey.c_str(), mapVal);
                if (mapVal.empty() == false)
                {
                    outData[mapKey] = mapVal; // add entry
                    mapVal = L""; // reset
                }
            }
        }
    }
}


/// @brief Write float value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<registry_io_mode_t>::writeInt(std::string& key, uint32_t val)
{
    std::wstring wideKey = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(key);
    writeInt(wideKey.c_str(), val);
}

/// @brief Write float value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<registry_io_mode_t>::writeFloat(const wchar_t* key, float val)
{
    char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    _snprintf_s(buffer, sizeof(buffer), "%f", val);
    ::RegSetValueEx(m_regKey, (LPCWSTR)key, 0, REG_SZ, (PBYTE)buffer, strlen(buffer));
}

/// @brief Write char string value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<registry_io_mode_t>::writeString(const wchar_t* key, char* val, size_t length)
{
    ::RegSetValueEx(m_regKey, (LPCWSTR)key, 0, REG_SZ, (PBYTE)val, length);
}

/// @brief Write standard string value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<registry_io_mode_t>::writeString(const wchar_t* key, std::string& val)
{
    ::RegSetValueEx(m_regKey, (LPCWSTR)key, 0, REG_SZ, (PBYTE)(val.c_str()), val.size());
}

/// @brief Write wide string value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<registry_io_mode_t>::writeWideString(const wchar_t* key, std::wstring& val)
{
    ::RegSetValueEx(m_regKey, (LPCWSTR)key, 0, REG_BINARY, (LPBYTE)(val.c_str()), val.size());
}

/// @brief Remove existing value from file
/// @param[in]  key Entry
/// @return Success
bool ConfigFileIO<registry_io_mode_t>::removeValue(std::string& key)
{
    std::wstring wideKey = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(key);
    char buffer[64];
    if (::RegQueryValueEx(m_regKey, (LPCWSTR)(wideKey.c_str()), 0, &m_keyStatus, (PBYTE)&buffer, &m_keySize) == ERROR_SUCCESS)
    {
        return (RegDeleteValue(m_regKey, (LPCWSTR)(wideKey.c_str())) == ERROR_SUCCESS);
    }
    return true;
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
    else // open / write
    {
        m_file.open(path, std::ios::out | std::ios::in | std::ios::app);
        createContentIndex();
        m_fileData[L"path"] = path;
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
        m_fileData.clear();
        m_openMode = file_io_mode_t::none;
    }
}

/// @brief Read and index all values
void ConfigFileIO<file_io_mode_t>::createContentIndex()
{
    // set UTF-8 charset
    const std::codecvt_utf8<wchar_t>* converter = new std::codecvt_utf8<wchar_t>();
    const std::locale utf8_locale = std::locale(std::locale::empty(), converter);
    m_file.imbue(utf8_locale);

    std::wstring buffer;
    std::getline(m_file, buffer); // first line
    while (!m_file.eof())
    {
        if (!buffer.empty())
        {
            // trim leading/trailing spaces
            buffer.erase(buffer.begin(), std::find_if(buffer.begin(), buffer.end(), std::not1(std::ptr_fun<int,int>(std::isspace))));
            buffer.erase(std::find_if(buffer.rbegin(), buffer.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), buffer.end());
            if (!buffer.empty())
            {
                // add entry to hashmap
                size_t sepPos = buffer.find(L';');
                if (sepPos != std::wstring::npos && sepPos > 0) // split
                {
                    if (sepPos < buffer.size() - 1)
                        m_fileData[buffer.substr(0, sepPos)] = buffer.substr(sepPos + 1);
                    else
                        m_fileData[buffer.substr(0, sepPos)] = L"";
                }
            }
        }
        std::getline(m_file, buffer); // next line
    }
}

/// @brief Destroy config file
/// @param path File path (without name)
/// @param fileName File name
/// @return Success
bool ConfigFileIO<file_io_mode_t>::remove(std::wstring path, std::wstring& fileName)
{
    std::string basePath = events::utils::FileIO::getWritableFilePath(); // get base folder path

    #ifdef _WINDOWS
    std::wstring fullPath = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(basePath);
    if (fullPath.size() > 0 && fullPath.at(fullPath.size() - 1) != L'/') // absolute path: replace slashes
    {
        for (std::wstring::size_type i = 0; (i = path.find(L'/', i)) != std::wstring::npos; ++i)
            path.replace(i, 1, L"\\");
    }
    fullPath += path + fileName;
    return (_wunlink(fullPath.c_str()) == 0); // remove file

    #else
    // linux/UNIX - no support for wide string path -> convert
    basePath += std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(path);
    basePath += std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(fileName);
    return (unlink(basePath.c_str()) == 0); // remove file
    #endif
}


/// @brief Read integer value
/// @param[in]  key File entry
/// @param[out] outIntVal Output value
void ConfigFileIO<file_io_mode_t>::read(std::string& key, uint32_t& outIntVal)
{
    std::wstring wideKey = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(key);
    read(wideKey.c_str(), outIntVal);
}

/// @brief Read float value
/// @param[in]  key File entry
/// @param[out] outFloatVal Output value
void ConfigFileIO<file_io_mode_t>::read(const wchar_t* key, float& outFloatVal)
{
    if (m_openMode != file_io_mode_t::read || m_fileData.find(key) == m_fileData.end())
        return;
    outFloatVal = static_cast<float>(std::stof(m_fileData[key]));
    if (outFloatVal != 0.0f)
        outFloatVal /= 1000.0f;
}

/// @brief Read char string value
/// @param[in]  key File entry
/// @param[out] outStringVal Output value
void ConfigFileIO<file_io_mode_t>::read(const wchar_t* key, char* outStringVal, size_t length)
{
    if (m_openMode != file_io_mode_t::read || m_fileData.find(key) == m_fileData.end())
        return;
    std::string buffer = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(m_fileData[key]);
    #ifdef _WINDOWS
    strncpy_s(outStringVal, length, buffer.c_str(), buffer.size());
    #else
    strcpy(outStringVal, buffer.c_str());
    #endif
}

/// @brief  Read standard string value
/// @param[in]  key File entry
/// @param[out] outStringVal Output value
void ConfigFileIO<file_io_mode_t>::read(const wchar_t* key, std::string& outStringVal)
{
    if (m_openMode != file_io_mode_t::read || m_fileData.find(key) == m_fileData.end())
        return;
    outStringVal = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(m_fileData[key]);
}

/// @brief Read wide string value
/// @param[in]  key File entry
/// @param[out] outWideStringVal Output value
void ConfigFileIO<file_io_mode_t>::read(const wchar_t* key, std::wstring& outWideStringVal)
{
    if (m_openMode != file_io_mode_t::read || m_fileData.find(key) == m_fileData.end())
        return;
    outWideStringVal = m_fileData[key];
}


/// @brief Read all available values
/// @param[out] outData Output values
void ConfigFileIO<file_io_mode_t>::readAll(std::map<std::string, uint32_t>& outData)
{
    for (auto it = m_fileData.begin(); it != m_fileData.end(); ++it)
    {
        std::string key = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(it->first);
        outData[key] = std::stoi(it->second);
    }
}

/// @brief Read all available values
/// @param[out] outData Output values
void ConfigFileIO<file_io_mode_t>::readAll(std::map<std::wstring, std::wstring>& outData)
{
    for (auto it = m_fileData.begin(); it != m_fileData.end(); ++it)
        outData[it->first] = it->second;
}


/// @brief Write boolean value
/// @param[in]  key File entry
/// @param[in]  val Written value
void ConfigFileIO<file_io_mode_t>::writeBool(const wchar_t* key, bool val)
{
    if (static_cast<uint32_t>(m_openMode) < static_cast<uint32_t>(file_io_mode_t::write))
        return;
    std::wstring valStr = (val) ? L"true" : L"false";
    appendValue(key, valStr);
}

/// @brief Write integer value
/// @param[in]  key File entry
/// @param[in]  val Written value
void ConfigFileIO<file_io_mode_t>::writeInt(const wchar_t* key, uint32_t val)
{
    if (static_cast<uint32_t>(m_openMode) < static_cast<uint32_t>(file_io_mode_t::write))
        return;
    std::wstring valStr = std::to_wstring(val);
    appendValue(key, valStr);
}

/// @brief Write float value
/// @param[in]  key Registry key item
/// @param[in]  val Written value
void ConfigFileIO<file_io_mode_t>::writeInt(std::string& key, uint32_t val)
{
    std::wstring wideKey = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(key);
    writeInt(wideKey.c_str(), val);
}

/// @brief Write float value
/// @param[in]  key File entry
/// @param[in]  val Written value
void ConfigFileIO<file_io_mode_t>::writeFloat(const wchar_t* key, float val)
{
    if (static_cast<uint32_t>(m_openMode) < static_cast<uint32_t>(file_io_mode_t::write))
        return;
    if (val != 0.0f)
        val *= 1000.0f;
    std::wstring valStr = std::to_wstring(static_cast<int32_t>(val));
    appendValue(key, valStr);
}

/// @brief Write char string value
/// @param[in]  key File entry
/// @param[in]  val Written value
void ConfigFileIO<file_io_mode_t>::writeString(const wchar_t* key, char* val, size_t length)
{
    if (static_cast<uint32_t>(m_openMode) < static_cast<uint32_t>(file_io_mode_t::write))
        return;
    std::wstring valStr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(val);
    appendValue(key, valStr);
}

/// @brief Write standard string value
/// @param[in]  key File entry
/// @param[in]  val Written value
void ConfigFileIO<file_io_mode_t>::writeString(const wchar_t* key, std::string& val)
{
    if (static_cast<uint32_t>(m_openMode) < static_cast<uint32_t>(file_io_mode_t::write))
        return;
    std::wstring valStr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(val);
    appendValue(key, valStr);
}

/// @brief Write wide string value
/// @param[in]  key File entry
/// @param[in]  val Written value
void ConfigFileIO<file_io_mode_t>::writeWideString(const wchar_t* key, std::wstring& val)
{
    if (static_cast<uint32_t>(m_openMode) < static_cast<uint32_t>(file_io_mode_t::write))
        return;
    appendValue(key, val);
}


/// @brief Remove existing value from file
/// @param[in]  key Entry
bool ConfigFileIO<file_io_mode_t>::removeValue(std::string& key)
{
    if (static_cast<uint32_t>(m_openMode) < static_cast<uint32_t>(file_io_mode_t::write))
        return false;

    bool isSuccess = false;
    std::wstring wideKey = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(key);
    if (m_fileData.find(wideKey) != m_fileData.end())
    {
        m_fileData.erase(wideKey); // remove key

        if (m_fileData.find(L"path") != m_fileData.end())
        {
            // change file mode
            std::wstring path = m_fileData[L"path"];
            m_fileData.erase(L"path");
            m_file.close();
            m_file.open(path, std::ios_base::out | std::ios_base::trunc);

            // rewrite entire file
            if (m_file.is_open())
            {
                for (auto it = m_fileData.begin(); it != m_fileData.end(); ++it)
                    appendValue(it->first, it->second);
                m_file.close();
                isSuccess = true;
            }

            // restore file mode
            m_fileData[L"path"] = path;
            if (m_openMode == file_io_mode_t::write) // replace / write
                m_file.open(path, std::ios_base::out | std::ios_base::trunc);
            else // open / write
                m_file.open(path, std::ios::out | std::ios::in | std::ios::app);
            if (m_file.is_open() == false)
                m_openMode = file_io_mode_t::none;
        }
    }
    else
    {
        isSuccess = true;
    }
    return isSuccess;
}
