/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   primitive_rect.h
Description : primitive processing - rectangles/sprites
*******************************************************************************/
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
        // length
        static inline long size() { return 3; }
    } tile_f_t;
    // 1 x 1 fixed-size tile
    typedef struct TILE_F1
    {
        rgb24_t color; // Primitive ID + tile color (RGB)
        coord16_t pos; // Position (top-left)
        // length
        static inline long size() { return 2; }
    } tile_f1_t;
    // 8 x 8 fixed-size tile
    typedef struct TILE_F8
    {
        rgb24_t color; // Primitive ID + tile color (RGB)
        coord16_t pos; // Position (top-left)
        // length
        static inline long size() { return 2; }
    } tile_f8_t;
    // 16 x 16 fixed-size tile
    typedef struct TILE_F16
    {
        rgb24_t color; // Primitive ID + tile color (RGB)
        coord16_t pos; // Position (top-left)
        // length
        static inline long size() { return 2; }
    } tile_f16_t;


    // Sprite of any desired size
    typedef struct SPRT_F
    {
        rgb24_t color;    // Primitive ID + tile color (RGB)
        coord16_t pos;    // Position (top-left)
        coord8_t texture; // Sprite texture coordinates + CLUT
        coord16_t size;   // Size (width/height)
        // length
        static inline long size() { return 4; }
    } sprite_f_t;
    // 1 x 1 fixed size texture-mapped sprite
    typedef struct SPRT_F1
    {
        rgb24_t color;    // Primitive ID + tile color (RGB)
        coord16_t pos;    // Position (top-left)
        coord8_t texture; // Sprite texture coordinates + CLUT
        // length
        static inline long size() { return 3; }
    } sprite_f1_t;
    // 8 x 8 fixed size texture-mapped sprite
    typedef struct SPRT_F8
    {
        rgb24_t color;    // Primitive ID + tile color (RGB)
        coord16_t pos;    // Position (top-left)
        coord8_t texture; // Sprite texture coordinates + CLUT
        // length
        static inline long size() { return 3; }
    } sprite_f8_t;
    // 16 x 16 fixed size texture-mapped sprite
    typedef struct SPRT_F16
    {
        rgb24_t color;    // Primitive ID + tile color (RGB)
        coord16_t pos;    // Position (top-left)
        coord8_t texture; // Sprite texture coordinates + CLUT
        // length
        static inline long size() { return 3; }
    } sprite_f16_t;


    // - primitive command functions - -----------------------------------------

    void cmTile(unsigned char* pData);         // RECT - tile custom
    void cmTile1(unsigned char* pData);        // RECT - tile 1x1
    void cmTile8(unsigned char* pData);        // RECT - tile 8x8
    void cmTile16(unsigned char* pData);       // RECT - tile 16x16
    void cmSprite(unsigned char* pData);       // RECT - sprite custom
    void cmSprite1(unsigned char* pData);      // RECT - sprite 1x1
    void cmSprite8(unsigned char* pData);      // RECT - sprite 8x8
    void cmSprite16(unsigned char* pData);     // RECT - sprite 16x16
}

#endif
