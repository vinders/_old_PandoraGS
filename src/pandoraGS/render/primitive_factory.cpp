/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   primitive_factory.cpp
Description : drawing primitive factory
*******************************************************************************/
#include <cstdlib>
using namespace std;
#include "geometry.hpp"
#include "config.h"
#include "status_register.h"
#include "primitive_factory.h"

#define NI cmVoid // non-implemented commands

// GPU operations
gpucmd_t PrimitiveFactory::s_gpuCommand;
unsigned long PrimitiveFactory::s_gpuMemCache[256]; // memory cache
long PrimitiveFactory::s_gpuDataCount;              // data set length
long PrimitiveFactory::s_gpuDataProcessed;          // current number of values cached

// pre-allocated data buffers (NOT thread-safe: display data will only ever come from one thread)
line_t g_curLine;
triangle_t g_curTriangle;
quad_t g_curQuad;
tile_t g_curRect;


// private command functions
void cmVoid(unsigned char* pData);         // not implemented
void cmClearCache(unsigned char* pData);   // GENERAL - clear cache
void cmBlankFill(unsigned char* pData);    // GENERAL - fill
void cmImageMove(unsigned char* pData);    // IMAGE - move
void cmImageLoad(unsigned char* pData);    // IMAGE - load
void cmImageStore(unsigned char* pData);   // IMAGE - store

void cmTriangle(unsigned char* pData);     // POLY - triangle - flat
void cmTriangleS(unsigned char* pData);    // POLY - triangle - shaded
void cmTriangleTx(unsigned char* pData);   // POLY - triangle - textured
void cmTriangleSTx(unsigned char* pData);  // POLY - triangle - shaded-textured
void cmQuad(unsigned char* pData);         // POLY - quad - flat
void cmQuadS(unsigned char* pData);        // POLY - quad - shaded
void cmQuadTx(unsigned char* pData);       // POLY - quad - textured
void cmQuadSTx(unsigned char* pData);      // POLY - quad - shaded-textured

void cmLine(unsigned char* pData);         // LINE - flat
void cmLineS(unsigned char* pData);        // LINE - shaded
void cmPolyLine(unsigned char* pData);     // LINE - poly-line - flat
void cmPolyLineS(unsigned char* pData);    // LINE - poly-line - shaded
void skipPolyLine(unsigned char* pData);   // LINE - skip poly-line - flat
void skipPolyLineS(unsigned char* pData);  // LINE - skip poly-line - shaded

void cmTile(unsigned char* pData);         // RECT - tile custom
void cmTile1(unsigned char* pData);        // RECT - tile 1x1
void cmTile8(unsigned char* pData);        // RECT - tile 8x8
void cmTile16(unsigned char* pData);       // RECT - tile 16x16
void cmSprite(unsigned char* pData);       // RECT - sprite custom
void cmSprite1(unsigned char* pData);      // RECT - sprite 1x1
void cmSprite8(unsigned char* pData);      // RECT - sprite 8x8
void cmSprite16(unsigned char* pData);     // RECT - sprite 16x16

void cmTexPage(unsigned char* pData);      // ATTR - texture page
void cmTexWindow(unsigned char* pData);    // ATTR - texture window
void cmDrawAreaStart(unsigned char* pData);// ATTR - draw area start
void cmDrawAreaEnd(unsigned char* pData);  // ATTR - draw area end
void cmDrawOffset(unsigned char* pData);   // ATTR - draw offset
void cmMaskBit(unsigned char* pData);      // ATTR - mask bit


