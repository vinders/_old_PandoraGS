/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : string management toolset
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include "chars.h"

/// @namespace utils
/// General utilities
namespace utils
{
    using ustring = std::basic_string<utils::unicode_char_t>; ///< Unicode (UCS) string
    
    /// @namespace strings
    /// @brief String management toolset
    namespace strings
    {
        // -- Get size --
    
        /// @brief Check if a string is null or empty
        /// @param[in] data  String value
        /// @returns Empty/null (true) or not
        template <typename T>
        inline bool isEmpty(const T* data) { return (data == nullptr || *data == 0); }
        /// @brief Check if a string is empty
        /// @param[in] data  String value
        /// @returns Empty (true) or not
        template <typename T>
        inline bool isEmpty(const std::basic_string<T>& data) { return data.empty(); }
        
        /// @brief Get the length of a string
        /// @param[in] data  String value
        /// @returns Length of the string value
        template <typename T>
        inline size_t getLength(const T* data) 
        { 
            size_t len = 0u;
            if (data != nullptr)
            {
                while (*data != 0)
                {
                    ++data;
                    ++len;
                }
            }
            return len;
        }
        template <> inline size_t getLength<char>(const char* data) { return (data != nullptr) ? strlen(data) : 0u; }
        template <> inline size_t getLength<wchar_t>(const wchar_t* data) { return (data != nullptr) ? wcslen(data) : 0u; }
        /// @brief Get the length of a string
        /// @param[in] data  String value
        /// @returns Length of the string value
        template <typename T>
        inline size_t getLength(const std::basic_string<T>& data) { return data.size(); }
        
        
        // -- Change size --
        
        /// @brief Truncate beginning of a string
        /// @param[in] data        String value
        /// @param[in] desiredLen  Desired length after truncation
        /// @returns Truncated string
        template <typename T>
        inline std::basic_string<T> ltruncate(const std::basic_string<T>& data, const size_t desiredLen) 
        { 
            return (desiredLen < data.size()) ? data.substr(data.size() - desiredLen, desiredLen) : data;
        }
        /// @brief Truncate beginning of a string
        /// @param[in|out] data    String value to truncate
        /// @param[in] desiredLen  Desired length after truncation
        template <typename T>
        inline void ltruncateSelf(std::basic_string<T>& data, const size_t desiredLen) 
        { 
            if (desiredLen < data.size()) 
                data = data.substr(data.size() - desiredLen, desiredLen);
        }
        
        /// @brief Truncate end of a string
        /// @param[in] data        String value
        /// @param[in] desiredLen  Desired length after truncation
        /// @returns Truncated string
        template <typename T>
        inline std::basic_string<T> rtruncate(const std::basic_string<T>& data, const size_t desiredLen) 
        { 
            return (desiredLen <= data.size()) ? data.substr(0, desiredLen) : data;
        }
        /// @brief Truncate end of a string
        /// @param[in|out] data    String value to truncate
        /// @param[in] desiredLen  Desired length after truncation
        template <typename T>
        inline void rtruncateSelf(std::basic_string<T>& data, const size_t desiredLen) 
        { 
            if (desiredLen <= data.size()) 
                data = data.substr(0, desiredLen);
        }

        
        /// @brief Add padding at the beginning of a string
        /// @param[in] data        String value
        /// @param[in] desiredLen  Desired length with padding
        /// @param[in] padChar     Padding character (default: space)
        /// @returns Padded string
        template <typename T>
        inline std::basic_string<T> lpad(const std::basic_string<T>& data, const size_t desiredLen, const T padChar = T { 0x20 }) 
        { 
            if (desiredLen > data.size())
            {
                std::basic_string<T> out(desiredLen - data.size(), padChar);
                out += data;
                return out;
            }
            else
                return data;
        }
        /// @brief Add padding at the beginning of a string
        /// @param[in|out] data    String value to pad
        /// @param[in] desiredLen  Desired length with padding
        /// @param[in] padChar     Padding character (default: space)
        template <typename T>
        inline void lpadSelf(std::basic_string<T>& data, const size_t desiredLen, const T padChar = T { 0x20 }) 
        { 
            if (desiredLen > data.size())
            {
                std::basic_string<T> pad(desiredLen - data.size(), padChar);
                pad += data;
                data = std::move(pad);
            }
        }
        
        /// @brief Add padding at the end of a string
        /// @param[in] data        String value
        /// @param[in] desiredLen  Desired length with padding
        /// @param[in] padChar     Padding character (default: space)
        /// @returns Padded string
        template <typename T>
        inline std::basic_string<T> rpad(const std::basic_string<T>& data, const size_t desiredLen, const T padChar = T { 0x20 }) 
        { 
            if (desiredLen > data.size())
            {
                std::basic_string<T> pad(desiredLen - data.size(), padChar);
                return data + pad;
            }
            else
                return data;
        }
        /// @brief Add padding at the end of a string
        /// @param[in|out] data    String value to pad
        /// @param[in] desiredLen  Desired length with padding
        /// @param[in] padChar     Padding character (default: space)
        template <typename T>
        inline void rpadSelf(std::basic_string<T>& data, const size_t desiredLen, const T padChar = T { 0x20 }) 
        { 
            if (desiredLen > data.size())
            {
                std::basic_string<T> pad(desiredLen - data.size(), padChar);
                data += pad;
            }
        }
        
        
        /// @brief Change the length of a string - truncate beginning or add padding at the beginning, depending on the current length
        /// @param[in] data        String value
        /// @param[in] desiredLen  Desired length
        /// @param[in] padChar     Padding character (default: space)
        template <typename T>
        inline std::basic_string<T> ltruncateOrPad(const std::basic_string<T>& data, const size_t desiredLen, const T padChar = T { 0x20 }) 
        { 
            if (desiredLen <= data.size())
                return ltruncate(data, desiredLen);
            else
                return lpad(data, desiredLen);
        }
        /// @brief Change the length of a string - truncate beginning or add padding at the beginning, depending on the current length
        /// @param[in] data        String value
        /// @param[in] desiredLen  Desired length
        /// @param[in] padChar     Padding character (default: space)
        template <typename T>
        inline void ltruncateOrPadSelf(std::basic_string<T>& data, const size_t desiredLen, const T padChar = T { 0x20 }) 
        { 
            if (desiredLen <= data.size())
                ltruncateSelf(data, desiredLen);
            else
                lpadSelf(data, desiredLen);
        }
        
        /// @brief Change the length of a string - truncate end or add padding at the end, depending on the current length
        /// @param[in] data        String value
        /// @param[in] desiredLen  Desired length
        /// @param[in] padChar     Padding character (default: space)
        template <typename T>
        inline std::basic_string<T> rtruncateOrPad(const std::basic_string<T>& data, const size_t desiredLen, const T padChar = T { 0x20 }) 
        { 
            if (desiredLen <= data.size())
                return rtruncate(data, desiredLen);
            else
                return rpad(data, desiredLen);
        }
        /// @brief Change the length of a string - truncate end or add padding at the end, depending on the current length
        /// @param[in] data        String value
        /// @param[in] desiredLen  Desired length
        /// @param[in] padChar     Padding character (default: space)
        template <typename T>
        inline void rtruncateOrPadSelf(std::basic_string<T>& data, const size_t desiredLen, const T padChar = T { 0x20 }) 
        { 
            if (desiredLen <= data.size())
                rtruncateSelf(data, desiredLen);
            else
                rpadSelf(data, desiredLen);
        }
        
        
        // -- Shift characters --
        
