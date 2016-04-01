/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   psx_core_memory.cpp
Description : playstation virtual video memory unit
*******************************************************************************/
#include <string>
using namespace std;
#include "psx_core_memory.h"

extern bool g_isZincEmu; // zinc interface emulation


/// <summary>Create empty instance</summary>
PsxCoreMemory::PsxCoreMemory()
{
    #ifdef _WINDOWS
    gen_hWindow = NULL;
    #endif
    sync_localize = LocalizationMode_Ntsc;
    sync_isInterlaced = false;
    mem_vramImage.pData = NULL;

    // native display widths
    ps_displayWidths[0] = 256;
    ps_displayWidths[1] = 320;
    ps_displayWidths[2] = 512;
    ps_displayWidths[3] = 640;
    ps_displayWidths[4] = 368;
    ps_displayWidths[5] = 384;
    ps_displayWidths[6] = 512;
    ps_displayWidths[7] = 640;
    // data transaction init
    mem_gpuDataTransaction = GPUDATA_INIT;
}

/// <summary>Destroy memory instance</summary>
PsxCoreMemory::~PsxCoreMemory()
{
    if (mem_vramImage.pData != NULL)
        free(mem_vramImage.pData);
}

/// <summary>Initialize memory instance values</summary>
/// <exception cref="std::exception">Memory allocation failure</exception>
void PsxCoreMemory::initMemory()
{
    // define VRAM size (standard or zinc emu)
    mem_vramImage.bufferSize = PSXVRAM_SIZE * 1024;
    if (g_isZincEmu)
        mem_vramImage.bufferSize = ZINCVRAM_SIZE * 1024;
    // alloc double buffered PSX VRAM image
    mem_vramImage.length = (mem_vramImage.bufferSize * 2) + (PSXVRAM_SECURE_EXTRA * 1024); // extra security for drawing functions
    if ((mem_vramImage.pData = (unsigned char*)malloc(mem_vramImage.length)) == NULL)
        throw new std::exception("VRAM allocation failure");

    // initialize VRAM
    memset(mem_vramImage.pData, 0x0, mem_vramImage.length);
    mem_vramImage.oddFrame = 0;
    mem_vramImage.pByte = mem_vramImage.pData + (PSXVRAM_SECURE_OFFSET * 1024); // start limit
    mem_vramImage.pWord = (unsigned short*)mem_vramImage.pByte;
    mem_vramImage.pDword = (unsigned long*)mem_vramImage.pByte;
    mem_vramImage.pEOM = mem_vramImage.pWord + mem_vramImage.bufferSize; // end limit

    // initialize VRAM load
    mem_vramReadMode = DR_NORMAL;
    memset(&mem_vramRead, 0x0, sizeof(VramLoad_t));
    mem_vramWriteMode = DR_NORMAL;
    memset(&mem_vramWrite, 0x0, sizeof(VramLoad_t));

    // initialize status and information
    memset(st_pGpuDrawInfo, 0x0, DRAWINFO_SIZE * sizeof(unsigned long));
    memset(st_pStatusControl, 0x0, STATUSCTRL_SIZE * sizeof(unsigned long));
    st_statusReg = GPUSTATUS_INIT;
    setStatus(GPUSTATUS_IDLE);
    setStatus(GPUSTATUS_READYFORCOMMANDS);
}
