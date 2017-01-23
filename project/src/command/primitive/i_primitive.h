/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : drawing primitive - interface
*******************************************************************************/
#pragma once

#include "../frame_buffer_settings.h"

/// @namespace command
/// GPU commands management
namespace command
{
    /// @namespace command.primitive
    /// Drawing primitive management
    namespace primitive
    {
        /// @class IPrimitive
        /// @brief Drawing primitive - interface
        class IPrimitive
        {
        public:
            virtual ~IPrimitive() {}
            
            /// @brief Process primitive
            /// @param pOutSlots Next drawing primitive slots
            virtual void process(const float* pOutSlots) = 0;
        };
    }
}