        /// @brief Left-shift characters of a string
        /// @param[in|out] data  String value
        /// @param[in] len       Length of the string
        /// @param[in] rows      Number of rows to shift
        /// @param[in] fillChar  Character used to fill missing rows at the end (default: space)
        template <typename T> 
        void lshiftSelf(T* data, const size_t len, const size_t rows, const T fillChar = T { 0x20 })
        {
            if (data != nullptr && rows > 0u)
            {
                if (rows < len)
                {
                    memmove(data, &data[rows], (len - rows) * sizeof(T));
                    data = &data[len - 1];
                    for (uint32_t i = rows; i > 0; --i)
                    {
                        *data = fillChar;
                        --data;
                    }
                }
                else
                {
                    for (uint32_t i = len; i > 0; --i)
                    {
                        *data = fillChar;
                        ++data;
                    }
                }
            }
        }
        template <> void lshiftSelf<char>(char* data, const size_t len, const size_t rows, const char fillChar = ' ')
        {
            if (data != nullptr && rows > 0u)
            {
                if (rows < len)
                {
                    size_t shiftedLen = len - rows;
                    memmove(data, &data[rows], shiftedLen);
                    memset(&data[shiftedLen], fillChar, shiftedLen);
                }
                else
                    memset(data, fillChar, len);
            }
        }
        template <> void lshiftSelf<wchar_t>(wchar_t* data, const size_t len, const size_t rows, const wchar_t fillChar = L' ')
        {
            if (data != nullptr && rows > 0u)
            {
                if (rows < len)
                {
                    size_t shiftedLen = len - rows;
                    wmemmove(data, &data[rows], shiftedLen);
                    wmemset(&data[shiftedLen], fillChar, shiftedLen);
                }
                else
                    wmemset(data, fillChar, len);
            }
        }
        /// @brief Left-shift characters of a string
        /// @param[in] data      String value
        /// @param[in] rows      Number of rows to shift
        /// @param[in] fillChar  Character used to fill missing rows at the end (default: space)
        /// @returns Shifted string
        template <typename T> 
        std::basic_string<T> lshift(const std::basic_string<T>& data, const size_t rows, const T fillChar = T { 0x20 })
        {
            if (data.empty() == false && rows > 0u)
            {
                if (rows < data.size())
                    return data.substr(rows) + std::basic_string<T>(rows, fillChar);
                else
                    return std::basic_string<T>(data.size(), fillChar);;
            }
        }
        /// @brief Left-shift characters of a string
        /// @param[in|out] data  String value
        /// @param[in] rows      Number of rows to shift
        /// @param[in] fillChar  Character used to fill missing rows at the end (default: space)
        template <typename T> 
        std::basic_string<T> lshiftSelf(std::basic_string<T>& data, const size_t rows, const T fillChar = T { 0x20 })
        {
            data = std::move(lshift(data, rows, fillChar));
        }
        
        /// @brief Left-shift characters of a string (circular)
        /// @param[in|out] data  String value
        /// @param[in] len       Length of the string
        /// @param[in] rows      Number of rows to shift
        template <typename T> 
        void lshiftCircularSelf(T* data, const size_t len, size_t rows)
        {
            rows %= len;
            if (data != nullptr && rows > 0u)
            {
                T* droppedChars = new T[rows];
                memcpy(droppedChars, data, rows * sizeof(T));
                
                memmove(data, &data[rows], len - rows * sizeof(T));
                
                memcpy(&data[len - rows], droppedChars, rows * sizeof(T));
                delete [] droppedChars;
            }
        }
        /// @brief Left-shift characters of a string (circular)
        /// @param[in] data      String value
        /// @param[in] rows      Number of rows to shift
        /// @param[in] fillChar  Character used to fill missing rows at the end (default: space)
        /// @returns Shifted string
        template <typename T> 
        std::basic_string<T> lshiftCircular(const std::basic_string<T>& data, const size_t rows, const T fillChar = T { 0x20 })
        {
            rows %= len;
            if (data.empty() == false && rows > 0u)
                return data.substr(rows) + data.substr(0, rows);
        }
        /// @brief Left-shift characters of a string (circular)
        /// @param[in|out] data  String value
        /// @param[in] rows      Number of rows to shift
        /// @param[in] fillChar  Character used to fill missing rows at the end (default: space)
        template <typename T> 
        std::basic_string<T> lshiftCircularSelf(std::basic_string<T>& data, const size_t rows, const T fillChar = T { 0x20 })
        {
            data = std::move(lshiftCircularSelf(data, rows, fillChar));
        }
        
        
        /// @brief Right-shift characters of a string
        /// @param[in|out] data  String value
        /// @param[in] len       Length of the string
        /// @param[in] rows      Number of rows to shift
        /// @param[in] fillChar  Character used to fill missing rows at the end (default: space)
        template <typename T> 
        void rshiftSelf(T* data, const size_t len, const size_t rows, const T fillChar = T { 0x20 })
        {
            if (data != nullptr && rows > 0u)
            {
                if (rows < len)
                {
                    memmove(&data[rows], data, (len - rows) * sizeof(T));
                    for (uint32_t i = rows; i > 0; --i)
                    {
                        *data = fillChar;
                        ++data;
                    }
                }
                else
                {
                    for (uint32_t i = len; i > 0; --i)
                    {
                        *data = fillChar;
                        ++data;
                    }
                }
            }
        }
        template <> void rshiftSelf<char>(char* data, const size_t len, const size_t rows, const char fillChar = ' ')
        {
            if (data != nullptr && rows > 0u)
            {
                if (rows < len)
                {
                    size_t shiftedLen = len - rows;
                    memmove(&data[rows], data, shiftedLen);
                    memset(data, fillChar, shiftedLen);
                }
                else
                    memset(data, fillChar, len);
            }
        }
        template <> void rshiftSelf<wchar_t>(wchar_t* data, const size_t len, const size_t rows, const wchar_t fillChar = L' ')
        {
            if (data != nullptr && rows > 0u)
            {
                if (rows < len)
                {
                    size_t shiftedLen = len - rows;
                    wmemmove(&data[rows], data, shiftedLen);
                    wmemset(data, fillChar, shiftedLen);
                }
                else
                    wmemset(data, fillChar, len);
            }
        }
        /// @brief Right-shift characters of a string
        /// @param[in] data      String value
        /// @param[in] rows      Number of rows to shift
        /// @param[in] fillChar  Character used to fill missing rows at the end (default: space)
        /// @returns Shifted string
        template <typename T> 
        std::basic_string<T> rshift(const std::basic_string<T>& data, const size_t rows, const T fillChar = T { 0x20 })
        {
            if (data.empty() == false && rows > 0u)
            {
                if (rows < data.size())
                    return std::basic_string<T>(rows, fillChar) + data.substr(0, data.size() - rows);
                else
                    return std::basic_string<T>(data.size(), fillChar);;
            }
        }
        /// @brief Right-shift characters of a string
        /// @param[in|out] data  String value
        /// @param[in] rows      Number of rows to shift
        /// @param[in] fillChar  Character used to fill missing rows at the end (default: space)
        template <typename T> 
        std::basic_string<T> rshiftSelf(std::basic_string<T>& data, const size_t rows, const T fillChar = T { 0x20 })
        {
            data = std::move(rshift(data, rows, fillChar));
        }
        
        /// @brief Right-shift characters of a string (circular)
        /// @param[in|out] data  String value
        /// @param[in] len       Length of the string
        /// @param[in] rows      Number of rows to shift
        template <typename T> 
        void rshiftCircularSelf(T* data, const size_t len, size_t rows)
        {
            rows %= len;
            if (data != nullptr && rows > 0u)
            {
                size_t shiftedLen = len - rows;
                T* droppedChars = new T[rows];
                memcpy(droppedChars, &data[shiftedLen], rows * sizeof(T));
                
                memmove(&data[rows], data, shiftedLen * sizeof(T));
                
                memcpy(data, droppedChars, rows * sizeof(T));
                delete [] droppedChars;
            }
        }
        /// @brief Right-shift characters of a string (circular)
        /// @param[in] data      String value
        /// @param[in] rows      Number of rows to shift
        /// @param[in] fillChar  Character used to fill missing rows at the end (default: space)
        /// @returns Shifted string
        template <typename T> 
        std::basic_string<T> rshiftCircular(const std::basic_string<T>& data, const size_t rows, const T fillChar = T { 0x20 })
        {
            rows %= len;
            if (data.empty() == false && rows > 0u)
                return data.substr(data.size() - rows) + data.substr(0, data.size() - rows);
        }
        /// @brief Right-shift characters of a string (circular)
        /// @param[in|out] data  String value
        /// @param[in] rows      Number of rows to shift
        /// @param[in] fillChar  Character used to fill missing rows at the end (default: space)
        template <typename T> 
        std::basic_string<T> rshiftCircularSelf(std::basic_string<T>& data, const size_t rows, const T fillChar = T { 0x20 })
        {
            data = std::move(rshiftCircularSelf(data, rows, fillChar));
        }
        
        

