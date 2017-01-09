#ifndef _PRIMITIVE_POLY_H
#define _PRIMITIVE_POLY_H

#define QUAD_TRIANGLE0_INDEX vertex
#define QUAD_TRIANGLE1_INDEX 3-vertex

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
      Bits 9 (dithering) and 10 (drawing allowed) must be specified through the texture page command (0xE1 -> primitive_attr.h).

    Clut Attribute (Color Lookup Table)
      Specifies the location of the CLUT data within VRAM (only for 4bit/8bit textures).
    */

    // Flat-shaded triangle
    typedef struct POLY_F3
    {
        rgb24_t color;           // Primitive ID (pad) + triangle color (RGB)
        vertex_f1_t vertices[3]; // Vertex coordinates
    } poly_f3_t;

    // Flat-shaded quad
    typedef struct POLY_F4
    {
        rgb24_t color;           // Primitive ID (pad) + quad color (RGB)
        vertex_f1_t vertices[4]; // Vertex coordinates (top-left)
        // sub-triangles
        inline vertex_f1_t triangle0(int vertex) { return vertices[QUAD_TRIANGLE0_INDEX]; }
        inline vertex_f1_t triangle1(int vertex) { return vertices[QUAD_TRIANGLE1_INDEX]; }
    } poly_f4_t;

    // Flat-shaded texture-mapped triangle
    typedef struct POLY_FT3
    {
        rgb24_t color;            // Primitive ID (pad) + triangle color (RGB)
        vertex_ft1_t vertices[3]; // Vertex coordinates/texture (coord + CLUT/texpage)
        // texture information
        inline unsigned long clutX()    { return vertices[0].texture.clutX(); }
        inline unsigned long clutY()    { return vertices[0].texture.clutY(); }
        inline unsigned long texpageX() { return vertices[1].texture.texpageX(); }
        inline unsigned long texpageY() { return vertices[1].texture.texpageY(); }
    } poly_ft3_t;

    // Flat-shaded texture-mapped quad
    typedef struct POLY_FT4
    {
        rgb24_t color;            // Primitive ID (pad) + quad color (RGB)
        vertex_ft1_t vertices[4]; // Vertex coordinates/texture (coord + CLUT/texpage)
        // sub-triangles
        inline vertex_ft1_t triangle0(int vertex) { return vertices[QUAD_TRIANGLE0_INDEX]; }
        inline vertex_ft1_t triangle1(int vertex) { return vertices[QUAD_TRIANGLE1_INDEX]; }
        // texture information
        inline unsigned long clutX()    { return vertices[0].texture.clutX(); }
        inline unsigned long clutY()    { return vertices[0].texture.clutY(); }
        inline unsigned long texpageX() { return vertices[1].texture.texpageX(); }
        inline unsigned long texpageY() { return vertices[1].texture.texpageY(); }
    } poly_ft4_t;


    // Gouraud-shaded triangle
    typedef struct POLY_G3
    {
        vertex_g1_t vertices[3]; // Primitive ID + vertex color/coordinates
    } poly_g3_t;

    // Gouraud-shaded quad
    typedef struct POLY_G4
    {
        vertex_g1_t vertices[4]; // Primitive ID + vertex color/coordinates
        // sub-triangles
        inline vertex_g1_t triangle0(int vertex) { return vertices[QUAD_TRIANGLE0_INDEX]; }
        inline vertex_g1_t triangle1(int vertex) { return vertices[QUAD_TRIANGLE1_INDEX]; }
    } poly_g4_t;

    // Gouraud-shaded texture-mapped triangle
    typedef struct POLY_GT3
    {
        vertex_gt1_t vertices[3]; // Primitive ID + vertex color/coordinates/texture (coord + CLUT/texpage)
        // texture information
        inline unsigned long clutX()    { return vertices[0].texture.clutX(); }
        inline unsigned long clutY()    { return vertices[0].texture.clutY(); }
        inline unsigned long texpageX() { return vertices[1].texture.texpageX(); }
        inline unsigned long texpageY() { return vertices[1].texture.texpageY(); }
    } poly_gt3_t;

    // Gouraud-shaded texture-mapped quad
    typedef struct POLY_GT4
    {
        vertex_gt1_t vertices[4]; // Primitive ID + vertex color/coordinates/texture (coord + CLUT/texpage)
        // sub-triangles
        inline vertex_gt1_t triangle0(int vertex) { return vertices[QUAD_TRIANGLE0_INDEX]; }
        inline vertex_gt1_t triangle1(int vertex) { return vertices[QUAD_TRIANGLE1_INDEX]; }
        // texture information
        inline unsigned long clutX()    { return vertices[0].texture.clutX(); }
        inline unsigned long clutY()    { return vertices[0].texture.clutY(); }
        inline unsigned long texpageX() { return vertices[1].texture.texpageX(); }
        inline unsigned long texpageY() { return vertices[1].texture.texpageY(); }
    } poly_gt4_t;
}

#endif
