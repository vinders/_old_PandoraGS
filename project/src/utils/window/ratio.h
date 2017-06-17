/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : screen ratio
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include "../rational.h"

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.window
    /// Window management utilities
    namespace window
    {
        /// @brief Screen ratio
        typedef rational32_t Ratio;
        
        /// @enum ratio_type_t
        /// @brief Common ratio types
        enum class ratio_type_t : float
        {
            ratio_1_1   = 1.0f,
            ratio_6_5   = 1.2f,
            ratio_5_4   = 1.25f,
            ratio_4_3   = 1.333333f,
            ratio_11_8  = 1.375f,
            ratio_3_2   = 1.5f,
            ratio_15_10 = 1.5f,
            ratio_16_10 = 1.6f,
            ratio_5_3   = 1.666667f,
            ratio_16_9  = 1.777778f,
            ratio_2_1   = 2.0f,
            ratio_18_9  = 2.0f,
            ratio_21_9  = 2.333333f
        };
        
        
        /// @class RatioFactory
        /// @brief Factory for pre-defined ratios
        class RatioFactory
        {
        public:
            RatioFactory() = delete;
            
            /// @brief Create pre-defined ratio object
            /// @param[in] type  Pre-defined type
            /// @returns Ratio object
            static Ratio create(const ratio_type_t type)
            {
                switch (type)
                {
                    case ratio_type_t::ratio_1_1:   return Ratio( 1, 1); break;
                    case ratio_type_t::ratio_6_5:   return Ratio( 6, 5); break;
                    case ratio_type_t::ratio_5_4:   return Ratio( 5, 4); break;
                    case ratio_type_t::ratio_4_3:   return Ratio( 4, 3); break;
                    case ratio_type_t::ratio_11_8:  return Ratio(11, 8); break;
                    case ratio_type_t::ratio_3_2:   return Ratio( 3, 2); break;
                    case ratio_type_t::ratio_15_10: return Ratio(15, 10); break;
                    case ratio_type_t::ratio_16_10: return Ratio(16, 10); break;
                    case ratio_type_t::ratio_5_3:   return Ratio( 5, 3); break;
                    case ratio_type_t::ratio_16_9:  return Ratio(16, 9); break;
                    case ratio_type_t::ratio_2_1:   return Ratio( 2, 1); break;
                    case ratio_type_t::ratio_18_9:  return Ratio(18, 9); break;
                    case ratio_type_t::ratio_21_9:  return Ratio(21, 9); break;
                }
                return Ratio(1, 1);
            }
        };
    }
}
