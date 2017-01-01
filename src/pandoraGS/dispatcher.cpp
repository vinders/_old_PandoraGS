/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   dispatcher.cpp
Description : GPU manager and dispatcher
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include "globals.h"
using namespace std;
#include "timer.h"
#include "config.h"
#include "logger.h"
#include "system_tools.h"
#include "dispatcher.h"
#define This Dispatcher

// video memory management
VideoMemory  Dispatcher::mem_vram;       // emulated console video memory
memoryload_t Dispatcher::mem_vramReader; // output memory load
memoryload_t Dispatcher::mem_vramWriter; // input memory load
unsigned long Dispatcher::mem_dataExchangeBuffer = GPUDATA_INIT; // data buffer read/written by emulator

// execution and display status
std::string Dispatcher::s_gameId(""); // game executable ID
DisplayState Dispatcher::st_displayState;
unsigned long Dispatcher::st_pControlReg[CTRLREG_SIZE]; // GPU status control
uint32_t Dispatcher::st_displayDevFlags = 0u; // 00 -> digital, 01 -> analog, 02 -> mouse, 03 -> gun
bool Dispatcher::st_isFirstOpen = true;      // first call to GPUopen()
bool Dispatcher::st_isUploadPending = false; // image needs to be uploaded to VRAM
long Dispatcher::st_selectedSaveSlot = 0L;   // selected save-state slot

bool Dispatcher::s_isZincEmu = false; // Zinc emulation


/// <summary>Display data summary in debug window</summary>
void Dispatcher::printDebugSummary()
{
    SystemTools::setConsoleCursorPos(0);
    printf("Status register : 0x%08x\n", StatusRegister::getStatusRegister());
    printf("\nVRAM : hit ESC and check 'pandoraGS_memdump.txt'\n");
    printf("\nState control register :\n");
    for (int i = 0; i < CTRLREG_SIZE; ++i)
    {
        printf("%08x ", st_pControlReg[i]);
        if ((i + 1) % 8 == 0)
            printf("\n");
    }
    printf("\n");
}

/// <summary>Export full status and VRAM data</summary>
void Dispatcher::exportData()
{
    try
    {
        // create output file
        std::string filePath = SystemTools::getWritableFilePath() + std::string("pandoraGS_memdump.txt");
        std::ofstream out;
        out.open(filePath, std::ofstream::trunc); // overwrite
        if (!out.is_open())
        {
            Logger::getInstance()->writeErrorEntry("Dispatcher.exportData", "Data file not created");
            return;
        }

        uint32_t index = 0u;
        // status register
        out << "Status register : 0x" << hex << setw(8) << setfill('0') << StatusRegister::getStatusRegister() << endl;
        // VRAM state
        out << "\nVRAM :\n" << endl;
        for (VideoMemory::iterator it = mem_vram.begin(); it.getPos() < mem_vram.end(); ++it)
        {
            out << hex << setw(4) << setfill('0') << it.getValue();
            if (++index >= 16u)
            {
                out << endl;
                index = 0u;
            }
        }
        // state control register
        index = 0u;
        out << "\nState Control Register :\n" << endl;
        for (int i = 0; i < CTRLREG_SIZE; ++i)
        {
            out << hex << setw(8) << setfill('0') << st_pControlReg[i];
            if (++index >= 8u)
            {
                out << endl;
                index = 0u;
            }
        }

        out << endl;
        out.close();
    }
    catch (const exception& exc)
    {
        Logger::getInstance()->writeErrorEntry("Dispatcher.exportData", exc.what());
    }
}


// -- DISPLAY STATUS CONTROL -- ------------------------------------------------

/// <summary>Read data from GPU status register</summary>
/// <returns>GPU status register data</returns>
unsigned long CALLBACK GPUreadStatus()
{
#if _TRACE_CALLS == 1
    Logger::getInstance()->writeEntry("GPUreadStatus", "trace", "");
#endif
    // interlacing CC game fix
    if (Config::isProfileSet() && Config::getCurrentProfile()->getFix(CFG_FIX_STATUS_INTERLACE))
    {
        static int readCount = 0;
        if (++readCount >= 2) // interlace bit toggle - every second read
        {
            readCount = 0;
            This::st_displayState.toggleOddFrameFlag();
        }
    }
    // fake busy status fix (while drawing)
    StatusRegister::setFakeBusyStep();

    // read status register
    return StatusRegister::getStatusRegister();
}

