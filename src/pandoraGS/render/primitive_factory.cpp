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

#define NI cmVoid   // non-implemented commands
#define PLINE_MAX_LEN 0xFE
#define PLINE_S_MAX_LEN 0xFF

// GPU operations
gpucmd_t PrimitiveFactory::s_gpuCommand;
unsigned long PrimitiveFactory::s_gpuMemCache[256]; // memory cache
long PrimitiveFactory::s_gpuDataCount;              // data set length
long PrimitiveFactory::s_gpuDataProcessed;          // current number of values cached

// fast pre-allocated data buffers (NOT thread-safe: display data will only ever come from one thread)
wline_t g_curLine;
wtriangle_t g_curTriangle;
wtriangle_t g_curTriangleTxCoords;
wquad_t g_curQuad;
wquad_t g_curQuadTxCoords;
wtile_t g_curRect;
wpoint_t g_curRectTxCoords;


// private command functions
void cmVoid(unsigned char* pData);         // no operation
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
    // GENERAL : 00 - 03
    { 0, cmVoid }, { 0, cmClearCache }, { 3, cmBlankFill }, {0,NI}/*?*/,
    // not implemented : 04 - 1F
    {0,NI}, {0,NI}, {0,NI}, {0,NI},
    {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI},
    {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI},
    {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI},
 
    // POLY - triangle monochrome : 20 - 23
    { 4, cmTriangle }, { 4, cmTriangle }, { 4, cmTriangle }, { 4, cmTriangle },
    // POLY - triangle textured : 24 - 27
    { 7, cmTriangleTx }, { 7, cmTriangleTx }, { 7, cmTriangleTx }, { 7, cmTriangleTx },
    // POLY - quad monochrome : 28 - 2B
    { 5, cmQuad }, { 5, cmQuad }, { 5, cmQuad }, { 5, cmQuad },
    // POLY - quad textured : 2C - 2F
    { 9, cmQuadTx }, { 9, cmQuadTx }, { 9, cmQuadTx }, { 9, cmQuadTx },
    // POLY - triangle shaded : 30 - 33
    { 6, cmTriangleS }, { 6, cmTriangleS }, { 6, cmTriangleS }, { 6, cmTriangleS },
    // POLY - triangle shaded-textured : 34 - 37
    { 9, cmTriangleSTx }, { 9, cmTriangleSTx }, { 9, cmTriangleSTx }, { 9, cmTriangleSTx },
    // POLY - quad shaded : 38 - 3B
    { 8, cmQuadS }, { 8, cmQuadS }, { 8, cmQuadS }, { 8, cmQuadS },
    // POLY - quad shaded-textured : 3C - 3F
    { 12, cmQuadSTx }, { 12, cmQuadSTx }, { 12, cmQuadSTx }, { 12, cmQuadSTx },
 
    // LINE - monochrome : 40 - 47
    { 3, cmLine }, { 3, cmLine }, { 3, cmLine }, { 3, cmLine },
    { 3, cmLine }, { 3, cmLine }, { 3, cmLine }, { 3, cmLine },
    // LINE - poly monochrome : 48 - 4F
    { PLINE_MAX_LEN, cmPolyLine }, { PLINE_MAX_LEN, cmPolyLine }, { PLINE_MAX_LEN, cmPolyLine }, { PLINE_MAX_LEN, cmPolyLine },
    { PLINE_MAX_LEN, cmPolyLine }, { PLINE_MAX_LEN, cmPolyLine }, { PLINE_MAX_LEN, cmPolyLine }, { PLINE_MAX_LEN, cmPolyLine },
    // LINE - shaded : 50 - 57
    { 4, cmLineS }, { 4, cmLineS }, { 4, cmLineS }, { 4, cmLineS },
    { 4, cmLineS }, { 4, cmLineS }, { 4, cmLineS }, { 4, cmLineS },
    // LINE - poly shaded : 58 - 5F
    { PLINE_S_MAX_LEN,cmPolyLineS }, { PLINE_S_MAX_LEN,cmPolyLineS }, { PLINE_S_MAX_LEN,cmPolyLineS }, { PLINE_S_MAX_LEN,cmPolyLineS },
    { PLINE_S_MAX_LEN,cmPolyLineS }, { PLINE_S_MAX_LEN,cmPolyLineS }, { PLINE_S_MAX_LEN,cmPolyLineS }, { PLINE_S_MAX_LEN,cmPolyLineS },
 
    // RECT - custom tile : 60 - 63
    { 3, cmTile }, { 3, cmTile }, { 3, cmTile }, { 3, cmTile },
    // RECT - custom sprite : 64 - 67
    { 4, cmSprite }, { 4, cmSprite }, { 4, cmSprite }, { 4, cmSprite },
    // RECT - 1x1 tile : 68 - 6B
    { 2, cmTile1 }, { 2, cmTile1 }, { 2, cmTile1 }, { 2, cmTile1 },
    // RECT - 1x1 sprite : 6C - 6F
    { 3, cmSprite1 }, { 3, cmSprite1 }, { 3, cmSprite1 }, { 3, cmSprite1 },
    // RECT - 8x8 tile : 70 - 73
    { 2, cmTile8 }, { 2, cmTile8 }, { 2, cmTile8 }, { 2, cmTile8 },
    // RECT - 8x8 sprite : 74 - 77
    { 3, cmSprite8 }, { 3, cmSprite8 }, { 3, cmSprite8 }, { 3, cmSprite8 },
    // RECT - 16x16 tile : 78 - 7B
    { 2, cmTile16 }, { 2, cmTile16 }, { 2, cmTile16 }, { 2, cmTile16 },
    // RECT - 16x16 sprite : 7C - 7F
    { 3, cmSprite16 }, { 3, cmSprite16 }, { 3, cmSprite16 }, { 3, cmSprite16 },
 
    // IMAGE - move : 80
    { 4, cmImageMove },
    // not implemented : 81 - 9F
    {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI},
    {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI},
    {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI},
    {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI},
 
    // IMAGE - load : A0
    { 3, cmImageLoad },
    // not implemented : A1 - BF
    {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI},
    {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI},
    {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI},
    {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI},
 
    // IMAGE - store : C0
    { 3, cmImageStore },
    // not implemented : C1 - DF
    {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI},
    {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI},
    {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI},
    {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI}, {0,NI},

    // ATTRIBUTES : E0 - E7
    {0,NI}/*?*/, { 1, cmTexPage }, { 1, cmTexWindow }, { 1, cmDrawAreaStart },
    { 1, cmDrawAreaEnd }, { 1, cmDrawOffset }, { 1, cmMaskBit }, {0,NI}/*?*/
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
                {
                    s_gpuCommand = PRIM_NO_OPERATION_ID;
                    continue;
                }
            }
            // same data set -> copy current value
            else
            {
                s_gpuMemCache[s_gpuDataProcessed] = gdata;
                // if poly-line, check for termination code
                if (s_gpuDataCount >= PLINE_MAX_LEN)
                {
                    if ((s_gpuDataCount == PLINE_MAX_LEN && s_gpuDataProcessed >= 3) // flat line: at least 1 color + 2 vertices
                        || (s_gpuDataCount == PLINE_S_MAX_LEN && s_gpuDataProcessed >= 4 && !(s_gpuDataProcessed & 1))) // shaded: N*(color+vertex), with N >= 2
                    {
                        // termination code -> force ending of data set
                        if ((s_gpuMemCache[s_gpuDataProcessed] & 0xF000F000) == 0x50005000) // should be 0x55555555, but some games (e.g. wild arms 2) use 0x50005000
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
                if (Timer::isPeriodSkipped() == false || s_gpuCommand < PRIM_GEOMETRY_MIN_ID || s_gpuCommand > PRIM_GEOMETRY_MAX_ID)
                    c_pPrimTable[s_gpuCommand].command((unsigned char*)s_gpuMemCache);

                // 'GPU busy' hack (while processing data)
                if (Config::misc_emuFixBits & 0x0001 || Config::getCurrentProfile()->getFix(CFG_FIX_FAKE_GPU_BUSY))
                    StatusRegister::initFakeBusySequence();
            }
        }
    }

    *pDest = gdata;
    return false; // no more data
}