        // -- Compare --
        
        /// @brief Compare two strings
        /// @param[in] lhs  First string value
        /// @param[in] rhs  Second string value
        /// @returns Comparison result : lhs==rhs (0), lhs<rhs (-1), lhs>rhs (1)
        template <typename T>
        inline int32_t compare(const T* lhs, const T* rhs)
        { 
            _STATIC_ASSERT_IS_CHARACTER_TYPE(lhs);
            if (lhs == rhs)
                return 0;
            if (lhs == nullptr)
                return (rhs == nullptr || *rhs == 0) ? 0 : -1;
            if (rhs == nullptr)
                return (*lhs == 0) ? 0 : 1;
            
            const T* lhsIt = lhs;
            const T* rhsIt = rhs;
            while (*lhsIt != 0 && *lhsIt == *rhsIt)
            {
                ++lhsIt;
                ++rhsIt;
            }
            if (*lhsIt == *rhsIt); // both '\0'
                return 0;
            else
                return (*lhsIt < *rhsIt) ? -1 : 1;
        }
        /// @brief Compare two strings
        /// @param[in] lhs  First string value
        /// @param[in] rhs  Second string value
        /// @returns Comparison result : lhs==rhs (0), lhs<rhs (-1), lhs>rhs (1)
        template <typename T>
        inline int32_t compare(const std::basic_string<T>& lhs, const std::basic_string<T>& rhs) { return lhs.compare(rhs); }
        
        
        /// @brief Check equality of two strings (case-sensitive)
        /// @param[in] lhs  First string value
        /// @param[in] rhs  Second string value
        /// @returns Equal (true) or not
        template <typename T>
        inline bool equals(const T* lhs, const T* rhs) 
        { 
            if (lhs != nullptr && rhs != nullptr)
            {
                const T* lhsIt = lhs;
                const T* rhsIt = rhs;
                while (*lhsIt != 0 && *lhsIt == *rhsIt)
                {
                    ++lhsIt;
                    ++rhsIt;
                }
                return (*lhsIt == *rhsIt); // both '\0'
            }
            return false;
        }
        /// @brief Check equality of two strings (case-sensitive)
        /// @param[in] lhs  First string value
        /// @param[in] rhs  Second string value
        /// @returns Equal (true) or not
        template <typename T>
        inline bool equals(const std::basic_string<T>& lhs, const std::basic_string<T>& rhs) { return (lhs.compare(rhs) == 0); }

        
        /// @brief Check equality of two strings (case-insensitive for ASCII alphabetic characters)
        /// @param[in] lhs  First string value
        /// @param[in] rhs  Second string value
        /// @returns Equal (true) or not
        template <typename T>
        inline bool iequalsAscii(const T* lhs, const T* rhs) 
        { 
            if (lhs != nullptr && rhs != nullptr)
            {
                const T* lhsIt = lhs;
                const T* rhsIt = rhs;
                while (*lhsIt != 0 && utils::chars::iequalsAscii(*lhsIt, *rhsIt))
                {
                    ++lhsIt;
                    ++rhsIt;
                }
                return (*lhsIt == *rhsIt); // both '\0'
            }
            return false;
        }
        /// @brief Check equality of two strings (case-insensitive for ASCII alphabetic characters)
        /// @param[in] lhs  First string value
        /// @param[in] rhs  Second string value
        /// @returns Equal (true) or not
        template <typename T>
        inline bool iequalsAscii(const std::basic_string<T>& lhs, const std::basic_string<T>& rhs) 
        { 
            const T* lhsIt = lhs.c_str();
            const T* rhsIt = rhs.c_str();
            while (*lhsIt != 0 && utils::chars::iequalsAscii(*lhsIt, *rhsIt))
            {
                ++lhsIt;
                ++rhsIt;
            }
            return (*lhsIt == *rhsIt); // both '\0'
        }
        
        
        /// @brief Check equality of two strings (case-insensitive for unicode alphabetic characters)
        /// @param[in] lhs  First string value
        /// @param[in] rhs  Second string value
        /// @returns Equal (true) or not
        template <typename T>
        inline bool iequalsUnicode(const T* lhs, const T* rhs) 
        { 
            if (lhs != nullptr && rhs != nullptr)
            {
                const T* lhsIt = lhs;
                const T* rhsIt = rhs;
                while (*lhsIt != 0 && utils::chars::iequalsUnicode(*lhsIt, *rhsIt))
                {
                    ++lhsIt;
                    ++rhsIt;
                }
                return (*lhsIt == *rhsIt); // both '\0'
            }
            return false;
        }
        /// @brief Check equality of two strings (case-insensitive for unicode alphabetic characters)
        /// @param[in] lhs  First string value
        /// @param[in] rhs  Second string value
        /// @returns Equal (true) or not
        template <typename T>
        inline bool iequalsUnicode(const std::basic_string<T>& lhs, const std::basic_string<T>& rhs) 
        { 
            const T* lhsIt = lhs.c_str();
            const T* rhsIt = rhs.c_str();
            while (*lhsIt != 0 && utils::chars::iequalsUnicode(*lhsIt, *rhsIt))
            {
                ++lhsIt;
                ++rhsIt;
            }
            return (*lhsIt == *rhsIt); // both '\0'
        }
        
        
        
        // -- Upper/lower-case converters --
        
