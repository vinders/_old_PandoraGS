#ifndef _PRIMITIVE_COMMON_H
#define _PRIMITIVE_COMMON_H

#include <cstdint>
#include <cstddef>
#include "frame_buffer.h"

namespace Primitive
{
    // -- data types - read raw data -- ----------------------------------------

    // 8-bit coordinates + texture attribut (MiscYyXx)
    typedef struct COORD8 
    {
        unsigned long raw;
        inline uint8_t x() // Xx
        {
            return (raw & 0x0FFuL);
        }
        inline uint8_t y() // Yy
        {
            return ((unsigned long)(raw >> 8) & 0x0FFuL);
        }

        inline uint16_t clutX() { return ((unsigned long)(raw >> 12) & 0x3F0uL); }  // CLUT - X coordinate: 0, 16, ...
        inline uint16_t clutY() { return ((unsigned long)(raw >> 22) & 0x1FFuL); }  // CLUT - Y coordinate: 0-511
        inline uint16_t tpageX() { return ((unsigned long)(raw >> 10) & 0x3C0uL); } // Texpage - X base: 0, 64, ...
        inline uint16_t tpageY() { return ((unsigned long)(raw >> 12) & 0x100uL); } // Texpage - Y base: 0 or 256
        inline bool isTextureDisabled() { return ((raw & 0x8000000uL) != 0uL); }    // Texpage - Texture mode - normal (0) or disable (1) if texture disabling allowed (GP1(09h).bit0 == 1)
        inline colordepth_t colorDepth() // Texpage - Y base: 0 or 256
        { 
            uint8_t val = ((unsigned long)(raw >> 23) & 0x3uL);
            return (colordepth_t)val;
        }    
        inline stp_t semiTransparency()  // Texpage - Semi-transparency
        { 
            uint8_t val = ((unsigned long)(raw >> 21) & 0x3uL); 
            return (stp_t)val; 
        } 
    } coord8_t;


    // 16-bit coordinates (YyyyXxxx)
    typedef struct COORD16 
    {
        unsigned long raw;
        inline uint16_t x() // Xx-xx
        {
            return (raw & 0x0FFFFuL);
        }
        inline uint16_t y() // Yy-yy
        {
            return ((unsigned long)(raw >> 16) & 0x0FFFFuL);
        }
    } coord16_t;

    // 2x 16-bit rectangular area (YyyyXxxx HhhhWwww)
    typedef struct RECT16 
    {
        coord16_t pos;  // top-left X / Y coordinates
        coord16_t size; // width / height
    } rect16_t;


    // 16-bit RGB color (0000MBGR)
    typedef struct RGB16
    {
        unsigned long raw;
        inline bool mask() // m
        {
            return ((raw >> 15) != 0uL);
        }
        inline int16_t rgb16() // mBbbbbGg-gggRrrrr
        {
            return (raw & 0x0FFFFuL);
        }
        inline int32_t rgb24() // 00-Bb-Gg-Rr
        {
            return (((raw << 9) & 0x0FF0000) | ((raw << 6) & 0x0FF00) | ((raw << 3) & 0x0FF));
        }
    } rgb16_t;

    // 24-bit RGB color (PpBbGgRr)
    typedef struct RGB24 
    {
        unsigned long raw;
        inline int8_t r() // Rr
        {
            return (raw & 0x0FFuL);
        }
        inline int8_t g() // Gg
        {
            return ((unsigned long)(raw >> 8) & 0x0FFuL);
        }
        inline int8_t b() // Bb
        {
            return ((unsigned long)(raw >> 16) & 0x0FFuL);
        }
        inline int8_t pad() // Pp
        {
            return ((unsigned long)(raw >> 24) & 0x0FFuL);
        }
        inline int16_t rgb16() // mBbbbbGg-gggRrrrr
        {
            return (0x8000uL | (unsigned long)(((raw >> 9) & 0x7C00uL) | ((raw >> 6) & 0x03E0uL) | (raw >> 3)));
        }
        inline int32_t rgb24() // 00-Bb-Gg-Rr
        {
            return (raw & 0x0FFFFFFuL);
        }
    } rgb24_t;
 

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


    
    /*
    // TIM format image data header
    struct TIM_IMAGE
    {
        uint32_t mode;   // Pixel mode
        RECT16 *crect;   // Pointer to destination rectangle in VRAM for CLUT data
        uint32_t *caddr; // Pointer to address of CLUT data in main memory
        RECT16 *prect;   // Pointer to destination rectangle in VRAM for texture image data
        uint32_t *paddr; // Pointer to address of texture image data in main memory
    };

    // Display environment
    struct DISPENV
    {
        RECT16 disp;   // Display area within frame buffer.Width: 256, 320, 384, 512, or 640.
        // Height : 240 or 480.
        RECT16 screen; // Output screen display area.It is calculated without regard to the value of disp,
        // using the standard monitor screen upper left-hand point(0,0) and lower right-hand point(256,240).
        uint8_t isinter;    // Interlace mode flag.  0: non-interlace;  1: interlace
        uint8_t isrgb24;    // 24-bit mode flag.  0: 16-bit mode;  1: 24 - bit mode
        uint8_t pad0, pad1; // Reserved by system
    };
    */
}




#endif
