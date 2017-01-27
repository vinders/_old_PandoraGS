/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : drawing primitive factory
*******************************************************************************/
#include "primitive_factory.h"
#include "line_primitive.h"
#include "poly_primitive.h"
#include "rect_primitive.h"
#include "image_transfer.h"
#include "attribute.h"
using namespace command::primitive;

command::memory::VideoMemory* PrimitiveFactory::s_pVramAccess = NULL;         ///< VRAM access used by primitives
command::FrameBufferSettings* PrimitiveFactory::s_pDrawSettingsAccess = NULL; ///< Frame buffer settings used by primitives

// multi-commands definition macros
#define CMDx4(cmd,size)  {cmd,size},{cmd,size},{cmd,size},{cmd,size}
#define CMDx8(cmd,size)  CMDx4(cmd,size),CMDx4(cmd,size)
#define CMDx16(cmd,size) CMDx8(cmd,size),CMDx8(cmd,size)
#define CMDx24(cmd,size) CMDx8(cmd,size),CMDx8(cmd,size),CMDx8(cmd,size)
#define CMDx32(cmd,size) CMDx16(cmd,size),CMDx16(cmd,size)


/// @brief Primitive index table
const index_row_t c_pPrimitiveIndex[PRIMITIVE_NUMBER] = 
{
    // GENERAL : 00 - 02
    { PRIMITIVE_NOP, 0L }, { cache_clear_t::process, cache_clear_t::size() }, { fill_area_t::process, fill_area_t::size() },
    // not implemented : 03 - 1E
    CMDx24(PRIMITIVE_NI, 0), CMDx4(PRIMITIVE_NI, 0),
    // IRQ : 1F
    { attr_irqflag_t::process, attr_irqflag_t::size() },

    // POLY - flat-shaded triangle : 20 - 27
    CMDx4(poly_f3_t::process, poly_f3_t::size()),   CMDx4(poly_ft3_t::process, poly_ft3_t::size()),
    // POLY - flat-shaded quad : 28 - 2F
    CMDx4(poly_f4_t::process, poly_f4_t::size()),   CMDx4(poly_ft4_t::process, poly_ft4_t::size()),
    // POLY - gouraud-shaded triangle : 30 - 37
    CMDx4(poly_g3_t::process, poly_g3_t::size()),   CMDx4(poly_gt3_t::process, poly_gt3_t::size()),
    // POLY - gouraud-shaded quad : 38 - 3F
    CMDx4(poly_g4_t::process, poly_g4_t::size()),   CMDx4(poly_gt4_t::process, poly_gt4_t::size()),

    // LINE - flat-shaded : 40 - 4F
    CMDx8(line_f2_t::process, line_f2_t::size()),   CMDx8(line_fp_t::process, line_fp_t::maxSize()),
    // LINE - gouraud-shaded : 50 - 5F
    CMDx8(line_g2_t::process, line_g2_t::size()),   CMDx8(line_gp_t::process, line_gp_t::maxSize()),

    // RECT - custom-sized tile/sprite : 60 - 67
    CMDx4(tile_f_t::process, tile_f_t::size()),     CMDx4(sprite_f_t::process, sprite_f_t::size()),
    // RECT - 1x1 tile/sprite : 68 - 6F
    CMDx4(tile_f1_t::process, tile_f1_t::size()),   CMDx4(sprite_f1_t::process, sprite_f1_t::size()),
    // RECT - 8x8 tile/sprite : 70 - 77
    CMDx4(tile_f8_t::process, tile_f8_t::size()),   CMDx4(sprite_f8_t::process, sprite_f8_t::size()),
    // RECT - 16x16 tile/sprite : 78 - 7F
    CMDx4(tile_f16_t::process, tile_f16_t::size()), CMDx4(sprite_f16_t::process, sprite_f16_t::size()),

    // IMAGE - move : 80 - 9F
    CMDx32(img_move_t::process, img_move_t::size()),
    // IMAGE - load : A0 - BF
    CMDx32(img_load_t::process, img_load_t::size()),
    // IMAGE - store : C0 - DF
    CMDx32(img_store_t::process, img_store_t::size()),

    // ATTRIBUTES : E0 - E7
    { PRIMITIVE_NI, 0 },                                   { attr_texpage_t::process, attr_texpage_t::size() },
    { attr_texwin_t::process, attr_texwin_t::size() },     { attr_drawarea_t::process, attr_drawarea_t::size() },
    { attr_drawarea_t::process, attr_drawarea_t::size() }, { attr_drawoffset_t::process, attr_drawoffset_t::size() },
    { attr_stpmask_t::process, attr_stpmask_t::size() },   { PRIMITIVE_NI, 0 },

    // not implemented : E8 - FF
    CMDx24(PRIMITIVE_NI, 0)
};