/// <summary>Process single primitive (for testing purpose)</summary>
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

/// <summary>No operation</summary>
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
    wtile_t zone;//...
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], zone.x, zone.y);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[2], zone.width, zone.height);

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
    
    wpoint_t baseCoord;//...
    baseCoord.x = ((primData & 0x0FuL) << 6);
    baseCoord.y = ((primData & 0x10uL) << 4);

    short transpMode = ((primData >> 5) & 0x3uL);//...
    short colorMode = ((primData >> 7) & 0x3uL);//...
    bool is24bit = (primData & 0x200uL);//...
    bool isDrawingToDisplay = (primData & 0x400uL);//...
    bool isTextureDisabled = ((primData & 0x800uL) && (primData & 0x1uL));//...
    bool isFlippedX = (primData & 0x1000uL);//...
    bool isFlippedY = (primData & 0x2000uL);//...

    //...
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

    wpoint_t mask;//...
    mask.x = ((primData << 3) & 0x0F8uL);
    mask.y = ((primData >> 2) & 0x0F8uL);
    wpoint_t off;//...
    off.x = ((primData >> 7) & 0x0F8uL);
    off.y = ((primData >> 12) & 0x0F8uL);

    //texcoord.x = (texcoord.x & ~mask.x) | (off.x & mask.x)
    //texcoord.y = (texcoord.y & ~mask.y) | (off.y & mask.y)
    //...
}

