#ifndef _PRIMITIVE_RECT_H
#define _PRIMITIVE_RECT_H

#include "primitive_common.h"

namespace Primitive
{
    // -- data types - read raw data -- ----------------------------------------

    /*
    Size Restriction: The maximum distance between two vertices is 1023 horizontally, and 511 vertically.
      Polygons and lines that are exceeding that dimensions are NOT rendered.
      If portions of the polygon/line/rectangle are located outside of the drawing area, then the hardware renders only the portion that is inside of the drawing area.

    Caution: For untextured graphics, 8bit RGB values of FFh are brightest.
      However, for texture blending, 8bit values of 80h are brightest (values 81h..FFh make textures brighter (up to twice as bright) as than they were originially stored in memory.
      Of course the results can't exceed the maximum brightness: the 5bit values written to the framebuffer are saturated to max 1Fh.

    Texpage Attribute
      Unust be set separately for sprites (unlike polygons): through the texture page command (0xE1 -> primitive_attr.h).

    Clut Attribute (Color Lookup Table)
      Specifies the location of the CLUT data within VRAM (only for 4bit/8bit textures).
    */

    // Tile of any desired size
    typedef struct TILE_F
    {
        rgb24_t color;  // Primitive ID + tile color (RGB)
        rect16_t coord; // Position (top-left) + size (width/height)
    } tile_f_t;
    // 1 x 1 fixed-size tile
    typedef struct TILE_F1
    {
        rgb24_t color; // Primitive ID + tile color (RGB)
        coord16_t pos; // Position (top-left)
    } tile_f1_t;
    // 8 x 8 fixed-size tile
    typedef struct TILE_F8
    {
        rgb24_t color; // Primitive ID + tile color (RGB)
        coord16_t pos; // Position (top-left)
    } tile_f8_t;
    // 16 x 16 fixed-size tile
    typedef struct TILE_F16
    {
        rgb24_t color; // Primitive ID + tile color (RGB)
        coord16_t pos; // Position (top-left)
    } tile_f16_t;


    // Sprite of any desired size
    typedef struct SPRT_F
    {
        rgb24_t color;    // Primitive ID + tile color (RGB)
        coord16_t pos;    // Position (top-left)
        coord8_t texture; // Sprite texture coordinates + CLUT
        coord16_t size;   // Size (width/height)
    } sprite_f_t;
    // 1 x 1 fixed size texture-mapped sprite
    typedef struct SPRT_F1
    {
        rgb24_t color;    // Primitive ID + tile color (RGB)
        coord16_t pos;    // Position (top-left)
        coord8_t texture; // Sprite texture coordinates + CLUT
    } sprite_f1_t;
    // 8 x 8 fixed size texture-mapped sprite
    typedef struct SPRT_F8
    {
        rgb24_t color;    // Primitive ID + tile color (RGB)
        coord16_t pos;    // Position (top-left)
        coord8_t texture; // Sprite texture coordinates + CLUT
    } sprite_f8_t;
    // 16 x 16 fixed size texture-mapped sprite
    typedef struct SPRT_F16
    {
        rgb24_t color;    // Primitive ID + tile color (RGB)
        coord16_t pos;    // Position (top-left)
        coord8_t texture; // Sprite texture coordinates + CLUT
    } sprite_f16_t;
}

#endif
