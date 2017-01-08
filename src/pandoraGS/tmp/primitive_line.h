#ifndef _PRIMITIVE_LINE_H
#define _PRIMITIVE_LINE_H

#define POLYLINE_MAX_LEN 254   // number of vertices
#define POLYLINE_G_MAX_LEN 127 // number of vertices

#include "primitive_common.h"

namespace Primitive
{
    // -- data types - read raw data -- ----------------------------------------

    /*
    Size Restriction: The maximum distance between two vertices is 1023 horizontally, and 511 vertically.
      Polygons and lines that are exceeding that dimensions are NOT rendered.
      If portions of the polygon/line/rectangle are located outside of the drawing area, then the hardware renders only the portion that is inside of the drawing area.
    */

    // Flat-shaded line
    typedef struct LINE_F2
    {
        rgb24_t color;       // Primitive ID (pad) + line color (RGB)
        vertex_f1_t vertex0; // Vertex coordinates
        vertex_f1_t vertex1; // Vertex coordinates
    } line_f2_t;

    // Gouraud-shaded line
    typedef struct LINE_G2
    {
        vertex_g1_t vertex0; // Primitive ID (pad) + vertex color/coordinates
        vertex_g1_t vertex1; // Vertex color/coordinates
    } line_g2_t;

    // Flat-shaded poly-line
    typedef struct LINE_FP
    {
        rgb24_t color; // Primitive ID (pad) + line color (RGB)
        vertex_f1_t vertices[POLYLINE_MAX_LEN]; // Vertices coordinates - end code: 0x55555555
    } line_fp_t;

    // Gouraud-shaded poly-line
    typedef struct LINE_GP
    {
        vertex_g1_t vertices[POLYLINE_G_MAX_LEN]; // Primitive ID + vertices color/coordinates - end code (can be after the array): 0x55555555
    } line_gp_t;
}

#endif