/// <summary>Set draw area start</summary>
void cmDrawAreaStart(unsigned char* pData)
{
    unsigned long primData = ((unsigned long *)pData)[0]; // 1x32 - CmBBBBBB
    // bits 0-9  : X coord
    // bits 10-19: Y coord

    wpoint_t baseCoord;//...
    baseCoord.x = (primData & 0x3FFuL);
    baseCoord.y = ((primData >> 10) & 0x3FFuL);

    //...
}

/// <summary>Set draw area end</summary>
void cmDrawAreaEnd(unsigned char* pData)
{
    unsigned long primData = ((unsigned long *)pData)[0]; // 1x32 - CmBBBBBB
    // bits 0-9  : X coord
    // bits 10-19: Y coord

    wpoint_t endCoord;//...
    endCoord.x = (primData & 0x3FFuL);
    endCoord.y = ((primData >> 10) & 0x3FFuL);

    //...
}

/// <summary>Set draw offset</summary>
void cmDrawOffset(unsigned char* pData)
{
    unsigned long primData = ((unsigned long *)pData)[0]; // 1x32 - CmBBBBBB
    // bits 0-10 : X offset
    // bits 11-21: Y offset

    wpoint_t off;//...
    off.x = (primData & 0x7FFuL);
    off.y = ((primData >> 11) & 0x7FFuL);

    //...
}

/// <summary>Set mask bit info</summary>
void cmMaskBit(unsigned char* pData)
{
    unsigned long primData = ((unsigned long *)pData)[0]; // 1x32 - Cm00000B
    // bit 0: mask while drawing (0 = texture with bit 15 ; 1 = force bit15=1)
    // bit 1: check mask before drawing (0 = always draw ; 1 = draw if bit15==0)

    bool isBit15Forced = (primData & 0x1uL);//...
    bool isBit15Checked = (primData & 0x2uL);//...

    //...
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
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curTriangle.x0, g_curTriangle.y0);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[2], g_curTriangle.x1, g_curTriangle.y1);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[3], g_curTriangle.x2, g_curTriangle.y2);
    //color = pPrimData[0];

    //...
}