/// <summary>Process data sent to GPU status register</summary>
/// <param name="gdata">Status register command</param>
void CALLBACK GPUwriteStatus(unsigned long gdata)
{
#if _TRACE_CALLS == 1
    Logger::getInstance()->writeEntry("GPUwriteStatus", "trace", std::to_string(gdata));
#endif
     // get command indicator
    ubuffer_t command = This::extractGpuCommandType((ubuffer_t)gdata);
    This::st_pControlReg[command] = gdata; // store command (for save-states)

    switch (command)
    {
        // set VRAM transfer mode
        case CMD_SETTRANSFERMODE: 
            This::setDataTransferMode((ubuffer_t)gdata); return;
        // check GPU information (version, draw info, ...)
        case CMD_GPUREQUESTINFO: 
            This::mem_dataExchangeBuffer = (unsigned long)This::st_displayState.getDrawInfo((ubuffer_t)gdata); return;
        // set display
        case CMD_TOGGLEDISPLAY: 
            This::st_displayState.toggleDisplay((gdata & 0x1) != 0); return;
        case CMD_SETDISPLAYPOSITION: 
            This::st_displayState.setDisplayPos(This::extractSmallPos((ubuffer_t)gdata, true)); return;
        case CMD_SETDISPLAYWIDTH:  
            This::st_displayState.setWidth(This::extractPos((ubuffer_t)gdata)); return;
        case CMD_SETDISPLAYHEIGHT: 
            This::st_displayState.setHeight(This::extractSmallPos((ubuffer_t)gdata, false)); return;
        case CMD_SETDISPLAYINFO:
        {
            This::st_displayState.setDisplayState((ubuffer_t)gdata);
            if (Config::sync_framerateLimit <= 0.02f) // == 0.0 (+ float error offset) = auto-detect
                This::setFrameRate();
            
            //... update display if changed //!
            return;
        }
        // reset GPU info
        case CMD_RESETGPU:
            This::reset(); return;
    }
}

/// <summary>Set special display flags</summary>
/// <param name="dwFlags">Display flags</param>
void CALLBACK GPUdisplayFlags(unsigned long dwFlags)
{
#if _TRACE_CALLS == 1
    Logger::getInstance()->writeEntry("GPUdisplayFlags", "trace", std::to_string(dwFlags));
#endif
    // display flags for GPU menu
    This::st_displayDevFlags = dwFlags; // 00 -> digital, 01 -> analog, 02 -> mouse, 03 -> gun
}

/// <summary>Set game executable ID (for config profiles associations)</summary>
/// <param name="pGameId">Newly started game identifier</param>
void CALLBACK GPUsetExeName(char* pGameId)
{
    This::setGameId(pGameId);
}


// -- DATA TRANSFER -- ---------------------------------------------------------

/// <summary>Get data transfer mode</summary>
/// <returns>Image transfer mode</returns>
long CALLBACK GPUgetMode()
{
#if _TRACE_CALLS == 1
    Logger::getInstance()->writeEntry("GPUgetMode", "trace", "");
#endif
    long imageTransfer = 0L;
    if (This::mem_vramWriter.mode == Loadmode_vramTransfer)
        imageTransfer |= 0x1;
    if (This::mem_vramReader.mode == Loadmode_vramTransfer)
        imageTransfer |= 0x2;
    return imageTransfer;
}
/// <summary>Set data transfer mode (deprecated)</summary>
/// <param name="gdataMode">Image transfer mode</param>
void CALLBACK GPUsetMode(unsigned long gdataMode)
{
#if _TRACE_CALLS == 1
    Logger::getInstance()->writeEntry("GPUsetMode", "trace", std::to_string(gdataMode));
#endif
    // This::mem_vramWriter.mode = (gdataMode&0x1) ? Loadmode_vramTransfer : Loadmode_normal;
    // This::mem_vramReader.mode = (gdataMode&0x2) ? Loadmode_vramTransfer : Loadmode_normal;
}

/// <summary>Read data from video memory (vram)</summary>
/// <returns>Raw GPU data</returns>
unsigned long CALLBACK GPUreadData()
{
    unsigned long gdata;
    GPUreadDataMem(&gdata, 1);
    return This::mem_dataExchangeBuffer;
}
/// <summary>Process and send data to video data register</summary>
/// <param name="gdata">Written data</param>
void CALLBACK GPUwriteData(unsigned long gdata)
{
    GPUwriteDataMem(&gdata, 1);
}

