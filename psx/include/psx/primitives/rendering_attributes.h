/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
--------------------------------------------------------------------------------
Description : GP0 primitives - rendering attributes (texture page, drawing area...)
*******************************************************************************/
#pragma once

#include <cstdint>
#include "./param_reader.h"

namespace psx { 
  namespace primitives { 
    /// @brief GPU command params: set draw mode for texture page
    /// @warning Textured polygon have their own texpage params (except for: dithering, drawing allowed).
    struct TexturePageDrawModeParams final {
      uint32_t raw;
      static constexpr inline size_t size() noexcept { return size_t{ 1u }; }
      
      constexpr inline uint32_t id() const noexcept                  { return ParamReader::toGpuCommandId(raw); }
      constexpr inline uint32_t texturePageBaseX() const noexcept    { return ((raw & 0x000Fu) << 6); } // pixels
      constexpr inline uint32_t texturePageBaseY() const noexcept    { return ((raw & 0x0010u) << 4); } // pixels
      constexpr inline uint32_t semiTransparency() const noexcept    { return (raw & 0x0060u); } // 00=B/2+F/2, 01=B+F, 10=B-F, 00=B+F/4
      constexpr inline uint32_t texturePageColors() const noexcept   { return (raw & 0x0180u); } // 00=4bit, 01=8bit, 10=15bit, 11=15bit
      constexpr inline bool dithering() const noexcept               { return (raw & 0x0200u); }
      constexpr inline bool canDrawToDisplayArea() const noexcept    { return (raw & 0x0400u) != 0; }
      constexpr inline bool isTexturedRectangleFlipX() const noexcept{ return (raw & 0x1000u) != 0; }
      constexpr inline bool isTexturedRectangleFlipY() const noexcept{ return (raw & 0x2000u) != 0; }
      
      /// @brief Update data exchange status register
      constexpr inline uint32_t dataExchangeStatus(uint32_t oldStatus, bool isTextureDisablingAllowed) const noexcept {
        return (isTextureDisablingAllowed) 
               ? ((oldStatus & ~0x87FFu) | (raw & 0x07FFu) | ((raw << 4) & 0x8000u))
               : ((oldStatus & ~0x07FFu) | (raw & 0x07FFu));
      }
    };
  
    /// @brief GPU command params: set texture window
    /// @warning GPU acts as if area within texture window was repeated throughout the texture page 
    ///          (not actually stored that way, but read by GPU as a repeated pattern).
    struct TextureWindowParams final {
      uint32_t raw;
      static constexpr inline size_t size() noexcept { return size_t{ 1u }; }
      
      constexpr inline uint32_t id() const noexcept      { return ParamReader::toGpuCommandId(raw); }
      constexpr inline uint32_t maskX() const noexcept   { return ((raw & 0x00001Fu) << 3); } // replacement mask in current coords
      constexpr inline uint32_t maskY() const noexcept   { return ((raw & 0x0003E0u) >> 2); } // replacement mask in current coords
      constexpr inline uint32_t offsetX() const noexcept { return ((raw & 0x007C00u) >> 7); }  // new bit values in mask
      constexpr inline uint32_t offsetY() const noexcept { return ((raw & 0x0F8000u) >> 12); } // new bit values in mask
      
      /// @brief Calculate new texture X coord
      constexpr inline uint32_t textureCoordX(uint32_t oldCoord) const noexcept {
        return ((oldCoord & ~maskX()) | (offsetX() & maskX()));
      }
      /// @brief Calculate new texture Y coord
      constexpr inline uint32_t textureCoordY(uint32_t oldCoord) const noexcept {
        return ((oldCoord & ~maskY()) | (offsetY() & maskY()));
      }
    };
    
    /// @brief GPU command params: set drawing area coords (start: top-left / end: bottom-right)
    /// @warning Polygon/line/tile render commands (GP0:[20h;7Fh]): pixels drawn outside of this region are clipped.
    struct DrawingAreaCoordsParams final {
      uint32_t raw;
      static constexpr inline size_t size() noexcept { return size_t{ 1u }; }
      
      constexpr inline uint32_t id() const noexcept              { return ParamReader::toGpuCommandId(raw); }
      constexpr inline uint32_t areaCoordX() const noexcept      { return (raw & 0x0003FFu); } // [0;1023]
      constexpr inline uint32_t areaCoordY() const noexcept      { return ((raw & 0x0FFC00u) >> 10); } // [0;1023]
      constexpr inline uint32_t legacyAreaCoordY() const noexcept{ return ((raw & 0x07FC00u) >> 10); } // old GPU: [0;511]
    };
    
    /// @brief GPU command params: set drawing offset, based on drawing area coords (start: X1,Y1 / end: X2/Y2)
    /// @warning - if GTE configured with (0,0) in the center:   offset should be X1+(X2-X1)/2, Y1+(Y2-Y1)/2.
    ///          - if GTE configured with (0,0) in the top-left: offset should be X1, Y1.
    struct DrawingOffsetParams final {
      uint32_t raw;
      static constexpr inline size_t size() noexcept { return size_t{ 1u }; }
      
      constexpr inline uint32_t id() const noexcept     { return ParamReader::toGpuCommandId(raw); }
      constexpr inline int32_t offsetX() const noexcept { return (static_cast<int32_t>(raw << 21) >> 21); } // [-1024;1023]
      constexpr inline int32_t offsetY() const noexcept { return (static_cast<int32_t>(raw << 10) >> 21); } // [-1024;1023]
    };
    
    /// @brief GPU command params: set mask bit
    /// @warning - affects all rendering commands + copy/write frame buffer commands (but NOT the fill vram command).
    ///          - apply mask bit:  true: force mask bit to 1  /  false: use bit15 of texture color (if marked as semi-transparent) or set to 0 for untextured polygons.
    struct MaskBitParams final {
      uint32_t raw;
      static constexpr inline size_t size() noexcept { return size_t{ 1u }; }
      
      constexpr inline uint32_t id() const noexcept                { return ParamReader::toGpuCommandId(raw); }
      constexpr inline bool applyMaskToDrawnPixels() const noexcept{ return (raw & 0x0001u) != 0; }
      constexpr inline bool canDrawToMaskedPixels() const noexcept { return (raw & 0x0002u) == 0; }
      
      /// @brief Update data exchange status register
      constexpr inline uint32_t dataExchangeStatus(uint32_t oldStatus) const noexcept {
        return ((oldStatus & ~0x1800u) | ((raw << 11) & 0x1800u));
      }
    };
  }
}