/// <summary>Draw shaded triangle</summary>
void cmTriangleS(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 6x32 - CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx 00BbGgRr YvtxXvtx

    // vertices
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curTriangle.x0, g_curTriangle.y0);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[3], g_curTriangle.x1, g_curTriangle.y1);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[5], g_curTriangle.x2, g_curTriangle.y2);
    //color = pPrimData[0];
    //color1 = pPrimData[2];
    //color2 = pPrimData[4];

    //...
}

/// <summary>Draw textured triangle</summary>
void cmTriangleTx(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 7x32 - CmBbGgRr YvtxXvtx ClutYcXc YvtxXvtx TxpgYcXc YvtxXvtx 0000YcXc

    // vertices
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curTriangle.x0, g_curTriangle.y0);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[3], g_curTriangle.x1, g_curTriangle.y1);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[5], g_curTriangle.x2, g_curTriangle.y2);
    // texture coords
    PrimitiveFactory::extractPrimitivePos16(pPrimData[2], g_curTriangleTxCoords.x0, g_curTriangleTxCoords.y0);
    PrimitiveFactory::extractPrimitivePos16(pPrimData[4], g_curTriangleTxCoords.x1, g_curTriangleTxCoords.y1);
    PrimitiveFactory::extractPrimitivePos16(pPrimData[6], g_curTriangleTxCoords.x2, g_curTriangleTxCoords.y2);
    //color = pPrimData[0];

    //...

    //... si texture 4bit ou 8bit, lire Clut, sinon ignorer
    //... si raw, ignorer couleur
}

/// <summary>Draw shaded-textured triangle</summary>
void cmTriangleSTx(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 9x32 - CmBbGgRr YvtxXvtx ClutYcXc 00BbGgRr YvtxXvtx TxpgYcXc 00BbGgRr YvtxXvtx 0000YcXc

    // vertices
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curTriangle.x0, g_curTriangle.y0);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[4], g_curTriangle.x1, g_curTriangle.y1);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[7], g_curTriangle.x2, g_curTriangle.y2);
    // texture coords
    PrimitiveFactory::extractPrimitivePos16(pPrimData[2], g_curTriangleTxCoords.x0, g_curTriangleTxCoords.y0);
    PrimitiveFactory::extractPrimitivePos16(pPrimData[5], g_curTriangleTxCoords.x1, g_curTriangleTxCoords.y1);
    PrimitiveFactory::extractPrimitivePos16(pPrimData[8], g_curTriangleTxCoords.x2, g_curTriangleTxCoords.y2);
    //color = pPrimData[0];
    //color1 = pPrimData[3];
    //color2 = pPrimData[6];

    //...
}

/// <summary>Draw flat quad</summary>
void cmQuad(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 5x32 - CmBbGgRr YvtxXvtx YvtxXvtx YvtxXvtx YvtxXvtx

    // vertices
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curQuad.x0, g_curQuad.y0);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[2], g_curQuad.x1, g_curQuad.y1);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[3], g_curQuad.x2, g_curQuad.y2);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[4], g_curQuad.x3, g_curQuad.y3);
    //color = pPrimData[0];

    //...
}

/// <summary>Draw shaded quad</summary>
void cmQuadS(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 8x32 - CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx 00BbGgRr YvtxXvtx 00BbGgRr YvtxXvtx

    // vertices
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curQuad.x0, g_curQuad.y0);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[3], g_curQuad.x1, g_curQuad.y1);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[5], g_curQuad.x2, g_curQuad.y2);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[7], g_curQuad.x3, g_curQuad.y3);
    //color = pPrimData[0];
    //color1 = pPrimData[2];
    //color2 = pPrimData[4];
    //color2 = pPrimData[6];

    //...
}