/// <summary>Direct core memory chain transfer to GPU driver</summary>
/// <param name="pDwBaseAddress">Pointer to memory chain</param>
/// <param name="offset">Memory offset</param>
/// <returns>Success indicator</returns>
long CALLBACK GPUdmaChain(unsigned long* pDwBaseAddress, unsigned long offset)
{
#if _TRACE_CALLS == 1
    Logger::getInstance()->writeEntry("GPUdmaChain", "trace", std::string("offset=") + std::to_string(offset));
#endif
    unsigned char* pByteBaseAddress = (unsigned char*)pDwBaseAddress;
    uint32_t dmaCommandCounter = 0u;
    StatusRegister::unsetStatus(GPUSTATUS_IDLE); // busy

    // direct memory access loop
    short curCount;
    unsigned long dmaOffset;
    This::mem_vram.resetDmaCheck();
    do
    {
        // prevent out of range memory access
        if (This::mem_vram.isDoubledSize() == false) // not Zinc emu
            offset &= PSXVRAM_MASK;
        if (dmaCommandCounter++ > PSXVRAM_THRESHOLD || This::mem_vram.checkDmaEndlessChain(offset))
            break;

        // read and process data (if not empty)
        curCount = pByteBaseAddress[(sizeof(offset) - 1u) + offset];
        dmaOffset = offset / sizeof(offset); // convert address to dword array index
        if (curCount > 0)
            GPUwriteDataMem(&pDwBaseAddress[1uL + dmaOffset], curCount);

        offset = pDwBaseAddress[dmaOffset] & THREEBYTES_MASK; // follow chain
    } while (offset != THREEBYTES_MASK);

    StatusRegister::setStatus(GPUSTATUS_IDLE); // idle
    return PSE_GPU_SUCCESS;
}

/// <summary>Read entire chunk of data from video memory (vram)</summary>
/// <param name="pDwMem">Pointer to chunk of data (destination)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUreadDataMem(unsigned long* pDwMem, int size)
{
#if _TRACE_CALLS == 1
    Logger::getInstance()->writeEntry("GPUreadDataMem", "trace", std::string("size=") + std::to_string(size));
#endif
    StatusRegister::unsetStatus(GPUSTATUS_IDLE); // busy

    // check/adjust vram reader position
    while (This::mem_vramReader.vramPos.getPos() >= This::mem_vram.end()) // max position
        This::mem_vramReader.vramPos -= This::mem_vram.size();
    while (This::mem_vramReader.vramPos.getPos() < This::mem_vram.rend()) // min position
        This::mem_vramReader.vramPos += This::mem_vram.size();

    //! ... si frameReadType impair && size > 1 && vramReader == vramWriter (pour x, y, w et h)
        //! ... vérif vramReader

    // read memory chunk of data
    int i = 0;
    bool bitLevel = false;
    if (This::mem_vramReader.colsRemaining > 0 && This::mem_vramReader.rowsRemaining > 0)
    {
        while (i < size) // check if end of memory chunk
        {
            // 2 separate 16 bits reads (compatibility: wrap issues)
            if (bitLevel == false) // read lower 16 bits
            {
                This::mem_dataExchangeBuffer = (unsigned long)(This::mem_vramReader.vramPos.getValue());
            }
            else // read higher 16 bits
            {
                This::mem_dataExchangeBuffer |= (unsigned long)(This::mem_vramReader.vramPos.getValue()) << 16;
                // update pointed memory
                *pDwMem = This::mem_dataExchangeBuffer;
                pDwMem++; // increment memory location

                if (This::mem_vramReader.colsRemaining <= 0) // check last column
                    break;
                i++;
            }

            // update cursor to higher bits position
            ++This::mem_vramReader.vramPos;
            if (This::mem_vramReader.vramPos.getPos() >= This::mem_vram.end()) // check max position
                This::mem_vramReader.vramPos -= This::mem_vram.size();
            This::mem_vramReader.rowsRemaining--;

            // if column is empty, check next column
            if (This::mem_vramReader.rowsRemaining <= 0)
            {
                This::mem_vramReader.rowsRemaining = This::mem_vramReader.range.width; // reset rows
                This::mem_vramReader.vramPos += (1024 - This::mem_vramReader.range.width);
                if (This::mem_vramReader.vramPos.getPos() >= This::mem_vram.end()) // check max position
                    This::mem_vramReader.vramPos -= This::mem_vram.size();

                This::mem_vramReader.colsRemaining--;
                if (bitLevel && This::mem_vramReader.colsRemaining <= 0) // high bits -> check if last column
                    break;
            }
            bitLevel = !bitLevel; // alternate low/high bits
        }
    }

    // if no columns remaining (transfer <= mem chunk size)
    if (i < size)
    {
        // reset transfer mode and values
        This::mem_vramReader.mode = Loadmode_normal;
        This::mem_vramReader.range.x = This::mem_vramReader.range.y = 0;
        This::mem_vramReader.range.width = This::mem_vramReader.range.height = 0;
        This::mem_vramReader.rowsRemaining = This::mem_vramReader.colsRemaining = 0;
        // signal VRAM transfer end
        StatusRegister::unsetStatus(GPUSTATUS_READYFORVRAM); // no longer ready
    }
    StatusRegister::setStatus(GPUSTATUS_IDLE); // idle
}

