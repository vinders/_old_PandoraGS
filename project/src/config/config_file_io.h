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
    /// @class ConfigFileIO
    /// @brief Configuration file or registry input/output
    class ConfigFileIO
    {
    public:
        int open(std::wstring path);
            // si windows : ouvre clé registre + vérif si ok
            // si linux : ouvre fichier + vérif si ok + lecture totale en ajoutant à hashmap chaque entrée (ex: "abc"=def -> hash["abc"] = "def")

        void close();

        inline void readBool(const wchar_t* key, bool& outVal)
        {

        }
        template<typename T> inline void readBoolEnum(const wchar_t* key, T& outVal)
        {
            bool buffer;
            readBool(key, buffer);
            outVal = static_cast<T>(buffer);
        }


        inline void readDword(const wchar_t* key, uint32_t& outVal)
        {

        }
        template<typename T> inline void readDwordEnum(const wchar_t* key, T& outVal, uint32_t superiorLimit, T default)
        {
            uint32_t buffer;
            readDword(key, buffer);
            outVal = (buffer < superiorLimit) ? static_cast<T>(buffer) : default;
        }

        inline void readFloat(const wchar_t* key, float& outVal)
        {

        }

        void readString(const wchar_t* key, char* outVal, size_t length);
        void readString(const wchar_t* key, std::string& outVal);
        void readWideString(const wchar_t* key, std::wstring& outVal);

    };
}
