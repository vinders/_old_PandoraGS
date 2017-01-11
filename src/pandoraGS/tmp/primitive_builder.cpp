/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   primitive_builder.cpp
Description : drawing primitive factory
*******************************************************************************/
#include <cstdlib>
#include <cstring>
using namespace std;
#include "config.h"
#include "primitive_builder.h"
#include "primitive_attr.h"
#include "primitive_poly.h"
#include "primitive_line.h"
#include "primitive_rect.h"
using namespace Primitive;

#define NI NULL  // non-implemented commands
#define NOP NULL // no-operation command

#define CMDx4(cmd,size)  {cmd,size},{cmd,size},{cmd,size},{cmd,size}
#define CMDx8(cmd,size)  CMDx4(cmd,size),CMDx4(cmd,size)
#define CMDx16(cmd,size) CMDx8(cmd,size),CMDx8(cmd,size)
#define CMDx24(cmd,size) CMDx8(cmd,size),CMDx8(cmd,size),CMDx8(cmd,size)
#define CMDx32(cmd,size) CMDx16(cmd,size),CMDx16(cmd,size)

// pending operation (used if end of data chain -> data blocks still required)
gpucmd_t PrimitiveBuilder::s_pendingCommand;        // primitive ID
unsigned long PrimitiveBuilder::s_pendingData[256]; // data container
long PrimitiveBuilder::s_pendingDataLen;            // data set length
long PrimitiveBuilder::s_copiedDataLen;             // data blocks already copied


// primitive factory table
const primtable_row_t c_pPrimTable[PRIMITIVE_NUMBER] =
{
    // GENERAL : 00 - 03
    { NOP, 0L }, { cmClearCache, 1L }, { cmFillArea, fill_area_t::size() }, { cmBufferWait, 1L },
    // not implemented : 04 - 1E
    CMDx24(NI,0), {NI,0}, {NI,0}, {NI,0},
    // IRQ : 1F
    { cmGpuIrq, 1L },
 
    // POLY - flat-shaded triangle : 20 - 27
    CMDx4(cmTriangleF, poly_f3_t::size()), CMDx4(cmTriangleFT, poly_ft3_t::size()), 
    // POLY - flat-shaded quad : 28 - 2F
    CMDx4(cmQuadF, poly_f4_t::size()),     CMDx4(cmQuadFT, poly_ft4_t::size()),
    // POLY - gouraud-shaded triangle : 30 - 37
    CMDx4(cmTriangleG, poly_g3_t::size()), CMDx4(cmTriangleGT, poly_gt3_t::size()), 
    // POLY - gouraud-shaded quad : 38 - 3F
    CMDx4(cmQuadG, poly_g4_t::size()),     CMDx4(cmQuadGT, poly_gt4_t::size()),
 
    // LINE - flat-shaded : 40 - 4F
    CMDx8(cmLineF, line_f2_t::size()),     CMDx8(cmPolyLineF, line_fp_t::maxSize()),
    // LINE - gouraud-shaded : 50 - 5F
    CMDx8(cmLineG, line_g2_t::size()),     CMDx8(cmPolyLineG, line_gp_t::maxSize()),
 
    // RECT - custom-sized tile/sprite : 60 - 67
    CMDx4(cmTile, tile_f_t::size()),       CMDx4(cmSprite, sprite_f_t::size()), 
    // RECT - 1x1 tile/sprite : 68 - 6F
    CMDx4(cmTile1, tile_f1_t::size()),     CMDx4(cmSprite1, sprite_f1_t::size()),
    // RECT - 8x8 tile/sprite : 70 - 77
    CMDx4(cmTile8, tile_f8_t::size()),     CMDx4(cmSprite8, sprite_f8_t::size()),
    // RECT - 16x16 tile/sprite : 78 - 7F
    CMDx4(cmTile16, tile_f16_t::size()),   CMDx4(cmSprite16, sprite_f16_t::size()),
 
    // IMAGE - move : 80 - 9F
    CMDx32(cmImageMove, img_move_t::size()),
    // IMAGE - load : A0 - BF
    CMDx32(cmImageLoad, img_load_t::size()),
    // IMAGE - store : C0 - DF
    CMDx32(cmImageStore, img_store_t::size()),

    // ATTRIBUTES : E0 - E7
    { NI/*?*/, 0 },                             { cmTexPage, attr_texpage_t::size() }, 
    { cmTexWindow, attr_texwin_t::size() },     { cmDrawAreaStart, attr_drawarea_t::size() },
    { cmDrawAreaEnd, attr_drawarea_t::size() }, { cmDrawOffset, attr_drawoffset_t::size() }, 
    { cmMaskBit, attr_stpmask_t::size() },      { NI/*?*/, 0 },

    // not implemented : E8 - FF
    CMDx24(NI,0)
};