        /// @brief Convert string to another upper-case string (ASCII alphabetic characters only)
        /// @param[in] data  String value
        /// @param[out] out  Output value
        template <typename T>
        inline void toUpperAscii(const T* data, T* out) 
        { 
            if (data != nullptr && out != nullptr)
            {
                while (*data != 0)
                {
                    *out = utils::chars::toUpperAscii(*data);
                    ++data;
                    ++out;
                }
                *out = 0;
            }
        }
        /// @brief Convert string to another upper-case string (ASCII alphabetic characters only)
        /// @param[in] data  String value
        /// @returns Output value
        template <typename T>
        inline std::basic_string<T> toUpperAscii(const std::basic_string<T>& data) 
        { 
            std::basic_string<T> out;
            out.reserve(data.size() + 1);
            for (uint32_t i = 0; i < data.size(); ++i)
                out += utils::chars::toUpperAscii(data[i]);
            return out;
        }
        /// @brief Convert string to upper-case string (ASCII alphabetic characters only)
        /// @param[in|out] data  String value to convert
        template <typename T>
        inline void toUpperAsciiSelf(T* data) 
        { 
            if (data != nullptr)
            {
                while (*data != 0)
                {
                    *data = utils::chars::toUpperAscii(*data);
                    ++data;
                }
            }
        }
        /// @brief Convert string to upper-case string (ASCII alphabetic characters only)
        /// @param[in|out] data  String value to convert
        template <typename T>
        inline void toUpperAsciiSelf(std::basic_string<T>& data) 
        { 
            for (int32_t i = data.size() - 1; i >= 0; --i)
                data[i] = utils::chars::toUpperAscii(data[i]);
        }
        
        
        /// @brief Convert string to another upper-case string (unicode alphabetic characters)
        /// @param[in] data  String value
        /// @param[out] out  Output value
        template <typename T>
        inline void toUpperUnicode(const T* data, T* out) 
        { 
            if (data != nullptr && out != nullptr)
            {
                while (*data != 0)
                {
                    *out = utils::chars::toUpperUnicode(*data);
                    ++data;
                    ++out;
                }
                *out = 0;
            }
        }
        /// @brief Convert string to another upper-case string (unicode alphabetic characters)
        /// @param[in] data  String value
        /// @returns Output value
        template <typename T>
        inline std::basic_string<T> toUpperUnicode(const std::basic_string<T>& data) 
        { 
            std::basic_string<T> out;
            out.reserve(data.size() + 1);
            for (uint32_t i = 0; i < data.size(); ++i)
                out += utils::chars::toUpperUnicode(data[i]);
            return out;
        }
        /// @brief Convert string to upper-case string (unicode alphabetic characters)
        /// @param[in|out] data  String value to convert
        template <typename T>
        inline void toUpperUnicodeSelf(T* data) 
        { 
            if (data != nullptr)
            {
                while (*data != 0)
                {
                    *data = utils::chars::toUpperUnicode(*data);
                    ++data;
                }
            }
        }
        /// @brief Convert string to upper-case string (unicode alphabetic characters)
        /// @param[in|out] data  String value to convert
        template <typename T>
        inline void toUpperUnicodeSelf(std::basic_string<T>& data) 
        { 
            for (int32_t i = data.size() - 1; i >= 0; --i)
                data[i] = utils::chars::toUpperUnicode(data[i]);
        }
        
        
        /// @brief Convert string to another lower-case string (ASCII alphabetic characters only)
        /// @param[in] data  String value
        /// @param[out] out  Output value
        template <typename T>
        inline void toLowerAscii(const T* data, T* out) 
        { 
            if (data != nullptr && out != nullptr)
            {
                while (*data != 0)
                {
                    *out = utils::chars::toLowerAscii(*data);
                    ++data;
                    ++out;
                }
                *out = 0;
            }
        }
        /// @brief Convert string to another lower-case string (ASCII alphabetic characters only)
        /// @param[in] data  String value
        /// @returns Output value
        template <typename T>
        inline std::basic_string<T> toLowerAscii(const std::basic_string<T>& data) 
        { 
            std::basic_string<T> out;
            out.reserve(data.size() + 1);
            for (uint32_t i = 0; i < data.size(); ++i)
                out += utils::chars::toLowerAscii(data[i]);
            return out;
        }
        /// @brief Convert string to lower-case string (ASCII alphabetic characters only)
        /// @param[in|out] data  String value to convert
        template <typename T>
        inline void toLowerAsciiSelf(T* data) 
        { 
            if (data != nullptr)
            {
                while (*data != 0)
                {
                    *data = utils::chars::toLowerAscii(*data);
                    ++data;
                }
            }
        }
        /// @brief Convert string to lower-case string (ASCII alphabetic characters only)
        /// @param[in|out] data  String value to convert
        template <typename T>
        inline void toLowerAsciiSelf(std::basic_string<T>& data) 
        { 
            for (int32_t i = data.size() - 1; i >= 0; --i)
                data[i] = utils::chars::toLowerAscii(data[i]);
        }
        
        
        /// @brief Convert string to another lower-case string (unicode alphabetic characters)
        /// @param[in] data  String value
        /// @param[out] out  Output value
        template <typename T>
        inline void toLowerUnicode(const T* data, T* out) 
        { 
            if (data != nullptr && out != nullptr)
            {
                while (*data != 0)
                {
                    *out = utils::chars::toLowerUnicode(*data);
                    ++data;
                    ++out;
                }
                *out = 0;
            }
        }
        /// @brief Convert string to another lower-case string (unicode alphabetic characters)
        /// @param[in] data  String value
        /// @returns Output value
        template <typename T>
        inline std::basic_string<T> toLowerUnicode(const std::basic_string<T>& data) 
        { 
            std::basic_string<T> out;
            out.reserve(data.size() + 1);
            for (uint32_t i = 0; i < data.size(); ++i)
                out += utils::chars::toLowerUnicode(data[i]);
            return out;
        }
        /// @brief Convert string to lower-case string (unicode alphabetic characters)
        /// @param[in|out] data  String value to convert
        template <typename T>
        inline void toLowerUnicodeSelf(T* data) 
        { 
            if (data != nullptr)
            {
                while (*data != 0)
                {
                    *data = utils::chars::toLowerUnicode(*data);
                    ++data;
                }
            }
        }
        /// @brief Convert string to lower-case string (unicode alphabetic characters)
        /// @param[in|out] data  String value to convert
        template <typename T>
        inline void toLowerUnicodeSelf(std::basic_string<T>& data) 
        { 
            for (int32_t i = data.size() - 1; i >= 0; --i)
                data[i] = utils::chars::toLowerUnicode(data[i]);
        }
        

        
        // -- Find first --
        
        constexpr size_t notFound = std::string::npos; ///< Needle not found (find/rfind methods)
        
        /// @brief Find first occurrence of a character in a string
        /// @param[in] data        String value
        /// @param[in] needleChar  Character to find (needle)
        /// @returns Needle position
        template <typename T>
        inline size_t find(const T* data, const T needleChar)
        {
            size_t needlePos = utils::String::notFound;
            size_t curPos = 0u;
            if (data != nullptr)
            {
                for (const T* it = data; *it != 0; ++it)
                {
                    if (*it == needleChar)
                    {
                        needlePos = curPos;
                        break;
                    }
                    ++curPos;
                }
            }
            return needlePos;
        }
        /// @brief Find first occurrence of a character in a string
        /// @param[in] data        String value
        /// @param[in] needleChar  Character to find (needle)
        /// @returns Needle position
        template <typename T>
        inline size_t find(const std::basic_string<T>& data, const T needleChar)
        {
            return data.find(needleChar);
        }
        
        /// @brief Find first occurrence of a pattern in a string
        /// @param[in] data     String value
        /// @param[in] pattern  String to find
        /// @returns Pattern position
        template <typename T>
        size_t find(const T* data, const T* pattern);
        /// @brief Find first occurrence of a pattern in a string
        /// @param[in] data     String value
        /// @param[in] pattern  String to find
        /// @returns Pattern position
        template <typename T>
        inline size_t find(const std::basic_string<T>& data, const std::basic_string<T>& pattern)
        {
            return data.find(pattern);
        }
        
        
        // -- Find last --
        
        /// @brief Find last occurrence of a character in a string
        /// @param[in] data        String value
        /// @param[in] needleChar  Character to find (needle)
        /// @returns Needle position
        /// @warning Very inefficient. Prefer rfind(const T* data, const size_t len, const T needleChar).
        template <typename T>
        inline size_t rfind(const T* data, const T needleChar)
        {
            size_t needlePos = utils::String::notFound;
            size_t curPos = 0u;
            if (data != nullptr)
            {
                for (const T* it = data; *it != 0; ++it)
                {
                    if (*it == needleChar)
                        needlePos = curPos;
                    ++curPos;
                }
            }
            return needlePos;
        }
        /// @brief Find last occurrence of a character in a string
        /// @param[in] data        String value
        /// @param[in] needleChar  Character to find (needle)
        /// @returns Needle position
        template <typename T>
        inline size_t rfind(const T* data, const size_t len, const T needleChar)
        {
            size_t needlePos = utils::String::notFound;
            size_t curPos = len;
            if (data != nullptr)
            {
                for (const T* it = data[len - 1]; curPos > 0; --it)
                {
                    if (*it == needleChar)
                    {
                        needlePos = curPos - 1u;
                        break;
                    }
                    --curPos;
                }
            }
            return needlePos;
        }
        /// @brief Find last occurrence of a character in a string
        /// @param[in] data        String value
        /// @param[in] needleChar  Character to find (needle)
        /// @returns Needle position
        template <typename T>
        inline size_t rfind(const std::basic_string<T>& data, const T needleChar)
        {
            return data.rfind(needleChar);
        }
        
