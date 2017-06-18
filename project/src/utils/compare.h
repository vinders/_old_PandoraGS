/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : comparison toolset
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @enum compare_type_t
    /// @brief Comparison types
    enum class compare_type_t : uint32_t
    {
        equal = 0,         ///< a == b
        different = 1,     ///< a != b
        opposite = 2,      ///< a == -b
        opposite_bits = 4, ///< a == NOT(b)
        
        mask_and = 5,      ///< a AND b != 0
        mask_or = 6,       ///< a OR b != 0
        mask_xor = 7,      ///< a XOR b != 0
        
        lower = 10,        ///< a < b
        lower_equal = 11,  ///< a <= b
        greater = 12,      ///< a > b
        greater_equal = 13 ///< a >= b
    };
    
    /// @brief Template comparison function
    /// @param[in] lhs  First value
    /// @param[in] rhs  Second value
    /// @returns Result
    template <typename T, compare_type_t Comparison>
    static inline bool compare(const T lhs, const T rhs)
    {
        switch (Comparison)
        {
            case compare_type_t::equal:         return (lhs == rhs); break;
            case compare_type_t::different:     return (lhs != rhs); break;
            case compare_type_t::opposite:      return (lhs == -rhs); break;
            case compare_type_t::opposite_bits: return (lhs == ~rhs); break;
            case compare_type_t::mask_and:      return ((lhs & rhs) != 0); break;
            case compare_type_t::mask_or:       return ((lhs | rhs) != 0); break;
            case compare_type_t::mask_xor:      return ((lhs ^ rhs) != 0); break;
            case compare_type_t::lower:         return (lhs < rhs); break;
            case compare_type_t::lower_equal:   return (lhs <= rhs); break;
            case compare_type_t::greater:       return (lhs > rhs); break;
            case compare_type_t::greater_equal: return (lhs >= rhs); break;
            default: return (lhs == rhs); break;
        }
    }
}