// primitive factory table (size, command, skip_action)
const primcmd_row_t c_pPrimTable[PRIMITIVE_NUMBER] =
{
    // GENERAL : 00 - 02
    { 0, cmVoid, cmVoid }, { 0, cmClearCache, cmClearCache }, { 3, cmBlankFill, cmBlankFill },
    // not implemented : 03 - 1F
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
 
    // POLY - triangle monochrome : 20 - 23
    { 4, cmTriangle, NI }, { 4, cmTriangle, NI }, { 4, cmTriangle, NI }, { 4, cmTriangle, NI },
    // POLY - triangle textured : 24 - 27
    { 7, cmTriangleTx, NI }, { 7, cmTriangleTx, NI }, { 7, cmTriangleTx, NI }, { 7, cmTriangleTx, NI },
    // POLY - quad monochrome : 28 - 2B
    { 5, cmQuad, NI }, { 5, cmQuad, NI }, { 5, cmQuad, NI }, { 5, cmQuad, NI },
    // POLY - quad textured : 2C - 2F
    { 9, cmQuadTx, NI }, { 9, cmQuadTx, NI }, { 9, cmQuadTx, NI }, { 9, cmQuadTx, NI },
    // POLY - triangle shaded : 30 - 33
    { 6, cmTriangleS, NI }, { 6, cmTriangleS, NI }, { 6, cmTriangleS, NI }, { 6, cmTriangleS, NI },
    // POLY - triangle shaded-textured : 34 - 37
    { 9, cmTriangleSTx, NI }, { 9, cmTriangleSTx, NI }, { 9, cmTriangleSTx, NI }, { 9, cmTriangleSTx, NI },
    // POLY - quad shaded : 38 - 3B
    { 8, cmQuadS, NI }, { 8, cmQuadS, NI }, { 8, cmQuadS, NI }, { 8, cmQuadS, NI },
    // POLY - quad shaded-textured : 3C - 3F
    { 12, cmQuadSTx, NI }, { 12, cmQuadSTx, NI }, { 12, cmQuadSTx, NI }, { 12, cmQuadSTx, NI },
 
    // LINE - monochrome : 40 - 47
    { 3, cmLine, NI }, { 3, cmLine, NI }, { 3, cmLine, NI }, { 3, cmLine, NI },
    { 3, cmLine, NI }, { 3, cmLine, NI }, { 3, cmLine, NI }, { 3, cmLine, NI },
    // LINE - poly monochrome : 48 - 4F
    { 0xFE, cmPolyLine, skipPolyLine }, { 0xFE, cmPolyLine, skipPolyLine }, { 0xFE, cmPolyLine, skipPolyLine }, { 0xFE, cmPolyLine, skipPolyLine },
    { 0xFE, cmPolyLine, skipPolyLine }, { 0xFE, cmPolyLine, skipPolyLine }, { 0xFE, cmPolyLine, skipPolyLine }, { 0xFE, cmPolyLine, skipPolyLine },
    // LINE - shaded : 50 - 57
    { 4, cmLineS, NI }, { 4, cmLineS, NI }, { 4, cmLineS, NI }, { 4, cmLineS, NI },
    { 4, cmLineS, NI }, { 4, cmLineS, NI }, { 4, cmLineS, NI }, { 4, cmLineS, NI },
    // LINE - poly shaded : 58 - 5F
    { 0xFF, cmPolyLineS, skipPolyLineS }, { 0xFF, cmPolyLineS, skipPolyLineS }, { 0xFF, cmPolyLineS, skipPolyLineS }, { 0xFF, cmPolyLineS, skipPolyLineS },
    { 0xFF, cmPolyLineS, skipPolyLineS }, { 0xFF, cmPolyLineS, skipPolyLineS }, { 0xFF, cmPolyLineS, skipPolyLineS }, { 0xFF, cmPolyLineS, skipPolyLineS },
 
    // RECT - custom tile : 60 - 63
    { 3, cmTile, NI }, { 3, cmTile, NI }, { 3, cmTile, NI }, { 3, cmTile, NI },
    // RECT - custom sprite : 64 - 67
    { 4, cmSprite, NI }, { 4, cmSprite, NI }, { 4, cmSprite, NI }, { 4, cmSprite, NI },
    // RECT - 1x1 tile : 68 - 6B
    { 2, cmTile1, NI }, { 2, cmTile1, NI }, { 2, cmTile1, NI }, { 2, cmTile1, NI },
    // RECT - 1x1 sprite : 6C - 6F
    { 3, cmSprite1, NI }, { 3, cmSprite1, NI }, { 3, cmSprite1, NI }, { 3, cmSprite1, NI },
    // RECT - 8x8 tile : 70 - 73
    { 2, cmTile8, NI }, { 2, cmTile8, NI }, { 2, cmTile8, NI }, { 2, cmTile8, NI },
    // RECT - 8x8 sprite : 74 - 77
    { 3, cmSprite8, NI }, { 3, cmSprite8, NI }, { 3, cmSprite8, NI }, { 3, cmSprite8, NI },
    // RECT - 16x16 tile : 78 - 7B
    { 2, cmTile16, NI }, { 2, cmTile16, NI }, { 2, cmTile16, NI }, { 2, cmTile16, NI },
    // RECT - 16x16 sprite : 7C - 7F
    { 3, cmSprite16, NI }, { 3, cmSprite16, NI }, { 3, cmSprite16, NI }, { 3, cmSprite16, NI },
 
    // IMAGE - move : 80
    { 4, cmImageMove, cmImageMove },
    // not implemented : 81 - 9F
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
 
    // IMAGE - load : A0
    { 3, cmImageLoad, cmImageLoad },
    // not implemented : A1 - BF
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
 
    // IMAGE - store : C0
    { 3, cmImageStore, cmImageStore },
    // not implemented : C1 - DF
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
    {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},

    // ATTRIBUTES : E0 - E7
    {0,NI,NI}, { 1, cmTexPage, cmTexPage }, { 1, cmTexWindow, cmTexWindow }, { 1, cmDrawAreaStart, cmDrawAreaStart },
    { 1, cmDrawAreaEnd, cmDrawAreaEnd }, { 1, cmDrawOffset, cmDrawOffset }, { 1, cmMaskBit, cmMaskBit }, {0,NI,NI}
};

