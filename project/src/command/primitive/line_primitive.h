/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : drawing line primitive (line / poly-line)
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
        /// @class LinePrimitive
        /// @brief Drawing line primitive (line / poly-line)
        class LinePrimitive : public IPrimitive
        {
        public:
            /// @brief Create primitive
            /// @param pData Raw primitive data
            /// @param frameSettings Current frame buffer settings
            LinePrimitive(unsigned long* pData, FrameBufferSettings& frameSettings);
            
            /// @brief Process primitive
            /// @param outputBuffer Output buffer for primitive formatted data
            virtual void process(std::vector<float>& outputBuffer);
        };
    }
}