        /// @brief Find last occurrence of a pattern in a string
        /// @param[in] data     String value
        /// @param[in] pattern  String to find
        /// @returns Pattern position
        /// @warning Very inefficient. Prefer rfind(const T* data, const size_t len, const T* pattern).
        template <typename T>
        size_t rfind(const T* data, const T* pattern);
        /// @brief Find last occurrence of a pattern in a string
        /// @param[in] data     String value
        /// @param[in] pattern  String to find
        /// @returns Pattern position
        template <typename T>
        size_t rfind(const T* data, const size_t len, const T* pattern);
        /// @brief Find last occurrence of a pattern in a string
        /// @param[in] data     String value
        /// @param[in] pattern  String to find
        /// @returns Pattern position
        template <typename T>
        inline size_t rfind(const std::basic_string<T>& data, const std::basic_string<T>& pattern)
        {
            return data.rfind(pattern);
        }
        
        
        // -- Find all --
        
        /// @brief Find all occurrences of a character in a string
        /// @param[in] data        String value
        /// @param[in] needleChar  Character to find (needle)
        /// @returns Vector with found positions
        template <typename T>
        inline std::vector<size_t> findAll(const T* data, const T needleChar)
        {
            std::vector<size_t> posArray;
            posArray.reserve(8);
            size_t curPos = 0u;
            if (data != nullptr)
            {
                for (const T* it = data; *it != 0; ++it)
                {
                    if (*it == needleChar)
                        posArray.push_back(curPos);
                    ++curPos;
                }
            }
            return posArray;
        }
        /// @brief Find all occurrences of a character in a string
        /// @param[in] data        String value
        /// @param[in] needleChar  Character to find (needle)
        /// @returns Vector with found positions
        template <typename T>
        inline std::vector<size_t> findAll(const std::basic_string<T>& data, const T needleChar)
        {
            std::vector<size_t> posArray;
            posArray.reserve(8);
            size_t curPos = 0u;
            if (data.empty() == false)
            {
                for (const T* it = data.c_str(); *it != 0; ++it)
                {
                    if (*it == needleChar)
                        posArray.push_back(curPos);
                    ++curPos;
                }
            }
            return posArray;
        }
        
        /// @brief Find all occurrences of a pattern in a string (occurrences may overlap)
        /// @param[in] data     String value
        /// @param[in] pattern  String to find
        /// @returns Vector with found positions
        template <typename T>
        std::vector<size_t> findAll(const T* data, const T* pattern);
        /// @brief Find all occurrences of a pattern in a string (occurrences may overlap)
        /// @param[in] data     String value
        /// @param[in] pattern  String to find
        /// @returns Vector with found positions
        template <typename T>
        std::vector<size_t> findAll(const std::basic_string<T>& data, const std::basic_string<T>& pattern);
        
        
        // -- Count all --

        /// @brief Count all occurrences of a character in a string
        /// @param[in] data        String value
        /// @param[in] needleChar  Character to find (needle)
        /// @returns Number of occurrences
        template <typename T>
        inline uint32_t countOccurrences(const T* data, const T needleChar)
        {
            uint32_t count = 0u;
            size_t curPos = 0u;
            if (data != nullptr)
            {
                for (const T* it = data; *it != 0; ++it)
                {
                    if (*it == needleChar)
                        ++count;
                    ++curPos;
                }
            }
            return count;
        }
        /// @brief Count all occurrences of a character in a string
        /// @param[in] data        String value
        /// @param[in] needleChar  Character to find (needle)
        /// @returns Number of occurrences
        template <typename T>
        inline uint32_t countOccurrences(const std::basic_string<T>& data, const T needleChar)
        {
            uint32_t count = 0u;
            size_t curPos = 0u;
            if (data.empty() == false)
            {
                for (const T* it = data.c_str(); *it != 0; ++it)
                {
                    if (*it == needleChar)
                        ++count;
                    ++curPos;
                }
            }
            return count;
        }
        
        /// @brief Count all occurrences of a pattern in a string (occurrences may overlap)
        /// @param[in] data     String value
        /// @param[in] pattern  String to find
        /// @returns Number of occurrences
        template <typename T>
        uint32_t countOccurrences(const T* data, const T* pattern);
        /// @brief Count all occurrences of a pattern in a string (occurrences may overlap)
        /// @param[in] data     String value
        /// @param[in] pattern  String to find
        /// @returns Number of occurrences
        template <typename T>
        uint32_t countOccurrences(const std::basic_string<T>& data, const std::basic_string<T>& pattern);
        
        
        // -- Other finding utilities --
        
        /// @brief Check whether a string contains a character or not
        /// @param[in] data        String value
        /// @param[in] needleChar  Character to find
        /// @returns Occurrence(s) found (true) or not
        template <typename T>
        inline bool contains(const T* data, const T needleChar) { return (find(data, needleChar) != utils::String::notFound); }
        /// @brief Check whether a string contains a character or not
        /// @param[in] data        String value
        /// @param[in] needleChar  Character to find
        /// @returns Occurrence(s) found (true) or not
        template <typename T>
        inline bool contains(const std::basic_string<T>& data, const T needleChar) { return (data.find(needleChar) != std::basic_string<T>::npos); }
        /// @brief Check whether a string contains a pattern or not
        /// @param[in] data     String value
        /// @param[in] pattern  Pattern to find
        /// @returns Occurrence(s) found (true) or not
        template <typename T>
        inline bool contains(const T* data, const T* pattern) { return (find(data, pattern) != utils::String::notFound); }
        /// @brief Check whether a string contains a pattern or not
        /// @param[in] data     String value
        /// @param[in] pattern  Pattern to find
        /// @returns Occurrence(s) found (true) or not
        template <typename T>
        inline bool contains(const std::basic_string<T>& data, const std::basic_string<T>& pattern) { return (data.find(pattern) != std::basic_string<T>::npos); }
        
        /// @brief Check whether a string starts with a character or not
        /// @param[in] data        String value
        /// @param[in] needleChar  Character to find
        /// @returns Occurrence(s) found at the beginning (true) or not
        template <typename T>
        inline bool startsWith(const T* data, const T needleChar) { return (data != nullptr && *data == needleChar); }
        /// @brief Check whether a string starts with a character or not
        /// @param[in] data        String value
        /// @param[in] needleChar  Character to find
        /// @returns Occurrence(s) found at the beginning (true) or not
        template <typename T>
        inline bool startsWith(const std::basic_string<T>& data, const T needleChar) { return (data.empty() == false && data[0] == needleChar); }
        /// @brief Check whether a string starts with a pattern or not
        /// @param[in] data     String value
        /// @param[in] pattern  Pattern to find
        /// @returns Occurrence(s) found at the beginning (true) or not
        template <typename T>
        inline bool startsWith(const T* data, const T* pattern)
        {
            if (data != nullptr && pattern != nullptr)
            {
                while (*data != 0 && *data == *pattern)
                {
                    ++data;
                    ++pattern;
                }
                return (*pattern == 0);
            }
            return false;
        }
        /// @brief Check whether a string starts with a pattern or not
        /// @param[in] data     String value
        /// @param[in] pattern  Pattern to find
        /// @returns Occurrence(s) found at the beginning (true) or not
        template <typename T>
        inline bool startsWith(const std::basic_string<T>& data, const std::basic_string<T>& pattern)
        {
            const T* it = data.c_str();
            const T* patternIt = pattern.c_str();
            while (*it != 0 && *it == *patternIt)
            {
                ++it;
                ++patternIt;
            }
            return (*patternIt == 0);
        }
        
