/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
--------------------------------------------------------------------------------
Description : GP0/GP1 primitives - common GPU data reader (metadata, color conversion, coords...)
*******************************************************************************/
#pragma once

#include <cstdint>

namespace psx { 
  namespace primitives { 
    class ParamReader final {
    public:
      ParamReader() = delete;
      
      // -- command & color/texture metadata --
      
      /// @brief Extract command identifier from first byte of command
      static constexpr inline uint32_t toGpuCommandId(uint32_t firstByte) noexcept { return ((firstByte >> 24) & 0xFFu); }
      /// @brief Verify if GPU data is a poly-line termination code
      static constexpr inline bool isPolyLineTermination(uint32_t raw) noexcept { return ((raw & 0xF000F000) == 0x50005000u); }
      
      /// @brief Get 24-bit color without conversion
      /// @warning - untextured graphics: RGB values of FFh are brightest.
      ///          - texture blending:    RGB values of 80h are brightest ([81h;FFh] make textures brighter, but the blending result can't exceed FFh).
      static constexpr inline uint32_t toColorBGR24(uint32_t raw) noexcept { return (raw & 0x00FFFFFFu); }
      /// @brief Convert BGR 24-bit color to BGR 16-bit color (strip LSBs)
      static constexpr inline uint32_t convertBGR24toBGR16(uint32_t bgr) noexcept {
        return ( ((bgr & 0x0000F8) >> 3)
               | ((bgr & 0x00F800) >> 6)
               | ((bgr & 0xF80000) >> 9) );
      }
      
      /// @brief Convert GPU data to texture page bits
      /// @warning Texture page bits: bit0-8==TexturePageDrawModeParams.bit0-8 / bit11==TexturePageDrawModeParams.bit11 / other bits unused.
      static constexpr inline uint32_t toTexturePage(uint32_t raw) noexcept  { return ((raw >> 16) & 0x09FFu); }
      /// @brief Convert GPU data to texture lookup table/palette X coord (steps of 16px): [0;1008]
      static constexpr inline uint32_t toLookupTableX(uint32_t raw) noexcept { return ((raw >> 16) & 0x003Fu); }
      /// @brief Convert GPU data to texture lookup table/palette Y coord: [0;511]
      static constexpr inline uint32_t toLookupTableY(uint32_t raw) noexcept { return ((raw >> 22) & 0x01FFu); }
      /// @brief Convert GPU data to texture lookup table/palette X and Y coords: x:[0;1008], y:[0;511]
      static inline void toLookupTableXY(uint32_t raw, uint32_t& x, uint32_t& y) noexcept { x = toLookupTableX(raw); y = toLookupTableY(raw); }
      
      
      // -- drawing coords --
      
      /// @brief Convert GPU data to relative X coord: [-1024;1023]
      static constexpr inline int32_t toRelativeX(uint32_t raw) noexcept { return (static_cast<int32_t>(raw << 21) >> 21); }
      /// @brief Convert GPU data to relative Y coord: [-1024;1023]
      static constexpr inline int32_t toRelativeY(uint32_t raw) noexcept { return (static_cast<int32_t>(raw << 5) >> 21); }
      /// @brief Convert GPU data to relative X and Y coords: [-1024;1023]
      static inline void toRelativeXY(uint32_t raw, int32_t& x, int32_t& y) noexcept { x = toRelativeX(raw); y = toRelativeY(raw); }
      
      /// @brief Convert GPU data to size X: [0;1023]
      static constexpr inline uint32_t toSizeX(uint32_t raw) noexcept { return (raw & 0x03FFu); }
      /// @brief Convert GPU data to size Y: [0;511]
      static constexpr inline uint32_t toSizeY(uint32_t raw) noexcept { return ((raw >> 16) & 0x01FFu); }
      
      /// @brief Convert GPU data to relative X coord: [0;255]
      /// @warning For lookup table (palette): for 4bit/px textures (CLUT), coord X must be even
      static constexpr inline uint32_t toTextureCoordX(uint32_t raw) noexcept { return (raw & 0x00FFu); }
      /// @brief Convert GPU data to relative Y coord: [0;255]
      static constexpr inline uint32_t toTextureCoordY(uint32_t raw) noexcept { return ((raw >> 8) & 0x00FFu); }
      /// @brief Convert GPU data to relative X and Y coords: [0;255]
      static inline uint32_t toTextureCoordXY(uint32_t raw, uint32_t& tx, uint32_t& ty) noexcept { tx = toTextureCoordX(raw); ty = toTextureCoordY(raw); }
      
      
      // -- VRAM limits --
      
      /// @brief Convert GPU data to absolute X VRAM coord: [0;1008] (floor-rounding by steps of 16px)
      static constexpr inline uint32_t toVram4bitAlignedX(uint32_t raw) noexcept { return (raw & 0x03F0u); }
      /// @brief Convert GPU data to absolute X VRAM coord: [0;1023]
      static constexpr inline uint32_t toAbsoluteVramX(uint32_t raw) noexcept    { return (raw & 0x03FFu); }
      /// @brief Convert GPU data to absolute Y VRAM coord: [0;511]
      static constexpr inline uint32_t toAbsoluteVramY(uint32_t raw) noexcept    { return ((raw >> 16) & 0x01FFu); }
      /// @brief Convert GPU data to VRAM width: [0;1024] (ceil-rounding by steps of 16px / if raw==1024 -> use 0)
      static constexpr inline uint32_t toVram4bitRoundedWidth(uint32_t raw) noexcept  { return (((raw & 0x03FFu) + 0x000Fu) & 0x0FF0u); }
      /// @brief Convert GPU data to non-zero VRAM width: [1;1024] (if raw==0 -> use max)
      static constexpr inline uint32_t toVramWidthWithZeroMax(uint32_t raw) noexcept  { return (((raw & 0xFFFFu) - 1u) & 0x03FFu) + 1u; }
      /// @brief Convert GPU data to non-zero VRAM height: [1;512] (if raw==0 -> use max)
      static constexpr inline uint32_t toVramHeightWithZeroMax(uint32_t raw) noexcept { return (((raw >> 16) - 1u) & 0x01FFu) + 1u; }
      /// @brief Convert GPU data to VRAM height: [0;511]
      static constexpr inline uint32_t toVramHeight(uint32_t raw) noexcept            { return ((raw >> 16) & 0x01FFu); }
    };
  }
}
