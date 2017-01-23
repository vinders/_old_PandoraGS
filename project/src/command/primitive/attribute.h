/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : drawing attribute (area / transparency)
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
        /// @class Attribute
        /// @brief Drawing attribute (area / transparency)
        class Attribute
        {
        public:
            /// @brief Create primitive
            /// @param pData Raw attribute data
            /// @param frameSettings Current frame buffer settings
            static void setAttribute(unsigned long* pData, FrameBufferSettings& frameSettings);
        };
    }
}