// -- PRIMITIVE FACTORY -- -----------------------------------------------------

/// <summary>Process chunk of display data (normal mode)</summary>
/// <param name="vramWriteMode">Reference to VRAM write mode</param>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="size">Memory chunk size</param>
/// <param name="pDest">Destination gdata pointer</param>
/// <param name="pI">Counter pointer</param>
/// <returns>Indicator if VRAM data to write</returns>
bool PrimitiveFactory::processDisplayData(loadmode_t& writeModeRef, unsigned long* pDwMem, int size, unsigned long* pDest, int* pI)
{
    unsigned long gdata = 0;
    gpucmd_t command;
    int i = *pI;

    if (writeModeRef == Loadmode_normal) // no VRAM transfer
    {
        // copy and process data
        while (i < size)
        {
            // back to VRAM transfer mode -> end function
            if (writeModeRef == Loadmode_vramTransfer)
            {
                *pDest = gdata;
                *pI = i;
                return true; // more data to process
            }

            gdata = *pDwMem++;
            i++;
            // new data set -> identify command + copy first value
            if (s_gpuDataCount == 0)
            {
                command = extractPrimitiveCommand(gdata);
                if (command < PRIMITIVE_NUMBER && (s_gpuDataCount = c_pPrimTable[command].size) > 0)
                {
                    s_gpuCommand = command;
                    s_gpuMemCache[0] = gdata;
                    s_gpuDataProcessed = 1;
                }
                else
                    continue;
            }
            // same data set -> copy current value
            else
            {
                s_gpuMemCache[s_gpuDataProcessed] = gdata;
                if (s_gpuDataCount > 128)
                {
                    if ((s_gpuDataCount == 254 && s_gpuDataProcessed >= 3)
                        || (s_gpuDataCount == 255 && s_gpuDataProcessed >= 4 && !(s_gpuDataProcessed & 1)))
                    {
                        // termination code for poly-lines -> force ending of data set
                        if ((s_gpuMemCache[s_gpuDataProcessed] & 0xF000F000) == 0x50005000) // code should be 0x55555555, but a few games (such as wild arms 2) use 0x50005000
                            s_gpuDataProcessed = s_gpuDataCount - 1;
                    }
                }
                s_gpuDataProcessed++;
            }
            // end of data set -> process cached data
            if (s_gpuDataProcessed == s_gpuDataCount)
            {
                s_gpuDataCount = s_gpuDataProcessed = 0;

                // process data set
                if (Timer::isPeriodSkipped())
                    c_pPrimTable[command].skip((unsigned char*)s_gpuMemCache);
                else
                    c_pPrimTable[command].command((unsigned char*)s_gpuMemCache);

                // 'GPU busy' hack (while processing data)
                if (Config::misc_emuFixBits & 0x0001 || Config::getCurrentProfile()->getFix(CFG_FIX_FAKE_GPU_BUSY))
                    StatusRegister::initFakeBusySequence();
            }
        }
    }

    *pDest = gdata;
    return false; // no more data
}

