/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : clock rate (hertz)
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include "../rational.h"

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.time
    /// Time management utilities
    namespace time
    {
        /// @brief Clock rate (hertz)
        typedef rational32_t Rate;
        
        /// @enum rate_type_t
        /// @brief Common rate types
        enum class rate_type_t : float
        {
            cinema      = 24.0f,
            cinema_ntsc = 23.97602f,
            secam       = 25.0f,
            pal_i       = 25.0f,
            pal_p       = 50.0f,
            pal_60_p    = 60.0f,
            ntsc_i      = 29.97003f,
            ntsc_drop_i = 30.0f,
            ntsc_p      = 59.94006f,
            ntsc_drop_p = 60.0f
        };
        
        
        /// @class RateFactory
        /// @brief Factory for pre-defined rates
        class RateFactory
        {
        public:
            RateFactory() = delete;
            
            /// @brief Create pre-defined rate object
            /// @param[in] type  Pre-defined type
            /// @returns Rate object
            static Rate create(const rate_type_t type)
            {
                switch (type)
                {
                    case rate_type_t::cinema:
                        return Rate(24, 1); break;
                    case rate_type_t::cinema_ntsc:
                        return Rate(24000, 1001); break;
                    
                    case rate_type_t::secam:
                    case rate_type_t::pal_i:
                        return Rate(25, 1); break;
                    case rate_type_t::pal_p:
                        return Rate(50, 1); break;
                    case rate_type_t::pal_60_p:
                        return Rate(60, 1); break;
                    
                    case rate_type_t::ntsc_i:
                        return Rate(30000, 1001); break;
                    case rate_type_t::ntsc_drop_i:
                        return Rate(30, 1); break;
                    case rate_type_t::ntsc_p:
                        return Rate(60000, 1001); break;
                    case rate_type_t::ntsc_drop_p:
                        return Rate(60, 1); break;
                }
                return Rate(1, 1);
            }
        };
    }
}
