/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
--------------------------------------------------------------------------------
Description : GP0 primitives - drawing tiles/sprites
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include "./texture_mode.h"
#include "./param_reader.h"

namespace psx { 
  class DisplayController;

  namespace primitives { 
    /// @brief Tile drawing commands 
    class TileDrawing final {
    public:
      TileDrawing() = delete;
  
      /// @brief GPU command params: draw tile/sprite with fixed size (1x1, 8x8, 16x16)
      /// @warning - no dithering and no texture rotation or scaling can be applied.
      ///          - texture page is set separately with TexturePageDrawModeParams.
      ///          - affected by x/y-flip in texture page settings (not on oldest GPU).
      ///          - rendered at once (no triangles).
      ///          - only the portion inside of the drawing area is rendered.
      template <TextureMode _Shading>
      struct DrawTileParams final {};
      
      // monochrome
      template <> struct DrawTileParams<TextureMode::none> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 2u }; }
        
        inline uint32_t id() const noexcept         { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t colorBGR16() const noexcept { return ParamReader::convertBGR24toBGR16(*raw); }
        inline int32_t x() const noexcept           { return ParamReader::toRelativeX(raw[1]); }
        inline int32_t y() const noexcept           { return ParamReader::toRelativeY(raw[1]); }
      };
      // monochrome with texture blending
      template <> struct DrawTileParams<TextureMode::blending> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 3u }; }
        
        inline uint32_t id() const noexcept          { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t colorBGR16() const noexcept  { return ParamReader::convertBGR24toBGR16(*raw); }
        inline int32_t x() const noexcept            { return ParamReader::toRelativeX(raw[1]); }
        inline int32_t y() const noexcept            { return ParamReader::toRelativeY(raw[1]); }
        inline void lookupTable(uint32_t& x, uint32_t& y) const noexcept { ParamReader::toLookupTableXY(raw[2], x, y); }
        inline void textureCoords(uint32_t x, uint32_t y) const noexcept { ParamReader::toTextureCoordXY(raw[2], x, y); }
      };
      // raw textured
      template <> struct DrawTileParams<TextureMode::raw> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 3u }; }
        
        inline uint32_t id() const noexcept          { return ParamReader::toGpuCommandId(*raw); }
        inline int32_t x() const noexcept            { return ParamReader::toRelativeX(raw[1]); }
        inline int32_t y() const noexcept            { return ParamReader::toRelativeY(raw[1]); }
        inline void lookupTable(uint32_t& x, uint32_t& y) const noexcept { ParamReader::toLookupTableXY(raw[2], x, y); }
        inline void textureCoords(uint32_t x, uint32_t y) const noexcept { ParamReader::toTextureCoordXY(raw[2], x, y); }
      };
      
      // ---
      
      /// @brief GPU command params: draw tile/sprite with custom size
      /// @warning - no dithering and no texture rotation or scaling can be applied.
      ///          - texture page is set separately with TexturePageDrawModeParams.
      ///          - affected by x/y-flip in texture page settings (not on oldest GPU).
      ///          - rendered at once (no triangles).
      ///          - size restriction: max size is 1023 horizontally and 511 vertically (not rendered if exceeded).
      ///          - only the portion inside of the drawing area is rendered.
      template <TextureMode _Shading>
      struct DrawCustomTileParams final {};
      
      // monochrome
      template <> struct DrawCustomTileParams<TextureMode::none> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 3u }; }
        
        inline uint32_t id() const noexcept         { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t colorBGR16() const noexcept { return ParamReader::convertBGR24toBGR16(*raw); }
        inline int32_t x() const noexcept           { return ParamReader::toRelativeX(raw[1]); }
        inline int32_t y() const noexcept           { return ParamReader::toRelativeY(raw[1]); }
        inline uint32_t width() const noexcept      { return ParamReader::toSizeX(raw[2]); }
        inline uint32_t height() const noexcept     { return ParamReader::toSizeY(raw[2]); }
      };
      // monochrome with texture blending
      template <> struct DrawCustomTileParams<TextureMode::blending> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 4u }; }
        
        inline uint32_t id() const noexcept          { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t colorBGR16() const noexcept  { return ParamReader::convertBGR24toBGR16(*raw); }
        inline int32_t x() const noexcept            { return ParamReader::toRelativeX(raw[1]); }
        inline int32_t y() const noexcept            { return ParamReader::toRelativeY(raw[1]); }
        inline uint32_t width() const noexcept       { return ParamReader::toSizeX(raw[3]); }
        inline uint32_t height() const noexcept      { return ParamReader::toSizeY(raw[3]); }
        inline void lookupTable(uint32_t& x, uint32_t& y) const noexcept { ParamReader::toLookupTableXY(raw[2], x, y); }
        inline void textureCoords(uint32_t x, uint32_t y) const noexcept { ParamReader::toTextureCoordXY(raw[2], x, y); }
      };
      // raw textured
      template <> struct DrawCustomTileParams<TextureMode::raw> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 4u }; }
        
        inline uint32_t id() const noexcept          { return ParamReader::toGpuCommandId(*raw); }
        inline int32_t x() const noexcept            { return ParamReader::toRelativeX(raw[1]); }
        inline int32_t y() const noexcept            { return ParamReader::toRelativeY(raw[1]); }
        inline uint32_t width() const noexcept       { return ParamReader::toSizeX(raw[3]); }
        inline uint32_t height() const noexcept      { return ParamReader::toSizeY(raw[3]); }
        inline void lookupTable(uint32_t& x, uint32_t& y) const noexcept { ParamReader::toLookupTableXY(raw[2], x, y); }
        inline void textureCoords(uint32_t x, uint32_t y) const noexcept { ParamReader::toTextureCoordXY(raw[2], x, y); }
      };
      
      
      // -- commands processing --
    
      template <uint32_t _PixelWidth, TextureMode _Texture, bool _SemiTransparency>
      static void drawTile(psx::DisplayController& controller, uint32_t* gdata) noexcept {
        DrawTileParams<_Texture> params{ gdata };
        //...
      }
      template <TextureMode _Texture, bool _SemiTransparency>
      static void drawCustomTile(psx::DisplayController& controller, uint32_t* gdata) noexcept {
        DrawCustomTileParams<_Texture> params{ gdata };
        //...
      }
    };
  }
}