/// <summary>Process single primitive</summary>
/// <param name="pData">Primitive raw data</param>
/// <param name="len">Primitive data length (number of 32bits blocks)</param>
void PrimitiveFactory::processSinglePrimitive(unsigned char* pData, int len)
{
    gpucmd_t command = pData[0];
    if (command < PRIMITIVE_NUMBER && (s_gpuDataCount = c_pPrimTable[command].size) > 0)
    {
        s_gpuCommand = command;
        memset(s_gpuMemCache, 0, 256 * sizeof(unsigned long));
        unsigned long* pSrcMem = (unsigned long*)pData;
        for (int i = 0; i < len; ++i) // test functions may pass data shorter than 256 -> no memcpy
        {
            s_gpuMemCache[i] = *pSrcMem;
            if ((*pSrcMem & 0xF000F000) == 0x50005000)
                break; // termination code for poly-lines
            ++pSrcMem;
        }
        s_gpuDataProcessed = len;
        c_pPrimTable[command].command((unsigned char*)s_gpuMemCache);
    }
}


// -- PRIMITIVE GENERAL COMMANDS -- --------------------------------------------
// Cm = command code (0x00 - 0xE7)
// BbGgRr = color code (R: 0-7 ; G: 8-15 ; B: 16-23)
// YtopXlft = top-left coords
// YhgtXwid = horizontal/vertical size
// YsrcXsrc = source coords
// YdstXdst = destination coords

/// <summary>Non-implemented command</summary>
void cmVoid(unsigned char* pData)
{
}

/// <summary>Clear texture cache</summary>
void cmClearCache(unsigned char* pData)
{
    //... vider cache de textures dans vram (et aussi dans openGL ???)
}

/// <summary>Fill zone (without checking draw areas)</summary>
void cmBlankFill(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 3x32 - CmBbGgRr YtopXlft YhgtXwid

    // coords
    tile_t zone;//...
    zone.x = (pPrimData[1] & 0x0FFFF);// no short* pointer used, to avoid little/big-endian potential problems
    zone.y = (pPrimData[1] >> 16);
    zone.width = (pPrimData[2] & 0x0FFFF);
    zone.height = (pPrimData[2] >> 16);

    //...
}

/// <summary>Move image (vram to vram)</summary>
void cmImageMove(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 4x32 - Cm000000 YsrcXsrc YdstXdst YhgtXwid

}

/// <summary>Load image (cpu to vram)</summary>
void cmImageLoad(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 3x32 - Cm000000 YdstXdst YhgtXwid [data->DMA]

}

/// <summary>Store image (vram to central memory)</summary>
void cmImageStore(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 3x32 - Cm000000 YsrcXsrc YhgtXwid [data->DMA]

}


// -- DRAWING ATTRIBUTE COMMANDS -- --------------------------------------------
// Cm = command code (0x00 - 0xE7)

