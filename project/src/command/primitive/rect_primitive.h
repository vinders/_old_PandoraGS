/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : drawing rectangle primitive (tile / sprite)
*******************************************************************************/
#pragma once

#include "i_primitive.h"

/// @namespace command
/// GPU commands management
namespace command
{
    /// @namespace command.primitive
    /// Drawing primitive management
    namespace primitive
    {
        /// @class RectPrimitive
        /// @brief Drawing rectangle primitive (tile / sprite)
        class RectPrimitive : public IPrimitive
        {
        public:
            /// @brief Create primitive
            /// @param pData Raw primitive data
            /// @param frameSettings Current frame buffer settings
            RectPrimitive(unsigned long* pData, FrameBufferSettings& frameSettings);
            
            /// @brief Process primitive
            /// @param pOutSlots Next drawing primitive slots
            virtual void process(const float* pOutSlots);
        };
    }
}
