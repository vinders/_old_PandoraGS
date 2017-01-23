/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : drawing primitive factory
*******************************************************************************/
#pragma once

#include "i_primitive.h"
#include "line_primitive.h"
#include "poly_primitive.h"
#include "rect_primitive.h"
#include "attribute.h"

/// @namespace command
/// GPU commands management
namespace command
{
    /// @namespace command.primitive
    /// Drawing primitive management
    namespace primitive
    {
        /// @class PrimitiveFactory
        /// @brief Drawing primitive factory
        class PrimitiveFactory
        {
            /// @brief Create primitive
            /// @param pData Raw primitive data
            static IPrimitive& createPrimitive(unsigned long* pData);
        };
    }
}