/// <summary>Set texture page</summary>
void cmTexPage(unsigned char* pData)
{
    unsigned long primData = ((unsigned long *)pData)[0]; // 1x32 - CmBBBBBB
    // bits 0-3: texture page X base (N*64)
    // bit 4   : texture page Y base (N*256)
    // bits 5-6: transparency (0 = B/2+F/2 ; 1 = B+F ; 2 = B-F ; 3 = B+F/4)
    // bits 7-8: colors (0 = 4bit ; 1 = 8bit ; 2 = 15bit ; 3 = reserved)
    // bit 9   : dither 24bit to 15bit (0 = off (strip LSBs) ; 1 = enabled)
    // bit 10  : drawing to display area (0 = forbidden ; 1 = allowed)
    // bit 11  : texture disable (0 = normal ; 1 = disable if bit0==1)
    // bit 12  : textured rectangle X-flip
    // bit 13  : textured rectangle Y-flip
    
}

/// <summary>Set texture window</summary>
void cmTexWindow(unsigned char* pData)
{
    unsigned long primData = ((unsigned long *)pData)[0]; // 1x32 - CmBBBBBB
    // bits 0-4  : mask X (in 8 pixel steps)
    // bits 5-9  : mask Y (in 8 pixel steps)
    // bits 10-14: offset X (in 8 pixel steps)
    // bits 15-19: offset Y (in 8 pixel steps)
    // texcoord = (texcoord AND (NOT (mask*8))) OR ((offset AND mask)*8)

}

/// <summary>Set draw area start</summary>
void cmDrawAreaStart(unsigned char* pData)
{
    unsigned long primData = ((unsigned long *)pData)[0]; // 1x32 - CmBBBBBB
    // bits 0-9  : X coord
    // bits 10-19: Y coord

}

/// <summary>Set draw area end</summary>
void cmDrawAreaEnd(unsigned char* pData)
{
    unsigned long primData = ((unsigned long *)pData)[0]; // 1x32 - CmBBBBBB
    // bits 0-9  : X coord
    // bits 10-19: Y coord

}

/// <summary>Set draw offset</summary>
void cmDrawOffset(unsigned char* pData)
{
    unsigned long primData = ((unsigned long *)pData)[0]; // 1x32 - CmBBBBBB
    // bits 0-10 : X offset
    // bits 11-21: Y offset

}

/// <summary>Set mask bit info</summary>
void cmMaskBit(unsigned char* pData)
{
    unsigned long primData = ((unsigned long *)pData)[0]; // 1x32 - Cm00000B
    // bit 0: mask while drawing (0 = texture with bit 15 ; 1 = force bit15=1)
    // bit 1: check mask before drawing (0 = always draw ; 1 = draw if bit15==0)

}


// -- PRIMITIVE POLYGON COMMANDS -- --------------------------------------------
// Cm = command code (0x00 - 0xE7)
// BbGgRr = color code (R: 0-7 ; G: 8-15 ; B: 16-23)
// YvtxXvtx = vertex coords (X: 0-10 ; Y: 16-26)
// YcXc = texture coords
// Clut = color lookup table, for 4bit/8bit textures only (X coord (N*16): 0-5 ; Y coord (line): 6-14)
// Txpg = texture page
       // 0-3: X base (N*64)
       // 4  : Y base (N*256)
       // 5-6: transparency (0 = B/2+F/2 ; 1 = B+F ; 2 = B-F ; 3 = B+F/4)
       // 7-8: colors (0 = 4bit ; 1 = 8bit ; 2 = 15bit ; 3 = reserved)
       // 11 : texture disable (0 = normal ; 1 = disable if bit0==1)

/// <summary>Draw flat triangle</summary>
void cmTriangle(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 4x32 - CmBbGgRr YvtxXvtx YvtxXvtx YvtxXvtx

    // vertices
    g_curTriangle.x0 = (pPrimData[1] & 0x0FFFF); // no short* pointer used, to avoid little/big-endian potential problems
    g_curTriangle.y0 = (pPrimData[1] >> 16);
    g_curTriangle.x1 = (pPrimData[2] & 0x0FFFF);
    g_curTriangle.y1 = (pPrimData[2] >> 16);
    g_curTriangle.x2 = (pPrimData[3] & 0x0FFFF);
    g_curTriangle.y2 = (pPrimData[3] >> 16);

    //...
}

