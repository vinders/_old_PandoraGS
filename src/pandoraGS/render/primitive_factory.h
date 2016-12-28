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

#define PRIMITIVE_NUMBER 0xE8 // 0x00 - 0xE7

// data types
typedef unsigned long gpucmd_t;
typedef struct PRIMTABLEROW // primitive command
{
    long size; // number of 32-bit blocks
    void(*command)(unsigned char*); // function to call to process primitive
    void(*skip)(unsigned char*);    // function to call to skip primitive
} primcmd_row_t;


// drawing primitive factory
namespace PrimitiveFactory
{
    /// <summary>Process chunk of display data (normal mode)</summary>
    /// <param name="pDwMem">Pointer to chunk of data (source)</param>
    /// <param name="size">Memory chunk size</param>
    /// <param name="pDest">Destination gdata pointer</param>
    /// <param name="pI">Counter pointer</param>
    /// <returns>Indicator if VRAM data to write</returns>
    bool processDisplayData(unsigned long* pDwMem, int size, unsigned long* pDest, int* pI);

    /// <summary>Process single primitive</summary>
    /// <param name="pData">Primitive raw data</param>
    /// <param name="len">Primitive data length (number of 32bits blocks)</param>
    void processSinglePrimitive(unsigned char* pData, int len);
}

#endif
