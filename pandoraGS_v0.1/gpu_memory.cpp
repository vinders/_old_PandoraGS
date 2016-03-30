/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   gpu_memory.cpp
    Description : memory management interface (PSEmu Pro spec)
*******************************************************************************/
using namespace std;
#include <string.h>

/* headers - globals/interface */
#include "globals.h"
#include "gpu_main.h"
#include "gpu_memory.h"

/* headers - config */
#include "config/config_manager.h"

/* headers - rendering */
#include "display/framerate.h"
#include "display/renderer.h"
#include "display/menu.h"


// GLOBAL VARIABLES ------------------------------------------------------------
/* plugin variables */
extern Renderer* gpu_gameRenderer;
extern unsigned long emuFixes;
extern bool zincEmulation;
#if _SYSTEM == _WINDOWS
    extern HWND  gpu_window;
#endif



// STATUS REGISTER CONTROL
//______________________________________________________________________________

/* process read request from GPU status register ---------------------------- */
unsigned long CALLBACK GPUreadStatus()
{
    if (gpu_gameRenderer == NULL) // security before init
    {
        unsigned long tempStatus = GPUSTATUS_INIT;
        unsetGpuStatus(tempStatus, GPUSTATUS_IDLE);
        unsetGpuStatus(tempStatus, GPUSTATUS_READYFORCOMMANDS);
        return tempStatus;
    }
    
    // odd/even-bit fix
    if(ConfigManager::getInstance()->bugFixes&FIX_ODDEVENBIT)
    {
        static int numRead = 0;
        if((numRead++) == 2) // interlace bit toggle - every second read
        {
            gpu_gameRenderer->gpuRawStatus ^= GPUSTATUS_ODDLINES; 
            numRead=0;
        }
    }
    // fake busy status fix -> busy/idle sequence (while drawing)
    if(gpu_gameRenderer->busySequence)
    {
        gpu_gameRenderer->busySequence--;
        if(gpu_gameRenderer->busySequence&1) // uneven value -> busy
        {
            unsetGpuStatus(gpu_gameRenderer->gpuRawStatus, GPUSTATUS_IDLE);
            unsetGpuStatus(gpu_gameRenderer->gpuRawStatus, GPUSTATUS_READYFORCOMMANDS);
        }
        else // even value -> idle
        {
            setGpuStatus(gpu_gameRenderer->gpuRawStatus, GPUSTATUS_IDLE);
            setGpuStatus(gpu_gameRenderer->gpuRawStatus, GPUSTATUS_READYFORCOMMANDS);
        }
    }
    
    return gpu_gameRenderer->gpuRawStatus;
}


/* process data sent to GPU status register --------------------------------- */
void CALLBACK GPUwriteStatus(unsigned long gdata)
{
    if (gpu_gameRenderer == NULL) // security before init
        return;
    if (gpu_gameRenderer->isApiSet() == false) // start thread-safe graphic APIs
        gpu_gameRenderer->setGraphicApi();
    
    // get command indicator
    unsigned long command = getGpuCommand(gdata);
    gpu_gameRenderer->statusControl[command] = gdata; // store command for freezing
    
    // process command
    switch (command)
    {
        case CMD_SETTRANSFERMODE:    gpu_gameRenderer->setTransferMode(gdata);    return;
        case CMD_GPUREQUEST:         gpu_gameRenderer->processRequest(gdata);     return;
        case CMD_TOGGLEDISPLAY:      gpu_gameRenderer->setDisplayState(gdata);    return;
        case CMD_SETDISPLAYPOSITION: gpu_gameRenderer->setDisplayPosition(gdata); return;
        case CMD_SETDISPLAYWIDTH:    
                        gpu_gameRenderer->consoleDisplay.setDisplayWidth(gdata);  return;
        case CMD_SETDISPLAYHEIGHT:   
                        if (gpu_gameRenderer->consoleDisplay.setDisplayHeight(gdata))
                            { /*! gpu_gameRenderer->updateDisplayIfChanged();*/ } 
                        return;
        case CMD_SETDISPLAYINFO:     gpu_gameRenderer->setDisplayInfo(gdata);     return;
        case CMD_RESETGPU:           gpu_gameRenderer->resetRenderer();           return;
    }
}



// DATA COMMUNICATION
//______________________________________________________________________________

/* read core data from emulated Vram ---------------------------------------- */
unsigned long CALLBACK GPUreadData()
{
    unsigned long bufferMem;
    GPUreadDataMem(&bufferMem, 1);
    return gpu_gameRenderer->gpuRawData;
}

/* read entire chunk of data from emulated Vram ----------------------------- */
void CALLBACK GPUreadDataMem(unsigned long * pMem, int size)
{
    if (gpu_gameRenderer == NULL) // security before init
        return;
    if (gpu_gameRenderer->inputMode != DR_VRAMTRANSFER) // check read mode
        return;
    
    unsetGpuStatus(gpu_gameRenderer->gpuRawStatus, GPUSTATUS_IDLE); // busy
    gpu_gameRenderer->readDataInput(pMem, size);
    setGpuStatus(gpu_gameRenderer->gpuRawStatus, GPUSTATUS_IDLE); // idle
}


/* write core data to emulated Vram ----------------------------------------- */
void CALLBACK GPUwriteData(unsigned long gdata)
{
    GPUwriteDataMem(&gdata, 1);
}

