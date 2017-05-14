/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : file input/output and encoding - base
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <codecvt>
#include <locale>
#include <vector>
#include "../thread/thread_mutex.hpp"

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.io
    /// Input/output utilities
    namespace io
    {
        /// @class FileReader
        /// @brief Advanced file reader
        class FileIO
        {
        public:
            /// @enum seek_reference_t
            /// @brief Reference point for seek operations
            enum class seek_reference_t : uint32_t
            {
                begin = 0u,   ///< reference from beginning of the file
                current = 1u, ///< reference from current read/write position
                end = 2u      ///< reference from end of the file
            };
        
            /// @enum file_encoder_t
            /// @brief File encoding types
            enum class file_encoder_t : uint32_t
            {
                binary = 0u,   ///< binary mode (depends on endianness)
                ansi = 1u,     ///< text mode - ANSI Windows-1252 (Western European)
                utf8 = 2u,     ///< text mode - UTF-8 (standard)
                utf8_bom = 3u, ///< text mode - UTF-8 with byte order mark
                utf16_le = 4u, ///< text mode - UTF-16 little endian (standard)
                utf16_be = 5u  ///< text mode - UTF-16 big endian
            };
            /// @enum string_encoder_t
            /// @brief String encoding types
            enum class string_encoder_t : uint32_t
            {
                ansi = 1u,
                utf8 = 2u
            };
            /// @enum wstring_encoder_t
            /// @brief Wide string encoding types
            enum class wstring_encoder_t : uint32_t
            {
                utf16 = 4u,
                ucs2 = 6u
            };
            
            /// @brief Get system home directory
            /// @returns Home directory path
            static std::wstring getHomeDirectoryPath() noexcept;
            /// @brief Get working directory
            /// @returns Working directory path
            static std::wstring getWorkingDirectoryPath() noexcept;
            
            
            // -- Character byte conversion --
            
            /// @brief Split wide character into 2 bytes
            /// @param[in] val       Wide character
            /// @param[out] outHigh  Byte with higher bits
            /// @param[out] outLow   Byte with lower bits
            static inline void wideCharToBytes(const wchar_t val, char& outHigh, char& outLow)
            {
                wchar_t highBits = val >> 8;
                wchar_t lowBits = (val & 0x0FF);
                outHigh = static_cast<char>(highBits);
                outLow = static_cast<char>(lowBits);
            }
            /// @brief Convert 2 bytes to wide character
            /// @param[in] valHigh  Byte with higher bits
            /// @param[in] valLow   Byte with lower bits
            /// @param[out] out     Wide character
            static inline void bytesToWideChar(const char valHigh, const char valLow, wchar_t& out)
            {
                out = ((static_cast<wchar_t>(valHigh) << 8) | static_cast<wchar_t>(valLow));
            }
            
            
            // -- Character encoding conversion --
            
            /// @brief Convert code-page string to UTF-8 string
            /// @param[in] str       Source string
            /// @param[in] codePage  Code-page identifier
            /// @returns UTF-8 string
            static inline std::string codePageToUtf8(const char* str, const std::locale codePage)
            {
                if (str == nullptr || str[0] == '\0')
                    return std::string("");
                
                // code page to unicode
                std::vector<wchar_t> codePageVec(unicodeStr.size());
                std::use_facet<std::ctype<wchar_t>>(codePage).widen(str, str + strlen(str), codePageVec.data());
                // unicode to UTF-8
                std::string unicodeStr(codePageVec.data(), codePageVec.size());
                return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(unicodeStr);
            }
            /// @brief Convert ANSI (Windows-1252) string to UTF-8 string
            /// @param[in] str       Source ANSI string
            /// @returns UTF-8 string
            static inline std::string ansi1252ToUtf8(const char* str)
            {
                return codePageToUtf8(str, std::locale(".1252"));
            }
            
            /// @brief Convert UTF-8 string to code-page string
            /// @param[in] utf8      Source UTF-8 string
            /// @param[in] codePage  Code-page identifier
            /// @returns Code-page string
            static inline std::string utf8ToCodePage(const char* utf8, const std::locale codePage)
            {
                if (utf8 == nullptr || str[0] == '\0')
                    return std::string("");
                
                // UTF-8 to unicode
                wstring unicodeStr = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(utf8str);
                // unicode to code page
                std::vector<char> codePageVec(unicodeStr.size());
                std::use_facet<std::ctype<wchar_t>>(codePage).narrow(unicodeStr.data(), unicodeStr.data() + unicodeStr.size(), '?', codePageVec.data());
                return std::string(codePageVec.data(), codePageVec.size());
            }
            /// @brief Convert UTF-8 string to ANSI (Windows-1252) string
            /// @param[in] utf8      Source UTF-8 string
            /// @returns Code-page string
            static inline std::string utf8ToAnsi1252(const char* utf8)
            {
                return utf8ToCodePage(utf8, std::locale(".1252"));
            }
            

            /// @brief Convert code-page string to UTF-16 string
            /// @param[in] str       Source string
            /// @param[in] codePage  Code-page identifier
            /// @returns UTF-16 string
            static inline std::wstring codePageToUtf16(const char* str, const std::locale codePage)
            {
                if (str == nullptr || str[0] == '\0')
                    return std::wstring("");
                
                // code page to unicode
                std::vector<wchar_t> unicodeVec(unicodeStr.size());
                std::use_facet<std::ctype<wchar_t>>(codePage).widen(str, str + strlen(str), unicodeVec.data());
                // unicode to UTF-16
                uint32_t tmpCode;
                std::vector<wchar_t> utf16Vec(unicodeStr.size());
                utf16Vec.reserve(unicodeVec.size() << 1);
                for (uint32_t i = 0; i < unicodeVec.size(); ++i)
                {
                    tmpCode = static_cast<uint32_t>(unicodeVec[i]);
                    if (tmpCode < 0xD800u || (tmpCode > 0xDFFF && tmpCode <= 0xFFFF))
                    {
                        utf16Vec.push_back(unicodeVec[i]);
                    }
                    else if (tmpCode > 0xFFFFu && tmpCode <= 0x1FFFFFu)
                    {
                        int32_t tmpCodeSigned = static_cast<int32_t>(unicodeVec[i]);
                        tmpCodeSigned -= 0x010000;
                        utf16Vec.push_back((tmpCodeSigned >> 10) | 0xD800);
                        utf16Vec.push_back((tmpCodeSigned & 0x3FF) | 0xDC00);
                    }
                    else
                    {
                        utf16Vec.push_back(0x003F); // '?'
                    }
                }
                return std::wstring(utf16Vec.data(), utf16Vec.size());
            }
            /// @brief Convert ANSI (Windows-1252) string to UTF-16 string
            /// @param[in] str       Source ANSI string
            /// @returns UTF-16 string
            static inline std::wstring ansi1252ToUtf16(const char* str)
            {
                return codePageToUtf16(str, std::locale(".1252"));
            }
            
            /// @brief Convert UTF-16 string to code-page string
            /// @param[in] utf16     Source UTF-16 string
            /// @param[in] codePage  Code-page identifier
            /// @returns Code-page string
            static inline std::string utf16ToCodePage(const wchar_t* utf16, const std::locale codePage)
            {
                if (utf16 == nullptr || str[0] == L'\0')
                    return std::string("");
                
                // UTF-16 to unicode
                uint32_t tmpCode;
                std::vector<wchar_t> unicodeStr(256);
                unicodeStr.reserve(unicodeVec.size() << 1);
                for (uint32_t i = 0; utf16[i] != L'\0'; ++i)
                {
                    tmpCode = static_cast<uint32_t>(utf16[i]);
                    if (tmpCode < 0xD800u || (tmpCode > 0xDFFF && tmpCode <= 0xFFFF))
                    {
                        unicodeStr.push_back(utf16[i]);
                    }
                    else
                    {
                        int32_t tmpCodeSigned = ((static_cast<int32_t>(utf16[i]) &~ 0xD800) << 10);
                        ++i
                        tmpCodeSigned += (static_cast<int32_t>(utf16[i]) &~ 0xDC00);
                        tmpCodeSigned += 0x010000;
                        unicodeStr.push_back(tmpCode);
                    }
                }
                // unicode to code page
                std::vector<char> codePageVec(unicodeStr.size());
                std::use_facet<std::ctype<wchar_t>>(codePage).narrow(unicodeStr.data(), unicodeStr.data() + unicodeStr.size(), '?', codePageVec.data());
                return std::string(codePageVec.data(), codePageVec.size());
            }
            /// @brief Convert UTF-16 string to ANSI (Windows-1252) string
            /// @param[in] utf16     Source UTF-16 string
            /// @returns Code-page string
            static inline std::string utf16ToAnsi1252(const wchar_t* utf16)
            {
                return utf16ToCodePage(utf16, std::locale(".1252"));
            }


            /// @brief Convert UTF-8 string to UTF-16 string
            /// @param[in] utf8  Source UTF-8 string
            /// @returns UTF-16 string
            static inline std::wstring utf8ToUtf16(const char* utf8)
            {
                return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conv.from_bytes(utf8);
            }
            /// @brief Convert UTF-16 string to UTF-8 string
            /// @param[in] utf16  Source UTF-16 string
            /// @returns UTF-8 string
            static inline std::string utf16ToUtf8(const wchar_t* utf16)
            {
                return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conv.to_bytes(utf16);
            }
            
            
            /// @brief Convert ANSI (Windows 1252) string to UCS-2 string
            /// @param[in] str  Source ANSI string
            /// @returns UCS-2 string
            static inline std::wstring ansi1252ToUcs2(const char* str)
            {
                return utf16ToUcs2(ansi1252ToUtf16(utf8).c_str());
            }
            /// @brief Convert UCS-2 string to ANSI (Windows 1252) string
            /// @param[in] ucs2  Source UCS-2 string
            /// @returns ANSI string
            static inline std::string ucs2ToAnsi1252(const wchar_t* ucs2)
            {
                return utf16ToAnsi1252(ucs2ToUtf16(ucs2).c_str());
            }
            
            /// @brief Convert UTF-8 string to UCS-2 string
            /// @param[in] utf8  Source UTF-8 string
            /// @returns UCS-2 string
            static inline std::wstring utf8ToUcs2(const char* utf8)
            {
                return utf16ToUcs2(utf8ToUtf16(utf8).c_str());
            }
            /// @brief Convert UCS-2 string to UTF-8 string
            /// @param[in] ucs2  Source UCS-2 string
            /// @returns UTF-8 string
            static inline std::string ucs2ToUtf8(const wchar_t* ucs2)
            {
                return utf16ToUtf8(ucs2ToUtf16(ucs2).c_str());
            }
            
            /// @brief Convert UTF-16 string to UCS-2 string
            /// @param[in] utf8  Source UTF-16 string
            /// @returns UCS-2 string
            static inline std::wstring utf16ToUcs2(const wchar_t* utf16)
            {
                uint32_t tmpCode;
                std::vector<wchar_t> ucs2Str;
                ucs2Str.reserve(256);
                for (uint32_t i = 0; utf16[i] != L'\0'; ++i)
                {
                    tmpCode = static_cast<uint32_t>(utf16[i])
                    if (tmpCode < 0xD800u || tmpCode > 0xDFFF) // valid
                    {
                        ucs2Str.push_back(utf16[i]);
                    }
                    else // 2-code-point character -> no conversion
                    {
                        ucs2Str.push_back(0x003F); // '?'
                        ++i; // skip second code-point
                    }
                }
                return std::wstring(ucs2Str.data(), ucs2Str.size());
            }
            /// @brief Convert UCS-2 string to UTF-16 string
            /// @param[in] ucs2  Source UCS-2 string
            /// @returns UTF-16 string
            static inline std::wstring ucs2ToUtf16(const wchar_t* ucs2)
            {
                return std::wstring(ucs2);
            }
            
            
        protected:
            /// @brief Default constructor
            FileIO() {}
            /// @brief Move instance
            /// @param[in] other  Other instance
            FileIO(FileIO&& other) : m_fileLock(std::move(other.m_fileLock)) {}
            /// @brief Swap instances
            /// @param[in] other  Other instance
            void swap(FileWriter& other) noexcept
            {
                ThreadSpinLock tmp = std::move(other.m_fileLock);
                other.m_fileLock = std::move(m_fileLock);
                m_fileLock = std::move(tmp);
            }
            
            
            // -- Concurrency protection --
            
            /// @brief Lock file access
            template <bool checkConcurrency = true>
            void lock();
            /// @brief Try to lock file access - with timeout
            template <bool checkConcurrency = true>
            void lock(uint32_t timeout);
            /// @brief Try to lock file access - no wait
            template <bool checkConcurrency = true>
            void tryLock();
            /// @brief Unlock file access
            template <bool checkConcurrency = true>
            void unlock();

            
        private:
            utils::thread::ThreadMutex m_fileLock;  ///< Concurrency protection lock
        };
        
        
        
        // -- Concurrency protection --
        
        /// @brief Lock file access
        template <>
        inline void FileIO::lock<true>()
        {
            m_fileLock.lock();
        }
        /// @brief Try to lock file access - with timeout (milliseconds)
        template <>
        inline void FileIO::lock<true>(uint32_t timeout)
        {
            m_fileLock.lock(timeout);
        }
        /// @brief Try to lock file access - no wait
        template <>
        inline void FileIO::tryLock<true>()
        {
            m_fileLock.tryLock();
        }
        /// @brief Unlock file access
        template <>
        inline void FileIO::unlock<true>()
        {
            m_fileLock.unlock();
        }
        
        /// @brief Lock file access
        template <>
        inline void FileIO::lock<false>() { (void)0; }
        /// @brief Try to lock file access - with timeout
        template <>
        inline void FileIO::lock<false>(uint32_t timeout) { (void)0; }
        /// @brief Try to lock file access - no wait
        template <>
        inline void FileIO::tryLock<false>() { (void)0; }
        /// @brief Unlock file access
        template <>
        inline void FileIO::unlock<false>() { (void)0; }
    }
}