        /// @brief Check whether a string ends with a character or not
        /// @param[in] data        String value
        /// @param[in] needleChar  Character to find
        /// @returns Occurrence(s) found at the end (true) or not
        template <typename T>
        inline bool endsWith(const T* data, const size_t len, const T needleChar) { return (data != nullptr && data[len - 1u] == needleChar); }
        /// @brief Check whether a string ends with a character or not
        /// @param[in] data        String value
        /// @param[in] needleChar  Character to find
        /// @returns Occurrence(s) found at the end (true) or not
        template <typename T>
        inline bool endsWith(const std::basic_string<T>& data, const T needleChar) { return (data.empty() == false && data[data.size() - 1u] == needleChar); }
        /// @brief Check whether a string ends with a pattern or not
        /// @param[in] data     String value
        /// @param[in] pattern  Pattern to find
        /// @returns Occurrence(s) found at the end (true) or not
        template <typename T>
        inline bool endsWith(const T* data, const size_t len, const T* pattern) { return (rfind(data, len, pattern) == len - getLength(pattern)); }
        {
            size_t patternLen = getLength(pattern);
            if (data != nullptr && patternLen > 0u && patternLen <= len)
            {
                const T* it = &data[len - patternLen];
                while (*it != 0 && *it == *pattern)
                {
                    ++it;
                    ++pattern;
                }
                return (*pattern == 0);
            }
            return false;
        }
        /// @brief Check whether a string ends with a pattern or not
        /// @param[in] data     String value
        /// @param[in] pattern  Pattern to find
        /// @returns Occurrence(s) found at the end (true) or not
        template <typename T>
        inline bool endsWith(const std::basic_string<T>& data, const std::basic_string<T>& pattern) { return (data.rfind(pattern) == data.size() - pattern.size()); }
        {
            size_t patternLen = pattern.size();
            if (patternLen > 0u && patternLen <= len)
            {
                const T* it = &data.c_str[data.size() - patternLen];
                const T* patternIt = pattern.c_str();
                while (*it != 0 && *it == *patternIt)
                {
                    ++it;
                    ++patternIt;
                }
                return (*patternIt == 0);
            }
            return false;
        }
        
        
        
        // -- Split --

        /// @brief Split a string at the specified position (character at position is included in second part)
        /// @param[in] data  String value
        /// @param[in] pos   Split position (included in second part)
        /// @returns Vector with sub-strings
        template <typename T>
        inline std::vector<std::basic_string<T>> splitAt(const std::basic_string<T>& data, const uint32_t pos)
        {
            if (data.size() > pos)
            {
                std::vector<std::basic_string<T>> parts { data.substr(0, pos), data.substr(pos) };
                return parts;
            }
            else
                return std::vector<std::basic_string<T>> { data };
        }
        /// @brief Split a string at the specified position (character at position is excluded)
        /// @param[in] data  String value
        /// @param[in] pos   Split position (excluded)
        /// @returns Vector with sub-strings
        template <typename T>
        inline std::vector<std::basic_string<T>> splitAtExcluded(const std::basic_string<T>& data, const uint32_t pos)
        {
            if (data.size() > pos + 1)
            {
                std::vector<std::basic_string<T>> parts { data.substr(0, pos), data.substr(pos + 1u) };
                return parts;
            }
            else if (data.size() > pos)
                return std::vector<std::basic_string<T>> { data.substr(0, pos) };
            else
                return std::vector<std::basic_string<T>> { data };
        }
    
        
        /// @brief Split a string every time an occurrence of a character is found
        /// @param[in] data                 String value
        /// @param[in] sep                  Separator (character)
        /// @param[in] isKeepingEmptyParts  If two separators are contiguous, add an empty sub-string (default: false)
        /// @returns Vector with sub-strings
        template <typename T>
        std::vector<std::basic_string<T>> split(const std::basic_string<T>& data, const T sep, const bool isKeepingEmptyParts = false);
        
        /// @brief Split a string every time an occurrence of a pattern is found (no overlap)
        /// @param[in] data                 String value
        /// @param[in] sepString            Separator (pattern)
        /// @param[in] isKeepingEmptyParts  If two separators are contiguous, add an empty sub-string (default: false)
        /// @returns Vector with sub-strings
        template <typename T>
        std::vector<std::basic_string<T>> split(const std::basic_string<T>& data, const std::basic_string<T>& sepString, const bool isKeepingEmptyParts = false);
        
        
        
        // -- Trim --
        
        /// @brief Remove all white-space characters at the beginning of a string
        /// @param[in|out] data  String value
        /// @param[in] len       Length of the string 'data'
        /// @returns Remaining length
        template <typename T>
        inline size_t ltrimSelf(T* data, const size_t len)
        {
            if (data != nullptr && len > 0)
            {
                // find first non-white-space character
                size_t pos = 0u;
                for (const T* it = data; pos < len && utils::chars::isWhiteSpace(*it); ++it)
                    ++pos;
                // trim
                if (pos > 0u)
                {
                    size_t remLen = len - pos;
                    memmove(data, &data[pos], remLen * sizeof(T));
                    data[remLen] = '\0';
                    return remLen;
                }
            }
            return len;
        }
        /// @brief Remove all white-space characters at the beginning of a string
        /// @param[in] data  String value
        /// @returns Left-trimmed string
        template <typename T>
        inline std::basic_string<T> ltrim(const std::basic_string<T>& data)
        {
            if (data.empty() == false)
            {
                // find first non-white-space character
                size_t pos = 0u;
                const T* it = data.c_str();
                while (utils::chars::isWhiteSpace(*it))
                {
                    ++it;
                    ++pos;
                }
                if (pos >= data.size())
                    pos = data.size() - 1u;
                // trim
                return (pos == 0u) ? data : data.substr(pos);
            }
        }
        /// @brief Remove all white-space characters at the beginning of a string
        /// @param[in|out] data  String value
        template <typename T>
        inline void ltrimSelf(std::basic_string<T>& data)
        {
            data = std::move(ltrim(data));
        }
        
        
        /// @brief Remove all white-space characters at the end of a string
        /// @param[in|out] data  String value
        /// @param[in] len       Length of the string 'data'
        /// @returns Remaining length
        template <typename T>
        inline size_t rtrimSelf(T* data, const size_t len)
        {
            if (data != nullptr && len > 0)
            {
                // find last non-white-space character
                int32_t pos = len - 1;
                for (const T* it = data + pos; pos >= 0 && utils::chars::isWhiteSpace(*it); --it)
                    --pos;
                // trim
                if (pos < len - 1)
                {
                    size_t remLen = pos + 1;
                    data[remLen] = '\0';
                    return remLen;
                }
            }
            return len;
        }
        /// @brief Remove all white-space characters at the end of a string
        /// @param[in] data  String value
        /// @returns Right-trimmed string
        template <typename T>
        inline std::basic_string<T> rtrim(const std::basic_string<T>& data)
        {
            if (data.empty() == false)
            {
                // find last non-white-space character
                int32_t pos = data.size() - 1;
                const T* it = data.c_str();
                it += pos;
                while (pos >= 0 && utils::chars::isWhiteSpace(*it))
                {
                    --it;
                    --pos;
                }
                // trim
                return (pos == data.size() - 1) ? data : data.substr(0, pos + 1);
            }
        }
        /// @brief Remove all white-space characters at the end of a string
        /// @param[in|out] data  String value
        template <typename T>
        inline void rtrimSelf(std::basic_string<T>& data)
        {
            data = std::move(rtrim(data));
        }
        
        
        /// @brief Remove all white-space characters at the beginning and the end of a string
        /// @param[in|out] data  String value
        /// @param[in] len       Length of the string 'data'
        /// @returns Remaining length
        template <typename T>
        inline size_t trimSelf(T* data, const size_t len)
        {
            return rtrimSelf(data, ltrimSelf(data, len));
        }
        /// @brief Remove all white-space characters at the beginning and the end of a string
        /// @param[in] data  String value
        /// @returns Trimmed string
        template <typename T>
        inline std::basic_string<T> trim(const std::basic_string<T>& data)
        {
            if (data.empty() == false)
            {
                // find first non-white-space character
                int32_t posFirst = 0;
                const T* it = data.c_str();
                while (utils::chars::isWhiteSpace(*it))
                {
                    ++it;
                    ++posFirst;
                }

                // find last non-white-space character
                int32_t posLast = data.size() - 1;
                const T* it = data.c_str();
                it += posLast;
                while (posLast >= 0 && utils::chars::isWhiteSpace(*it))
                {
                    --it;
                    --posLast;
                }
                // trim
                if (posFirst == 0)
                {
                    return (posLast == data.size() - 1u) ? data : data.substr(0, posLast + 1);
                }
                else if (posLast == data.size() - 1)
                    return data.substr(posFirst);
                else
                    return (posFirst < posLast) ? data.substr(posFirst, (posLast + 1) - posFirst) : std::basic_string<T> {};
            }
        }
        /// @brief Remove all white-space characters at the beginning and the end of a string
        /// @param[in|out] data  String value
        template <typename T>
        void trimSelf(std::basic_string<T>& data)
        {
            data = std::move(trim(data));
        }
    }
}

