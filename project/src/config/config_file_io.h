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
#include <string>

/// @namespace config
/// Configuration management
namespace config
{
    /// @enum file_io_mode_t
    /// @brief File IO mode
    enum class file_io_mode_t : uint32_t
    {
        none = 0,
        read = 1, ///< Open and read data file
        write = 2 ///< Create / overwrite data file
    };

    /// @class ConfigFileIO
    /// @brief Configuration file or registry input/output
    class ConfigFileIO
    {
    private:
        file_io_mode_t m_openMode;
        #ifdef _WINDOWS
        HKEY m_regKey;
        DWORD m_keyStatus;
        DWORD m_regBuffer;
        #else
        //...
        #endif

    public:
        ConfigFileIO() : m_openMode(file_io_mode_t::none) {}
        ~ConfigFileIO()
        {
            if (m_openMode != file_io_mode_t::none)
                close();
        }

        static bool remove(std::wstring path, std::wstring fileName);

        bool open(std::wstring path, file_io_mode_t mode = file_io_mode_t::read);
        // READ
            // si windows : ouvre clé registre + vérif si ok
            // si linux : ouvre fichier + vérif si ok + lecture totale en ajoutant à hashmap chaque entrée (ex: "abc"=def -> hash["abc"] = "def")
        // WRITE
            // si windows : crée/écrase clé registre + vérif si ok
            // si linux : crée/écrase fichier + vérif si ok

        inline void close()
        {
            m_openMode = file_io_mode_t::none;
            #ifdef _WINDOWS
            RegCloseKey(m_regKey);
            #else
            //...
            #endif
        }


        inline bool readBool(const wchar_t* key, bool& outVal)
        {
            if (m_openMode != file_io_mode_t::read)
                return false;
            //...
        }
        template<typename T> inline void readBoolType(const wchar_t* key, T& outVal, T defaultVal)
        {
            bool buffer;
            if (readBool(key, buffer))
                outVal = static_cast<T>(buffer);
            else
                outVal = defaultVal;
        }

        inline bool readDword(const wchar_t* key, uint32_t& outVal)
        {
            if (m_openMode != file_io_mode_t::read)
                return false;
            //...
        }
        template<typename T> inline void readDwordType(const wchar_t* key, T& outVal, uint32_t superiorLimit, T defaultVal)
        {
            uint32_t buffer;
            if (readDword(key, buffer))
                outVal = (buffer < superiorLimit) ? static_cast<T>(buffer) : defaultVal;
            else
                outVal = defaultVal;
        }

        void readFloat(const wchar_t* key, float& outVal);
        void readString(const wchar_t* key, char* outVal, size_t length);
        void readString(const wchar_t* key, std::string& outVal);
        void readWideString(const wchar_t* key, std::wstring& outVal);


        inline void writeBool(const wchar_t* key, bool val)
        {
            if (m_openMode != file_io_mode_t::write)
                return;
            //...
        }
        template<typename T> inline void writeBoolType(const wchar_t* key, T val)
        {
            bool buffer = static_cast<bool>(val);
            writeBoolBool(key, buffer);
        }


        inline void writeDword(const wchar_t* key, uint32_t val)
        {
            if (m_openMode != file_io_mode_t::write)
                return;
            //...
        }
        template<typename T> inline void writeDwordType(const wchar_t* key, T val)
        {
            uint32_t buffer = static_cast<uint32_t>(val);
            writeDword(key, buffer);
        }

        void writeFloat(const wchar_t* key, float val);
        void writeString(const wchar_t* key, char* val, size_t length);
        void writeString(const wchar_t* key, std::string& val);
        void writeWideString(const wchar_t* key, std::wstring& val);
    };
}
