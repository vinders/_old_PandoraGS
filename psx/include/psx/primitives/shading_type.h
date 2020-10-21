/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
--------------------------------------------------------------------------------
Description : Primitives - tiles/sprites command params
*******************************************************************************/
#pragma once

#include <cstdint>

namespace psx { 
  namespace primitives { 
    /// @brief Type of shading used to draw a polygon/line
    enum class ShadingType : uint32_t {
      monochrome = 0u,
      gouraudShaded = 1u
    };
  }
}