// -- Specialized templates --

template <> size_t utils::strings::find<char>        (const char* data, const char* pattern);
template <> size_t utils::strings::find<unsigned char>(const unsigned char* data, const unsigned char* pattern);
template <> size_t utils::strings::find<signed char> (const signed char* data, const signed char* pattern);
template <> size_t utils::strings::find<wchar_t>     (const wchar_t* data, const wchar_t* pattern);
template <> size_t utils::strings::find<char16_t>    (const char16_t* data, const char16_t* pattern);
template <> size_t utils::strings::find<char32_t>    (const char32_t* data, const char32_t* pattern);
template <> size_t utils::strings::find<utils::unicode_char_t>(const utils::unicode_char_t* data, const utils::unicode_char_t* pattern);
template <> size_t utils::strings::find<int>         (const int* data, const int* pattern);
template <> size_t utils::strings::find<unsigned int>(const unsigned int* data, const unsigned int* pattern);
template <> size_t utils::strings::find<uint8_t>    (const uint8_t* data, const uint8_t* pattern);
template <> size_t utils::strings::find<uint16_t>    (const uint16_t* data, const uint16_t* pattern);
template <> size_t utils::strings::find<uint32_t>    (const uint32_t* data, const uint32_t* pattern);
template <> size_t utils::strings::find<int32_t>     (const int32_t* data, const int32_t* pattern);

template <> size_t utils::strings::rfind<char>        (const char* data, const char* pattern);
template <> size_t utils::strings::rfind<unsigned char>(const unsigned char* data, const unsigned char* pattern);
template <> size_t utils::strings::rfind<signed char> (const signed char* data, const signed char* pattern);
template <> size_t utils::strings::rfind<wchar_t>     (const wchar_t* data, const wchar_t* pattern);
template <> size_t utils::strings::rfind<char16_t>    (const char16_t* data, const char16_t* pattern);
template <> size_t utils::strings::rfind<char32_t>    (const char32_t* data, const char32_t* pattern);
template <> size_t utils::strings::rfind<utils::unicode_char_t>(const utils::unicode_char_t* data, const utils::unicode_char_t* pattern);
template <> size_t utils::strings::rfind<int>         (const int* data, const int* pattern);
template <> size_t utils::strings::rfind<unsigned int>(const unsigned int* data, const unsigned int* pattern);
template <> size_t utils::strings::rfind<uint8_t>    (const uint8_t* data, const uint8_t* pattern);
template <> size_t utils::strings::rfind<uint16_t>    (const uint16_t* data, const uint16_t* pattern);
template <> size_t utils::strings::rfind<uint32_t>    (const uint32_t* data, const uint32_t* pattern);
template <> size_t utils::strings::rfind<int32_t>     (const int32_t* data, const int32_t* pattern);

template <> size_t utils::strings::rfind<char>        (const char* data, const size_t len, const char* pattern);
template <> size_t utils::strings::rfind<unsigned char>(const unsigned char* data, const size_t len, const unsigned char* pattern);
template <> size_t utils::strings::rfind<signed char> (const signed char* data, const size_t len, const signed char* pattern);
template <> size_t utils::strings::rfind<wchar_t>     (const wchar_t* data, const size_t len, const wchar_t* pattern);
template <> size_t utils::strings::rfind<char16_t>    (const char16_t* data, const size_t len, const char16_t* pattern);
template <> size_t utils::strings::rfind<char32_t>    (const char32_t* data, const size_t len, const char32_t* pattern);
template <> size_t utils::strings::rfind<utils::unicode_char_t>(const utils::unicode_char_t* data, const size_t len, const utils::unicode_char_t* pattern);
template <> size_t utils::strings::rfind<int>         (const int* data, const size_t len, const int* pattern);
template <> size_t utils::strings::rfind<unsigned int>(const unsigned int* data, const size_t len, const unsigned int* pattern);
template <> size_t utils::strings::rfind<uint8_t>    (const uint8_t* data, const size_t len, const uint8_t* pattern);
template <> size_t utils::strings::rfind<uint16_t>    (const uint16_t* data, const size_t len, const uint16_t* pattern);
template <> size_t utils::strings::rfind<uint32_t>    (const uint32_t* data, const size_t len, const uint32_t* pattern);
template <> size_t utils::strings::rfind<int32_t>     (const int32_t* data, const size_t len, const int32_t* pattern);

template <> std::vector<size_t> utils::strings::findAll<char>        (const char* data, const char* pattern);
template <> std::vector<size_t> utils::strings::findAll<unsigned char>(const unsigned char* data, const unsigned char* pattern);
template <> std::vector<size_t> utils::strings::findAll<signed char> (const signed char* data, const signed char* pattern);
template <> std::vector<size_t> utils::strings::findAll<wchar_t>     (const wchar_t* data, const wchar_t* pattern);
template <> std::vector<size_t> utils::strings::findAll<char16_t>    (const char16_t* data, const char16_t* pattern);
template <> std::vector<size_t> utils::strings::findAll<char32_t>    (const char32_t* data, const char32_t* pattern);
template <> std::vector<size_t> utils::strings::findAll<utils::unicode_char_t>(const utils::unicode_char_t* data, const utils::unicode_char_t* pattern);
template <> std::vector<size_t> utils::strings::findAll<int>         (const int* data, const int* pattern);
template <> std::vector<size_t> utils::strings::findAll<unsigned int>(const unsigned int* data, const unsigned int* pattern);
template <> std::vector<size_t> utils::strings::findAll<uint8_t>    (const uint8_t* data, const uint8_t* pattern);
template <> std::vector<size_t> utils::strings::findAll<uint16_t>    (const uint16_t* data, const uint16_t* pattern);
template <> std::vector<size_t> utils::strings::findAll<uint32_t>    (const uint32_t* data, const uint32_t* pattern);
template <> std::vector<size_t> utils::strings::findAll<int32_t>     (const int32_t* data, const int32_t* pattern);

