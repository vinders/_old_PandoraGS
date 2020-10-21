/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
--------------------------------------------------------------------------------
Description : GP0 primitives - direct VRAM access (clear/fill)
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include "./param_reader.h"

namespace psx { 
  class DisplayController;

  namespace primitives { 
    /// @brief Direct VRAM access commands
    class DirectVramAccess final {
    public:
      DirectVramAccess() = delete;
    
      /// @brief GPU command params: fill rectangle in frame buffer
      /// @warning: - if width()==0 or height()==0: the command has no effect.
      ///           - X/Y coordinates are absolute coordinates of the frame buffer (not affected by draw area/offset).
      ///           - if x+width/y+height exceed the VRAM size (1024x512): operations wrap to opposite edge.
      ///           - not affected by mask setting.
      struct FillRectangleParams final {
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 3u }; }
        
        inline uint32_t id() const noexcept         { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t colorBGR16() const noexcept { return ParamReader::convertBGR24toBGR16(*raw); }
        inline uint32_t x() const noexcept          { return ParamReader::toVram4bitAlignedX(raw[1]); }
        inline uint32_t y() const noexcept          { return ParamReader::toAbsoluteVramY(raw[1]); }
        inline uint32_t width() const noexcept      { return ParamReader::toVram4bitRoundedWidth(raw[2]); }
        inline uint32_t height() const noexcept     { return ParamReader::toVramHeight(raw[2]); }
      };
      
      
      // -- commands processing --
      
      static void clearCache(psx::DisplayController& controller, uint32_t* gdata) noexcept {
        // flush texture cache
        //...
      }
      static void fillRectangle(psx::DisplayController& controller, uint32_t* gdata) noexcept {
        FillRectangleParams params{ gdata };
        //...
      }
      
    };

  }
}