/// <summary>Draw shaded triangle</summary>
void cmTriangleS(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 6x32 - CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx 00BbGgRr YvtxXvtx

    // vertices
    g_curTriangle.x0 = (pPrimData[1] & 0x0FFFF);
    g_curTriangle.y0 = (pPrimData[1] >> 16);
    g_curTriangle.x1 = (pPrimData[3] & 0x0FFFF);
    g_curTriangle.y1 = (pPrimData[3] >> 16);
    g_curTriangle.x2 = (pPrimData[5] & 0x0FFFF);
    g_curTriangle.y2 = (pPrimData[5] >> 16);

    //...
}

/// <summary>Draw textured triangle</summary>
void cmTriangleTx(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 7x32 - CmBbGgRr YvtxXvtx ClutYcXc YvtxXvtx TxpgYcXc YvtxXvtx 0000YcXc

    // vertices
    g_curTriangle.x0 = (pPrimData[1] & 0x0FFFF);
    g_curTriangle.y0 = (pPrimData[1] >> 16);
    g_curTriangle.x1 = (pPrimData[3] & 0x0FFFF);
    g_curTriangle.y1 = (pPrimData[3] >> 16);
    g_curTriangle.x2 = (pPrimData[5] & 0x0FFFF);
    g_curTriangle.y2 = (pPrimData[5] >> 16);

    //...

    //... si texture 4bit ou 8bit, lire Clut, sinon ignorer
    //... si raw, ignorer couleur
}

/// <summary>Draw shaded-textured triangle</summary>
void cmTriangleSTx(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 9x32 - CmBbGgRr YvtxXvtx ClutYcXc 00BbGgRr YvtxXvtx TxpgYcXc 00BbGgRr YvtxXvtx 0000YcXc

    // vertices
    g_curTriangle.x0 = (pPrimData[1] & 0x0FFFF);
    g_curTriangle.y0 = (pPrimData[1] >> 16);
    g_curTriangle.x1 = (pPrimData[4] & 0x0FFFF);
    g_curTriangle.y1 = (pPrimData[4] >> 16);
    g_curTriangle.x2 = (pPrimData[7] & 0x0FFFF);
    g_curTriangle.y2 = (pPrimData[7] >> 16);

    //...
}

/// <summary>Draw flat quad</summary>
void cmQuad(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 5x32 - CmBbGgRr YvtxXvtx YvtxXvtx YvtxXvtx YvtxXvtx

    // vertices
    g_curQuad.x0 = (pPrimData[1] & 0x0FFFF);
    g_curQuad.y0 = (pPrimData[1] >> 16);
    g_curQuad.x1 = (pPrimData[2] & 0x0FFFF);
    g_curQuad.y1 = (pPrimData[2] >> 16);
    g_curQuad.x2 = (pPrimData[3] & 0x0FFFF);
    g_curQuad.y2 = (pPrimData[3] >> 16);
    g_curQuad.x3 = (pPrimData[4] & 0x0FFFF);
    g_curQuad.y3 = (pPrimData[4] >> 16);

    //...
}

/// <summary>Draw shaded quad</summary>
void cmQuadS(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 8x32 - CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx 00BbGgRr YvtxXvtx 00BbGgRr YvtxXvtx

    // vertices
    g_curQuad.x0 = (pPrimData[1] & 0x0FFFF);
    g_curQuad.y0 = (pPrimData[1] >> 16);
    g_curQuad.x1 = (pPrimData[3] & 0x0FFFF);
    g_curQuad.y1 = (pPrimData[3] >> 16);
    g_curQuad.x2 = (pPrimData[5] & 0x0FFFF);
    g_curQuad.y2 = (pPrimData[5] >> 16);
    g_curQuad.x3 = (pPrimData[7] & 0x0FFFF);
    g_curQuad.y3 = (pPrimData[7] >> 16);

    //...
}

