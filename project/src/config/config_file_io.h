/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration file or registry input/output
*******************************************************************************/
#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <map>
#ifdef _WINDOWS
#include <Windows.h>
#include <tchar.h>
#endif

/// @namespace config
/// Configuration management
namespace config
{
    #ifdef _WINDOWS
    /// @enum registry_io_mode_t
    /// @brief Registry IO mode
    enum class registry_io_mode_t : uint32_t
    {
        none = 0,
        read = 1,  ///< Open and read registry key
        write = 2, ///< Create / overwrite registry key
        append = 3 ///< Open and write registry key
    };
    #endif

    /// @enum file_io_mode_t
    /// @brief File IO mode
    enum class file_io_mode_t : uint32_t
    {
        none = 0,
        read = 1,  ///< Open and read data file (csv)
        write = 2, ///< Create / overwrite data file (csv)
        append = 3 ///< Open and write at the end of data file (csv)
    };

    template<typename T> class ConfigFileIO;


    // -- registry key -- ------------------------------------------------------

    #ifdef _WINDOWS
    /// @class ConfigFileIO<registry_io_mode_t>
    /// @brief Configuration registry key input/output
    template<> class ConfigFileIO<registry_io_mode_t>
    {
    private:
        registry_io_mode_t m_openMode; ///< Opening mode (read / write)
        HKEY m_regKey;     ///< Registry key
        DWORD m_keyStatus; ///< Status buffer
        DWORD m_keySize;   ///< Size buffer
        DWORD m_regBuffer; ///< Data buffer

    public:
        /// @brief Create registry IO tool
        ConfigFileIO() : m_openMode(registry_io_mode_t::none), m_keyStatus(0) {}
        /// @brief Destroy registry IO tool
        ~ConfigFileIO()
        {
            if (m_openMode != registry_io_mode_t::none)
                close();
        }

        /// @brief Open data source
        /// @param key Key path
        /// @param accessMode IO mode
        bool open(std::wstring path, registry_io_mode_t accessMode = registry_io_mode_t::read)
        {
            if (accessMode != registry_io_mode_t::write)
                return (RegOpenKeyEx(HKEY_CURRENT_USER, (LPCWSTR)(path.c_str()), 0, KEY_ALL_ACCESS, &m_regKey) == ERROR_SUCCESS);
            else
                return (RegCreateKeyEx(HKEY_CURRENT_USER, (LPCWSTR)(path.c_str()), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &m_regKey, &m_keyStatus) == ERROR_SUCCESS);
        }
        /// @brief Close data source
        inline void close()
        {
            if (m_openMode != registry_io_mode_t::none)
            {
                ::RegCloseKey(m_regKey);
                m_openMode = registry_io_mode_t::none;
            }
        }
        /// @brief Destroy config key
        /// @param path Key path (without name)
        /// @param fileName Key name
        /// @return Success
        static bool remove(std::wstring path, std::wstring& keyName);

        /// @brief Read boolean value
        /// @param[in]  key Registry key item
        /// @param[out] outBoolVal Output value
        /// @return     Success
        inline bool read(const wchar_t* key, bool& outBoolVal)
        {
            m_keySize = sizeof(DWORD);
            if (::RegQueryValueEx(m_regKey, (LPCWSTR)key, 0, &m_keyStatus, (LPBYTE)&m_regBuffer, &m_keySize) == ERROR_SUCCESS)
            {
                outBoolVal = (m_regBuffer != 0uL);
                return true;
            }
            return false;
        }
        /// @brief Read boolean-typed enum value
        /// @param[in]  key Registry key item
        /// @param[out] outBoolTypeVal Output value
        /// @param[in]  defaultBoolTypeVal Default value (if not found or invalid)
        template<class T> inline void read(const wchar_t* key, T& outBoolTypeVal, T defaultBoolTypeVal)
        {
            bool buffer;
            if (read(key, buffer))
                outBoolTypeVal = static_cast<T>(buffer);
            else
                outBoolTypeVal = defaultBoolTypeVal;
        }
        /// @brief Read integer value
        /// @param[in]  key Registry key item
        /// @param[out] outIntVal Output value
        /// @return     Success
        inline bool read(const wchar_t* key, uint32_t& outIntVal)
        {
            m_keySize = sizeof(DWORD);
            if (::RegQueryValueEx(m_regKey, (LPCWSTR)key, 0, &m_keyStatus, (LPBYTE)&m_regBuffer, &m_keySize) == ERROR_SUCCESS)
            {
                outIntVal = (uint32_t)m_regBuffer;
                return true;
            }
            return false;
        }
        /// @brief Read integer-typed enum value
        /// @param[in]  key Registry key item
        /// @param[out] outIntTypeVal Output value
        /// @param[in]  superiorIntLimit Superior limit (max allowed value + 1)
        /// @param[in]  defaultIntTypeVal Default value (if not found or invalid)
        template<class T> inline void read(const wchar_t* key, T& outIntTypeVal, uint32_t superiorIntLimit, T defaultIntTypeVal)
        {
            uint32_t buffer;
            if (read(key, buffer))
                outIntTypeVal = (buffer < superiorIntLimit) ? static_cast<T>(buffer) : defaultIntTypeVal;
            else
                outIntTypeVal = defaultIntTypeVal;
        }
        /// @brief Read integer value
        /// @param[in]  key File entry
        /// @param[out] outIntVal Output value
        void read(std::string& key, uint32_t& outIntVal);
        /// @brief Read float value
        /// @param[in]  key Registry key item
        /// @param[out] outFloatVal Output value
        void read(const wchar_t* key, float& outFloatVal);
        /// @brief Read char string value
        /// @param[in]  key Registry key item
        /// @param[out] outStringVal Output value
        void read(const wchar_t* key, char* outStringVal, size_t length);
        /// @brief  Read standard string value
        /// @param[in]  key Registry key item
        /// @param[out] outStringVal Output value
        void read(const wchar_t* key, std::string& outStringVal);
        /// @brief Read wide string value
        /// @param[in]  key Registry key item
        /// @param[out] outWideStringVal Output value
        void read(const wchar_t* key, std::wstring& outWideStringVal);

        /// @brief Read all available values
        /// @param[out] outData Output values
        void readAll(std::map<std::string, uint32_t>& outData);
        /// @brief Read all available values
        /// @param[out] outData Output values
        void readAll(std::map<std::wstring, std::wstring>& outData);
    private:
        /// @brief Create hash-map with all available values
        /// @param[out] outData Output map
        template <typename KeyType, typename ValType>
        void mapAllValues(std::map<KeyType, ValType>& outData);

    public:
        /// @brief Write boolean value
        /// @param[in]  key Registry key item
        /// @param[in]  val Written value
        inline void writeBool(const wchar_t* key, bool val)
        {
            m_regBuffer = (val) ? 1uL : 0uL;
            ::RegSetValueEx(m_regKey, (LPCWSTR)key, 0, REG_DWORD, (LPBYTE)&m_regBuffer, sizeof(val));
        }
        /// @brief Write boolean-typed enum value
        /// @param[in]  key Registry key item
        /// @param[in]  val Written value
        template<class T> inline void writeBoolType(const wchar_t* key, T val, T falseBoolVal)
        {
            uint32_t buffer = (val != falseBoolVal) ? 1uL : 0uL;
            writeInt(key, buffer);
        }
        /// @brief Write integer value
        /// @param[in]  key Registry key item
        /// @param[in]  val Written value
        inline void writeInt(const wchar_t* key, uint32_t val)
        {
            m_regBuffer = val;
            ::RegSetValueEx(m_regKey, (LPCWSTR)key, 0, REG_DWORD, (LPBYTE)&m_regBuffer, sizeof(val));
        }
        /// @brief Write integer-typed enum value
        /// @param[in]  key Registry key item
        /// @param[in]  val Written value
        template<class T> inline void writeIntType(const wchar_t* key, T val)
        {
            writeInt(key, static_cast<uint32_t>(val));
        }
        /// @brief Write float value
        /// @param[in]  key Registry key item
        /// @param[in]  val Written value
        void writeInt(std::string& key, uint32_t val);
        /// @brief Write float value
        /// @param[in]  key Registry key item
        /// @param[in]  val Written value
        void writeFloat(const wchar_t* key, float val);
        /// @brief Write char string value
        /// @param[in]  key Registry key item
        /// @param[in]  val Written value
        void writeString(const wchar_t* key, char* val, size_t length);
        /// @brief Write standard string value
        /// @param[in]  key Registry key item
        /// @param[in]  val Written value
        void writeString(const wchar_t* key, std::string& val);
        /// @brief Write wide string value
        /// @param[in]  key Registry key item
        /// @param[in]  val Written value
        void writeWideString(const wchar_t* key, std::wstring& val);

        /// @brief Remove existing value from file
        /// @param[in]  key Entry
        /// @return Success
        bool removeValue(std::string& key);
    };
    #endif