template <> std::vector<size_t> utils::strings::findAll<char>        (const std::basic_string<char>& data, const std::basic_string<char>& pattern);
template <> std::vector<size_t> utils::strings::findAll<unsigned char>(const std::basic_string<unsigned char>& data, const std::basic_string<unsigned char>& pattern);
template <> std::vector<size_t> utils::strings::findAll<signed char> (const std::basic_string<signed char>& data, const std::basic_string<signed char>& pattern);
template <> std::vector<size_t> utils::strings::findAll<wchar_t>     (const std::basic_string<wchar_t>& data, const std::basic_string<wchar_t>& pattern);
template <> std::vector<size_t> utils::strings::findAll<char16_t>    (const std::basic_string<char16_t>& data, const std::basic_string<char16_t>& pattern);
template <> std::vector<size_t> utils::strings::findAll<char32_t>    (const std::basic_string<char32_t>& data, const std::basic_string<char32_t>& pattern);
template <> std::vector<size_t> utils::strings::findAll<utils::unicode_char_t>(const std::basic_string<utils::unicode_char_t>& data, const std::basic_string<utils::unicode_char_t>& pattern);
template <> std::vector<size_t> utils::strings::findAll<int>         (const std::basic_string<int>& data, const std::basic_string<int>& pattern);
template <> std::vector<size_t> utils::strings::findAll<unsigned int>(const std::basic_string<unsigned int>& data, const std::basic_string<unsigned int>& pattern);
template <> std::vector<size_t> utils::strings::findAll<uint8_t>    (const std::basic_string<uint8_t>& data, const std::basic_string<uint8_t>& pattern);
template <> std::vector<size_t> utils::strings::findAll<uint16_t>    (const std::basic_string<uint16_t>& data, const std::basic_string<uint16_t>& pattern);
template <> std::vector<size_t> utils::strings::findAll<uint32_t>    (const std::basic_string<uint32_t>& data, const std::basic_string<uint32_t>& pattern);
template <> std::vector<size_t> utils::strings::findAll<int32_t>     (const std::basic_string<int32_t>& data, const std::basic_string<int32_t>& pattern);

template <> uint32_t utils::strings::countOccurrences<char>        (const char* data, const char* pattern);
template <> uint32_t utils::strings::countOccurrences<unsigned char>(const unsigned char* data, const unsigned char* pattern);
template <> uint32_t utils::strings::countOccurrences<signed char> (const signed char* data, const signed char* pattern);
template <> uint32_t utils::strings::countOccurrences<wchar_t>     (const wchar_t* data, const wchar_t* pattern);
template <> uint32_t utils::strings::countOccurrences<char16_t>    (const char16_t* data, const char16_t* pattern);
template <> uint32_t utils::strings::countOccurrences<char32_t>    (const char32_t* data, const char32_t* pattern);
template <> uint32_t utils::strings::countOccurrences<utils::unicode_char_t>(const utils::unicode_char_t* data, const utils::unicode_char_t* pattern);
template <> uint32_t utils::strings::countOccurrences<int>         (const int* data, const int* pattern);
template <> uint32_t utils::strings::countOccurrences<unsigned int>(const unsigned int* data, const unsigned int* pattern);
template <> uint32_t utils::strings::countOccurrences<uint8_t>    (const uint8_t* data, const uint8_t* pattern);
template <> uint32_t utils::strings::countOccurrences<uint16_t>    (const uint16_t* data, const uint16_t* pattern);
template <> uint32_t utils::strings::countOccurrences<uint32_t>    (const uint32_t* data, const uint32_t* pattern);
template <> uint32_t utils::strings::countOccurrences<int32_t>     (const int32_t* data, const int32_t* pattern);

template <> uint32_t utils::strings::countOccurrences<char>        (const std::basic_string<char>& data, const std::basic_string<char>& pattern);
template <> uint32_t utils::strings::countOccurrences<unsigned char>(const std::basic_string<unsigned char>& data, const std::basic_string<unsigned char>& pattern);
template <> uint32_t utils::strings::countOccurrences<signed char> (const std::basic_string<signed char>& data, const std::basic_string<signed char>& pattern);
template <> uint32_t utils::strings::countOccurrences<wchar_t>     (const std::basic_string<wchar_t>& data, const std::basic_string<wchar_t>& pattern);
template <> uint32_t utils::strings::countOccurrences<char16_t>    (const std::basic_string<char16_t>& data, const std::basic_string<char16_t>& pattern);
template <> uint32_t utils::strings::countOccurrences<char32_t>    (const std::basic_string<char32_t>& data, const std::basic_string<char32_t>& pattern);
template <> uint32_t utils::strings::countOccurrences<utils::unicode_char_t>(const std::basic_string<utils::unicode_char_t>& data, const std::basic_string<utils::unicode_char_t>& pattern);
template <> uint32_t utils::strings::countOccurrences<int>         (const std::basic_string<int>& data, const std::basic_string<int>& pattern);
template <> uint32_t utils::strings::countOccurrences<unsigned int>(const std::basic_string<unsigned int>& data, const std::basic_string<unsigned int>& pattern);
template <> uint32_t utils::strings::countOccurrences<uint8_t>    (const std::basic_string<uint8_t>& data, const std::basic_string<uint8_t>& pattern);
template <> uint32_t utils::strings::countOccurrences<uint16_t>    (const std::basic_string<uint16_t>& data, const std::basic_string<uint16_t>& pattern);
template <> uint32_t utils::strings::countOccurrences<uint32_t>    (const std::basic_string<uint32_t>& data, const std::basic_string<uint32_t>& pattern);
template <> uint32_t utils::strings::countOccurrences<int32_t>     (const std::basic_string<int32_t>& data, const std::basic_string<int32_t>& pattern);

template <> std::vector<std::basic_string<T>> utils::strings::split<char>        (const std::basic_string<char>& data, const char sep, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<unsigned char>(const std::basic_string<unsigned char>& data, const unsigned char sep, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<signed char> (const std::basic_string<signed char>& data, const signed char sep, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<wchar_t>     (const std::basic_string<wchar_t>& data, const wchar_t sep, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<char16_t>    (const std::basic_string<char16_t>& data, const char16_t sep, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<char32_t>    (const std::basic_string<char32_t>& data, const char32_t sep, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<utils::unicode_char_t>(const std::basic_string<utils::unicode_char_t>& data, const utils::unicode_char_t sep, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<int>         (const std::basic_string<int>& data, const int sep, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<unsigned int>(const std::basic_string<unsigned int>& data, const unsigned int sep, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<uint8_t>    (const std::basic_string<uint8_t>& data, const uint8_t sep, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<uint16_t>    (const std::basic_string<uint16_t>& data, const uint16_t sep, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<uint32_t>    (const std::basic_string<uint32_t>& data, const uint32_t sep, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<int32_t>     (const std::basic_string<int32_t>& data, const int32_t sep, const bool isKeepingEmptyParts = false);

template <> std::vector<std::basic_string<T>> utils::strings::split<char>        (const std::basic_string<char>& data, const std::basic_string<char>& sepString, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<unsigned char>(const std::basic_string<unsigned char>& data, const std::basic_string<unsigned char>& sepString, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<signed char> (const std::basic_string<signed char>& data, const std::basic_string<signed char>& sepString, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<wchar_t>     (const std::basic_string<wchar_t>& data, const std::basic_string<wchar_t>& sepString, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<char16_t>    (const std::basic_string<char16_t>& data, const std::basic_string<char16_t>& sepString, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<char32_t>    (const std::basic_string<char32_t>& data, const std::basic_string<char32_t>& sepString, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<utils::unicode_char_t>(const std::basic_string<utils::unicode_char_t>& data, const std::basic_string<utils::unicode_char_t>& sepString, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<int>         (const std::basic_string<int>& data, const std::basic_string<int>& sepString, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<unsigned int>(const std::basic_string<unsigned int>& data, const std::basic_string<unsigned int>& sepString, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<uint8_t>    (const std::basic_string<uint8_t>& data, const std::basic_string<uint8_t>& sepString, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<uint16_t>    (const std::basic_string<uint16_t>& data, const std::basic_string<uint16_t>& sepString, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<uint32_t>    (const std::basic_string<uint32_t>& data, const std::basic_string<uint32_t>& sepString, const bool isKeepingEmptyParts = false);
template <> std::vector<std::basic_string<T>> utils::strings::split<int32_t>     (const std::basic_string<int32_t>& data, const std::basic_string<int32_t>& sepString, const bool isKeepingEmptyParts = false);