/// <summary>Draw textured quad</summary>
void cmQuadTx(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 9x32 - CmBbGgRr YvtxXvtx ClutYcXc YvtxXvtx TxpgYcXc YvtxXvtx 0000YcXc YvtxXvtx 0000YcXc

    // vertices
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curQuad.x0, g_curQuad.y0);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[3], g_curQuad.x1, g_curQuad.y1);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[5], g_curQuad.x2, g_curQuad.y2);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[7], g_curQuad.x3, g_curQuad.y3);
    // texture coords
    PrimitiveFactory::extractPrimitivePos16(pPrimData[2], g_curQuadTxCoords.x0, g_curQuadTxCoords.y0);
    PrimitiveFactory::extractPrimitivePos16(pPrimData[4], g_curQuadTxCoords.x1, g_curQuadTxCoords.y1);
    PrimitiveFactory::extractPrimitivePos16(pPrimData[6], g_curQuadTxCoords.x2, g_curQuadTxCoords.y2);
    PrimitiveFactory::extractPrimitivePos16(pPrimData[8], g_curQuadTxCoords.x3, g_curQuadTxCoords.y3);
    //color = pPrimData[0];

    //...
}

/// <summary>Draw shaded-textured quad</summary>
void cmQuadSTx(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 12x32 - CmBbGgRr YvtxXvtx ClutYcXc 00BbGgRr YvtxXvtx TxpgYcXc 00BbGgRr YvtxXvtx 0000YcXc 00BbGgRr YvtxXvtx 0000YcXc

    // vertices
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curQuad.x0, g_curQuad.y0);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[4], g_curQuad.x1, g_curQuad.y1);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[7], g_curQuad.x2, g_curQuad.y2);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[10], g_curQuad.x3, g_curQuad.y3);
    // texture coords
    PrimitiveFactory::extractPrimitivePos16(pPrimData[2], g_curQuadTxCoords.x0, g_curQuadTxCoords.y0);
    PrimitiveFactory::extractPrimitivePos16(pPrimData[5], g_curQuadTxCoords.x1, g_curQuadTxCoords.y1);
    PrimitiveFactory::extractPrimitivePos16(pPrimData[8], g_curQuadTxCoords.x2, g_curQuadTxCoords.y2);
    PrimitiveFactory::extractPrimitivePos16(pPrimData[11], g_curQuadTxCoords.x3, g_curQuadTxCoords.y3);
    //color = pPrimData[0];
    //color1 = pPrimData[3];
    //color2 = pPrimData[6];
    //color2 = pPrimData[9];

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
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curLine.x0, g_curLine.y0);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[2], g_curLine.x1, g_curLine.y1);
    //color = pPrimData[0];

    //...
}

/// <summary>Draw shaded line</summary>
void cmLineS(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 4x32 - CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx

    // vertices
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curLine.x0, g_curLine.y0);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[3], g_curLine.x1, g_curLine.y1);
    //color = pPrimData[0];
    //color1 = pPrimData[2];

    //...
}

/// <summary>Draw flat poly-line</summary>
void cmPolyLine(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // [3-254]x32 - CmBbGgRr YvtxXvtx YvtxXvtx [...] 55555555

    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curLine.x0, g_curLine.y0);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[2], g_curLine.x1, g_curLine.y1);
    //color = pPrimData[0];
    //... draw

    pPrimData = &pPrimData[3]; // use pointer as iterator (faster than indexes)
    for (int i = 3; i <= PLINE_MAX_LEN && (*pPrimData & 0xF000F000) != 0x50005000; ++i)
    {
        g_curLine.x0 = g_curLine.x1;
        g_curLine.y0 = g_curLine.y1;
        PrimitiveFactory::extractPrimitivePos32(*pPrimData, g_curLine.x1, g_curLine.y1);
        ++pPrimData;
        //... draw
    }
}

