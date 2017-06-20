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
        using Rate = rational32_t;
        
        /// @enum rate_type_t
        /// @brief Common rate types
        enum class rate_type_t : uint32_t
        {
            24_p      = 24,
            ntsc_24_p = 23,
            48_p      = 48,
            ntsc_48_p = 47,
            pal_i     = 25,
            pal_p     = 50,
            pal_60_p  = 60,
            ntsc_i    = 29,
            atfs_i    = 30,
            ntsc_p    = 59,
            atfs_p    = 60
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
                    case rate_type_t::24_p:
                        return Rate(24, 1); break;
                    case rate_type_t::ntsc_24_p:
                        return Rate(24000, 1001); break;
                    case rate_type_t::48_p:
                        return Rate(48, 1); break;
                    case rate_type_t::ntsc_48_p:
                        return Rate(48000, 1001); break;
                    
                    case rate_type_t::pal_i:
                        return Rate(25, 1); break;
                    case rate_type_t::pal_p:
                        return Rate(50, 1); break;
                    case rate_type_t::pal_60_p:
                        return Rate(60, 1); break;
                    
                    case rate_type_t::ntsc_i:
                        return Rate(30000, 1001); break;
                    case rate_type_t::atfs_i:
                        return Rate(30, 1); break;
                    case rate_type_t::ntsc_p:
                        return Rate(60000, 1001); break;
                    case rate_type_t::atfs_p:
                        return Rate(60, 1); break;
                }
                return Rate(1, 1);
            }
        };
    }
}
