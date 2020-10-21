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
    /// @brief Type of rendering of textures on polygons/lines/tiles
    enum class TextureMode : uint32_t {
      none = 0u,
      blending = 1u,
      raw = 2u
    };
  }
}
