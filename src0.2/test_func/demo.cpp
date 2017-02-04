/*******************************************************************************
PANDORAGS project - PS1 GPU driver test app
------------------------------------------------------------------------
File name :   demo.cpp
Description : test app - short rendering demo (to test primitives and shaders)
*******************************************************************************/
#include <cstdlib>
using namespace std;
#include "demo.h"
#include "psemu.h"

/// <summary>Process data sent to GPU status register</summary>
/// <param name="gdata">Status register command</param>
void writeStatus(unsigned long gdata)
{
    GPUwriteStatus(gdata);
}

/// <summary>Process and send chunk of data to video data register</summary>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="len">Memory chunk size</param>
void writeData(unsigned long* pData, int len)
{
    GPUwriteDataMem(pData, len);
}

/// <summary>Process and send chunk of only one block of data</summary>
/// <param name="pDataContainer">Pointer to chunk of data (destination)</param>
/// <param name="val">Value</param>
void writeDataBlock(unsigned long* pDataContainer, unsigned long val)
{
    pDataContainer[0] = val;
    GPUwriteDataMem(pDataContainer, 1);
}

///<summary>Play demo animation</summary>
void renderDemoAnimation()
{
    //...
}
