/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   primitive_line.h
Description : primitive processing - lines
*******************************************************************************/
#ifndef _PRIMITIVE_LINE_H
#define _PRIMITIVE_LINE_H

#define POLYLINE_MAX_DATA_LEN 255
#define POLYLINE_F_MIN_DATA_LEN 3 // flat-shaded line: at least 1 color + 2 vertices
#define POLYLINE_G_MIN_DATA_LEN 4 // gouraud-shaded line: at least 2 colors + 2 vertices
#define isPolyLineEndCode(DATA) ((DATA&0xF000F000)==0x50005000)
#define isNotPolyLineEndCode(DATA) ((DATA&0xF000F000)!=0x50005000)
#define isFPolyLineEndable(LEN) ((LEN)>=line_fp_t::minSize())
#define isGPolyLineEndable(LEN) ((LEN)>=line_gp_t::minSize() && !((LEN)&1)) // gouraud-shaded: N*(color+vertex) -> even number

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
        // length
        static inline long size() { return 3; }
    } line_f2_t;

    // Gouraud-shaded line
    typedef struct LINE_G2
    {
        vertex_g1_t vertex0; // Primitive ID (pad) + vertex color/coordinates
        vertex_g1_t vertex1; // Vertex color/coordinates
        // length
        static inline long size() { return 4; }
    } line_g2_t;

    // Flat-shaded poly-line
    typedef struct LINE_FP
    {
        rgb24_t color; // Primitive ID (pad) + line color (RGB)
        vertex_f1_t vertex0; // Vertex coordinates
        vertex_f1_t vertex1; // Vertex coordinates
        vertex_f1_t vertex2; // Vertex coordinates OR end code (0x55555555)
        // up to 253 vertices + end code
        static inline long maxSize() { return POLYLINE_MAX_DATA_LEN; }
        static inline long minSize() { return POLYLINE_F_MIN_DATA_LEN; }
    } line_fp_t;

    // Gouraud-shaded poly-line
    typedef struct LINE_GP
    {
        vertex_g1_t vertex0; // Primitive ID (pad) + vertex color/coordinates
        vertex_g1_t vertex1; // Vertex color/coordinates
        vertex_g1_t vertex2; // Vertex color/coordinates OR end code (0x55555555)
        // up to 127 vertices + end code
        static inline long maxSize() { return POLYLINE_MAX_DATA_LEN; }
        static inline long minSize() { return POLYLINE_G_MIN_DATA_LEN; }
    } line_gp_t;


    // Flat-shaded poly-line iterator
    typedef struct LINE_FP_IT
    {
      private:
        int count;
        vertex_f1_t* pVertex;

      public:
        /// <summary>Iterator construction</summary>
        /// <param name="dataSource">Data source reference</param>
        LINE_FP_IT(line_fp_t& dataSource) 
        {
            count = 0;
            pVertex = &dataSource.vertex0;
        }
        /// <summary>Next element</summary>
        /// <returns>Next element exists (or end)</returns>
        inline bool next()
        {
            if (++count < 254) // less than max vertices nb
            {
                ++pVertex;
                return isNotPolyLineEndCode(pVertex->raw); // end code
            }
            return false;
        }
        /// <summary>Get current element</summary>
        /// <returns>Current element reference</returns>
        inline vertex_f1_t& get()
        {
            return *pVertex;
        }
    } line_fp_iterator;


    // Gouraud-shaded poly-line iterator
    typedef struct LINE_GP_IT
    {
      private:
        int count;
        vertex_g1_t* pVertex;

      public:
        /// <summary>Iterator construction</summary>
        /// <param name="dataSource">Data source reference</param>
        LINE_GP_IT(line_gp_t& dataSource) 
        {
            count = 0;
            pVertex = &dataSource.vertex0;
        }
        /// <summary>Next element</summary>
        /// <returns>Next element exists (or end)</returns>
        inline bool next()
        {
            if (++count < 127) // less than max vertices nb
            {
                ++pVertex;
                return isNotPolyLineEndCode(pVertex->color.raw); // end code
            }
            return false;
        }
        /// <summary>Get current element</summary>
        /// <returns>Current element reference</returns>
        inline vertex_g1_t& get()
        {
            return *pVertex;
        }
    } line_gp_iterator;


    // - primitive command functions - -----------------------------------------

    void cmLineF(unsigned char* pData);     // LINE - flat-shaded
    void cmLineG(unsigned char* pData);     // LINE - gouraud-shaded
    void cmPolyLineF(unsigned char* pData); // LINE - poly-line - flat-shaded
    void cmPolyLineG(unsigned char* pData); // LINE - poly-line - gouraud-shaded
}

#endif
