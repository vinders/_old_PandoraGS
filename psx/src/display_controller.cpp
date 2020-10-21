/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
*******************************************************************************/
#include <cstddef>
#include <cstdint>
#include <system/preprocessor_tools.h>
#include "psx/display_controller.h"
#include "psx/primitives/direct_vram_access.h"
#include "psx/primitives/polygon_drawing.h"
#include "psx/primitives/line_drawing.h"
#include "psx/primitives/tile_drawing.h"
#include "psx/primitives/vram_transfer.h"
#include "psx/primitives/rendering_attributes.h"
#if !defined(_CPP_REVISION) || _CPP_REVISION != 14
# define __if_constexpr if constexpr
#else
# define __if_constexpr if
#endif

using namespace psx::primitives;

namespace psx {
  namespace gp0 {
    // -- GP0 infrastructure/interruption commands --
    
    static void setIrq1(psx::DisplayController& controller, uint32_t*) noexcept {
      controller.getStatusRegister().interruptRequest1(true);
    }

    
    // -- GP0 rendering attributes --
    
    static void setAttributeDrawMode(psx::DisplayController& controller, uint32_t* gdata) noexcept {
      TexturePageDrawModeParams params{ *gdata };
      controller.getStatusRegister().write(
        params.dataExchangeStatus(controller.getStatusRegister().read(), controller.isTextureDisablingAllowed()) 
      );
      //flipX/Y: TODO
      //...
    }
    static void setAttributeTextureWindow(psx::DisplayController& controller, uint32_t* gdata) noexcept {
      TextureWindowParams params{ *gdata };
      //...
    }
    static void setAttributeDrawingAreaStart(psx::DisplayController& controller, uint32_t* gdata) noexcept {
      DrawingAreaCoordsParams params{ *gdata };
      //...
    }
    static void setAttributeDrawingAreaEnd(psx::DisplayController& controller, uint32_t* gdata) noexcept {
      DrawingAreaCoordsParams params{ *gdata };
      //...
    }
    static void setAttributeDrawingOffset(psx::DisplayController& controller, uint32_t* gdata) noexcept {
      DrawingOffsetParams params{ *gdata };
      //...
    }
    static void setAttributeMaskBit(psx::DisplayController& controller, uint32_t* gdata) noexcept {
      MaskBitParams params{ *gdata };
      //...
    }
    
  
    // -- GP0 display commands - redirection table --
    
    struct DisplayCommandHandler final {
      void (*handler)(psx::DisplayController&, uint32_t*);
      size_t paramSize;
      
      static constexpr inline uint32_t tableSize() noexcept { return 256u; }
    };

#   define _CMD_DRAW_POLYGON_ENTRY(handler,shading,texturing,transparency,paramsType)   { handler < shading,texturing,transparency >, paramsType < shading,texturing >::size() }
#   define _CMD_DRAW_LINE_ENTRY(handler,shading,texturing,transparency,paramsType)      { handler < shading,transparency >, paramsType < shading >::size() }
#   define _CMD_DRAW_POLYLINE_ENTRY(handler,shading,none,transparency,paramsType)       { handler < shading,transparency >, paramsType < shading >::maxSize() }
#   define _CMD_DRAW_CUSTOM_TILE_ENTRY(handler,none,texturing,transparency,paramsType)  { handler < texturing,transparency >, paramsType < texturing >::size() }
#   define _CMD_DRAW_FIXED_TILE_ENTRY(handler,width,texturing,transparency,paramsType)  { handler < width, texturing,transparency >, paramsType < texturing >::size() }
#   define _CMD_DRAW_8X_GROUP(_ENTRY_MACRO,handler,shading,paramsType) \
              _ENTRY_MACRO(handler, shading, TextureMode::none,     false, paramsType), \
              _ENTRY_MACRO(handler, shading, TextureMode::none,     false, paramsType), \
              _ENTRY_MACRO(handler, shading, TextureMode::none,     true,  paramsType), \
              _ENTRY_MACRO(handler, shading, TextureMode::none,     true,  paramsType), \
              _ENTRY_MACRO(handler, shading, TextureMode::blending, false, paramsType), \
              _ENTRY_MACRO(handler, shading, TextureMode::raw,      false, paramsType), \
              _ENTRY_MACRO(handler, shading, TextureMode::blending, true,  paramsType), \
              _ENTRY_MACRO(handler, shading, TextureMode::raw,      true,  paramsType)

#   define NOP_CMD                   { nullptr, size_t{ 0 } }
#   define CMD_ENTRY(func,paramSize) { func, paramSize }
#   define CMD_DRAW_POLYGON_8X_GROUP(handler,shading,paramsType)   _CMD_DRAW_8X_GROUP(_CMD_DRAW_POLYGON_ENTRY, handler, shading, paramsType)
#   define CMD_DRAW_LINE_8X_GROUP(handler,shading,paramsType)      _CMD_DRAW_8X_GROUP(_CMD_DRAW_LINE_ENTRY, handler, shading, paramsType)
#   define CMD_DRAW_POLYLINE_8X_GROUP(handler,shading,paramsType)  _CMD_DRAW_8X_GROUP(_CMD_DRAW_POLYLINE_ENTRY, handler, shading, paramsType)
#   define CMD_DRAW_CUSTOM_TILE_8X_GROUP(handler,paramsType)       _CMD_DRAW_8X_GROUP(_CMD_DRAW_CUSTOM_TILE_ENTRY, handler, 0, paramsType)
#   define CMD_DRAW_FIXED_TILE_8X_GROUP(handler,width,paramsType)  _CMD_DRAW_8X_GROUP(_CMD_DRAW_FIXED_TILE_ENTRY, handler, width, paramsType)
    