/* write entire chunk of data to emulated Vram ------------------------------ */
void CALLBACK GPUwriteDataMem(unsigned long * pMem, int size)
{
    if (gpu_gameRenderer == NULL) // security before init
        return;
    
    unsetGpuStatus(gpu_gameRenderer->gpuRawStatus, GPUSTATUS_IDLE); // busy
    unsetGpuStatus(gpu_gameRenderer->gpuRawStatus, GPUSTATUS_READYFORCOMMANDS);// not ready
    gpu_gameRenderer->writeDataOutput(pMem, size);
    setGpuStatus(gpu_gameRenderer->gpuRawStatus, GPUSTATUS_READYFORCOMMANDS); // ready
    setGpuStatus(gpu_gameRenderer->gpuRawStatus, GPUSTATUS_IDLE); // idle
}


/* direct memory access - smart check (Pete) -------------------------------- */
unsigned long dmaUsedAddr[3];
inline bool checkForEndlessLoop(unsigned long addr)
{
    if (addr == dmaUsedAddr[1] || addr == dmaUsedAddr[2]) 
        return true;
    if (addr < dmaUsedAddr[0]) 
        dmaUsedAddr[1] = addr;
    else 
        dmaUsedAddr[2] = addr;
    dmaUsedAddr[0] = addr;
    return false;
}

/* direct memory access chain - process GPU commands ------------------------ */
long CALLBACK GPUdmaChain(unsigned long* pBaseAddressDword, unsigned long index)
{
    if (gpu_gameRenderer == NULL) // security before init
        return -1;
    if (gpu_gameRenderer->isApiSet() == false) // start thread-safe graphic APIs
        gpu_gameRenderer->setGraphicApi();

    unsigned char* pBaseAddressByte = (unsigned char*)pBaseAddressDword;
    unsigned int DMACommandCounter = 0;
    short countValue;

    // set busy status and enable DMA smart check
    unsetGpuStatus(gpu_gameRenderer->gpuRawStatus, GPUSTATUS_IDLE); // busy
    dmaUsedAddr[0] = dmaUsedAddr[1] = dmaUsedAddr[2] = TRIPLEBYTES_MASK;
    
    // direct memory access loop
    do
    {
        // prevent out of range memory access
        if (zincEmulation) 
            index &= TRIPLEBYTES_MASK;
        else
            index &= PSVRAM_MASK;
        // infinite loop protection
        if(DMACommandCounter++ > PSVRAM_MASK || checkForEndlessLoop(index)) 
            break;

        // read data and process it (if not empty)
        countValue = pBaseAddressByte[index + (sizeof(index) - BYTE_SIZE)]; // byte to dword offset
        if (countValue > 0) 
            GPUwriteDataMem(&pBaseAddressDword[1 + (index/sizeof(index))], countValue);
            // division to convert memory address to dword array index

        index = pBaseAddressDword[index/sizeof(index)]; // follow chain
    }
    while (index != TRIPLEBYTES_MASK);

    setGpuStatus(gpu_gameRenderer->gpuRawStatus, GPUSTATUS_IDLE); // idle
    return 0;
}



// MISCELLANEOUS
//______________________________________________________________________________

/* save-state function ------------------------------------------------------ */
long CALLBACK GPUfreeze(unsigned long freezeDataType, GPUFreeze_t* pMem)
{
    if (gpu_gameRenderer == NULL) // security before init
        return 0;
    if (pMem == NULL) // check memory pointer
        return 0;
    
    // process save-state request
    switch (freezeDataType)
    {
        // selected save slot (for display)
        case SAVESTATE_INFO:
        {
            long slotNumber = *((long *)pMem);
            if (slotNumber < 0 || slotNumber > 8) 
                return 0;
            
            gpu_gameRenderer->saveSlot = slotNumber + 1;
            //! BuildDispMenu(0);//directdraw
            return 1;
        }
        // get data
        case SAVESTATE_GET:
        {
            if (pMem->freezeVersion != 1) // check version
                return 0;
            
            // copy data into pointed memory
            pMem->status = gpu_gameRenderer->gpuRawStatus;
            memcpy(pMem->control, gpu_gameRenderer->statusControl, PSSTATUSCONTROL_SIZE*sizeof(unsigned long));
            if (zincEmulation)
                memcpy(pMem->consoleVram, gpu_gameRenderer->consoleVramByte, ZINCVRAM_SIZE*1024*2);
            else
                memcpy(pMem->consoleVram, gpu_gameRenderer->consoleVramByte, PSVRAM_SIZE*1024*2);
            return 1;
        }
        // set data
        case SAVESTATE_SET:
        {
            if (pMem->freezeVersion != 1) // check version
                return 0;
            
            // read data from pointed memory
            gpu_gameRenderer->gpuRawStatus = pMem->status;
            memcpy(gpu_gameRenderer->statusControl, pMem->control, PSSTATUSCONTROL_SIZE*sizeof(unsigned long));
            if (zincEmulation)
                memcpy(gpu_gameRenderer->consoleVramByte, pMem->consoleVram, ZINCVRAM_SIZE*1024*2);
            else
                memcpy(gpu_gameRenderer->consoleVramByte, pMem->consoleVram, PSVRAM_SIZE*1024*2);

            //! ResetTextureArea(TRUE);//opengl
                 
            // status control (size 256) update based on the first read values
            // - order important ? - comes from Peops' save-state specification
            GPUwriteStatus(gpu_gameRenderer->statusControl[0]);
            GPUwriteStatus(gpu_gameRenderer->statusControl[1]);
            GPUwriteStatus(gpu_gameRenderer->statusControl[2]);
            GPUwriteStatus(gpu_gameRenderer->statusControl[3]);
            GPUwriteStatus(gpu_gameRenderer->statusControl[8]);
            GPUwriteStatus(gpu_gameRenderer->statusControl[6]);
            GPUwriteStatus(gpu_gameRenderer->statusControl[7]);
            GPUwriteStatus(gpu_gameRenderer->statusControl[5]);
            GPUwriteStatus(gpu_gameRenderer->statusControl[4]);
            return 1;
        }
        default: return 0;
    }
}

