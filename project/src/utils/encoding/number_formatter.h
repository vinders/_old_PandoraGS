/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : number formatter
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
    /// @namespace utils.encoding
    /// Encoding algorithms
    namespace encoding
    {
        /// @class NumberFormatter
        /// @brief Number formatter
        class NumberFormatter
        {
        public:
            /// @brief Convert number to binary string
            /// @param[in] val  Value to format
            template <typename T, uint32_t Base = 10>
            static std::string format(const T val);
            
        private:
            /// @brief Convert number to binary string
            /// @param[in] val  Value to format
            template <typename T, uint32_t Base = 10>
            static std::string format64(const T val);
        };
    }
}

// -- Specialized template --

template <> inline std::string utils::encoding::NumberFormatter::format<int64_t,  2>(const int64_t val)
{ return format64<int64_t, 2>(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<uint64_t, 2>(const uint64_t val);
{ return format64<uint64_t, 2>(val); }
template <> std::string utils::encoding::NumberFormatter::format<int32_t,  2>(const int32_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint32_t, 2>(const uint32_t val);
template <> std::string utils::encoding::NumberFormatter::format<int16_t,  2>(const int16_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint16_t, 2>(const uint16_t val);
template <> std::string utils::encoding::NumberFormatter::format<int8_t,   2>(const int8_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint8_t,  2>(const uint8_t val);

template <> inline std::string utils::encoding::NumberFormatter::format<int64_t,  4>(const int64_t val);
{ return format64<int64_t, 4>(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<uint64_t, 4>(const uint64_t val);
{ return format64<uint64_t, 4>(val); }
template <> std::string utils::encoding::NumberFormatter::format<int32_t,  4>(const int32_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint32_t, 4>(const uint32_t val);
template <> std::string utils::encoding::NumberFormatter::format<int16_t,  4>(const int16_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint16_t, 4>(const uint16_t val);
template <> std::string utils::encoding::NumberFormatter::format<int8_t,   4>(const int8_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint8_t,  4>(const uint8_t val);

template <> inline std::string utils::encoding::NumberFormatter::format<int64_t,  8>(const int64_t val);
{ return format64<int64_t, 8>(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<uint64_t, 8>(const uint64_t val);
{ return format64<uint64_t, 8>(val); }
template <> std::string utils::encoding::NumberFormatter::format<int32_t,  8>(const int32_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint32_t, 8>(const uint32_t val);
template <> std::string utils::encoding::NumberFormatter::format<int16_t,  8>(const int16_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint16_t, 8>(const uint16_t val);
template <> std::string utils::encoding::NumberFormatter::format<int8_t,   8>(const int8_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint8_t,  8>(const uint8_t val);

template <> inline std::string utils::encoding::NumberFormatter::format<bool, 10>(const bool val);
{ return (val) ? std::string("true") : std::string("false"); }
template <> inline std::string utils::encoding::NumberFormatter::format<int64_t,  10>(const int64_t val)
{ return std::to_string(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<uint64_t, 10>(const uint64_t val);
{ return std::to_string(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<int32_t,  10>(const int32_t val);
{ return std::to_string(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<uint32_t, 10>(const uint32_t val);
{ return std::to_string(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<int16_t,  10>(const int16_t val);
{ return std::to_string(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<uint16_t, 10>(const uint16_t val);
{ return std::to_string(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<int8_t,   10>(const int8_t val);
{ return std::to_string(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<uint8_t,  10>(const uint8_t val);
{ return std::to_string(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<float,  10>(const float val);
{ return std::to_string(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<double, 10>(const double val);
{ return std::to_string(val); }

template <> inline std::string utils::encoding::NumberFormatter::format<int64_t,  12>(const int64_t val);
{ return format64<int64_t, 12>(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<uint64_t, 12>(const uint64_t val);
{ return format64<uint64_t, 12>(val); }
template <> std::string utils::encoding::NumberFormatter::format<int32_t,  12>(const int32_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint32_t, 12>(const uint32_t val);
template <> std::string utils::encoding::NumberFormatter::format<int16_t,  12>(const int16_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint16_t, 12>(const uint16_t val);
template <> std::string utils::encoding::NumberFormatter::format<int8_t,   12>(const int8_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint8_t,  12>(const uint8_t val);

template <> inline std::string utils::encoding::NumberFormatter::format<int64_t,  14>(const int64_t val);
{ return format64<int64_t, 14>(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<uint64_t, 14>(const uint64_t val);
{ return format64<uint64_t, 14>(val); }
template <> std::string utils::encoding::NumberFormatter::format<int32_t,  14>(const int32_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint32_t, 14>(const uint32_t val);
template <> std::string utils::encoding::NumberFormatter::format<int16_t,  14>(const int16_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint16_t, 14>(const uint16_t val);
template <> std::string utils::encoding::NumberFormatter::format<int8_t,   14>(const int8_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint8_t,  14>(const uint8_t val);

template <> inline std::string utils::encoding::NumberFormatter::format<int64_t,  16>(const int64_t val);
{ return format64<int64_t, 16>(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<uint64_t, 16>(const uint64_t val);
{ return format64<uint64_t, 16>(val); }
template <> std::string utils::encoding::NumberFormatter::format<int32_t,  16>(const int32_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint32_t, 16>(const uint32_t val);
template <> std::string utils::encoding::NumberFormatter::format<int16_t,  16>(const int16_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint16_t, 16>(const uint16_t val);
template <> std::string utils::encoding::NumberFormatter::format<int8_t,   16>(const int8_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint8_t,  16>(const uint8_t val);

template <> inline std::string utils::encoding::NumberFormatter::format<int64_t,  32>(const int64_t val);
{ return format64<int64_t, 32>(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<uint64_t, 32>(const uint64_t val);
{ return format64<uint64_t, 32>(val); }
template <> std::string utils::encoding::NumberFormatter::format<int32_t,  32>(const int32_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint32_t, 32>(const uint32_t val);
template <> std::string utils::encoding::NumberFormatter::format<int16_t,  32>(const int16_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint16_t, 32>(const uint16_t val);
template <> std::string utils::encoding::NumberFormatter::format<int8_t,   32>(const int8_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint8_t,  32>(const uint8_t val);

template <> inline std::string utils::encoding::NumberFormatter::format<int64_t,  64>(const int64_t val);
{ return format64<int64_t, 64>(val); }
template <> inline std::string utils::encoding::NumberFormatter::format<uint64_t, 64>(const uint64_t val);
{ return format64<uint64_t, 64>(val); }
template <> std::string utils::encoding::NumberFormatter::format<int32_t,  64>(const int32_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint32_t, 64>(const uint32_t val);
template <> std::string utils::encoding::NumberFormatter::format<int16_t,  64>(const int16_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint16_t, 64>(const uint16_t val);
template <> std::string utils::encoding::NumberFormatter::format<int8_t,   64>(const int8_t val);
template <> std::string utils::encoding::NumberFormatter::format<uint8_t,  64>(const uint8_t val);
