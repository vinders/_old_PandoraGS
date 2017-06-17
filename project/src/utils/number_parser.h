/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : number parser
*******************************************************************************/
#pragma once

#undef min
#undef max

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @class NumberParser
    /// @brief Number parser
    class NumberParser
    {
    public:
        /// @brief Parse string with leading number
        /// @returns Output value
        template <typename T>
        static inline T parse(const char* val);
        /// @brief Parse string with leading number
        /// @param[in] val   String to parse
        /// @param[out] out  Output value
        /// @returns Number of characters read
        template <typename T>
        static inline size_t parse(const char* val, T& out);
        /// @brief Parse sub-string with leading number
        /// @param[in] val   String to parse
        /// @param[in] len   String length
        /// @param[out] out  Output value
        /// @returns Number of characters read
        template <typename T>
        static inline size_t parse(const char* val, const size_t len, T& out);
        
        /// @brief Parse string with leading integer number, formatted with specific base type
        /// @returns Output value
        template <typename T, uint32_t Base>
        static inline T parseInt(const char* val);
        /// @brief Parse string with leading integer number, formatted with specific base type
        /// @param[in] val   String to parse
        /// @param[out] out  Output value
        /// @returns Number of characters read
        template <typename T, uint32_t Base>
        static inline size_t parseInt(const char* val, T& out);
        /// @brief Parse sub-string with leading integer number, formatted with specific base type
        /// @param[in] val   String to parse
        /// @param[in] len   String length
        /// @param[out] out  Output value
        /// @returns Number of characters read
        template <typename T, uint32_t Base>
        static inline size_t parseInt(const char* val, const size_t len, T& out);
        

        /// @brief Get error message (call this if parsed size is 0)
        /// @returns Last error message
        static const std::string& getLastError() { return m_lastError; }
        
        
    private:
        // -- Parser methods --
        
        /// @brief Parse string with leading number
        /// @param[in] val   String to parse
        /// @param[out] out  Output value (0 if empty/null string)
        /// @returns Number of characters read
        template <typename T, bool isSigned, uint32_t Base>
        static size_t parseString(const char* val, T& out);
        /// @brief Parse sub-string with leading number
        /// @param[in] val   String to parse
        /// @param[in] len   String length
        /// @param[out] out  Output value (0 if empty/null string)
        /// @returns Number of characters read
        template <typename T, bool isSigned, uint32_t Base>
        static size_t parseString(const char* val, const size_t len, T& out);
        
        /// @brief Parse string with leading floating-point number
        /// @param[in] val   String to parse
        /// @param[out] out  Output value
        /// @returns Number of characters read
        template <typename T>
        static size_t parseFloatString(const char* val, T& out);
        /// @brief Parse string with leading floating-point number
        /// @param[in] val   String to parse
        /// @param[in] len   String length
        /// @param[out] out  Output value
        /// @returns Number of characters read
        template <typename T>
        static size_t parseFloatString(const char* val, const size_t len, T& out);
        /// @brief Parse string with leading boolean value (allowed values : true/false/TRUE/FALSE/0/1/00...00/00..01)
        /// @param[in] val   String to parse
        /// @param[out] out  Output value (false if empty or null string)
        /// @returns Number of characters read
        static size_t parseBoolString(const char* val, bool& out);
        
    private:
        static std::string m_lastError; ///< Last error message
    };
}

// -- Specialized template - parse --

template <> inline bool   utils::NumberParser::parse<bool>(const char* val)
{ bool parsed; parseBoolString(val, parsed); return parsed; }
template <> inline size_t utils::NumberParser::parse<bool>(const char* val, bool& out);
{ return parseBoolString(val, out); }