/// <summary>Process and send chunk of data to video data register</summary>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUwriteDataMem(unsigned long* pDwMem, int size)
{
#if _TRACE_CALLS == 1
    Logger::getInstance()->writeEntry("GPUwriteDataMem", "trace", std::string("size=") + std::to_string(size));
    {
        std::string traceData("pDwMem[...]=");
        unsigned long* pTraceIterator = pDwMem;
        for (int tr = 0; tr < size; ++tr)
        {
            traceData += std::string(" ") + std::to_string(*pTraceIterator);
            ++pTraceIterator;
        }
        Logger::getInstance()->writeEntry("GPUwriteDataMem", "trace", traceData);
    }
#endif
    unsigned long gdata = 0;
    unsigned long bitHandler;
    int i = 0;

    StatusRegister::unsetStatus(GPUSTATUS_IDLE | GPUSTATUS_READYFORCOMMANDS); // busy + not ready
    do
    {
        // write memory chunk of data to VRAM
        if (This::mem_vramWriter.mode == Loadmode_vramTransfer)
        {
            // check vram limits
            while (This::mem_vramWriter.vramPos.getPos() >= This::mem_vram.end()) // max position
                This::mem_vramWriter.vramPos -= This::mem_vram.size();
            while (This::mem_vramWriter.vramPos.getPos() < This::mem_vram.rend()) // min position
                This::mem_vramWriter.vramPos += This::mem_vram.size();

            // upload data
            bool isChunkEnd = false;
            while (This::mem_vramWriter.colsRemaining > 0)
            {
                while (This::mem_vramWriter.rowsRemaining > 0)
                {
                    if (i < size)
                    {
                        // set data source
                        gdata = *pDwMem++;
                        i++;

                        // copy odd pixel data (gdata lower bits)
                        bitHandler = gdata & 0x0FFFFu;
                        This::mem_vramWriter.vramPos.setValue((uint16_t)bitHandler);
                        This::mem_vramWriter.rowsRemaining--;

                        ++This::mem_vramWriter.vramPos; // next
                        if (This::mem_vramWriter.vramPos.getPos() >= This::mem_vram.end())
                            This::mem_vramWriter.vramPos -= This::mem_vram.size(); // loop

                        // if column is empty, check next column
                        if (This::mem_vramWriter.rowsRemaining <= 0)
                        {
                            // if last column, end transfer
                            This::mem_vramWriter.colsRemaining--;
                            if (This::mem_vramWriter.colsRemaining <= 0) // last pixel is odd pixel
                            {
                                gdata = (bitHandler | (((unsigned long)(This::mem_vramWriter.vramPos.getValue())) << 16));
                                goto ENDVRAMTRANSFER; // double break (faster than having to check at each iteration)
                            }
                            // set next column
                            This::mem_vramWriter.rowsRemaining = This::mem_vramWriter.range.width;
                            This::mem_vramWriter.vramPos += 1024 - This::mem_vramWriter.range.width;
                        }

                        // copy even pixel data (gdata higher bits)
                        bitHandler = gdata >> 16;
                        This::mem_vramWriter.vramPos.setValue((uint16_t)bitHandler);
                        This::mem_vramWriter.rowsRemaining--;

                        ++This::mem_vramWriter.vramPos; // next
                        if (This::mem_vramWriter.vramPos.getPos() >= This::mem_vram.end())
                            This::mem_vramWriter.vramPos -= This::mem_vram.size(); // loop
                    }
                    else // mem chunk size reached
                    {
                        isChunkEnd = true;
                        goto ENDVRAMTRANSFER; // double break (faster than having to check at each iteration)
                    }
                }
                // set next column
                This::mem_vramWriter.colsRemaining--;
                This::mem_vramWriter.rowsRemaining = This::mem_vramWriter.range.width;
                This::mem_vramWriter.vramPos += 1024 - This::mem_vramWriter.range.width;
            }
        ENDVRAMTRANSFER:

            // data transfer end, if no columns remaining (didn't end because of mem chunk size)
            if (isChunkEnd == false)
            {
                if (This::st_isUploadPending)
                {
                    This::st_isUploadPending = false;
                    //... check write update //!
                }
                // reset transfer mode to NORMAL operations
                This::mem_vramWriter.mode = Loadmode_normal;
                // reset transfer values (to prevent mis-transfer of data)
                This::mem_vramWriter.colsRemaining = 0;
                This::mem_vramWriter.rowsRemaining = 0;
            }
        }
    } 
    while (PrimitiveBuilder::processDisplayData(This::mem_vramWriter.mode, pDwMem, size, &gdata, &i)); // true = VRAM transfer again

    This::mem_dataExchangeBuffer = gdata;
    StatusRegister::setStatus(GPUSTATUS_READYFORCOMMANDS | GPUSTATUS_IDLE); // ready + idle
}


