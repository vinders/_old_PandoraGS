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
#include "config.h"
#include "memory_dispatcher.h"
#include "primitive_factory.h"

#define NI cmVoid // non-implemented commands

namespace PrimitiveFactory
{
    // - private namespace data -
    gpucmd_t g_gpuCommand;
    unsigned long g_gpuMemCache[256]; // memory cache
    long g_gpuDataCount;              // data set length
    long g_gpuDataProcessed;          // current number of values cached

    // - private namespace methods -

    /// <summary>Extract command bits from display data</summary>
    /// <param name="gdata">Display data (first block)</param>
    /// <returns>Command</returns>
    inline unsigned long extractPrimitiveCommand(unsigned long gdata)
    {
        return ((gdata >> 24) & 0x0FFu);
    }

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


    // primitive factory table
    const primcmd_row_t c_pPrimTable[PRIMITIVE_NUMBER] =
    {
        // GENERAL : 00 - 03
        {0,NI,NI}, { 0, cmClearCache, cmClearCache }, { 3, cmBlankFill, cmBlankFill }, {0,NI,NI},
        // not implemented : 04 - 1F
        {0,NI,NI}, {0,NI,NI}, {0,NI,NI}, {0,NI,NI},
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
}

// -- PRIMITIVE FACTORY -- -----------------------------------------------------

/// <summary>Process chunk of display data (normal mode)</summary>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="size">Memory chunk size</param>
/// <param name="pDest">Destination gdata pointer</param>
/// <param name="pI">Counter pointer</param>
/// <returns>Indicator if VRAM data to write</returns>
bool PrimitiveFactory::processDisplayData(unsigned long* pDwMem, int size, unsigned long* pDest, int* pI)
{
    unsigned long gdata = 0;
    gpucmd_t command;
    int i = *pI;

    if (MemoryDispatcher::mem_vramWriter.mode == Loadmode_normal) // no VRAM transfer
    {
        // copy and process data
        while (i < size)
        {
            // back to VRAM transfer mode -> end function
            if (MemoryDispatcher::mem_vramWriter.mode == Loadmode_vramTransfer)
            {
                *pDest = gdata;
                *pI = i;
                return true; // more data to process
            }

            gdata = *pDwMem++;
            i++;
            // new data set -> identify command + copy first value
            if (g_gpuDataCount == 0)
            {
                command = extractPrimitiveCommand(gdata);
                if (command < PRIMITIVE_NUMBER && (g_gpuDataCount = c_pPrimTable[command].size) > 0)
                {
                    g_gpuCommand = command;
                    g_gpuMemCache[0] = gdata;
                    g_gpuDataProcessed = 1;
                }
                else
                    continue;
            }
            // same data set -> copy current value
            else
            {
                g_gpuMemCache[g_gpuDataProcessed] = gdata;
                if (g_gpuDataCount > 128)
                {
                    if ((g_gpuDataCount == 254 && g_gpuDataProcessed >= 3)
                        || (g_gpuDataCount == 255 && g_gpuDataProcessed >= 4 && !(g_gpuDataProcessed & 1)))
                    {
                        // termination code for poly-lines -> force ending of data set
                        if ((g_gpuMemCache[g_gpuDataProcessed] & 0xF000F000) == 0x50005000) // code should be 0x55555555, but a few games (such as wild arms 2) use 0x50005000
                            g_gpuDataProcessed = g_gpuDataCount - 1;
                    }
                }
                g_gpuDataProcessed++;
            }
            // end of data set -> process cached data
            if (g_gpuDataProcessed == g_gpuDataCount)
            {
                g_gpuDataCount = g_gpuDataProcessed = 0;

                // process data set
                if (Timer::isPeriodSkipped())
                    c_pPrimTable[command].skip((unsigned char*)g_gpuMemCache);
                else
                    c_pPrimTable[command].command((unsigned char*)g_gpuMemCache);

                // 'GPU busy' hack (while processing data)
                if (Config::misc_emuFixBits & 0x0001 || Config::getCurrentProfile()->getFix(CFG_FIX_FAKE_GPU_BUSY))
                    MemoryDispatcher::st_busyEmuSequence = 4;
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
    if (command < PRIMITIVE_NUMBER && (g_gpuDataCount = c_pPrimTable[command].size) > 0)
    {
        g_gpuCommand = command;
        memset(g_gpuMemCache, 0, 256 * sizeof(unsigned long));
        unsigned long* srcMem = (unsigned long*)pData;
        for (int i = 0; i < len; ++i) // test functions may pass data shorter than 256 -> no memcpy
        {
            g_gpuMemCache[i] = *srcMem;
            if ((*srcMem & 0xF000F000) == 0x50005000) 
                break; // termination code for poly-lines
            ++srcMem;
        }
        g_gpuDataProcessed = len;
        c_pPrimTable[command].command((unsigned char*)g_gpuMemCache);
    }
}


// -- PRIMITIVE GENERAL COMMANDS -- --------------------------------------------

/// <summary>Non-implemented command</summary>
void PrimitiveFactory::cmVoid(unsigned char* pData)
{
}

/// <summary>Clear texture cache</summary>
void PrimitiveFactory::cmClearCache(unsigned char* pData)
{

}

/// <summary>Fill space (without checking draw areas)</summary>
void PrimitiveFactory::cmBlankFill(unsigned char* pData)
{

}

/// <summary>Move image (vram to vram)</summary>
void PrimitiveFactory::cmImageMove(unsigned char* pData)
{

}

/// <summary>Load image (cpu to vram)</summary>
void PrimitiveFactory::cmImageLoad(unsigned char* pData)
{

}

/// <summary>Store image (vram to central memory)</summary>
void PrimitiveFactory::cmImageStore(unsigned char* pData)
{

}


// -- DRAWING ATTRIBUTE COMMANDS -- --------------------------------------------

/// <summary>Set texture page</summary>
void PrimitiveFactory::cmTexPage(unsigned char* pData)
{

}

/// <summary>Set texture window</summary>
void PrimitiveFactory::cmTexWindow(unsigned char* pData)
{

}

/// <summary>Set draw area start</summary>
void PrimitiveFactory::cmDrawAreaStart(unsigned char* pData)
{

}

/// <summary>Set draw area end</summary>
void PrimitiveFactory::cmDrawAreaEnd(unsigned char* pData)
{

}

/// <summary>Set draw offset</summary>
void PrimitiveFactory::cmDrawOffset(unsigned char* pData)
{

}

/// <summary>Set mask bit info</summary>
void PrimitiveFactory::cmMaskBit(unsigned char* pData)
{

}


// -- PRIMITIVE POLYGON COMMANDS -- --------------------------------------------

/// <summary>Draw flat triangle</summary>
void PrimitiveFactory::cmTriangle(unsigned char* pData)
{

}

/// <summary>Draw shaded triangle</summary>
void PrimitiveFactory::cmTriangleS(unsigned char* pData)
{

}

/// <summary>Draw textured triangle</summary>
void PrimitiveFactory::cmTriangleTx(unsigned char* pData)
{

}

/// <summary>Draw shaded-textured triangle</summary>
void PrimitiveFactory::cmTriangleSTx(unsigned char* pData)
{

}

/// <summary>Draw flat quad</summary>
void PrimitiveFactory::cmQuad(unsigned char* pData)
{

}

/// <summary>Draw shaded quad</summary>
void PrimitiveFactory::cmQuadS(unsigned char* pData)
{

}

/// <summary>Draw textured quad</summary>
void PrimitiveFactory::cmQuadTx(unsigned char* pData)
{

}

/// <summary>Draw shaded-textured quad</summary>
void PrimitiveFactory::cmQuadSTx(unsigned char* pData)
{

}


// -- PRIMITIVE LINE COMMANDS -- -----------------------------------------------

/// <summary>Draw flat line</summary>
void PrimitiveFactory::cmLine(unsigned char* pData)
{

}

/// <summary>Draw shaded line</summary>
void PrimitiveFactory::cmLineS(unsigned char* pData)
{

}

/// <summary>Draw flat poly-line</summary>
void PrimitiveFactory::cmPolyLine(unsigned char* pData)
{

}

/// <summary>Draw shaded poly-line</summary>
void PrimitiveFactory::cmPolyLineS(unsigned char* pData)
{

}

/// <summary>Skip poly-line</summary>
void PrimitiveFactory::skipPolyLine(unsigned char* pData)
{

}

/// <summary>Skip poly-line</summary>
void PrimitiveFactory::skipPolyLineS(unsigned char* pData)
{

}


// -- PRIMITIVE RECTANGLE COMMANDS -- ------------------------------------------

/// <summary>Draw custom-sized rectangle</summary>
void PrimitiveFactory::cmTile(unsigned char* pData)
{

}

/// <summary>Draw 1x1 rectangle</summary>
void PrimitiveFactory::cmTile1(unsigned char* pData)
{

}

/// <summary>Draw 8x8 rectangle</summary>
void PrimitiveFactory::cmTile8(unsigned char* pData)
{

}

/// <summary>Draw 16x16 rectangle</summary>
void PrimitiveFactory::cmTile16(unsigned char* pData)
{

}

/// <summary>Draw custom-sized sprite</summary>
void PrimitiveFactory::cmSprite(unsigned char* pData)
{

}

/// <summary>Draw 1x1 sprite</summary>
void PrimitiveFactory::cmSprite1(unsigned char* pData)
{

}

/// <summary>Draw 8x8 sprite</summary>
void PrimitiveFactory::cmSprite8(unsigned char* pData)
{

}

/// <summary>Draw 16x16 sprite</summary>
void PrimitiveFactory::cmSprite16(unsigned char* pData)
{

}