template <> inline int64_t utils::NumberParser::parse<int64_t>(const char* val);
{ int64_t parsed; parseString<int64_t, true, 10>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parse<int64_t>(const char* val, int64& out);
{ return parseString<int64_t, true, 10>(val, out); }
template <> inline size_t  utils::NumberParser::parse<int64_t>(const char* val, size_t len, int64& out);
{ return parseString<int64_t, true, 10>(val, len, out); }
template <> inline int32_t utils::NumberParser::parse<int32_t>(const char* val);
{ int32_t parsed; parseString<int32_t, true, 10>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parse<int32_t>(const char* val, int32_t& out);
{ return parseString<int32_t, true, 10>(val, out); }
template <> inline size_t  utils::NumberParser::parse<int32_t>(const char* val, size_t len, int32_t& out);
{ return parseString<int32_t, true, 10>(val, len, out); }
template <> inline int16_t utils::NumberParser::parse<int16_t>(const char* val);
{ int16_t parsed; parseString<int16_t, true, 10>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parse<int16_t>(const char* val, int16_t& out);
{ return parseString<int16_t, true, 10>(val, out); }
template <> inline size_t  utils::NumberParser::parse<int16_t>(const char* val, size_t len, int16_t& out);
{ return parseString<int16_t, true, 10>(val, len, out); }
template <> inline int8_t  utils::NumberParser::parse<int8_t>(const char* val);
{ int8_t parsed; parseString<int8_t, true, 10>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parse<int8_t>(const char* val, int8_t& out);
{ return parseString<int8_t, true, 10>(val, out); }
template <> inline size_t  utils::NumberParser::parse<int8_t>(const char* val, size_t len, int8_t& out);
{ return parseString<int8_t, true, 10>(val, len, out); }

template <> inline uint64_t utils::NumberParser::parse<uint64_t>(const char* val);
{ uint64_t parsed; parseString<uint64_t, false, 10>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parse<uint64_t>(const char* val, uint64& out);
{ return parseString<uint64_t, false, 10>(val, out); }
template <> inline size_t   utils::NumberParser::parse<uint64_t>(const char* val, size_t len, uint64& out);
{ return parseString<uint64_t, false, 10>(val, len, out); }
template <> inline uint32_t utils::NumberParser::parse<uint32_t>(const char* val);
{ uint32_t parsed; parseString<uint32_t, false, 10>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parse<uint32_t>(const char* val, uint32_t& out);
{ return parseString<uint32_t, false, 10>(val, out); }
template <> inline size_t   utils::NumberParser::parse<uint32_t>(const char* val, size_t len, iunt32_t& out);
{ return parseString<uint32_t, false, 10>(val, len, out); }
template <> inline uint16_t utils::NumberParser::parse<uint16_t>(const char* val);
{ uint16_t parsed; parseString<uint16_t, false, 10>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parse<uint16_t>(const char* val, uint16_t& out);
{ return parseString<uint16_t, false, 10>(val, out); }
template <> inline size_t   utils::NumberParser::parse<uint16_t>(const char* val, size_t len, uint16_t& out);
{ return parseString<uint16_t, false, 10>(val, len, out); }
template <> inline uint8_t  utils::NumberParser::parse<uint8_t>(const char* val);
{ uint8_t parsed; parseString<uint8_t, false, 10>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parse<uint8_t>(const char* val, uint8_t& out);
{ return parseString<uint8_t, false, 10>(val, out); }
template <> inline size_t   utils::NumberParser::parse<uint8_t>(const char* val, size_t len, uint8_t& out);
{ return parseString<uint8_t, false, 10>(val, len, out); }

template <> inline float  utils::NumberParser::parse<float>(const char* val);
{ float parsed; parseFloatString<float>(val, parsed); return parsed; }
template <> inline size_t utils::NumberParser::parse<float>(const char* val, float& out);
{ return parseString<float>(val, out); }
template <> inline size_t utils::NumberParser::parse<float>(const char* val, size_t len, float& out);
{ return parseString<float>(val, len, out); }
template <> inline double utils::NumberParser::parse<double>(const char* val);
{ float parsed; parseFloatString<double>(val, parsed); return parsed; }
template <> inline size_t utils::NumberParser::parse<double>(const char* val, double& out);
{ return parseString<double>(val, out); }
template <> inline size_t utils::NumberParser::parse<double>(const char* val, size_t len, double& out);
{ return parseString<double>(val, len, out); }

// -- Specialized template - parseInt --

template <> inline int64_t utils::NumberParser::parseInt<int64_t, 2>(const char* val);
{ int64_t parsed; parseString<int64_t, true, 2>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 2>(const char* val, int64& out);
{ return parseString<int64_t, true, 2>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 2>(const char* val, size_t len, int64& out);
{ return parseString<int64_t, true, 2>(val, len, out); }
template <> inline int32_t utils::NumberParser::parseInt<int32_t, 2>(const char* val);
{ int32_t parsed; parseString<int32_t, true, 2>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 2>(const char* val, int32_t& out);
{ return parseString<int32_t, true, 2>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 2>(const char* val, size_t len, int32_t& out);
{ return parseString<int32_t, true, 2>(val, len, out); }
template <> inline int16_t utils::NumberParser::parseInt<int16_t, 2>(const char* val);
{ int16_t parsed; parseString<int16_t, true, 2>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 2>(const char* val, int16_t& out);
{ return parseString<int16_t, true, 2>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 2>(const char* val, size_t len, int16_t& out);
{ return parseString<int16_t, true, 2>(val, len, out); }
template <> inline int8_t  utils::NumberParser::parseInt<int8_t,  2>(const char* val);
{ int8_t parsed; parseString<int8_t, true, 2>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  2>(const char* val, int8_t& out);
{ return parseString<int8_t, true, 2>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  2>(const char* val, size_t len, int8_t& out);
{ return parseString<int8_t, true, 2>(val, len, out); }
template <> inline uint64_t utils::NumberParser::parseInt<uint64_t, 2>(const char* val);
{ uint64_t parsed; parseString<uint64_t, false, 2>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 2>(const char* val, int64& out);
{ return parseString<uint64_t, false, 2>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 2>(const char* val, size_t len, int64& out);
{ return parseString<uint64_t, false, 2>(val, len, out); }
template <> inline uint32_t utils::NumberParser::parseInt<uint32_t, 2>(const char* val);
{ uint32_t parsed; parseString<uint32_t, false, 2>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 2>(const char* val, int32_t& out);
{ return parseString<uint32_t, false, 2>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 2>(const char* val, size_t len, int32_t& out);
{ return parseString<uint32_t, false, 2>(val, len, out); }
template <> inline uint16_t utils::NumberParser::parseInt<uint16_t, 2>(const char* val);
{ uint16_t parsed; parseString<uint16_t, false, 2>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 2>(const char* val, int16_t& out);
{ return parseString<uint16_t, false, 2>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 2>(const char* val, size_t len, int16_t& out);
{ return parseString<uint16_t, false, 2>(val, len, out); }
template <> inline uint8_t  utils::NumberParser::parseInt<uint8_t,  2>(const char* val);
{ uint8_t parsed; parseString<uint8_t, false, 2>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  2>(const char* val, int8_t& out);
{ return parseString<uint8_t, false, 2>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  2>(const char* val, size_t len, int8_t& out);
{ return parseString<uint8_t, false, 2>(val, len, out); }

template <> inline int64_t utils::NumberParser::parseInt<int64_t, 4>(const char* val);
{ int64_t parsed; parseString<int64_t, true, 4>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 4>(const char* val, int64& out);
{ return parseString<int64_t, true, 4>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 4>(const char* val, size_t len, int64& out);
{ return parseString<int64_t, true, 4>(val, len, out); }
template <> inline int32_t utils::NumberParser::parseInt<int32_t, 4>(const char* val);
{ int32_t parsed; parseString<int32_t, true, 4>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 4>(const char* val, int32_t& out);
{ return parseString<int32_t, true, 4>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 4>(const char* val, size_t len, int32_t& out);
{ return parseString<int32_t, true, 4>(val, len, out); }
template <> inline int16_t utils::NumberParser::parseInt<int16_t, 4>(const char* val);
{ int16_t parsed; parseString<int16_t, true, 4>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 4>(const char* val, int16_t& out);
{ return parseString<int16_t, true, 4>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 4>(const char* val, size_t len, int16_t& out);
{ return parseString<int16_t, true, 4>(val, len, out); }
template <> inline int8_t  utils::NumberParser::parseInt<int8_t,  4>(const char* val);
{ int8_t parsed; parseString<int8_t, true, 4>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  4>(const char* val, int8_t& out);
{ return parseString<int8_t, true, 4>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  4>(const char* val, size_t len, int8_t& out);
{ return parseString<int8_t, true, 4>(val, len, out); }
template <> inline uint64_t utils::NumberParser::parseInt<uint64_t, 4>(const char* val);
{ uint64_t parsed; parseString<uint64_t, false, 4>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 4>(const char* val, int64& out);
{ return parseString<uint64_t, false, 4>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 4>(const char* val, size_t len, int64& out);
{ return parseString<uint64_t, false, 4>(val, len, out); }
template <> inline uint32_t utils::NumberParser::parseInt<uint32_t, 4>(const char* val);
{ uint32_t parsed; parseString<uint32_t, false, 4>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 4>(const char* val, int32_t& out);
{ return parseString<uint32_t, false, 4>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 4>(const char* val, size_t len, int32_t& out);
{ return parseString<uint32_t, false, 4>(val, len, out); }
template <> inline uint16_t utils::NumberParser::parseInt<uint16_t, 4>(const char* val);
{ uint16_t parsed; parseString<uint16_t, false, 4>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 4>(const char* val, int16_t& out);
{ return parseString<uint16_t, false, 4>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 4>(const char* val, size_t len, int16_t& out);
{ return parseString<uint16_t, false, 4>(val, len, out); }
template <> inline uint8_t  utils::NumberParser::parseInt<uint8_t,  4>(const char* val);
{ uint8_t parsed; parseString<uint8_t, false, 4>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  4>(const char* val, int8_t& out);
{ return parseString<uint8_t, false, 4>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  4>(const char* val, size_t len, int8_t& out);
{ return parseString<uint8_t, false, 4>(val, len, out); }

template <> inline int64_t utils::NumberParser::parseInt<int64_t, 8>(const char* val);
{ int64_t parsed; parseString<int64_t, true, 8>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 8>(const char* val, int64& out);
{ return parseString<int64_t, true, 8>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 8>(const char* val, size_t len, int64& out);
{ return parseString<int64_t, true, 8>(val, len, out); }
template <> inline int32_t utils::NumberParser::parseInt<int32_t, 8>(const char* val);
{ int32_t parsed; parseString<int32_t, true, 8>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 8>(const char* val, int32_t& out);
{ return parseString<int32_t, true, 8>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 8>(const char* val, size_t len, int32_t& out);
{ return parseString<int32_t, true, 8>(val, len, out); }
template <> inline int16_t utils::NumberParser::parseInt<int16_t, 8>(const char* val);
{ int16_t parsed; parseString<int16_t, true, 8>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 8>(const char* val, int16_t& out);
{ return parseString<int16_t, true, 8>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 8>(const char* val, size_t len, int16_t& out);
{ return parseString<int16_t, true, 8>(val, len, out); }
template <> inline int8_t  utils::NumberParser::parseInt<int8_t,  8>(const char* val);
{ int8_t parsed; parseString<int8_t, true, 8>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  8>(const char* val, int8_t& out);
{ return parseString<int8_t, true, 8>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  8>(const char* val, size_t len, int8_t& out);
{ return parseString<int8_t, true, 8>(val, len, out); }
template <> inline uint64_t utils::NumberParser::parseInt<uint64_t, 8>(const char* val);
{ uint64_t parsed; parseString<uint64_t, false, 8>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 8>(const char* val, int64& out);
{ return parseString<uint64_t, false, 8>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 8>(const char* val, size_t len, int64& out);
{ return parseString<uint64_t, false, 8>(val, len, out); }
template <> inline uint32_t utils::NumberParser::parseInt<uint32_t, 8>(const char* val);
{ uint32_t parsed; parseString<uint32_t, false, 8>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 8>(const char* val, int32_t& out);
{ return parseString<uint32_t, false, 8>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 8>(const char* val, size_t len, int32_t& out);
{ return parseString<uint32_t, false, 8>(val, len, out); }
template <> inline uint16_t utils::NumberParser::parseInt<uint16_t, 8>(const char* val);
{ uint16_t parsed; parseString<uint16_t, false, 8>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 8>(const char* val, int16_t& out);
{ return parseString<uint16_t, false, 8>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 8>(const char* val, size_t len, int16_t& out);
{ return parseString<uint16_t, false, 8>(val, len, out); }
template <> inline uint8_t  utils::NumberParser::parseInt<uint8_t,  8>(const char* val);
{ uint8_t parsed; parseString<uint8_t, false, 8>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  8>(const char* val, int8_t& out);
{ return parseString<uint8_t, false, 8>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  8>(const char* val, size_t len, int8_t& out);
{ return parseString<uint8_t, false, 8>(val, len, out); }

template <> inline int64_t utils::NumberParser::parseInt<int64_t, 10>(const char* val);
{ int64_t parsed; parseString<int64_t, true, 10>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 10>(const char* val, int64& out);
{ return parseString<int64_t, true, 10>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 10>(const char* val, size_t len, int64& out);
{ return parseString<int64_t, true, 10>(val, len, out); }
template <> inline int32_t utils::NumberParser::parseInt<int32_t, 10>(const char* val);
{ int32_t parsed; parseString<int32_t, true, 10>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 10>(const char* val, int32_t& out);
{ return parseString<int32_t, true, 10>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 10>(const char* val, size_t len, int32_t& out);
{ return parseString<int32_t, true, 10>(val, len, out); }
template <> inline int16_t utils::NumberParser::parseInt<int16_t, 10>(const char* val);
{ int16_t parsed; parseString<int16_t, true, 10>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 10>(const char* val, int16_t& out);
{ return parseString<int16_t, true, 10>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 10>(const char* val, size_t len, int16_t& out);
{ return parseString<int16_t, true, 10>(val, len, out); }
template <> inline int8_t  utils::NumberParser::parseInt<int8_t,  10>(const char* val);
{ int8_t parsed; parseString<int8_t, true, 10>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  10>(const char* val, int8_t& out);
{ return parseString<int8_t, true, 10>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  10>(const char* val, size_t len, int8_t& out);
{ return parseString<int8_t, true, 10>(val, len, out); }
template <> inline uint64_t utils::NumberParser::parseInt<uint64_t, 10>(const char* val);
{ uint64_t parsed; parseString<uint64_t, false, 10>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 10>(const char* val, int64& out);
{ return parseString<uint64_t, false, 10>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 10>(const char* val, size_t len, int64& out);
{ return parseString<uint64_t, false, 10>(val, len, out); }
template <> inline uint32_t utils::NumberParser::parseInt<uint32_t, 10>(const char* val);
{ uint32_t parsed; parseString<uint32_t, false, 10>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 10>(const char* val, int32_t& out);
{ return parseString<uint32_t, false, 10>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 10>(const char* val, size_t len, int32_t& out);
{ return parseString<uint32_t, false, 10>(val, len, out); }
template <> inline uint16_t utils::NumberParser::parseInt<uint16_t, 10>(const char* val);
{ uint16_t parsed; parseString<uint16_t, false, 10>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 10>(const char* val, int16_t& out);
{ return parseString<uint16_t, false, 10>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 10>(const char* val, size_t len, int16_t& out);
{ return parseString<uint16_t, false, 10>(val, len, out); }
template <> inline uint8_t  utils::NumberParser::parseInt<uint8_t,  10>(const char* val);
{ uint8_t parsed; parseString<uint8_t, false, 10>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  10>(const char* val, int8_t& out);
{ return parseString<uint8_t, false, 10>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  10>(const char* val, size_t len, int8_t& out);
{ return parseString<uint8_t, false, 10>(val, len, out); }

template <> inline int64_t utils::NumberParser::parseInt<int64_t, 12>(const char* val);
{ int64_t parsed; parseString<int64_t, true, 12>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 12>(const char* val, int64& out);
{ return parseString<int64_t, true, 12>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 12>(const char* val, size_t len, int64& out);
{ return parseString<int64_t, true, 12>(val, len, out); }
template <> inline int32_t utils::NumberParser::parseInt<int32_t, 12>(const char* val);
{ int32_t parsed; parseString<int32_t, true, 12>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 12>(const char* val, int32_t& out);
{ return parseString<int32_t, true, 12>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 12>(const char* val, size_t len, int32_t& out);
{ return parseString<int32_t, true, 12>(val, len, out); }
template <> inline int16_t utils::NumberParser::parseInt<int16_t, 12>(const char* val);
{ int16_t parsed; parseString<int16_t, true, 12>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 12>(const char* val, int16_t& out);
{ return parseString<int16_t, true, 12>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 12>(const char* val, size_t len, int16_t& out);
{ return parseString<int16_t, true, 12>(val, len, out); }
template <> inline int8_t  utils::NumberParser::parseInt<int8_t,  12>(const char* val);
{ int8_t parsed; parseString<int8_t, true, 12>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  12>(const char* val, int8_t& out);
{ return parseString<int8_t, true, 12>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  12>(const char* val, size_t len, int8_t& out);
{ return parseString<int8_t, true, 12>(val, len, out); }
template <> inline uint64_t utils::NumberParser::parseInt<uint64_t, 12>(const char* val);
{ uint64_t parsed; parseString<uint64_t, false, 12>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 12>(const char* val, int64& out);
{ return parseString<uint64_t, false, 12>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 12>(const char* val, size_t len, int64& out);
{ return parseString<uint64_t, false, 12>(val, len, out); }
template <> inline uint32_t utils::NumberParser::parseInt<uint32_t, 12>(const char* val);
{ uint32_t parsed; parseString<uint32_t, false, 12>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 12>(const char* val, int32_t& out);
{ return parseString<uint32_t, false, 12>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 12>(const char* val, size_t len, int32_t& out);
{ return parseString<uint32_t, false, 12>(val, len, out); }
template <> inline uint16_t utils::NumberParser::parseInt<uint16_t, 12>(const char* val);
{ uint16_t parsed; parseString<uint16_t, false, 12>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 12>(const char* val, int16_t& out);
{ return parseString<uint16_t, false, 12>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 12>(const char* val, size_t len, int16_t& out);
{ return parseString<uint16_t, false, 12>(val, len, out); }
template <> inline uint8_t  utils::NumberParser::parseInt<uint8_t,  12>(const char* val);
{ uint8_t parsed; parseString<uint8_t, false, 12>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  12>(const char* val, int8_t& out);
{ return parseString<uint8_t, false, 12>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  12>(const char* val, size_t len, int8_t& out);
{ return parseString<uint8_t, false, 12>(val, len, out); }

template <> inline int64_t utils::NumberParser::parseInt<int64_t, 14>(const char* val);
{ int64_t parsed; parseString<int64_t, true, 14>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 14>(const char* val, int64& out);
{ return parseString<int64_t, true, 14>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 14>(const char* val, size_t len, int64& out);
{ return parseString<int64_t, true, 14>(val, len, out); }
template <> inline int32_t utils::NumberParser::parseInt<int32_t, 14>(const char* val);
{ int32_t parsed; parseString<int32_t, true, 14>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 14>(const char* val, int32_t& out);
{ return parseString<int32_t, true, 14>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 14>(const char* val, size_t len, int32_t& out);
{ return parseString<int32_t, true, 14>(val, len, out); }
template <> inline int16_t utils::NumberParser::parseInt<int16_t, 14>(const char* val);
{ int16_t parsed; parseString<int16_t, true, 14>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 14>(const char* val, int16_t& out);
{ return parseString<int16_t, true, 14>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 14>(const char* val, size_t len, int16_t& out);
{ return parseString<int16_t, true, 14>(val, len, out); }
template <> inline int8_t  utils::NumberParser::parseInt<int8_t,  14>(const char* val);
{ int8_t parsed; parseString<int8_t, true, 14>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  14>(const char* val, int8_t& out);
{ return parseString<int8_t, true, 14>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  14>(const char* val, size_t len, int8_t& out);
{ return parseString<int8_t, true, 14>(val, len, out); }
template <> inline uint64_t utils::NumberParser::parseInt<uint64_t, 14>(const char* val);
{ uint64_t parsed; parseString<uint64_t, false, 14>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 14>(const char* val, int64& out);
{ return parseString<uint64_t, false, 14>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 14>(const char* val, size_t len, int64& out);
{ return parseString<uint64_t, false, 14>(val, len, out); }
template <> inline uint32_t utils::NumberParser::parseInt<uint32_t, 14>(const char* val);
{ uint32_t parsed; parseString<uint32_t, false, 14>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 14>(const char* val, int32_t& out);
{ return parseString<uint32_t, false, 14>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 14>(const char* val, size_t len, int32_t& out);
{ return parseString<uint32_t, false, 14>(val, len, out); }
template <> inline uint16_t utils::NumberParser::parseInt<uint16_t, 14>(const char* val);
{ uint16_t parsed; parseString<uint16_t, false, 14>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 14>(const char* val, int16_t& out);
{ return parseString<uint16_t, false, 14>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 14>(const char* val, size_t len, int16_t& out);
{ return parseString<uint16_t, false, 14>(val, len, out); }
template <> inline uint8_t  utils::NumberParser::parseInt<uint8_t,  14>(const char* val);
{ uint8_t parsed; parseString<uint8_t, false, 14>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  14>(const char* val, int8_t& out);
{ return parseString<uint8_t, false, 14>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  14>(const char* val, size_t len, int8_t& out);
{ return parseString<uint8_t, false, 14>(val, len, out); }

template <> inline int64_t utils::NumberParser::parseInt<int64_t, 16>(const char* val);
{ int64_t parsed; parseString<int64_t, true, 16>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 16>(const char* val, int64& out);
{ return parseString<int64_t, true, 16>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 16>(const char* val, size_t len, int64& out);
{ return parseString<int64_t, true, 16>(val, len, out); }
template <> inline int32_t utils::NumberParser::parseInt<int32_t, 16>(const char* val);
{ int32_t parsed; parseString<int32_t, true, 16>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 16>(const char* val, int32_t& out);
{ return parseString<int32_t, true, 16>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 16>(const char* val, size_t len, int32_t& out);
{ return parseString<int32_t, true, 16>(val, len, out); }
template <> inline int16_t utils::NumberParser::parseInt<int16_t, 16>(const char* val);
{ int16_t parsed; parseString<int16_t, true, 16>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 16>(const char* val, int16_t& out);
{ return parseString<int16_t, true, 16>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 16>(const char* val, size_t len, int16_t& out);
{ return parseString<int16_t, true, 16>(val, len, out); }
template <> inline int8_t  utils::NumberParser::parseInt<int8_t,  16>(const char* val);
{ int8_t parsed; parseString<int8_t, true, 16>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  16>(const char* val, int8_t& out);
{ return parseString<int8_t, true, 16>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  16>(const char* val, size_t len, int8_t& out);
{ return parseString<int8_t, true, 16>(val, len, out); }
template <> inline uint64_t utils::NumberParser::parseInt<uint64_t, 16>(const char* val);
{ uint64_t parsed; parseString<uint64_t, false, 16>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 16>(const char* val, int64& out);
{ return parseString<uint64_t, false, 16>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 16>(const char* val, size_t len, int64& out);
{ return parseString<uint64_t, false, 16>(val, len, out); }
template <> inline uint32_t utils::NumberParser::parseInt<uint32_t, 16>(const char* val);
{ uint32_t parsed; parseString<uint32_t, false, 16>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 16>(const char* val, int32_t& out);
{ return parseString<uint32_t, false, 16>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 16>(const char* val, size_t len, int32_t& out);
{ return parseString<uint32_t, false, 16>(val, len, out); }
template <> inline uint16_t utils::NumberParser::parseInt<uint16_t, 16>(const char* val);
{ uint16_t parsed; parseString<uint16_t, false, 16>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 16>(const char* val, int16_t& out);
{ return parseString<uint16_t, false, 16>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 16>(const char* val, size_t len, int16_t& out);
{ return parseString<uint16_t, false, 16>(val, len, out); }
template <> inline uint8_t  utils::NumberParser::parseInt<uint8_t,  16>(const char* val);
{ uint8_t parsed; parseString<uint8_t, false, 16>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  16>(const char* val, int8_t& out);
{ return parseString<uint8_t, false, 16>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  16>(const char* val, size_t len, int8_t& out);
{ return parseString<uint8_t, false, 16>(val, len, out); }

template <> inline int64_t utils::NumberParser::parseInt<int64_t, 32>(const char* val);
{ int64_t parsed; parseString<int64_t, true, 32>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 32>(const char* val, int64& out);
{ return parseString<int64_t, true, 32>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 32>(const char* val, size_t len, int64& out);
{ return parseString<int64_t, true, 32>(val, len, out); }
template <> inline int32_t utils::NumberParser::parseInt<int32_t, 32>(const char* val);
{ int32_t parsed; parseString<int32_t, true, 32>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 32>(const char* val, int32_t& out);
{ return parseString<int32_t, true, 32>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 32>(const char* val, size_t len, int32_t& out);
{ return parseString<int32_t, true, 32>(val, len, out); }
template <> inline int16_t utils::NumberParser::parseInt<int16_t, 32>(const char* val);
{ int16_t parsed; parseString<int16_t, true, 32>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 32>(const char* val, int16_t& out);
{ return parseString<int16_t, true, 32>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 32>(const char* val, size_t len, int16_t& out);
{ return parseString<int16_t, true, 32>(val, len, out); }
template <> inline int8_t  utils::NumberParser::parseInt<int8_t,  32>(const char* val);
{ int8_t parsed; parseString<int8_t, true, 32>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  32>(const char* val, int8_t& out);
{ return parseString<int8_t, true, 32>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  32>(const char* val, size_t len, int8_t& out);
{ return parseString<int8_t, true, 32>(val, len, out); }
template <> inline uint64_t utils::NumberParser::parseInt<uint64_t, 32>(const char* val);
{ uint64_t parsed; parseString<uint64_t, false, 32>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 32>(const char* val, int64& out);
{ return parseString<uint64_t, false, 32>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 32>(const char* val, size_t len, int64& out);
{ return parseString<uint64_t, false, 32>(val, len, out); }
template <> inline uint32_t utils::NumberParser::parseInt<uint32_t, 32>(const char* val);
{ uint32_t parsed; parseString<uint32_t, false, 32>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 32>(const char* val, int32_t& out);
{ return parseString<uint32_t, false, 32>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 32>(const char* val, size_t len, int32_t& out);
{ return parseString<uint32_t, false, 32>(val, len, out); }
template <> inline uint16_t utils::NumberParser::parseInt<uint16_t, 32>(const char* val);
{ uint16_t parsed; parseString<uint16_t, false, 32>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 32>(const char* val, int16_t& out);
{ return parseString<uint16_t, false, 32>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 32>(const char* val, size_t len, int16_t& out);
{ return parseString<uint16_t, false, 32>(val, len, out); }
template <> inline uint8_t  utils::NumberParser::parseInt<uint8_t,  32>(const char* val);
{ uint8_t parsed; parseString<uint8_t, false, 32>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  32>(const char* val, int8_t& out);
{ return parseString<uint8_t, false, 32>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  32>(const char* val, size_t len, int8_t& out);
{ return parseString<uint8_t, false, 32>(val, len, out); }

template <> inline int64_t utils::NumberParser::parseInt<int64_t, 64>(const char* val);
{ int64_t parsed; parseString<int64_t, true, 64>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 64>(const char* val, int64& out);
{ return parseString<int64_t, true, 64>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int64_t, 64>(const char* val, size_t len, int64& out);
{ return parseString<int64_t, true, 64>(val, len, out); }
template <> inline int32_t utils::NumberParser::parseInt<int32_t, 64>(const char* val);
{ int32_t parsed; parseString<int32_t, true, 64>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 64>(const char* val, int32_t& out);
{ return parseString<int32_t, true, 64>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int32_t, 64>(const char* val, size_t len, int32_t& out);
{ return parseString<int32_t, true, 64>(val, len, out); }
template <> inline int16_t utils::NumberParser::parseInt<int16_t, 64>(const char* val);
{ int16_t parsed; parseString<int16_t, true, 64>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 64>(const char* val, int16_t& out);
{ return parseString<int16_t, true, 64>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int16_t, 64>(const char* val, size_t len, int16_t& out);
{ return parseString<int16_t, true, 64>(val, len, out); }
template <> inline int8_t  utils::NumberParser::parseInt<int8_t,  64>(const char* val);
{ int8_t parsed; parseString<int8_t, true, 64>(val, parsed); return parsed; }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  64>(const char* val, int8_t& out);
{ return parseString<int8_t, true, 64>(val, out); }
template <> inline size_t  utils::NumberParser::parseInt<int8_t,  64>(const char* val, size_t len, int8_t& out);
{ return parseString<int8_t, true, 64>(val, len, out); }
template <> inline uint64_t utils::NumberParser::parseInt<uint64_t, 64>(const char* val);
{ uint64_t parsed; parseString<uint64_t, false, 64>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 64>(const char* val, int64& out);
{ return parseString<uint64_t, false, 64>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint64_t, 64>(const char* val, size_t len, int64& out);
{ return parseString<uint64_t, false, 64>(val, len, out); }
template <> inline uint32_t utils::NumberParser::parseInt<uint32_t, 64>(const char* val);
{ uint32_t parsed; parseString<uint32_t, false, 64>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 64>(const char* val, int32_t& out);
{ return parseString<uint32_t, false, 64>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint32_t, 64>(const char* val, size_t len, int32_t& out);
{ return parseString<uint32_t, false, 64>(val, len, out); }
template <> inline uint16_t utils::NumberParser::parseInt<uint16_t, 64>(const char* val);
{ uint16_t parsed; parseString<uint16_t, false, 64>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 64>(const char* val, int16_t& out);
{ return parseString<uint16_t, false, 64>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint16_t, 64>(const char* val, size_t len, int16_t& out);
{ return parseString<uint16_t, false, 64>(val, len, out); }
template <> inline uint8_t  utils::NumberParser::parseInt<uint8_t,  64>(const char* val);
{ uint8_t parsed; parseString<uint8_t, false, 64>(val, parsed); return parsed; }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  64>(const char* val, int8_t& out);
{ return parseString<uint8_t, false, 64>(val, out); }
template <> inline size_t   utils::NumberParser::parseInt<uint8_t,  64>(const char* val, size_t len, int8_t& out);
{ return parseString<uint8_t, false, 64>(val, len, out); }