// -- LOAD/SAVE MEMORY STATE -- ------------------------------------------------

/// <summary>Save/load current state</summary>
/// <param name="dataMode">Transaction type (0 = setter / 1 = getter / 2 = slot selection)</param>
/// <param name="pMem">Save-state structure pointer (to read or write)</param>
/// <returns>Success/compatibility indicator</returns>
long CALLBACK GPUfreeze(unsigned long dataMode, GPUFreeze_t* pMem)
{
    switch (dataMode)
    {
        // select save slot (for display)
        case SAVESTATE_INFO:
        {
            long slotNumber = *((long *)pMem);
            if (slotNumber < 0 || slotNumber > 8)
                return GPUFREEZE_ERR;

            This::st_selectedSaveSlot = slotNumber + 1;
            return GPUFREEZE_SUCCESS;
        }
        // save state
        case SAVESTATE_SAVE:
        {
            if (pMem == NULL || pMem->freezeVersion != 1) // check version
                return GPUFREEZE_ERR;

            // copy status into destination
            pMem->status = (unsigned long)StatusRegister::getStatusRegister();
            memcpy(pMem->pControlReg, This::st_pControlReg, CTRLREG_SIZE*sizeof(unsigned long));

            // copy data into destination memory
            #if _ARCHITECTURE == ARCHITECTURE_64
            int vramLen = This::mem_vram.size() * 2;
            uint8_t* pVram = This::mem_vram.get();
            unsigned char* pVramCopy = pMem->pPsxVram;
            for (int i = 0; i < vramLen; i++) // size of types may differ -> no memcpy
            {
                *pVramCopy = (unsigned char)*pVram;
                ++pVramCopy;
                ++pVram;
            }
            #else
            memcpy(pMem->pPsxVram, This::mem_vram.get(), This::mem_vram.size() * 2);
            #endif

            Timer::resetTimeReference(); // avoid frame skipping
            return GPUFREEZE_SUCCESS;
        }
        // load state
        case SAVESTATE_LOAD:
        {
            if (pMem == NULL || pMem->freezeVersion != 1) // check version
                return GPUFREEZE_ERR;

            // read status from source
            StatusRegister::setStatusRegister((uint32_t)pMem->status);
            memcpy(This::st_pControlReg, pMem->pControlReg, CTRLREG_SIZE*sizeof(unsigned long));

            // read data from source memory
            #if _ARCHITECTURE == ARCHITECTURE_64
            int vramLen = This::mem_vram.size() * 2;
            uint8_t* pVram = This::mem_vram.get();
            unsigned char* pVramCopy = pMem->pPsxVram;
            for (int i = 0; i < vramLen; i++) // size of types may differ -> no memcpy
            {
                *pVram = (uint8_t)*pVramCopy;
                ++pVramCopy;
                ++pVram;
            }
            #else
            memcpy(This::mem_vram.get(), pMem->pPsxVram, This::mem_vram.size() * 2);
            #endif

            //... reset opengl texture area //!

            // update status based on new control data
            GPUwriteStatus(This::st_pControlReg[0]);
            GPUwriteStatus(This::st_pControlReg[1]);
            GPUwriteStatus(This::st_pControlReg[2]);
            GPUwriteStatus(This::st_pControlReg[3]);
            GPUwriteStatus(This::st_pControlReg[8]);
            GPUwriteStatus(This::st_pControlReg[6]);
            GPUwriteStatus(This::st_pControlReg[7]);
            GPUwriteStatus(This::st_pControlReg[5]);
            GPUwriteStatus(This::st_pControlReg[4]);
            Timer::resetTimeReference(); // avoid frame skipping
            return GPUFREEZE_SUCCESS;
        }
    }
    return GPUFREEZE_ERR;
}