/// <summary>Draw shaded poly-line</summary>
void cmPolyLineS(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // [4-255]x32 - CmBbGgRr YvtxXvtx 00BbGgRr YvtxXvtx [...] 55555555

    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curLine.x0, g_curLine.y0);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[3], g_curLine.x1, g_curLine.y1);
    //color = pPrimData[0];
    //color1 = pPrimData[2];
    //... draw

    pPrimData = &pPrimData[4]; // use pointer as iterator (faster than indexes)
    for (int i = 4; i <= PLINE_S_MAX_LEN && (*pPrimData & 0xF000F000) != 0x50005000; i += 2)
    {
        //color = color1
        //color1 = *pPrimData;
        ++pPrimData;

        g_curLine.x0 = g_curLine.x1;
        g_curLine.y0 = g_curLine.y1;
        PrimitiveFactory::extractPrimitivePos32(*pPrimData, g_curLine.x1, g_curLine.y1);
        ++pPrimData;
        //... draw
    }
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
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curRect.x, g_curRect.y);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[2], g_curRect.width, g_curRect.height);
    //color = pPrimData[0];

    //...
}

/// <summary>Draw 1x1 rectangle</summary>
void cmTile1(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 2x32 - CmBbGgRr YvtxXvtx

    // coords
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curRect.x, g_curRect.y);
    g_curRect.width = g_curRect.height = 1;
    //color = pPrimData[0];

    //...
}

/// <summary>Draw 8x8 rectangle</summary>
void cmTile8(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 2x32 - CmBbGgRr YvtxXvtx

    // coords
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curRect.x, g_curRect.y);
    g_curRect.width = g_curRect.height = 8;
    //color = pPrimData[0];

    //...
}

/// <summary>Draw 16x16 rectangle</summary>
void cmTile16(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 2x32 - CmBbGgRr YvtxXvtx

    // coords
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curRect.x, g_curRect.y);
    g_curRect.width = g_curRect.height = 16;
    //color = pPrimData[0];

    //...
}

/// <summary>Draw custom-sized sprite</summary>
void cmSprite(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 4x32 - CmBbGgRr YvtxXvtx ClutYcXc YhgtXwid

    // coords
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curRect.x, g_curRect.y);
    PrimitiveFactory::extractPrimitivePos32(pPrimData[3], g_curRect.width, g_curRect.height);
    // texture coords
    PrimitiveFactory::extractPrimitivePos16(pPrimData[2], g_curRectTxCoords.x, g_curRectTxCoords.y);
    //color = pPrimData[0];

    //...
}

/// <summary>Draw 1x1 sprite</summary>
void cmSprite1(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 3x32 - CmBbGgRr YvtxXvtx ClutYcXc

    // coords
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curRect.x, g_curRect.y);
    g_curRect.width = g_curRect.height = 1;
    // texture coords
    PrimitiveFactory::extractPrimitivePos16(pPrimData[2], g_curRectTxCoords.x, g_curRectTxCoords.y);
    //color = pPrimData[0];

    //...
}

/// <summary>Draw 8x8 sprite</summary>
void cmSprite8(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 3x32 - CmBbGgRr YvtxXvtx ClutYcXc

    // coords
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curRect.x, g_curRect.y);
    g_curRect.width = g_curRect.height = 8;
    // texture coords
    PrimitiveFactory::extractPrimitivePos16(pPrimData[2], g_curRectTxCoords.x, g_curRectTxCoords.y);
    //color = pPrimData[0];

    //...
}

/// <summary>Draw 16x16 sprite</summary>
void cmSprite16(unsigned char* pData)
{
    unsigned long *pPrimData = ((unsigned long *)pData); // 3x32 - CmBbGgRr YvtxXvtx ClutYcXc

    // coords
    PrimitiveFactory::extractPrimitivePos32(pPrimData[1], g_curRect.x, g_curRect.y);
    g_curRect.width = g_curRect.height = 16;
    // texture coords
    PrimitiveFactory::extractPrimitivePos16(pPrimData[2], g_curRectTxCoords.x, g_curRectTxCoords.y);
    //color = pPrimData[0];

    //...
}
