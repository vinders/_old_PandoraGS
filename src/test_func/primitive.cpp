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
char* primTestTable[MAX_PRIMITIVE_ID] =
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
    "\x1\x1\0\0\0",                           // 01 clear txtr cache - 1x32 - Cm000000
    "\x3\x2\x17\x77\0\0\x20\0\x10\x1\0\x1\0", // 02 fill blank rect  - 3x32 - CmBbGgRr YtopXlft YhgtXwid 
    "\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0", // unused (03 to 1F)
    // POLY
    "\x4\x20\x00\x00\xAA\x0\x20\x0\x80\x1\x10\x0\x10\x1\x00\x2\x00", // 20 3pt mono opaque     - 4x32
    "\x4\x21\x77\x00\xAA\x0\x20\x0\x80\x1\x10\x0\x10\x1\x00\x2\x00", // 21 3pt mono opaque n-b - 4x32
    "\x4\x22\xAA\x44\x00\x0\x20\x0\x80\x1\x10\x0\x10\x1\x00\x2\x00", // 22 3pt mono transp     - 4x32
    "\x4\x23\x44\xAA\x00\x0\x20\x0\x80\x1\x10\x0\x10\x1\x00\x2\x00", // 23 3pt mono transp n-b - 4x32
        // CmBbGgRr YvtxXvtx YvtxXvtx YvtxXvtx
    "\x7\x24\x00\x00\xAA\x1\x10\x0\x10    \x1\x00\x2\x00    \x1\x80\x0\x30    ", // 24 3pt txtr opaque bld - 7x32
    "\x7\x25\x77\x00\xAA\x1\x10\x0\x10    \x1\x00\x2\x00    \x1\x80\x0\x30    ", // 25 3pt txtr opaque raw - 7x32
    "\x7\x26\xAA\x44\x00\x1\x10\x0\x10    \x1\x00\x2\x00    \x1\x80\x0\x30    ", // 26 3pt txtr transp bld - 7x32
    "\x7\x27\x44\xAA\x00\x1\x10\x0\x10    \x1\x00\x2\x00    \x1\x80\x0\x30    ", // 27 3pt txtr transp raw - 7x32
        // CmBbGgRr YvtxXvtx ClutYcXc YvtxXvtx TxpgYcXc YvtxXvtx 0000YcXc
    "\x5\x28\x00\x00\xAA\x1\x00\x2\x00\x1\x10\x3\x00\x2\x00\x2\x20\x2\x10\x3\x10", // 28 4pt mono opaque     - 5x32
    "\x5\x29\x77\x00\xAA\x1\x00\x2\x00\x1\x10\x3\x00\x2\x00\x2\x20\x2\x10\x3\x10", // 29 4pt mono opaque n-b - 5x32
    "\x5\x2A\xAA\x44\x00\x1\x00\x2\x00\x1\x10\x3\x00\x2\x00\x2\x20\x2\x10\x3\x10", // 2A 4pt mono transp     - 5x32
    "\x5\x2B\x44\xAA\x00\x1\x00\x2\x00\x1\x10\x3\x00\x2\x00\x2\x20\x2\x10\x3\x10", // 2B 4pt mono transp n-b - 5x32
        // CmBbGgRr YvtxXvtx YvtxXvtx YvtxXvtx YvtxXvtx
    "\x9\x2C\x00\x00\xAA\x2\x00\x2\x20\x2\x10\x3\x10\x3\x00\x2\x80\x3\x10\x3\x40", // 2C 4pt txtr opaque bld - 9x32
    "\x9\x2D\x77\x00\xAA\x2\x00\x2\x20\x2\x10\x3\x10\x3\x00\x2\x80\x3\x10\x3\x40", // 2D 4pt txtr opaque raw - 9x32
    "\x9\x2E\xAA\x44\x00\x2\x00\x2\x20\x2\x10\x3\x10\x3\x00\x2\x80\x3\x10\x3\x40", // 2E 4pt txtr transp bld - 9x32
    "\x9\x2F\x44\xAA\x00\x2\x00\x2\x20\x2\x10\x3\x10\x3\x00\x2\x80\x3\x10\x3\x40", // 2F 4pt txtr transp raw - 9x32
        // CmBbGgRr YvtxXvtx ClutYcXc YvtxXvtx TxpgYcXc YvtxXvtx 0000YcXc YvtxXvtx 0000YcXc
    "\x6\x30\x00\x00\xAA                    ", // 30 3pt shad opaque     - 6x32
    "\x6\x31\x77\x00\xAA                    ", // 31 3pt shad opaque n-b - 6x32
    "\x6\x32\xAA\x44\x00                    ", // 32 3pt shad transp     - 6x32
    "\x6\x33\x44\xAA\x00                    ", // 33 3pt shad transp n-b - 6x32
        // CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx 00BbGgRr YvtxXvtx
    "\x9\x34\x00\x00\xAA                                ", // 34 3pt shtx opaque bld - 9x32
    "\x9\x35\x77\x00\xAA                                ", // 35 3pt shtx opaque raw - 9x32
    "\x9\x36\xAA\x44\x00                                ", // 36 3pt shtx transp bld - 9x32
    "\x9\x37\x44\xAA\x00                                ", // 37 3pt shtx transp raw - 9x32
        //CmBbGgRr YvtxXvtx ClutYcXc 00BbGgRr YvtxXvtx TxpgYcXc 00BbGgRr YvtxXvtx 0000YcXc
    "\x8\x38\x00\x00\xAA                            ", // 38 4pt shad opaque     - 8x32
    "\x8\x39\x77\x00\xAA                            ", // 39 4pt shad opaque n-b - 8x32
    "\x8\x3A\xAA\x44\x00                            ", // 3A 4pt shad transp     - 8x32
    "\x8\x3B\x44\xAA\x00                            ", // 3B 4pt shad transp n-b - 8x32
        // CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx 00BbGgRr YvtxXvtx 00BbGgRr YvtxXvtx
    "\xC\x3C\x00\x00\xAA                                            ", // 3C 4pt shtx opaque bld - 12x32
    "\xC\x3D\x77\x00\xAA                                            ", // 3D 4pt shtx opaque raw - 12x32
    "\xC\x3E\xAA\x44\x00                                            ", // 3E 4pt shtx transp bld - 12x32
    "\xC\x3F\x44\xAA\x00                                            ", // 3F 4pt shtx transp raw - 12x32
        // CmBbGgRr YvtxXvtx ClutYcXc 00BbGgRr YvtxXvtx TxpgYcXc 00BbGgRr YvtxXvtx 0000YcXc 00BbGgRr YvtxXvtx 0000YcXc
    // LINE
    "\x3\x40\x44\x44\xCC\x1\x10\x0\x10\x2\x00\x2\x20", // 40 line mono opaque     - 3x32
    "\x3\x41\xAA\x44\xCC\x1\x10\x0\x10\x2\x00\x2\x20", // 41 line mono opaque n-b - 3x32
    "\x3\x42\xCC\xAA\x44\x1\x10\x0\x10\x2\x00\x2\x20", // 42 line mono transp     - 3x32
    "\x3\x43\xAA\xCC\x44\x1\x10\x0\x10\x2\x00\x2\x20", // 43 line mono transp n-b - 3x32
    "\x3\x44\x44\x44\xCC\x1\x10\x0\x10\x2\x00\x2\x20", // 44 line mono opaque     - 3x32
    "\x3\x45\xAA\x44\xCC\x1\x10\x0\x10\x2\x00\x2\x20", // 45 line mono opaque n-b - 3x32
    "\x3\x46\xCC\xAA\x44\x1\x10\x0\x10\x2\x00\x2\x20", // 46 line mono transp     - 3x32
    "\x3\x47\xAA\xCC\x44\x1\x10\x0\x10\x2\x00\x2\x20", // 47 line mono transp n-b - 3x32
        // CmBbGgRr YvtxXvtx YvtxXvtx
    "\xFE\x48\x44\x44\xCC\x3\x10\x0\x10\x2\x10\x1\x10\x2\x00\x2\x20\x55\x55\x55\x55", // 48 plin mono opaque     - var.x32
    "\xFE\x49\xAA\x44\xCC\x3\x10\x0\x10\x2\x10\x1\x10\x2\x00\x2\x20\x55\x55\x55\x55", // 49 plin mono opaque n-b - var.x32
    "\xFE\x4A\xCC\xAA\x44\x3\x10\x0\x10\x2\x10\x1\x10\x2\x00\x2\x20\x55\x55\x55\x55", // 4A plin mono transp     - var.x32
    "\xFE\x4B\xAA\xCC\x44\x3\x10\x0\x10\x2\x10\x1\x10\x2\x00\x2\x20\x55\x55\x55\x55", // 4B plin mono transp n-b - var.x32
    "\xFE\x4C\x44\x44\xCC\x3\x10\x0\x10\x2\x10\x1\x10\x2\x00\x2\x20\x55\x55\x55\x55", // 4C plin mono opaque bld - var.x32
    "\xFE\x4D\xAA\x44\xCC\x3\x10\x0\x10\x2\x10\x1\x10\x2\x00\x2\x20\x55\x55\x55\x55", // 4D plin mono opaque raw - var.x32
    "\xFE\x4E\xCC\xAA\x44\x3\x10\x0\x10\x2\x10\x1\x10\x2\x00\x2\x20\x55\x55\x55\x55", // 4E plin mono transp bld - var.x32
    "\xFE\x4F\xAA\xCC\x44\x3\x10\x0\x10\x2\x10\x1\x10\x2\x00\x2\x20\x55\x55\x55\x55", // 4F plin mono transp raw - var.x32
        // CmBbGgRr YvtxXvtx YvtxXvtx [...] 55555555
    "\x4\x50\x44\x44\xCC            ", // 50 line shad opaque     - 4x32
    "\x4\x51\xAA\x44\xCC            ", // 51 line shad opaque n-b - 4x32
    "\x4\x52\xCC\xAA\x44            ", // 52 line shad transp     - 4x32
    "\x4\x53\xAA\xCC\x44            ", // 53 line shad transp n-b - 4x32
    "\x4\x54\x44\x44\xCC            ", // 54 line shad opaque     - 4x32
    "\x4\x55\xAA\x44\xCC            ", // 55 line shad opaque n-b - 4x32
    "\x4\x56\xCC\xAA\x44            ", // 56 line shad transp     - 4x32
    "\x4\x57\xAA\xCC\x44            ", // 57 line shad transp n-b - 4x32
        // CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx
    "\xFF\x58\x44\x44\xCC                                \x55\x55\x55\x55", // 58 plin shad opaque     - var.x32
    "\xFF\x59\xAA\x44\xCC                                \x55\x55\x55\x55", // 59 plin shad opaque n-b - var.x32
    "\xFF\x5A\xCC\xAA\x44                                \x55\x55\x55\x55", // 5A plin shad transp     - var.x32
    "\xFF\x5B\xAA\xCC\x44                                \x55\x55\x55\x55", // 5B plin shad transp n-b - var.x32
    "\xFF\x5C\x44\x44\xCC                                \x55\x55\x55\x55", // 5C plin shad opaque bld - var.x32
    "\xFF\x5D\xAA\x44\xCC                                \x55\x55\x55\x55", // 5D plin shad opaque raw - var.x32
    "\xFF\x5E\xCC\xAA\x44                                \x55\x55\x55\x55", // 5E plin shad transp bld - var.x32
    "\xFF\x5F\xAA\xCC\x44                                \x55\x55\x55\x55", // 5F plin shad transp raw - var.x32
        // CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx [...] 55555555
    // RECT
    "\x3\x60\xAA\xAA\xAA        ", // 60 var mono opaque     - 3x32
    "\x3\x61\x88\x88\x88        ", // 61 var mono opaque n-b - 3x32
    "\x3\x62\xAA\x88\x44        ", // 62 var mono transp     - 3x32
    "\x3\x63\xAA\x88\x44        ", // 63 var mono transp n-b - 3x32
        // CmBbGgRr YvtxXvtx YhgtXwid
    "\x4\x64\xAA\xAA\xAA            ", // 64 var txtr opaque bld - 4x32
    "\x4\x65\x88\x88\x88            ", // 65 var txtr opaque raw - 4x32
    "\x4\x66\xCC\xAA\x77            ", // 66 var txtr transp bld - 4x32
    "\x4\x67\xAA\x88\x44            ", // 67 var txtr transp raw - 4x32
        // CmBbGgRr YvtxXvtx ClutYcXc YhgtXwid
    "\x2\x68\xAA\xAA\xAA    ", // 68 1x1 mono opaque     - 2x32
    "\x2\x69\x88\x88\x88    ", // 69 1x1 mono opaque n-b - 2x32
    "\x2\x6A\xCC\xAA\x77    ", // 6A 1x1 mono transp     - 2x32
    "\x2\x6B\xAA\x88\x44    ", // 6B 1x1 mono transp n-b - 2x32
        // CmBbGgRr YvtxXvtx
    "\x3\x6C\xAA\xAA\xAA        ", // 6C 1x1 txtr opaque bld - 3x32
    "\x3\x6D\x88\x88\x88        ", // 6D 1x1 txtr opaque raw - 3x32
    "\x3\x6E\xCC\xAA\x77        ", // 6E 1x1 txtr transp bld - 3x32
    "\x3\x6F\xAA\x88\x44        ", // 6F 1x1 txtr transp raw - 3x32
        // CmBbGgRr YvtxXvtx ClutYcXc
    "\x2\x70\xAA\xAA\xAA    ", // 70 8x8 mono opaque     - 2x32
    "\x2\x71\x88\x88\x88    ", // 71 8x8 mono opaque n-b - 2x32
    "\x2\x72\xCC\xAA\x77    ", // 72 8x8 mono transp     - 2x32
    "\x2\x73\xAA\x88\x44    ", // 73 8x8 mono transp n-b - 2x32
        // CmBbGgRr YvtxXvtx
    "\x3\x74\xAA\xAA\xAA        ", // 74 8x8 txtr opaque bld - 3x32
    "\x3\x75\x88\x88\x88        ", // 75 8x8 txtr opaque raw - 3x32
    "\x3\x76\xCC\xAA\x77        ", // 76 8x8 txtr transp bld - 3x32
    "\x3\x77\xAA\x88\x44        ", // 77 8x8 txtr transp raw - 3x32
        // CmBbGgRr YvtxXvtx ClutYcXc
    "\x2\x78\xAA\xAA\xAA    ", // 78 16x mono opaque     - 2x32
    "\x2\x79\x88\x88\x88    ", // 79 16x mono opaque n-b - 2x32
    "\x2\x7A\xCC\xAA\x77    ", // 7A 16x mono transp     - 2x32
    "\x2\x7B\xAA\x88\x44    ", // 7B 16x mono transp n-b - 2x32
        // CmBbGgRr YvtxXvtx
    "\x3\x7C\xAA\xAA\xAA        ", // 7C 16x txtr opaque bld - 3x32
    "\x3\x7D\x88\x88\x88        ", // 7D 16x txtr opaque raw - 3x32
    "\x3\x7E\xCC\xAA\x77        ", // 7E 16x txtr transp bld - 3x32
    "\x3\x7F\xAA\x88\x44        ", // 7F 16x txtr transp raw - 3x32
        // CmBbGgRr YvtxXvtx ClutYcXc
    // MISC
    "\x4\x80\0\0\0\0\0\0\0\0\x10\0\x10\0\x20\0\x20", // 80 move image - 4x32 - Cm000000 YsrcXsrc YdstXdst YhgtXwid
    "\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0", // unused (81 to 9F)
    "\x3\xA0\0\0\0\0\0\0\0\0\0\0\0", // A0 load image  - 3x32 - Cm000000 YdstXdst YhgtXwid [data]
    "\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0", // unused (A1 to BF)
    "\x3\xC0\0\0\0\0\0\0\0\0\0\0\0", // C0 store image - 3x32 - Cm000000 YsrcXsrc YhgtXwid [data]
    "\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0", // unused (C1 to E0)
    // ATTR
    "\x1\xE1\0\x3\x40",   // E1 texture page        - 1x32 - CmConfig
    "\x1\xE2\x2\x10\x1",  // E2 texture window      - 1x32 - CmConfig
    "\x1\xE3\0\x20\x8",   // E3 draw area top left  - 1x32 - CmConfig
    "\x1\xE4\x3\x21\x10", // E4 draw area btm right - 1x32 - CmConfig
    "\x1\xE5\x2\0\x40",   // E5 draw offset         - 1x32 - CmConfig
    "\x1\xE6\0\0\x3"      // E6 mask bit            - 1x32 - Cm00000B
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
