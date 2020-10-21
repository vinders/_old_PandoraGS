/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
--------------------------------------------------------------------------------
Description : GP0 primitives - drawing triangles/quads
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include "./shading_type.h"
#include "./texture_mode.h"
#include "./param_reader.h"

namespace psx { 
  class DisplayController;

  namespace primitives { 
    /// @brief Polygon drawing commands
    class PolygonDrawing final {
    public:
      PolygonDrawing() = delete;
      
      /// @brief GPU command params: draw triangle
      /// @warning - dithering only affects gouraud-shaded or texture-blended triangles.
      ///          - polygons are displayed up to EXCLUDED lower-right coords.
      ///          - size restriction: max distance between two vertices is 1023 horizontally and 511 vertically (not rendered if exceeded).
      ///          - only the portion inside of the drawing area is rendered.
      ///          - texture page bits: bit0-8==TexturePageDrawModeParams.bit0-8 / bit11==TexturePageDrawModeParams.bit11 / other bits unused.
      template <ShadingType _Shading, TextureMode _Texture>
      struct DrawTriangleParams final {};
      
      // monochrome
      template <> struct DrawTriangleParams<ShadingType::monochrome, TextureMode::none> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 4u }; }
        
        inline uint32_t id() const noexcept                        { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t colorBGR16() const noexcept                { return ParamReader::convertBGR24toBGR16(*raw); }
        inline uint32_t colorBGR24() const noexcept                { return ParamReader::toColorBGR24(*raw); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[1], x, y); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[2], x, y); }
        inline void vertex3(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[3], x, y); }
      };
      // monochrome with texture blending
      template <> struct DrawTriangleParams<ShadingType::monochrome, TextureMode::blending> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 7u }; }
        
        inline uint32_t id() const noexcept                                { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t colorBGR16() const noexcept                        { return ParamReader::convertBGR24toBGR16(*raw); }
        inline uint32_t colorBGR24() const noexcept                        { return ParamReader::toColorBGR24(*raw); }
        inline void lookupTable(uint32_t& x, uint32_t& y) const noexcept   { ParamReader::toLookupTableXY(raw[2], x, y); }
        inline uint32_t texturePage() const noexcept                       { return ParamReader::toTexturePage(raw[4]); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[1], x, y); }
        inline void textureCoords1(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[2], x, y); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[3], x, y); }
        inline void textureCoords2(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[4], x, y); }
        inline void vertex3(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[5], x, y); }
        inline void textureCoords3(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[6], x, y); }
      };
      // raw textured
      template <> struct DrawTriangleParams<ShadingType::monochrome, TextureMode::raw> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 7u }; }
        
        inline uint32_t id() const noexcept                                { return ParamReader::toGpuCommandId(*raw); }
        inline void lookupTable(uint32_t& x, uint32_t& y) const noexcept   { ParamReader::toLookupTableXY(raw[2], x, y); }
        inline uint32_t texturePage() const noexcept                       { return ParamReader::toTexturePage(raw[4]); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[1], x, y); }
        inline void textureCoords1(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[2], x, y); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[3], x, y); }
        inline void textureCoords2(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[4], x, y); }
        inline void vertex3(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[5], x, y); }
        inline void textureCoords3(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[6], x, y); }
      };
      
      // gouraud-shaded
      template <> struct DrawTriangleParams<ShadingType::gouraudShaded, TextureMode::none> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 6u }; }
        
        inline uint32_t id() const noexcept                        { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t color1_BGR16() const noexcept              { return ParamReader::convertBGR24toBGR16(*raw); }
        inline uint32_t color1_BGR24() const noexcept              { return ParamReader::toColorBGR24(*raw); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[1], x, y); }
        inline uint32_t color2_BGR16() const noexcept              { return ParamReader::convertBGR24toBGR16(raw[2]); }
        inline uint32_t color2_BGR24() const noexcept              { return ParamReader::toColorBGR24(raw[2]); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[3], x, y); }
        inline uint32_t color3_BGR16() const noexcept              { return ParamReader::convertBGR24toBGR16(raw[4]); }
        inline uint32_t color3_BGR24() const noexcept              { return ParamReader::toColorBGR24(raw[4]); }
        inline void vertex3(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[5], x, y); }
      };
      // gouraud-shaded with texture blending
      template <> struct DrawTriangleParams<ShadingType::gouraudShaded, TextureMode::blending> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 9u }; }
        
        inline uint32_t id() const noexcept                                { return ParamReader::toGpuCommandId(*raw); }
        inline void lookupTable(uint32_t& x, uint32_t& y) const noexcept   { ParamReader::toLookupTableXY(raw[2], x, y); }
        inline uint32_t texturePage() const noexcept                       { return ParamReader::toTexturePage(raw[5]); }
        inline uint32_t color1_BGR16() const noexcept                      { return ParamReader::convertBGR24toBGR16(*raw); }
        inline uint32_t color1_BGR24() const noexcept                      { return ParamReader::toColorBGR24(*raw); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[1], x, y); }
        inline void textureCoords1(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[2], x, y); }
        inline uint32_t color2_BGR16() const noexcept                      { return ParamReader::convertBGR24toBGR16(raw[3]); }
        inline uint32_t color2_BGR24() const noexcept                      { return ParamReader::toColorBGR24(raw[3]); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[4], x, y); }
        inline void textureCoords2(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[5], x, y); }
        inline uint32_t color3_BGR16() const noexcept                      { return ParamReader::convertBGR24toBGR16(raw[6]); }
        inline uint32_t color3_BGR24() const noexcept                      { return ParamReader::toColorBGR24(raw[6]); }
        inline void vertex3(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[7], x, y); }
        inline void textureCoords3(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[8], x, y); }
      };
      // raw textured
      template <> struct DrawTriangleParams<ShadingType::gouraudShaded, TextureMode::raw> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 9u }; }
        
        inline uint32_t id() const noexcept                                { return ParamReader::toGpuCommandId(*raw); }
        inline void lookupTable(uint32_t& x, uint32_t& y) const noexcept   { ParamReader::toLookupTableXY(raw[2], x, y); }
        inline uint32_t texturePage() const noexcept                       { return ParamReader::toTexturePage(raw[5]); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[1], x, y); }
        inline void textureCoords1(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[2], x, y); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[4], x, y); }
        inline void textureCoords2(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[5], x, y); }
        inline void vertex3(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[7], x, y); }
        inline void textureCoords3(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[8], x, y); }
      };
      
      // ---
      
      /// @brief GPU command params: draw quad as two triangles (<vertices 1,2,3> + <vertices 2,3,4>)
      /// @warning - dithering only affects gouraud-shaded or texture-blended quads.
      ///          - polygons are displayed up to EXCLUDED lower-right coords.
      ///          - size restriction: max distance between two vertices is 1023 horizontally and 511 vertically (not rendered if exceeded).
      ///          - only the portion inside of the drawing area is rendered.
      ///          - texture page bits: bit0-8==TexturePageDrawModeParams.bit0-8 / bit11==TexturePageDrawModeParams.bit11 / other bits unused.
      template <ShadingType _Shading, TextureMode _Texture>
      struct DrawQuadParams final {};
      
      // monochrome
      template <> struct DrawQuadParams<ShadingType::monochrome, TextureMode::none> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 5u }; }
        
        inline uint32_t id() const noexcept                        { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t colorBGR16() const noexcept                { return ParamReader::convertBGR24toBGR16(*raw); }
        inline uint32_t colorBGR24() const noexcept                { return ParamReader::toColorBGR24(*raw); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[1], x, y); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[2], x, y); }
        inline void vertex3(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[3], x, y); }
        inline void vertex4(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[4], x, y); }
      };
      // monochrome with texture blending
      template <> struct DrawQuadParams<ShadingType::monochrome, TextureMode::blending> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 9u }; }
        
        inline uint32_t id() const noexcept                                { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t colorBGR16() const noexcept                        { return ParamReader::convertBGR24toBGR16(*raw); }
        inline uint32_t colorBGR24() const noexcept                        { return ParamReader::toColorBGR24(*raw); }
        inline void lookupTable(uint32_t& x, uint32_t& y) const noexcept   { ParamReader::toLookupTableXY(raw[2], x, y); }
        inline uint32_t texturePage() const noexcept                       { return ParamReader::toTexturePage(raw[4]); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[1], x, y); }
        inline void textureCoords1(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[2], x, y); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[3], x, y); }
        inline void textureCoords2(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[4], x, y); }
        inline void vertex3(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[5], x, y); }
        inline void textureCoords3(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[6], x, y); }
        inline void vertex4(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[7], x, y); }
        inline void textureCoords4(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[8], x, y); }
      };
      // raw textured
      template <> struct DrawQuadParams<ShadingType::monochrome, TextureMode::raw> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 9u }; }
        
        inline uint32_t id() const noexcept                                { return ParamReader::toGpuCommandId(*raw); }
        inline void lookupTable(uint32_t& x, uint32_t& y) const noexcept   { ParamReader::toLookupTableXY(raw[2], x, y); }
        inline uint32_t texturePage() const noexcept                       { return ParamReader::toTexturePage(raw[4]); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[1], x, y); }
        inline void textureCoords1(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[2], x, y); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[3], x, y); }
        inline void textureCoords2(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[4], x, y); }
        inline void vertex3(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[5], x, y); }
        inline void textureCoords3(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[6], x, y); }
        inline void vertex4(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[7], x, y); }
        inline void textureCoords4(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[8], x, y); }
      };
      
      // gouraud-shaded
      template <> struct DrawQuadParams<ShadingType::gouraudShaded, TextureMode::none> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 8u }; }
        
        inline uint32_t id() const noexcept                        { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t color1_BGR16() const noexcept              { return ParamReader::convertBGR24toBGR16(*raw); }
        inline uint32_t color1_BGR24() const noexcept              { return ParamReader::toColorBGR24(*raw); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[1], x, y); }
        inline uint32_t color2_BGR16() const noexcept              { return ParamReader::convertBGR24toBGR16(raw[2]); }
        inline uint32_t color2_BGR24() const noexcept              { return ParamReader::toColorBGR24(raw[2]); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[3], x, y); }
        inline uint32_t color3_BGR16() const noexcept              { return ParamReader::convertBGR24toBGR16(raw[4]); }
        inline uint32_t color3_BGR24() const noexcept              { return ParamReader::toColorBGR24(raw[4]); }
        inline void vertex3(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[5], x, y); }
        inline uint32_t color4_BGR16() const noexcept              { return ParamReader::convertBGR24toBGR16(raw[6]); }
        inline uint32_t color4_BGR24() const noexcept              { return ParamReader::toColorBGR24(raw[6]); }
        inline void vertex4(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[7], x, y); }
      };
      // gouraud-shaded with texture blending
      template <> struct DrawQuadParams<ShadingType::gouraudShaded, TextureMode::blending> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 12u }; }
        
        inline uint32_t id() const noexcept                                { return ParamReader::toGpuCommandId(*raw); }
        inline void lookupTable(uint32_t& x, uint32_t& y) const noexcept   { ParamReader::toLookupTableXY(raw[2], x, y); }
        inline uint32_t texturePage() const noexcept                       { return ParamReader::toTexturePage(raw[5]); }
        inline uint32_t color1_BGR16() const noexcept                      { return ParamReader::convertBGR24toBGR16(*raw); }
        inline uint32_t color1_BGR24() const noexcept                      { return ParamReader::toColorBGR24(*raw); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[1], x, y); }
        inline void textureCoords1(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[2], x, y); }
        inline uint32_t color2_BGR16() const noexcept                      { return ParamReader::convertBGR24toBGR16(raw[3]); }
        inline uint32_t color2_BGR24() const noexcept                      { return ParamReader::toColorBGR24(raw[3]); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[4], x, y); }
        inline void textureCoords2(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[5], x, y); }
        inline uint32_t color3_BGR16() const noexcept                      { return ParamReader::convertBGR24toBGR16(raw[6]); }
        inline uint32_t color3_BGR24() const noexcept                      { return ParamReader::toColorBGR24(raw[6]); }
        inline void vertex3(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[7], x, y); }
        inline void textureCoords3(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[8], x, y); }
        inline uint32_t color4_BGR16() const noexcept                      { return ParamReader::convertBGR24toBGR16(raw[9]); }
        inline uint32_t color4_BGR24() const noexcept                      { return ParamReader::toColorBGR24(raw[9]); }
        inline void vertex4(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[10], x, y); }
        inline void textureCoords4(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[11], x, y); }
      };
      // raw textured
      template <> struct DrawQuadParams<ShadingType::gouraudShaded, TextureMode::raw> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 12u }; }
        
        inline uint32_t id() const noexcept                                { return ParamReader::toGpuCommandId(*raw); }
        inline void lookupTable(uint32_t& x, uint32_t& y) const noexcept   { ParamReader::toLookupTableXY(raw[2], x, y); }
        inline uint32_t texturePage() const noexcept                       { return ParamReader::toTexturePage(raw[5]); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[1], x, y); }
        inline void textureCoords1(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[2], x, y); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[4], x, y); }
        inline void textureCoords2(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[5], x, y); }
        inline void vertex3(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[7], x, y); }
        inline void textureCoords3(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[8], x, y); }
        inline void vertex4(int32_t& x, int32_t& y) const noexcept         { ParamReader::toRelativeXY(raw[10], x, y); }
        inline void textureCoords4(uint32_t& x, uint32_t& y) const noexcept{ ParamReader::toTextureCoordXY(raw[11], x, y); }
      };
      
      
      // -- commands processing --
      
      template <ShadingType _Shading, TextureMode _Texture, bool _SemiTransparency>
      static void drawTriangle(psx::DisplayController& controller, uint32_t* gdata) noexcept {
        DrawTriangleParams<_Shading,_Texture> params{ gdata };
        //...
      }
      template <ShadingType _Shading, TextureMode _Texture, bool _SemiTransparency>
      static void drawQuad(psx::DisplayController& controller, uint32_t* gdata) noexcept {
        DrawQuadParams<_Shading,_Texture> params{ gdata };
        //...
      }
    };
  }
}
