/*******************************************************************************
PANDORAGS project - PS1 GPU driver test app
------------------------------------------------------------------------
File name :   primitive.cpp
Description : test app - flow/unit tests -- primitive list/creation functions
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
using namespace std;
#include "primitive.h"

#define MAX_PRIMITIVE_ID 0xE6

// primitive table ("length"+"data")
unsigned long primTestTable[MAX_PRIMITIVE_ID][16] =
{
    // Cm = command
    // BbGgRr = color code
    // YtopXlft = top-left coords (Y/X)
    // YhgtXwid = size (height/width)
    // YsrcXsrc = source coords (Y/X)
    // YdstXdst = destination coords (Y/X)
    // YvtxXvtx = vertex XY coords (Y/X)
    // YcXc = texture XY coords (Y/X)
    // Clut = color lookup table (palette)
    // Txpg = texture page
    { 0x1, 0x01000000 },                         // 01 clear txtr cache - 1x32 - Cm000000
    { 0x3, 0x02177700,0x00200010,0x01000100 }, // 02 fill blank rect  - 3x32 - CmBbGgRr YtopXlft YhgtXwid 
    {0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}, // unused (03 to 1F)
    // POLY
    { 0x4, 0x200000AA,0x00200080,0x01100010,0x01000200 }, // 20 3pt mono opaque     - 4x32
    { 0x4, 0x217700AA,0x00200080,0x01100010,0x01000200 }, // 21 3pt mono opaque n-b - 4x32
    { 0x4, 0x22AA4400,0x00200080,0x01100010,0x01000200 }, // 22 3pt mono transp     - 4x32
    { 0x4, 0x2344AA00,0x00200080,0x01100010,0x01000200 }, // 23 3pt mono transp n-b - 4x32
        // CmBbGgRr YvtxXvtx YvtxXvtx YvtxXvtx
    { 0x7, 0x240000AA,0x01100010, 0 ,0x01000200, 0 ,0x01800030, 0 }, // 24 3pt txtr opaque bld - 7x32
    { 0x7, 0x257700AA,0x01100010, 0 ,0x01000200, 0 ,0x01800030, 0 }, // 25 3pt txtr opaque raw - 7x32
    { 0x7, 0x26AA4400,0x01100010, 0 ,0x01000200, 0 ,0x01800030, 0 }, // 26 3pt txtr transp bld - 7x32
    { 0x7, 0x2744AA00,0x01100010, 0 ,0x01000200, 0 ,0x01800030, 0 }, // 27 3pt txtr transp raw - 7x32
        // CmBbGgRr YvtxXvtx ClutYcXc YvtxXvtx TxpgYcXc YvtxXvtx 0000YcXc
    { 0x5, 0x280000AA,0x01000200,0x01100300,0x02000220,0x02100310 }, // 28 4pt mono opaque     - 5x32
    { 0x5, 0x297700AA,0x01000200,0x01100300,0x02000220,0x02100310 }, // 29 4pt mono opaque n-b - 5x32
    { 0x5, 0x2AAA4400,0x01000200,0x01100300,0x02000220,0x02100310 }, // 2A 4pt mono transp     - 5x32
    { 0x5, 0x2B44AA00,0x01000200,0x01100300,0x02000220,0x02100310 }, // 2B 4pt mono transp n-b - 5x32
        // CmBbGgRr YvtxXvtx YvtxXvtx YvtxXvtx YvtxXvtx
    { 0x9, 0x2C0000AA,0x02000220,0x02100310,0x03000280,0x03100340 }, // 2C 4pt txtr opaque bld - 9x32
    { 0x9, 0x2D7700AA,0x02000220,0x02100310,0x03000280,0x03100340 }, // 2D 4pt txtr opaque raw - 9x32
    { 0x9, 0x2EAA4400,0x02000220,0x02100310,0x03000280,0x03100340 }, // 2E 4pt txtr transp bld - 9x32
    { 0x9, 0x2F44AA00,0x02000220,0x02100310,0x03000280,0x03100340 }, // 2F 4pt txtr transp raw - 9x32
        // CmBbGgRr YvtxXvtx ClutYcXc YvtxXvtx TxpgYcXc YvtxXvtx 0000YcXc YvtxXvtx 0000YcXc
    { 0x6, 0x300000AA, 0, 0, 0, 0, 0 }, // 30 3pt shad opaque     - 6x32
    { 0x6, 0x317700AA, 0, 0, 0, 0, 0 }, // 31 3pt shad opaque n-b - 6x32
    { 0x6, 0x32AA4400, 0, 0, 0, 0, 0 }, // 32 3pt shad transp     - 6x32
    { 0x6, 0x3344AA00, 0, 0, 0, 0, 0 }, // 33 3pt shad transp n-b - 6x32
        // CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx 00BbGgRr YvtxXvtx
    { 0x9, 0x340000AA, 0, 0, 0, 0, 0, 0, 0, 0 }, // 34 3pt shtx opaque bld - 9x32
    { 0x9, 0x357700AA, 0, 0, 0, 0, 0, 0, 0, 0 }, // 35 3pt shtx opaque raw - 9x32
    { 0x9, 0x36AA4400, 0, 0, 0, 0, 0, 0, 0, 0 }, // 36 3pt shtx transp bld - 9x32
    { 0x9, 0x3744AA00, 0, 0, 0, 0, 0, 0, 0, 0 }, // 37 3pt shtx transp raw - 9x32
        //CmBbGgRr YvtxXvtx ClutYcXc 00BbGgRr YvtxXvtx TxpgYcXc 00BbGgRr YvtxXvtx 0000YcXc
    { 0x8, 0x380000AA, 0, 0, 0, 0, 0, 0, 0 }, // 38 4pt shad opaque     - 8x32
    { 0x8, 0x397700AA, 0, 0, 0, 0, 0, 0, 0 }, // 39 4pt shad opaque n-b - 8x32
    { 0x8, 0x3AAA4400, 0, 0, 0, 0, 0, 0, 0 }, // 3A 4pt shad transp     - 8x32
    { 0x8, 0x3B44AA00, 0, 0, 0, 0, 0, 0, 0 }, // 3B 4pt shad transp n-b - 8x32
        // CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx 00BbGgRr YvtxXvtx 00BbGgRr YvtxXvtx
    { 0xC, 0x3C0000AA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 3C 4pt shtx opaque bld - 12x32
    { 0xC, 0x3D7700AA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 3D 4pt shtx opaque raw - 12x32
    { 0xC, 0x3EAA4400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 3E 4pt shtx transp bld - 12x32
    { 0xC, 0x3F44AA00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 3F 4pt shtx transp raw - 12x32
        // CmBbGgRr YvtxXvtx ClutYcXc 00BbGgRr YvtxXvtx TxpgYcXc 00BbGgRr YvtxXvtx 0000YcXc 00BbGgRr YvtxXvtx 0000YcXc
    // LINE
    { 0x3, 0x404444CC,0x01100010,0x02000220 }, // 40 line mono opaque     - 3x32
    { 0x3, 0x41AA44CC,0x01100010,0x02000220 }, // 41 line mono opaque n-b - 3x32
    { 0x3, 0x42CCAA44,0x01100010,0x02000220 }, // 42 line mono transp     - 3x32
    { 0x3, 0x43AACC44,0x01100010,0x02000220 }, // 43 line mono transp n-b - 3x32
    { 0x3, 0x444444CC,0x01100010,0x02000220 }, // 44 line mono opaque     - 3x32
    { 0x3, 0x45AA44CC,0x01100010,0x02000220 }, // 45 line mono opaque n-b - 3x32
    { 0x3, 0x46CCAA44,0x01100010,0x02000220 }, // 46 line mono transp     - 3x32
    { 0x3, 0x47AACC44,0x01100010,0x02000220 }, // 47 line mono transp n-b - 3x32
        // CmBbGgRr YvtxXvtx YvtxXvtx
    { 0xFE, 0x484444CC,0x03100010,0x02100110,0x02000220,0x55555555 }, // 48 plin mono opaque     - var.x32
    { 0xFE, 0x49AA44CC,0x03100010,0x02100110,0x02000220,0x55555555 }, // 49 plin mono opaque n-b - var.x32
    { 0xFE, 0x4ACCAA44,0x03100010,0x02100110,0x02000220,0x55555555 }, // 4A plin mono transp     - var.x32
    { 0xFE, 0x4BAACC44,0x03100010,0x02100110,0x02000220,0x55555555 }, // 4B plin mono transp n-b - var.x32
    { 0xFE, 0x4C4444CC,0x03100010,0x02100110,0x02000220,0x55555555 }, // 4C plin mono opaque bld - var.x32
    { 0xFE, 0x4DAA44CC,0x03100010,0x02100110,0x02000220,0x55555555 }, // 4D plin mono opaque raw - var.x32
    { 0xFE, 0x4ECCAA44,0x03100010,0x02100110,0x02000220,0x55555555 }, // 4E plin mono transp bld - var.x32
    { 0xFE, 0x4FAACC44,0x03100010,0x02100110,0x02000220,0x55555555 }, // 4F plin mono transp raw - var.x32
        // CmBbGgRr YvtxXvtx YvtxXvtx [...] 55555555
    { 0x4, 0x504444CC, 0, 0, 0 }, // 50 line shad opaque     - 4x32
    { 0x4, 0x51AA44CC, 0, 0, 0 }, // 51 line shad opaque n-b - 4x32
    { 0x4, 0x52CCAA44, 0, 0, 0 }, // 52 line shad transp     - 4x32
    { 0x4, 0x53AACC44, 0, 0, 0 }, // 53 line shad transp n-b - 4x32
    { 0x4, 0x544444CC, 0, 0, 0 }, // 54 line shad opaque     - 4x32
    { 0x4, 0x55AA44CC, 0, 0, 0 }, // 55 line shad opaque n-b - 4x32
    { 0x4, 0x56CCAA44, 0, 0, 0 }, // 56 line shad transp     - 4x32
    { 0x4, 0x57AACC44, 0, 0, 0 }, // 57 line shad transp n-b - 4x32
        // CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx
    { 0xFF, 0x584444CC, 0, 0, 0, 0, 0, 0x55555555 }, // 58 plin shad opaque     - var.x32
    { 0xFF, 0x59AA44CC, 0, 0, 0, 0, 0, 0x55555555 }, // 59 plin shad opaque n-b - var.x32
    { 0xFF, 0x5ACCAA44, 0, 0, 0, 0, 0, 0x55555555 }, // 5A plin shad transp     - var.x32
    { 0xFF, 0x5BAACC44, 0, 0, 0, 0, 0, 0x55555555 }, // 5B plin shad transp n-b - var.x32
    { 0xFF, 0x5C4444CC, 0, 0, 0, 0, 0, 0x55555555 }, // 5C plin shad opaque bld - var.x32
    { 0xFF, 0x5DAA44CC, 0, 0, 0, 0, 0, 0x55555555 }, // 5D plin shad opaque raw - var.x32
    { 0xFF, 0x5ECCAA44, 0, 0, 0, 0, 0, 0x55555555 }, // 5E plin shad transp bld - var.x32
    { 0xFF, 0x5FAACC44,0,0,0,0,0,0x55555555 }, // 5F plin shad transp raw - var.x32
        // CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx [...] 55555555
    // RECT
    { 0x3, 0x60AAAAAA, 0, 0 }, // 60 var mono opaque     - 3x32
    { 0x3, 0x61888888, 0, 0 }, // 61 var mono opaque n-b - 3x32
    { 0x3, 0x62AA8844, 0, 0 }, // 62 var mono transp     - 3x32
    { 0x3, 0x63AA8844, 0, 0 }, // 63 var mono transp n-b - 3x32
        // CmBbGgRr YvtxXvtx YhgtXwid
    { 0x4, 0x64AAAAAA, 0, 0, 0 }, // 64 var txtr opaque bld - 4x32
    { 0x4, 0x65888888, 0, 0, 0 }, // 65 var txtr opaque raw - 4x32
    { 0x4, 0x66CCAA77, 0, 0, 0 }, // 66 var txtr transp bld - 4x32
    { 0x4, 0x67AA8844, 0, 0, 0 }, // 67 var txtr transp raw - 4x32
        // CmBbGgRr YvtxXvtx ClutYcXc YhgtXwid
    { 0x2, 0x68AAAAAA, 0 }, // 68 1x1 mono opaque     - 2x32
    { 0x2, 0x69888888, 0 }, // 69 1x1 mono opaque n-b - 2x32
    { 0x2, 0x6ACCAA77, 0 }, // 6A 1x1 mono transp     - 2x32
    { 0x2, 0x6BAA8844, 0 }, // 6B 1x1 mono transp n-b - 2x32
        // CmBbGgRr YvtxXvtx
    { 0x3, 0x6CAAAAAA, 0, 0 }, // 6C 1x1 txtr opaque bld - 3x32
    { 0x3, 0x6D888888, 0, 0 }, // 6D 1x1 txtr opaque raw - 3x32
    { 0x3, 0x6ECCAA77, 0, 0 }, // 6E 1x1 txtr transp bld - 3x32
    { 0x3, 0x6FAA8844, 0, 0 }, // 6F 1x1 txtr transp raw - 3x32
        // CmBbGgRr YvtxXvtx ClutYcXc
    { 0x2, 0x70AAAAAA, 0 }, // 70 8x8 mono opaque     - 2x32
    { 0x2, 0x71888888, 0 }, // 71 8x8 mono opaque n-b - 2x32
    { 0x2, 0x72CCAA77, 0 }, // 72 8x8 mono transp     - 2x32
    { 0x2, 0x73AA8844, 0 }, // 73 8x8 mono transp n-b - 2x32
        // CmBbGgRr YvtxXvtx
    { 0x3, 0x74AAAAAA, 0, 0 }, // 74 8x8 txtr opaque bld - 3x32
    { 0x3, 0x75888888, 0, 0 }, // 75 8x8 txtr opaque raw - 3x32
    { 0x3, 0x76CCAA77, 0, 0 }, // 76 8x8 txtr transp bld - 3x32
    { 0x3, 0x77AA8844, 0, 0 }, // 77 8x8 txtr transp raw - 3x32
        // CmBbGgRr YvtxXvtx ClutYcXc
    { 0x2, 0x78AAAAAA, 0 }, // 78 16x mono opaque     - 2x32
    { 0x2, 0x79888888, 0 }, // 79 16x mono opaque n-b - 2x32
    { 0x2, 0x7ACCAA77, 0 }, // 7A 16x mono transp     - 2x32
    { 0x2, 0x7BAA8844, 0 }, // 7B 16x mono transp n-b - 2x32
        // CmBbGgRr YvtxXvtx
    { 0x3, 0x7CAAAAAA, 0, 0 }, // 7C 16x txtr opaque bld - 3x32
    { 0x3, 0x7D888888, 0, 0 }, // 7D 16x txtr opaque raw - 3x32
    { 0x3, 0x7ECCAA77, 0, 0 }, // 7E 16x txtr transp bld - 3x32
    { 0x3, 0x7FAA8844, 0, 0 }, // 7F 16x txtr transp raw - 3x32
        // CmBbGgRr YvtxXvtx ClutYcXc
    // MISC
    { 0x4, 0x80000000,0x00000000,0x00100010,0x00200020 }, // 80 move image - 4x32 - Cm000000 YsrcXsrc YdstXdst YhgtXwid
    {0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}, // unused (81 to 9F)
    { 0x3, 0xA0000000,0x00000000,0x00000000 }, // A0 load image  - 3x32 - Cm000000 YdstXdst YhgtXwid [data]
    {0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}, // unused (A1 to BF)
    { 0x3, 0xC0000000,0x00000000,0x00000000 }, // C0 store image - 3x32 - Cm000000 YsrcXsrc YhgtXwid [data]
    {0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}, // unused (C1 to E0)
    // ATTR
    { 0x1, 0xE1000340 },   // E1 texture page        - 1x32 - CmConfig
    { 0x1, 0xE2021001 },  // E2 texture window      - 1x32 - CmConfig
    { 0x1, 0xE3002008 },   // E3 draw area top left  - 1x32 - CmConfig
    { 0x1, 0xE4032110 }, // E4 draw area btm right - 1x32 - CmConfig
    { 0x1, 0xE5020040 },   // E5 draw offset         - 1x32 - CmConfig
    { 0x1, 0xE6000003 }      // E6 mask bit            - 1x32 - Cm00000B
};


///<summary>Show full list of available primitives</summary>
void listPrimitives()
{
    printf("\n POLY                      ");
    printf("LINE                      ");
    printf("RECT                      \n");
    printf("  20: 3pt mono opaque      "); // mono = monochrome
    printf(" 40: line mono opaque     ");
    printf(" 60: var mono opaque      \n");
    printf("  21: 3pt mono opaque n-b  "); // n-b = no texture-blending
    printf(" 41: line mono opaque n-b ");
    printf(" 61: var mono opaque n-b  \n"); 
    printf("  22: 3pt mono transp      ");
    printf(" 42: line mono transp     ");
    printf(" 62: var mono transp      \n");
    printf("  23: 3pt mono transp n-b  ");
    printf(" 43: line mono transp n-b ");
    printf(" 63: var mono transp n-b  \n");
    printf("  24: 3pt txtr opaque bld  "); // bld = texture-blending
    printf(" 44: line mono opaque     ");
    printf(" 64: var txtr opaque bld  \n"); 
    printf("  25: 3pt txtr opaque raw  "); // raw = raw-texture
    printf(" 45: line mono opaque n-b ");
    printf(" 65: var txtr opaque raw  \n"); 
    printf("  26: 3pt txtr transp bld  "); // txtr = textured
    printf(" 46: line mono transp     ");
    printf(" 66: var txtr transp bld  \n"); 
    printf("  27: 3pt txtr transp raw  ");
    printf(" 47: line mono transp n-b ");
    printf(" 67: var txtr transp raw  \n");
    printf("  28: 4pt mono opaque      ");
    printf(" 48: plin mono opaque     "); // plin = poly-line
    printf(" 68: 1x1 mono opaque      \n");
    printf("  29: 4pt mono opaque n-b  ");
    printf(" 49: plin mono opaque n-b ");
    printf(" 69: 1x1 mono opaque n-b  \n");
    printf("  2A: 4pt mono transp      ");
    printf(" 4A: plin mono transp     ");
    printf(" 6A: 1x1 mono transp      \n");
    printf("  2B: 4pt mono transp n-b  ");
    printf(" 4B: plin mono transp n-b ");
    printf(" 6B: 1x1 mono transp n-b  \n");
    printf("  2C: 4pt txtr opaque bld  ");
    printf(" 4C: plin mono opaque     ");
    printf(" 6C: 1x1 txtr opaque bld  \n");
    printf("  2D: 4pt txtr opaque raw  ");
    printf(" 4D: plin mono opaque n-b ");
    printf(" 6D: 1x1 txtr opaque raw  \n");
    printf("  2E: 4pt txtr transp bld  ");
    printf(" 4E: plin mono transp     ");
    printf(" 6E: 1x1 txtr transp bld  \n");
    printf("  2F: 4pt txtr transp raw  ");
    printf(" 4F: plin mono transp n-b ");
    printf(" 6F: 1x1 txtr transp raw  \n");
    printf("  30: 3pt shad opaque      "); // shad = shaded
    printf(" 50: line shad opaque     "); 
    printf(" 70: 8x8 mono opaque      \n"); 
    printf("  31: 3pt shad opaque n-b  ");
    printf(" 51: line shad opaque n-b ");
    printf(" 71: 8x8 mono opaque n-b  \n");
    printf("  32: 3pt shad transp      ");
    printf(" 52: line shad transp     ");
    printf(" 72: 8x8 mono transp      \n");
    printf("  33: 3pt shad transp n-b  ");
    printf(" 53: line shad transp n-b ");
    printf(" 73: 8x8 mono transp n-b  \n");
    printf("  34: 3pt shtx opaque bld  "); // shtx = shaded textured
    printf(" 54: line shad opaque     "); 
    printf(" 74: 8x8 txtr opaque bld  \n");
    printf("  35: 3pt shtx opaque n-b  ");
    printf(" 55: line shad opaque n-b ");
    printf(" 75: 8x8 txtr opaque raw  \n");
    printf("  36: 3pt shtx transp bld  ");
    printf(" 56: line shad transp     ");
    printf(" 76: 8x8 txtr transp bld  \n");
    printf("  37: 3pt shtx transp n-b  ");
    printf(" 57: line shad transp n-b ");
    printf(" 77: 8x8 txtr transp raw  \n");
    printf("  38: 4pt shad opaque      ");
    printf(" 58: plin shad opaque     ");
    printf(" 78: 16x mono opaque      \n");
    printf("  39: 4pt shad opaque n-b  ");
    printf(" 59: plin shad opaque n-b ");
    printf(" 79: 16x mono opaque n-b  \n");
    printf("  3A: 4pt shad transp      ");
    printf(" 5A: plin shad transp     ");
    printf(" 7A: 16x mono transp      \n");
    printf("  3B: 4pt shad transp n-b  ");
    printf(" 5B: plin shad transp n-b ");
    printf(" 7B: 16x mono transp n-b  \n");
    printf("  3C: 4pt shtx opaque bld  ");
    printf(" 5C: plin shad opaque     ");
    printf(" 7C: 16x txtr opaque bld  \n");
    printf("  3D: 4pt shtx opaque n-b  ");
    printf(" 5D: plin shad opaque n-b ");
    printf(" 7D: 16x txtr opaque raw  \n");
    printf("  3E: 4pt shtx transp bld  ");
    printf(" 5E: plin shad transp     ");
    printf(" 7E: 16x txtr transp bld  \n");
    printf("  3F: 4pt shtx transp n-b  ");
    printf(" 5F: plin shad transp n-b ");
    printf(" 7F: 16x txtr transp raw  \n\n");

    printf(" ATTR                      ");
    printf("MISC                      ");
    printf(" * plin = poly-line       \n");
    printf("  E1: texture page         ");
    printf(" 01: clear texture cache  ");
    printf(" * txtr = textured        \n");
    printf("  E2: texture window       ");
    printf(" 02: fill blank rect      ");
    printf(" * shad = shaded          \n");
    printf("  E3: draw area top left   ");
    printf(" 80: move image           ");
    printf(" * shtx = shaded-textured \n");
    printf("  E4: draw area btm right  ");
    printf(" A0: load image           ");
    printf(" * bld = texture blending \n");
    printf("  E5: draw offset          ");
    printf(" C0: store image          ");
    printf(" * n-b = no blending      \n");
    printf("  E6: mask bit             \n\n");

    printf(" XY FLIP: use negative value (only for RECT types).\n");
    printf("\nEnter 0 to exit.\n");
}

///<summary>Create a primitive</summary>
///<param name="id">Primitive ID (type)</param>
///<param name="pOut">Output reference</param>
///<returns>Primitive length</returns>
int createPrimitive(int id, unsigned char** pOut)
{
    if (id < 1 || id > MAX_PRIMITIVE_ID || (id > 0x2 && id < 0x20) || (id > 0x80 && id < 0xE1 && id != 0xA0 && id != 0xC0))
        return -1;

    --id;
    *pOut = (unsigned char*)&primTestTable[id][1];
    return (int)primTestTable[id][0];
}

///<summary>Check if primitive can be flipped (rectangle)</summary>
///<param name="id">Primitive ID (type)</param>
bool isPrimitiveFlippable(int id)
{
    return (id >= 0x60 && id <= 0x7F);
}
