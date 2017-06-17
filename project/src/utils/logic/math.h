/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : mathematics toolset
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include <cmath>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace algorithm
    /// Algorithms
    namespace algorithm
    {
        /// @class Math
        /// @brief Mathematics toolset
        class Math
        {
        public:
            //sqrt
            //sqrt inv
        
            /// @brief Get greatest common divider (Euclide)
            template <typename T>
            static T gcd(T lhs, T rhs) noexcept
            {
                if (lhs == 0) 
                    return rhs;
                if (rhs == 0) 
                    return lhs;
                
                T remainder;
                while (rhs != 0)
                {
                    remainder = lhs % rhs;
                    lhs = rhs;
                    rhs = remainder;
                }
                return lhs;
            }
        };
    }
}
