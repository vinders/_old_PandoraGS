/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
--------------------------------------------------------------------------------
Description : GP0 primitives - VRAM transfers (copy/read/write)
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include "./param_reader.h"

namespace psx { 
  class DisplayController;

  namespace primitives { 
    /// @brief VRAM transfer commands 
    class VramTransfer final {
    public:
      VramTransfer() = delete;
    
      /// @brief GPU command params: copy rectangle from/to frame buffer
      /// @warning: - X/Y coordinates are absolute coordinates of the frame buffer (not affected by draw area/offset).
      ///           - if source/destination x+width/y+height exceed the VRAM size (1024x512): operations wrap to opposite edge.
      ///           - continuous DMA transfers only work if requested during V-Blank.
      ///             (only with continuous DMA, the problem doesn't occur with linked-list DMA / ordering table).
      ///           - affected by mask setting.
      struct CopyRectangleParams final {
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 4u }; }
        
        inline uint32_t id() const noexcept           { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t sourceX() const noexcept      { return ParamReader::toAbsoluteVramX(raw[1]); }
        inline uint32_t sourceY() const noexcept      { return ParamReader::toAbsoluteVramY(raw[1]); }
        inline uint32_t destinationX() const noexcept { return ParamReader::toAbsoluteVramX(raw[2]); }
        inline uint32_t destinationY() const noexcept { return ParamReader::toAbsoluteVramY(raw[2]); }
        inline uint32_t width() const noexcept        { return ParamReader::toVramWidthWithZeroMax(raw[3]); }
        inline uint32_t height() const noexcept       { return ParamReader::toVramHeightWithZeroMax(raw[3]); }
      };
      
      /// @brief GPU command params: read/write rectangle to/from frame buffer (data transfer through DMA)
      /// @warning: - X/Y coordinates are absolute coordinates of the frame buffer (not affected by draw area/offset).
      ///           - if x+width/y+height exceed the VRAM size (1024x512): operations wrap to opposite edge.
      ///           - continuous DMA transfers (GP0(A0h,C0h)) only work if requested during V-Blank 
      ///             (only with continuous DMA, the problem doesn't occur with linked-list DMA (GP0(20h..7Fh,E1h..E6h)) / ordering table).
      ///           - in write mode: affected by mask setting.
      ///           - set bit27 of status register when image data can be read.
      struct ReadWriteRectangleParams final {
        uint32_t* raw;
        static constexpr inline size_t size() noexcept { return size_t{ 3u }; }
        
        inline uint32_t id() const noexcept     { return ParamReader::toGpuCommandId(*raw); }
        inline uint32_t x() const noexcept      { return ParamReader::toAbsoluteVramX(raw[1]); }
        inline uint32_t y() const noexcept      { return ParamReader::toAbsoluteVramY(raw[1]); }
        inline uint32_t width() const noexcept  { return ParamReader::toVramWidthWithZeroMax(raw[2]); }
        inline uint32_t height() const noexcept { return ParamReader::toVramHeightWithZeroMax(raw[2]); }
      };
      
      
      // -- commands processing --
      
      static void copyRectangle(psx::DisplayController& controller, uint32_t* gdata) noexcept {
        CopyRectangleParams params{ gdata };
        //...
      }
      static void readRectangle(psx::DisplayController& controller, uint32_t* gdata) noexcept {
        ReadWriteRectangleParams params{ gdata };
        //...
      }
      static void writeRectangle(psx::DisplayController& controller, uint32_t* gdata) noexcept {
        ReadWriteRectangleParams params{ gdata };
        //...
      }
    };

  }
}
