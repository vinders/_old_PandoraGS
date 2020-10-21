/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
--------------------------------------------------------------------------------
Description : GP0 primitives - drawing lines/poly-lines
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include "./shading_type.h"
#include "./param_reader.h"

namespace psx { 
  class DisplayController;

  namespace primitives { 
    /// @brief Line drawing commands 
    class LineDrawing final {
    public:
      LineDrawing() = delete;
      
      /// @brief GPU command params: draw single line
      /// @warning - dithering affects all lines.
      ///          - lines are displayed up to INCLUDED lower-right coords.
      ///          - size restriction: max distance between two vertices is 1023 horizontally and 511 vertically (not rendered if exceeded).
      ///          - only the portion inside of the drawing area is rendered.
      template <ShadingType _Shading>
      struct DrawLineParams final {};
      
      // monochrome
      template <> struct DrawLineParams<ShadingType::monochrome> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 3u }; }
        
        inline uint32_t id() const noexcept                        { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t colorBGR16() const noexcept                { return ParamReader::convertBGR24toBGR16(*raw); }
        inline uint32_t colorBGR24() const noexcept                { return ParamReader::toColorBGR24(*raw); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[1], x, y); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[2], x, y); }
      };
      // gouraud-shaded
      template <> struct DrawLineParams<ShadingType::gouraudShaded> final { 
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 4u }; }
        
        inline uint32_t id() const noexcept                        { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t color1_BGR16() const noexcept              { return ParamReader::convertBGR24toBGR16(*raw); }
        inline uint32_t color1_BGR24() const noexcept              { return ParamReader::toColorBGR24(*raw); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[1], x, y); }
        inline uint32_t color2_BGR16() const noexcept              { return ParamReader::convertBGR24toBGR16(raw[2]); }
        inline uint32_t color2_BGR24() const noexcept              { return ParamReader::toColorBGR24(raw[2]); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[3], x, y); }
      };
      
      // ---
      
      /// @brief GPU command params: draw poly-line
      /// @warning - dithering affects all lines.
      ///          - lines are displayed up to INCLUDED lower-right coords.
      ///          - size restriction: max distance between two vertices is 1023 horizontally and 511 vertically (not rendered if exceeded).
      ///          - only the portion inside of the drawing area is rendered.
      template <ShadingType _Shading>
      struct DrawPolyLineParams final {};
      
      // monochrome
      template <> struct DrawPolyLineParams<ShadingType::monochrome> final { 
        uint32_t* raw;
        static constexpr inline size_t maxSize() noexcept { return size_t{ 254u }; }
        
        inline uint32_t id() const noexcept                        { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t colorBGR16() const noexcept                { return ParamReader::convertBGR24toBGR16(*raw); }
        inline uint32_t colorBGR24() const noexcept                { return ParamReader::toColorBGR24(*raw); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[1], x, y); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[2], x, y); }
        
        inline uint32_t* begin() const noexcept { return raw; }
        inline bool nextVertex(uint32_t* iter, int32_t& x, int32_t& y) const noexcept { 
          if (iter < raw + maxSize() && !(ParamReader::isPolyLineTermination(*iter)) ) {
            ParamReader::toRelativeXY(*iter, x, y);
            ++iter;
            return true;
          }
          return false;
        }
      };
      // gouraud-shaded
      template <> struct DrawPolyLineParams<ShadingType::gouraudShaded> final { 
        uint32_t* raw;
        static constexpr inline size_t maxSize() noexcept { return size_t{ 255u }; }
        
        inline uint32_t id() const noexcept                        { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t color1_BGR16() const noexcept              { return ParamReader::convertBGR24toBGR16(*raw); }
        inline uint32_t color1_BGR24() const noexcept              { return ParamReader::toColorBGR24(*raw); }
        inline void vertex1(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[1], x, y); }
        inline uint32_t color2_BGR16() const noexcept              { return ParamReader::convertBGR24toBGR16(raw[2]); }
        inline uint32_t color2_BGR24() const noexcept              { return ParamReader::toColorBGR24(raw[2]); }
        inline void vertex2(int32_t& x, int32_t& y) const noexcept { ParamReader::toRelativeXY(raw[3], x, y); }
        
        inline uint32_t* begin() const noexcept { return raw; }
        inline bool nextVertexAndColorBGR16(uint32_t* iter, int32_t& x, int32_t& y, uint32_t& color) const noexcept { 
          if (iter < raw + maxSize() && !(ParamReader::isPolyLineTermination(*iter)) ) {
            color = ParamReader::convertBGR24toBGR16(*iter);
            ++iter;
            ParamReader::toRelativeXY(*iter, x, y);
            ++iter;
            return true;
          }
          return false;
        }
        inline bool nextVertexAndColorBGR24(uint32_t* iter, int32_t& x, int32_t& y, uint32_t& color) const noexcept { 
          if (iter < raw + maxSize() && !(ParamReader::isPolyLineTermination(*iter)) ) {
            color = ParamReader::toColorBGR24(*iter);
            ++iter;
            ParamReader::toRelativeXY(*iter, x, y);
            ++iter;
            return true;
          }
          return false;
        }
      };
      
      
      // -- commands processing --
      
      template <ShadingType _Shading, bool _SemiTransparency>
      static void drawLine(psx::DisplayController& controller, uint32_t* gdata) noexcept {
        DrawLineParams<_Shading> params{ gdata };
        //...
      }
      template <ShadingType _Shading, bool _SemiTransparency>
      static void drawPolyLine(psx::DisplayController& controller, uint32_t* gdata) noexcept {
        DrawPolyLineParams<_Shading> params{ gdata };
        //...
      }
    };
  }
}