/// <summary>Draw textured quad</summary>
void cmQuadTx(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 9x32 - CmBbGgRr YvtxXvtx ClutYcXc YvtxXvtx TxpgYcXc YvtxXvtx 0000YcXc YvtxXvtx 0000YcXc

    // vertices
    g_curQuad.x0 = (pPrimData[1] & 0x0FFFF);
    g_curQuad.y0 = (pPrimData[1] >> 16);
    g_curQuad.x1 = (pPrimData[3] & 0x0FFFF);
    g_curQuad.y1 = (pPrimData[3] >> 16);
    g_curQuad.x2 = (pPrimData[5] & 0x0FFFF);
    g_curQuad.y2 = (pPrimData[5] >> 16);
    g_curQuad.x3 = (pPrimData[7] & 0x0FFFF);
    g_curQuad.y3 = (pPrimData[7] >> 16);

    //...
}

/// <summary>Draw shaded-textured quad</summary>
void cmQuadSTx(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 12x32 - CmBbGgRr YvtxXvtx ClutYcXc 00BbGgRr YvtxXvtx TxpgYcXc 00BbGgRr YvtxXvtx 0000YcXc 00BbGgRr YvtxXvtx 0000YcXc

    // vertices
    g_curQuad.x0 = (pPrimData[1] & 0x0FFFF);
    g_curQuad.y0 = (pPrimData[1] >> 16);
    g_curQuad.x1 = (pPrimData[4] & 0x0FFFF);
    g_curQuad.y1 = (pPrimData[4] >> 16);
    g_curQuad.x2 = (pPrimData[7] & 0x0FFFF);
    g_curQuad.y2 = (pPrimData[7] >> 16);
    g_curQuad.x3 = (pPrimData[10] & 0x0FFFF);
    g_curQuad.y3 = (pPrimData[10] >> 16);

    //...
}


// -- PRIMITIVE LINE COMMANDS -- -----------------------------------------------
// Cm = command code (0x00 - 0xE7)
// BbGgRr = color code (R: 0-7 ; G: 8-15 ; B: 16-23)
// YvtxXvtx = vertex coords (X: 0-10 ; Y: 16-26)

/// <summary>Draw flat line</summary>
void cmLine(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 3x32 - CmBbGgRr YvtxXvtx YvtxXvtx

    // vertices
    g_curLine.x0 = (pPrimData[1] & 0x0FFFF);
    g_curLine.y0 = (pPrimData[1] >> 16);
    g_curLine.x1 = (pPrimData[2] & 0x0FFFF);
    g_curLine.y1 = (pPrimData[2] >> 16);

    //...
}

/// <summary>Draw shaded line</summary>
void cmLineS(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 4x32 - CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx

    // vertices
    g_curLine.x0 = (pPrimData[1] & 0x0FFFF);
    g_curLine.y0 = (pPrimData[1] >> 16);
    g_curLine.x1 = (pPrimData[3] & 0x0FFFF);
    g_curLine.y1 = (pPrimData[3] >> 16);

    //...
}

/// <summary>Draw flat poly-line</summary>
void cmPolyLine(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // [3-254]x32 - CmBbGgRr YvtxXvtx YvtxXvtx [...] 55555555

}

/// <summary>Draw shaded poly-line</summary>
void cmPolyLineS(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // [4-255]x32 - CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx [...] 55555555

}

/// <summary>Skip poly-line</summary>
void skipPolyLine(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // [4-254]x32 - CmBbGgRr YvtxXvtx YvtxXvtx [...] 55555555

}

/// <summary>Skip poly-line</summary>
void skipPolyLineS(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // [5-255]x32 - CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx [...] 55555555

}