    static constexpr const DisplayCommandHandler displayCommandTable[DisplayCommandHandler::tableSize()] = {
      // 0x00: infrastructure/direct VRAM access/interruptions
      NOP_CMD, 
      CMD_ENTRY(DirectVramAccess::clearCache,    size_t{ 1u }), 
      CMD_ENTRY(DirectVramAccess::fillRectangle, DirectVramAccess::FillRectangleParams::size()), 
      _P_DUPLICATE_5X_COMMA(NOP_CMD), 
      _P_DUPLICATE_16X_COMMA(NOP_CMD), 
      _P_DUPLICATE_7X_COMMA(NOP_CMD), 
      CMD_ENTRY(setIrq1, size_t{ 1u }),

      // 0x20: render polygons (triangles/quads)
      CMD_DRAW_POLYGON_8X_GROUP(PolygonDrawing::drawTriangle, ShadingType::monochrome,    PolygonDrawing::DrawTriangleParams),
      CMD_DRAW_POLYGON_8X_GROUP(PolygonDrawing::drawQuad,     ShadingType::monochrome,    PolygonDrawing::DrawQuadParams),
      CMD_DRAW_POLYGON_8X_GROUP(PolygonDrawing::drawTriangle, ShadingType::gouraudShaded, PolygonDrawing::DrawTriangleParams),
      CMD_DRAW_POLYGON_8X_GROUP(PolygonDrawing::drawQuad,     ShadingType::gouraudShaded, PolygonDrawing::DrawQuadParams),
      
      // 0x40: render lines
      CMD_DRAW_LINE_8X_GROUP(LineDrawing::drawLine,         ShadingType::monochrome,    LineDrawing::DrawLineParams),
      CMD_DRAW_POLYLINE_8X_GROUP(LineDrawing::drawPolyLine, ShadingType::monochrome,    LineDrawing::DrawPolyLineParams),
      CMD_DRAW_LINE_8X_GROUP(LineDrawing::drawLine,         ShadingType::gouraudShaded, LineDrawing::DrawLineParams),
      CMD_DRAW_POLYLINE_8X_GROUP(LineDrawing::drawPolyLine, ShadingType::gouraudShaded, LineDrawing::DrawPolyLineParams),

      // 0x60: render tiles (rectangles/sprites)
      CMD_DRAW_CUSTOM_TILE_8X_GROUP(TileDrawing::drawCustomTile, TileDrawing::DrawCustomTileParams),
      CMD_DRAW_FIXED_TILE_8X_GROUP(TileDrawing::drawTile,  1u,   TileDrawing::DrawTileParams),
      CMD_DRAW_FIXED_TILE_8X_GROUP(TileDrawing::drawTile,  8u,   TileDrawing::DrawTileParams),
      CMD_DRAW_FIXED_TILE_8X_GROUP(TileDrawing::drawTile, 16u,   TileDrawing::DrawTileParams),

      // 0x80: frame buffer IO
      _P_DUPLICATE_32X_COMMA(CMD_ENTRY(VramTransfer::copyRectangle,  VramTransfer::CopyRectangleParams::size())),
      _P_DUPLICATE_32X_COMMA(CMD_ENTRY(VramTransfer::writeRectangle, VramTransfer::ReadWriteRectangleParams::size())),
      _P_DUPLICATE_32X_COMMA(CMD_ENTRY(VramTransfer::readRectangle,  VramTransfer::ReadWriteRectangleParams::size())),

      // 0xE0: rendering attributes
      NOP_CMD, 
      CMD_ENTRY(setAttributeDrawMode,         TexturePageDrawModeParams::size()), 
      CMD_ENTRY(setAttributeTextureWindow,    TextureWindowParams::size()), 
      CMD_ENTRY(setAttributeDrawingAreaStart, DrawingAreaCoordsParams::size()), 
      CMD_ENTRY(setAttributeDrawingAreaEnd,   DrawingAreaCoordsParams::size()), 
      CMD_ENTRY(setAttributeDrawingOffset,    DrawingOffsetParams::size()), 
      CMD_ENTRY(setAttributeMaskBit,          MaskBitParams::size()), 
      NOP_CMD,
      _P_DUPLICATE_8X_COMMA(NOP_CMD),

      // 0xF0: reserved (not implemented)
      _P_DUPLICATE_16X_COMMA(NOP_CMD)
    };
  }
}

void psx::DisplayController::writeDisplayCommand(uint32_t* gdata, int32_t length) noexcept {
  uint32_t commandId = ParamReader::toGpuCommandId(*gdata);
  if (commandId < psx::gp0::DisplayCommandHandler::tableSize()) {
    //TODO
    
    const psx::gp0::DisplayCommandHandler& commandEntry = psx::gp0::displayCommandTable[commandId];
    if (commandEntry.handler != nullptr)
      commandEntry.handler(*this, gdata);
  }
  //else LOG
}