// -- PRIMITIVE FACTORY -- -----------------------------------------------------

/// <summary>Process chunk of display data (normal mode)</summary>
/// <param name="vramWriteMode">Reference to VRAM write mode</param>
/// <param name="pMem">Pointer to chunk of data (source)</param>
/// <param name="len">Memory chunk size</param>
/// <param name="latestGdata">Destination gdata reference</param>
/// <param name="curI">Counter reference</param>
/// <returns>Data remaining (or end of chunk)</returns>
bool PrimitiveBuilder::processDisplayData(loadmode_t& writeModeRef, unsigned long* pMem, int len, unsigned long& latestGdata, int& curI)
{
    if (writeModeRef != Loadmode_normal) // no data left -> end of data transfer
        return false;

    unsigned long gdata;
    int i = curI;
    gpucmd_t curCommand;
    long curSize;

    // pending primitive part (from previous memory IO) -> complete it
    if (s_pendingCommand != PRIM_NO_OPERATION_ID && s_copiedDataLen > 0) 
        goto PRIM_PENDING_COPY;

    // new primitives
    while (i < len)
    {
        if (writeModeRef == Loadmode_vramTransfer) // back to VRAM transfer mode -> end function
        {
            latestGdata = gdata;
            curI = i;
            return true; // more data to process
        }

        // read data block
        gdata = *pMem;
        curCommand = getPrimitiveId(*pMem);
        curSize = c_pPrimTable[curCommand].size;

        if (c_pPrimTable[curCommand].command != NI) // implemented
        {
            // poly-line -> copy + process if complete
            if (curSize >= POLYLINE_MAX_DATA_LEN)
            {
                s_copiedDataLen = 0;
                s_pendingCommand = curCommand;
                s_pendingDataLen = curSize;

                PRIM_PENDING_COPY: // pending primitive part (from previous memory IO) -> complete it from here

                while (i < len && s_copiedDataLen < s_pendingDataLen)
                {
                    gdata = *pMem;
                    s_pendingData[s_copiedDataLen] = gdata;
                    ++s_copiedDataLen; ++pMem; ++i;

                    if (s_pendingDataLen >= POLYLINE_MAX_DATA_LEN // poly-line
                     && ((s_pendingCommand < PRIM_GOURAUD_LINE_MIN_ID  && isFPolyLineEndable(s_copiedDataLen)) // flat
                     || (s_pendingCommand >= PRIM_GOURAUD_LINE_MIN_ID && isGPolyLineEndable(s_copiedDataLen)))) // gouraud
                    {
                        if (isPolyLineEndCode(gdata)) // termination code -> force ending of data set
                            s_copiedDataLen = s_pendingDataLen;
                    }
                }
                // primitive entirely copied -> process
                if (s_copiedDataLen >= s_pendingDataLen) 
                {
                    if (isPrimitiveProcessed(s_pendingCommand))
                        c_pPrimTable[s_pendingCommand].command((unsigned char*)s_pendingData);
                    s_pendingCommand = PRIM_NO_OPERATION_ID;
                }
            }
            // fixed-sized primitive
            else 
            {
                if (i + curSize <= len) 
                {
                    // process (if frame not skipped)
                    if (isPrimitiveProcessed(curCommand))
                        c_pPrimTable[curCommand].command((unsigned char*)pMem);
                    if (Config::misc_emuFixBits & 0x0001 || Config::getCurrentProfile()->getFix(CFG_FIX_FAKE_GPU_BUSY)) // 'GPU busy' hack
                        StatusRegister::initFakeBusySequence();

                    // memory offset
                    pMem += curSize - 1;
                    gdata = *pMem;
                    ++pMem;
                    i += curSize;
                }
                else // primitive too large for memory chunk 
                {
                    // copy available part of primitive
                    s_pendingCommand = curCommand;
                    s_pendingDataLen = curSize;
                    s_copiedDataLen = len - i;
                    memcpy(s_pendingData, pMem, s_copiedDataLen);
                    // memory offset
                    pMem += s_copiedDataLen;
                    i += s_copiedDataLen;
                    gdata = s_pendingData[s_copiedDataLen - 1];
                }
            }
        }
        else // not implemented -> skip
        {
            ++pMem;
            ++i;
        }
    }

    latestGdata = gdata;
    return false; // no more data
}

/// <summary>Process single primitive (for testing purpose)</summary>
/// <param name="pData">Primitive raw data</param>
/// <param name="len">Primitive data length (number of 32bits blocks)</param>
void PrimitiveBuilder::processSinglePrimitive(unsigned long* pData, int len)
{
    if (len > 0)
    {
        gpucmd_t curCommand = getPrimitiveId(pData[0]);
        if (c_pPrimTable[curCommand].command != NI)
            c_pPrimTable[curCommand].command((unsigned char*)pData);
    }
}