// -- PRIMITIVE RECTANGLE COMMANDS -- ------------------------------------------
// Cm = command code (0x00 - 0xE7)
// BbGgRr = color code (R: 0-7 ; G: 8-15 ; B: 16-23)
// YhgtXwid = horizontal/vertical size
// YvtxXvtx = vertex coords (X: 0-10 ; Y: 16-26)
// YcXc = texture coords
// Clut = color lookup table, for 4bit/8bit textures only (X coord (N*16): 0-5 ; Y coord (line): 6-14)
// Txpg = texture page
       // 0-3: X base (N*64)
       // 4  : Y base (N*256)
       // 5-6: transparency (0 = B/2+F/2 ; 1 = B+F ; 2 = B-F ; 3 = B+F/4)
       // 7-8: colors (0 = 4bit ; 1 = 8bit ; 2 = 15bit ; 3 = reserved)
       // 11 : texture disable (0 = normal ; 1 = disable if bit0==1)

/// <summary>Draw custom-sized rectangle</summary>
void cmTile(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 3x32 - CmBbGgRr YvtxXvtx YhgtXwid

    // coords
    g_curRect.x = (pPrimData[1] & 0x0FFFF);
    g_curRect.y = (pPrimData[1] >> 16);
    g_curRect.width = (pPrimData[3] & 0x0FFFF);
    g_curRect.height = (pPrimData[3] >> 16);

    //...
}

/// <summary>Draw 1x1 rectangle</summary>
void cmTile1(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 2x32 - CmBbGgRr YvtxXvtx

    // coords
    g_curRect.x = (pPrimData[1] & 0x0FFFF);
    g_curRect.y = (pPrimData[1] >> 16);
    g_curRect.width = g_curRect.height = 1;

    //...
}

/// <summary>Draw 8x8 rectangle</summary>
void cmTile8(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 2x32 - CmBbGgRr YvtxXvtx

    // coords
    g_curRect.x = (pPrimData[1] & 0x0FFFF);
    g_curRect.y = (pPrimData[1] >> 16);
    g_curRect.width = g_curRect.height = 8;

    //...
}

/// <summary>Draw 16x16 rectangle</summary>
void cmTile16(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 2x32 - CmBbGgRr YvtxXvtx

    // coords
    g_curRect.x = (pPrimData[1] & 0x0FFFF);
    g_curRect.y = (pPrimData[1] >> 16);
    g_curRect.width = g_curRect.height = 16;

    //...
}

/// <summary>Draw custom-sized sprite</summary>
void cmSprite(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 4x32 - CmBbGgRr YvtxXvtx ClutYcXc YhgtXwid

    // coords
    g_curRect.x = (pPrimData[1] & 0x0FFFF);
    g_curRect.y = (pPrimData[1] >> 16);
    g_curRect.width = (pPrimData[3] & 0x0FFFF);
    g_curRect.height = (pPrimData[3] >> 16);

    //...
}

/// <summary>Draw 1x1 sprite</summary>
void cmSprite1(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 3x32 - CmBbGgRr YvtxXvtx ClutYcXc

    // coords
    g_curRect.x = (pPrimData[1] & 0x0FFFF);
    g_curRect.y = (pPrimData[1] >> 16);
    g_curRect.width = g_curRect.height = 1;

    //...
}

/// <summary>Draw 8x8 sprite</summary>
void cmSprite8(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 3x32 - CmBbGgRr YvtxXvtx ClutYcXc

    // coords
    g_curRect.x = (pPrimData[1] & 0x0FFFF);
    g_curRect.y = (pPrimData[1] >> 16);
    g_curRect.width = g_curRect.height = 8;

    //...
}

/// <summary>Draw 16x16 sprite</summary>
void cmSprite16(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 3x32 - CmBbGgRr YvtxXvtx ClutYcXc

    // coords
    g_curRect.x = (pPrimData[1] & 0x0FFFF);
    g_curRect.y = (pPrimData[1] >> 16);
    g_curRect.width = g_curRect.height = 16;

    //...
}
