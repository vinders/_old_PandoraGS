#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <cstdint>
#include "video_memory.hpp"
#include "geometry.hpp"

// datatypes
enum stp_t : uint8_t // semi-transparency (B=back ; F=forward)
{
    Stp_mean = 0, // B/2 + F/2
    Stp_add = 1,  // B + F
    Stp_sub = 2,  // B - F
    Stp_addQuarter = 3  // B + F/4
};
enum colordepth_t : uint8_t // semi-transparency (B=back ; F=forward)
{
    Colordepth_4 = 0, // 4-bit
    Colordepth_8 = 1,  // 8-bit
    Colordepth_15 = 2,  // 15-bit
    Colordepth_res = 3  // reserved
};

// Emulated GPU framebuffer
class FrameBuffer
{
private:
    static VideoMemory s_vram;

    static rect_t s_drawArea;
    static point_t s_drawOffset;
    static bool s_isMaskBitForced;
    static bool s_isMaskBitChecked;

    // index des texture pages, CLUT, texture window, ...
    // cache de textures


public:
    //...
};

#endif
