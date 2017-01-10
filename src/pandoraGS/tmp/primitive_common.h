/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   primitive_common.h
Description : primitive processing - common tools
*******************************************************************************/
#ifndef _PRIMITIVE_COMMON_H
#define _PRIMITIVE_COMMON_H

#include "frame_buffer.h"

namespace Primitive
{
    // -- data types - coordinates -- ------------------------------------------

    // 8-bit coordinates + texture attribute (MiscYyXx)
    typedef struct COORD8 
    {
        unsigned long raw;
        inline unsigned long x() // Xx
        {
            return (raw & 0x0FFuL);
        }
        inline unsigned long y() // Yy
        {
            return ((raw >> 8) & 0x0FFuL);
        }
        // Misc - CLUT
        inline unsigned long clutX()    { return ((raw >> 12) & 0x3F0uL); } // X coordinate: 0, 16, ...
        inline unsigned long clutY()    { return ((raw >> 22) & 0x1FFuL); } // Y coordinate: 0-511
        // Misc - Texture page
        inline unsigned long texpageX() { return ((raw >> 10) & 0x3C0uL); }      // X base: 0, 64, ...
        inline unsigned long texpageY() { return ((raw >> 12) & 0x100uL); }      // Y base: 0 or 256
        inline bool isTextureDisabled() { return ((raw & 0x8000000uL) != 0uL); } // Texture mode - normal (0) or disable (1) if texture disabling allowed (GP1(09h).bit0==1)
        inline colordepth_t colorDepth() // Color depth mode (4-bit, 8-bit, 15-bit)
        { 
            unsigned long val = ((raw >> 23) & 0x3uL);
            return (colordepth_t)val;
        }    
        inline stp_t semiTransparency()  // Semi-transparency mode
        { 
            unsigned long val = ((raw >> 21) & 0x3uL);
            return (stp_t)val; 
        } 
    } coord8_t;

    // 16-bit coordinates (YyyyXxxx)
    typedef struct COORD16 
    {
        unsigned long raw;
        inline unsigned long x() // Xx-xx
        {
            return (raw & 0x0FFFFuL);
        }
        inline unsigned long y() // Yy-yy
        {
            return ((raw >> 16) & 0x0FFFFuL);
        }
    } coord16_t;

    // 2x 16-bit rectangular area (YyyyXxxx HhhhWwww)
    typedef struct RECT16 
    {
        coord16_t pos;  // top-left X / Y coordinates
        coord16_t size; // width / height
    } rect16_t;


    // -- data types - colors -- -----------------------------------------------

    // 16-bit RGB color (0000MBGR)
    typedef struct RGB16
    {
        unsigned long raw;
        inline bool isMask() // m0000000
        {
            return ((raw >> 15) != 0uL);
        }
        inline unsigned long rgb16() // mBbbbbGg-gggRrrrr
        {
            return (raw & 0x0FFFFuL);
        }
        inline unsigned long rgb24() // 00-Bb-Gg-Rr
        {
            return (((raw << 9) & 0x0FF0000) | ((raw << 6) & 0x0FF00) | ((raw << 3) & 0x0FF));
        }
    } rgb16_t;

    // 24-bit RGB color (PpBbGgRr)
    typedef struct RGB24 
    {
        unsigned long raw;
        inline unsigned long r() // Rr
        {
            return (raw & 0x0FFuL);
        }
        inline unsigned long g() // Gg
        {
            return ((raw >> 8) & 0x0FFuL);
        }
        inline unsigned long b() // Bb
        {
            return ((raw >> 16) & 0x0FFuL);
        }
        inline unsigned long pad() // Pp
        {
            return ((raw >> 24) & 0x0FFuL);
        }
        inline unsigned long rgb16() // mBbbbbGg-gggRrrrr
        {
            return (0x8000uL | (((raw >> 9) & 0x7C00uL) | ((raw >> 6) & 0x03E0uL) | (raw >> 3)));
        }
        inline unsigned long rgb24() // 00-Bb-Gg-Rr
        {
            return (raw & 0x0FFFFFFuL);
        }
    } rgb24_t;
 

    // -- data types - vertices -- ---------------------------------------------

    // Flat-shaded vertex (YyyyXxxx)
    typedef coord16_t vertex_f1_t;
    // Textured flat-shaded vertex (YyyyXxxx MiscYyXx)
    typedef struct VERTEX_FT1
    {
        coord16_t coord;  // Vertex coordinates
        coord8_t texture; // Texture coordinates + misc texture information
    } vertex_ft1_t;

    // Gouraud-shaded vertex (00BbGgRr YyyyXxxx)
    typedef struct VERTEX_G1
    {
        rgb24_t color;    // Vertex color (RGB)
        coord16_t coord;  // Vertex coordinates
    } vertex_g1_t;
    // Textured gouraud-shaded vertex (00BbGgRr YyyyXxxx MiscYyXx)
    typedef struct VERTEX_GT1
    {
        rgb24_t color;    // Vertex color (RGB)
        coord16_t coord;  // Vertex coordinates
        coord8_t texture; // Texture coordinates + misc texture information
    } vertex_gt1_t;
}

#endif