    // -- external config file -- ----------------------------------------------

    /// @class ConfigFileIO<file_io_mode_t>
    /// @brief Configuration file input/output
    template<> class ConfigFileIO<file_io_mode_t>
    {
    private:
        file_io_mode_t m_openMode;
        std::wfstream m_file;
        std::map<std::wstring, std::wstring> m_fileData;

    public:
        /// @brief Create CSV file IO tool
        ConfigFileIO() : m_openMode(file_io_mode_t::none) {}
        /// @brief Destroy CSV file IO tool
        ~ConfigFileIO()
        {
            close();
        }

        /// @brief Open data source
        /// @param path File path
        /// @param fileMode IO mode
        bool open(std::wstring path, file_io_mode_t fileMode = file_io_mode_t::read);
        /// @brief Close data source
        void close();
        /// @brief Destroy config file
        /// @param path File path (without name)
        /// @param fileName File name
        /// @return Success
        static bool remove(std::wstring path, std::wstring& fileName);
    private:
        /// @brief Read and index all values
        void createContentIndex();

    public:
        /// @brief Read boolean value
        /// @param[in]  key File entry
        /// @param[out] outBoolVal Output value
        /// @return     Success
        inline bool read(const wchar_t* key, bool& outBoolVal)
        {
            if (m_openMode != file_io_mode_t::read || m_fileData.find(key) == m_fileData.end())
                return false;
            outBoolVal = (m_fileData[key] == L"true" || m_fileData[key] == L"1");
            return true;
        }
        /// @brief Read boolean-typed enum value
        /// @param[in]  key File entry
        /// @param[out] outBoolTypeVal Output value
        /// @param[in]  defaultBoolTypeVal Default value (if not found or invalid)
        template<class T> inline void read(const wchar_t* key, T& outBoolTypeVal, T defaultBoolTypeVal)
        {
            bool buffer;
            if (read(key, buffer))
                outBoolTypeVal = static_cast<T>(buffer);
            else
                outBoolTypeVal = defaultBoolTypeVal;
        }
        /// @brief Read integer value
        /// @param[in]  key File entry
        /// @param[out] outIntVal Output value
        /// @return     Success
        inline bool read(const wchar_t* key, uint32_t& outIntVal)
        {
            if (m_openMode != file_io_mode_t::read || m_fileData.find(key) == m_fileData.end())
                return false;
            outIntVal = std::stoi(m_fileData[key]);
            return true;
        }
        /// @brief Read integer-typed enum value
        /// @param[in]  key File entry
        /// @param[out] outIntTypeVal Output value
        /// @param[in]  superiorIntLimit Superior limit (max allowed value + 1)
        /// @param[in]  defaultIntTypeVal Default value (if not found or invalid)
        template<class T> inline void read(const wchar_t* key, T& outIntTypeVal, uint32_t superiorIntLimit, T defaultIntTypeVal)
        {
            uint32_t buffer;
            if (read(key, buffer))
                outIntTypeVal = (buffer < superiorIntLimit) ? static_cast<T>(buffer) : defaultIntTypeVal;
            else
                outIntTypeVal = defaultIntTypeVal;
        }
        /// @brief Read integer value
        /// @param[in]  key File entry
        /// @param[out] outIntVal Output value
        void read(std::string& key, uint32_t& outIntVal);
        /// @brief Read float value
        /// @param[in]  key File entry
        /// @param[out] outFloatVal Output value
        void read(const wchar_t* key, float& outFloatVal);
        /// @brief Read char string value
        /// @param[in]  key File entry
        /// @param[out] outStringVal Output value
        void read(const wchar_t* key, char* outStringVal, size_t length);
        /// @brief  Read standard string value
        /// @param[in]  key File entry
        /// @param[out] outStringVal Output value
        void read(const wchar_t* key, std::string& outStringVal);
        /// @brief Read wide string value
        /// @param[in]  key Registry key item
        /// @param[out] outWideStringVal Output value
        void read(const wchar_t* key, std::wstring& outWideStringVal);

        /// @brief Read all available values
        /// @param[out] outData Output values
        void readAll(std::map<std::string, uint32_t>& outData);
        /// @brief Read all available values
        /// @param[out] outData Output values
        void readAll(std::map<std::wstring, std::wstring>& outData);

        /// @brief Write boolean value
        /// @param[in]  key File entry
        /// @param[in]  val Written value
        void writeBool(const wchar_t* key, bool val);
        /// @brief Write boolean-typed enum value
        /// @param[in]  key File entry
        /// @param[in]  val Written value
        template<class T> inline void writeBoolType(const wchar_t* key, T val)
        {
            bool buffer = static_cast<bool>(val);
            writeBoolBool(key, buffer);
        }
        /// @brief Write integer value
        /// @param[in]  key File entry
        /// @param[in]  val Written value
        void writeInt(const wchar_t* key, uint32_t val);
        /// @brief Write integer-typed enum value
        /// @param[in]  key File entry
        /// @param[in]  val Written value
        template<class T> inline void writeIntType(const wchar_t* key, T val)
        {
            uint32_t buffer = static_cast<uint32_t>(val);
            writeInt(key, buffer);
        }
        /// @brief Write float value
        /// @param[in]  key Registry key item
        /// @param[in]  val Written value
        void writeInt(std::string& key, uint32_t val);
        /// @brief Write float value
        /// @param[in]  key File entry
        /// @param[in]  val Written value
        void writeFloat(const wchar_t* key, float val);
        /// @brief Write char string value
        /// @param[in]  key File entry
        /// @param[in]  val Written value
        void writeString(const wchar_t* key, char* val, size_t length);
        /// @brief Write standard string value
        /// @param[in]  key File entry
        /// @param[in]  val Written value
        void writeString(const wchar_t* key, std::string& val);
        /// @brief Write wide string value
        /// @param[in]  key File entry
        /// @param[in]  val Written value
        void writeWideString(const wchar_t* key, std::wstring& val);

        /// @brief Remove existing value from file
        /// @param[in]  key Entry
        /// @return Success
        bool removeValue(std::string& key);

    private:
        /// @brief Write value at the end of the file
        /// @param[in]  key File entry
        /// @param[in]  val Written value
        inline void appendValue(const wchar_t* key, std::wstring& val)
        {
            std::wstring valStr = std::wstring(key) + std::wstring(L";") + val + std::wstring(L"\n");
            m_file.write(valStr.c_str(), valStr.size());
        }
        /// @brief Write value at the end of the file
        /// @param[in]  key File entry
        /// @param[in]  val Written value
        inline void appendValue(const std::wstring& key, std::wstring& val)
        {
            std::wstring valStr = key + std::wstring(L";") + val + std::wstring(L"\n");
            m_file.write(valStr.c_str(), valStr.size());
        }
    };


    // -- common template tools -- ---------------------------------------------

    /// @brief Check if empty value
    /// @param[in] val Value to check
    template<typename T>
    bool isEmptyValue(T& val);
    template<> inline bool isEmptyValue<uint32_t>(uint32_t& val)
    {
        return (val == 0u);
    }
    template<> inline bool isEmptyValue<std::wstring>(std::wstring& val)
    {
        return (val.empty());
    }
    /// @brief Set empty value
    /// @param[out] outVal Value to reset
    template<typename T>
    void setEmptyValue(T& outVal);
    template<> inline void setEmptyValue<uint32_t>(uint32_t& outVal)
    {
        outVal = 0u;
    }
    template<> inline void setEmptyValue<std::wstring>(std::wstring& outVal)
    {
        outVal = L"";
    }
}
