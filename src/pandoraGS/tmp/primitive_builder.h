/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   primitive_builder.h
Description : drawing primitive factory
*******************************************************************************/
#ifndef _PRIMITIVE_BUILDER_H
#define _PRIMITIVE_BUILDER_H

#include "timer.h"
#include "video_memory.h"
#include "status_register.h"

#define PRIMITIVE_NUMBER 256 // 0x00 - 0xFF
#define PRIM_NO_OPERATION_ID 0
#define PRIM_GEOMETRY_MIN_ID 0x20
#define PRIM_GEOMETRY_MAX_ID 0x7F
#define PRIM_GOURAUD_LINE_MIN_ID 0x50

// data types
typedef unsigned long gpucmd_t;
typedef struct PRIMTABLEROW 
{
    void(*command)(unsigned char*); // function to call to process primitive
    long size; // number of 32-bit blocks
} primtable_row_t;

// Drawing primitive factory
class PrimitiveBuilder
{
private:
    // pending operation (used if end of data chain -> data blocks still required)
    static gpucmd_t s_pendingCommand;        // primitive ID
    static unsigned long s_pendingData[256]; // data container
    static long s_pendingDataLen;            // data set length
    static long s_copiedDataLen;             // data blocks already copied

public:
    /// <summary>Initialize primitive factory</summary>
    static inline void init()
    {
        s_pendingCommand = PRIM_NO_OPERATION_ID;
    }

    /// <summary>Process chunk of display data (normal mode)</summary>
    /// <param name="writeModeRef">Reference to VRAM write mode</param>
    /// <param name="pMem">Pointer to chunk of data (source)</param>
    /// <param name="len">Memory chunk size</param>
    /// <param name="latestGdata">Destination gdata reference</param>
    /// <param name="curI">Counter reference</param>
    /// <returns>Data remaining (or end of chunk)</returns>
    static bool processDisplayData(loadmode_t& writeModeRef, unsigned long* pMem, int len, unsigned long& latestGdata, int& curI);

    /// <summary>Process single primitive (for testing purpose)</summary>
    /// <param name="pData">Primitive raw data</param>
    /// <param name="len">Primitive data length (number of 32bits blocks)</param>
    static void processSinglePrimitive(unsigned long* pData, int len);

    /// <summary>Extract command bits from display data</summary>
    /// <param name="gdata">Display data (first block)</param>
    /// <returns>Command</returns>
    static inline unsigned long getPrimitiveId(unsigned long gdata)
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

    /// <summary>Check if primitive is processed or skipped</summary>
    /// <param name="cmd">Primitive ID</param>
    /// <returns>Processed true) or skipped (false)</returns>
    static inline bool isPrimitiveProcessed(gpucmd_t& cmd)
    {
        return (Timer::isPeriodSkipped() == false || cmd < PRIM_GEOMETRY_MIN_ID || cmd > PRIM_GEOMETRY_MAX_ID);
    }
};

#endif
