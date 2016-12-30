/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   primitive_factory.h
Description : drawing primitive factory
*******************************************************************************/
#ifndef _PRIMITIVE_FACTORY_H
#define _PRIMITIVE_FACTORY_H

#include "video_memory.h"

#define PRIMITIVE_NUMBER 0xE8 // 0x00 - 0xE7
#define PRIM_NO_OPERATION_ID 0
#define PRIM_GEOMETRY_MIN_ID 0x20
#define PRIM_GEOMETRY_MAX_ID 0x7F

// data types
typedef unsigned long gpucmd_t;
typedef struct PRIMTABLEROW // primitive command
{
    long size; // number of 32-bit blocks
    void(*command)(unsigned char*); // function to call to process primitive
} primcmd_row_t;


// Drawing primitive factory
class PrimitiveFactory
{
private:
    // GPU operations
    static gpucmd_t s_gpuCommand;
    static unsigned long s_gpuMemCache[256]; // memory cache
    static long s_gpuDataCount;              // data set length
    static long s_gpuDataProcessed;          // current number of values cached

public:
    /// <summary>Initialize primitive factory</summary>
    static inline void init()
    {
        s_gpuCommand = PRIM_NO_OPERATION_ID;
    }

    /// <summary>Process chunk of display data (normal mode)</summary>
    /// <param name="writeModeRef">Reference to VRAM write mode</param>
    /// <param name="pDwMem">Pointer to chunk of data (source)</param>
    /// <param name="size">Memory chunk size</param>
    /// <param name="pDest">Destination gdata pointer</param>
    /// <param name="pI">Counter pointer</param>
    /// <returns>Indicator if VRAM data to write</returns>
    static bool processDisplayData(loadmode_t& writeModeRef, unsigned long* pDwMem, int size, unsigned long* pDest, int* pI);

    /// <summary>Process single primitive (for testing purpose)</summary>
    /// <param name="pData">Primitive raw data</param>
    /// <param name="len">Primitive data length (number of 32bits blocks)</param>
    static void processSinglePrimitive(unsigned char* pData, int len);

    /// <summary>Extract command bits from display data</summary>
    /// <param name="gdata">Display data (first block)</param>
    /// <returns>Command</returns>
    static inline unsigned long extractPrimitiveCommand(unsigned long gdata)
    {
        return ((gdata >> 24) & 0x0FFuL);
    }
    /// <summary>Check if shape uses blending (display data)</summary>
    /// <param name="gdata">Display data (first block)</param>
    /// <returns>Blending enabled (true) or raw (false)</returns>
    static inline bool isShapeBlended(unsigned long gdata)
    {
        return ((gdata & 0x01000000uL) == 0uL);
    }
    /// <summary>Check if shape is opaque (display data)</summary>
    /// <param name="gdata">Display data (first block)</param>
    /// <returns>Opaque (true) or semi-transparent (false)</returns>
    static inline bool isShapeOpaque(unsigned long gdata)
    {
        return ((gdata & 0x02000000uL) == 0uL);
    }
    /// <summary>Extract X/Y coords from display data half-block (16 bits: YyXx)</summary>
    /// <param name="gdata">Display data block</param>
    /// <param name="outX">X coord destination</param>
    /// <param name="outY">Y coord destination</param>
    static inline void extractPrimitivePos16(unsigned long gdata, long& outX, long& outY)
    {
        outX = (gdata & 0x0FFuL); // no <short*> pointer used, to avoid little/big-endian potential problems
        outY = ((gdata >> 8) & 0x0FFuL);
    }
    /// <summary>Extract X/Y position from display data block (32 bits: YyyyXxxx)</summary>
    /// <param name="gdata">Display data block</param>
    /// <param name="outX">X position destination</param>
    /// <param name="outY">Y position destination</param>
    static inline void extractPrimitivePos32(unsigned long gdata, long& outX, long& outY)
    {
        outX = (gdata & 0x0FFFFuL); // no <short*> pointer used, to avoid little/big-endian potential problems
        outY = (gdata >> 16);
    }
};

#endif
