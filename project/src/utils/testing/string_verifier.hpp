/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : string verification toolset
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstring>
#include <string>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.testing
    /// Testing utilities
    namespace testing
    {
        /// @class StringVerifier
        /// @brief Strings verifications
        class StringVerifier
        {
        public:
            // StringVerifier.isEmpty ("")
            static bool isEmpty(const char* data) { return (data == nullptr || data[0] == '\0'); }
            static bool isEmpty(const std::string data) { return (data.empty()); }
            static bool isEmpty(const std::wstring data) { return (data.empty()); }
            template <typename T>
            static bool isEmpty(const std::basic_string<T> data) { return (data.empty()); }
            // StringVerifier.isNotEmpty ("...")
            static bool isNotEmpty(const char* data) { return (data != nullptr && data[0] != '\0'); }
            static bool isNotEmpty(const std::string data) { return (!data.empty()); }
            static bool isNotEmpty(const std::wstring data) { return (!data.empty()); }
            template <typename T>
            static bool isNotEmpty(const std::basic_string<T> data) { return (!data.empty()); }
            
            // StringVerifier.isEqual (==)
            static bool isEqual(const char* first, const char* second) { return (first == second || strcmp(first, second) == 0); }
            static bool isEqual(const std::string first, const std::string second) { return (first.compare(second) == 0); }
            static bool isEqual(const std::wstring first, const std::wstring second) { return (first.compare(second) == 0); }
            template <typename T>
            static bool isEqual(const std::basic_string<T> first, const std::basic_string<T> second) { return (data.empty()); }
            // StringVerifier.isNotEqual (!=)
            static bool isNotEqual(const char* first, const char* second) { return (first != second || strcmp(first, second) != 0); }
            static bool isNotEqual(const std::string first, const std::string second) { return (first.compare(second) != 0); }
            static bool isNotEqual(const std::wstring first, const std::wstring second) { return (first.compare(second) != 0); }
            template <typename T>
            static bool isNotEqual(const std::basic_string<T> first, const std::basic_string<T> second) { return (first.compare(second) != 0); }
            
            // StringVerifier.startsWith (Abc[...])
            static bool startsWith(const std::string data, const std::string pattern) { return (data.find(pattern) == 0u); }
            static bool startsWith(const std::wstring data, const std::wstring pattern) { return (data.find(pattern) == 0u); }
            // StringVerifier.endsWith ([...]abc)
            static bool endsWith(const std::string data, const std::string pattern) 
            { 
                size_t pos = data.find(pattern); 
                return (pos != std::string::npos && pos + pattern.size() == data.size()); 
            }
            static bool endsWith(const std::wstring data, const std::wstring pattern) 
            { 
                size_t pos = data.find(pattern); 
                return (pos != std::wstring::npos && pos + pattern.size() == data.size()); 
            }
            // StringVerifier.contains ([...]abc[...])
            static bool contains(const std::string data, const std::string pattern) { return (data.find(pattern) != std::string::npos); }
            static bool contains(const std::wstring data, const std::wstring pattern) { return (data.find(pattern) != std::wstring::npos); }
        };
    }
}
