/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   service_main.cpp
Description : driver service interface (PSEmu)
*******************************************************************************/
#include <string>
#include <string.h>
using namespace std;

#include "config.h"
#include "config_io.h"
#include "lang.h"
#include "display_thread.h"
#include "psx_core_memory.h"
#include "framerate_manager.h"
#include "input_manager.h"

#include "about_dialog_viewmodel.h"
#include "config_dialog_viewmodel.h"
#include "log_utility.h"
#include "service_main.h"

static char* s_pLibName = PLUGIN_NAME;
#ifndef _WINDOWS
static char* s_pLibInfo = PLUGIN_INFO;
#endif
extern bool          g_isZincEmu;
extern unsigned long zincGPUVersion;

// global data
Config* g_pConfig = NULL;        // main configuration reference
PsxCoreMemory* g_pMemory = NULL; // emulated video memory
DisplayThread* g_pDisplayManager = NULL; // display thread manager


// -- DRIVER INIT INTERFACE -- -------------------------------------------------

/// <summary>Driver init (called once)</summary>
/// <returns>Success indicator</returns>
long CALLBACK GPUinit()
{
    try
    {
        // initialize config container
        g_pConfig = new Config(); 
        g_pConfig->setDefaultValues(); // default config
        ConfigIO::loadConfig(g_pConfig, true, false); // user config

        // initialize display thread and rendering pipeline
        g_pDisplayManager = new DisplayThread();
        g_pDisplayManager->initThread(g_pConfig);

        // initialize memory container
        g_pMemory = new PsxCoreMemory();
        g_pMemory->initMemory();
        LanguageGameMenuResource::setLanguage((LangCode)g_pConfig->gen_langCode);
    }
    catch (std::exception& exc) // init failure
    {
        try { LogUtility::getInstance()->writeErrorEntry("GPUinit",exc.what()); } catch(...) {}
        
        GPUshutdown();
        return PSE_INIT_ERR_NOHARDWARE;
    }
    return PSE_INIT_SUCCESS;
}

/// <summary>Driver shutdown (called once)</summary>
/// <returns>Success indicator</returns>
long CALLBACK GPUshutdown()
{
    // stop and release threads
    DisplayThread::releaseInstances();
    g_pDisplayManager = NULL;
    LogUtility::closeInstance();

    // release memory container
    if (g_pMemory != NULL)
        delete g_pMemory;
    g_pMemory = NULL;
    // release config container
    if (g_pConfig != NULL)
        delete g_pConfig;
    g_pConfig = NULL;

    return PSE_SUCCESS;
}

/// <summary>Driver opening/reopening (game started)</summary>
/// <param name="hWindow">Emulator main window handle</param>
/// <returns>Success indicator</returns>
long CALLBACK GPUopen_PARAM_
{
    try
    {
        // reload framerate values (may have changed in previous session)
        ConfigIO::loadFrameLimitConfig(g_pConfig);
        // load associated profile (if not already loaded)
        g_pConfig->useProfile(ConfigIO::getGameAssociation(g_pConfig->gen_gameId));
        g_pMemory->dsp_displayWidths[4] = (g_pConfig->getCurrentProfile()->dsp_hasFixExpandScreen) ? 384 : 368;

        #ifdef _WINDOWS
        // disable screensaver (if possible)
        if (g_pConfig->misc_isScreensaverDisabled)
            InputManager::setScreensaver(false);
        // hide emulator window, create rendering window and shaders
        g_pMemory->gen_hWindow = hWindow;
        #endif
        g_pDisplayManager->changeQuery();

        // configure framerate manager (default)
        FramerateManager::initFramerate();
        FramerateManager::setFramerate(false);
        // start user input tracker
        InputManager::initListener();
    }
    catch (std::exception& exc) // allocation failure
    {
        try { LogUtility::getInstance()->writeErrorEntry("GPUopen", exc.what()); } catch (...) {}
        return PSE_ERR_FATAL;
    }
    return PSE_SUCCESS;
}

/// <summary>Driver closed (game stopped)</summary>
/// <returns>Success indicator</returns>
long CALLBACK GPUclose()
{
    // stop user input tracker
    InputManager::stopListener();

    // wait until current frame complete and close window
    g_pDisplayManager->pauseQuery();
    // enable screensaver (if disabled)
    if (g_pConfig->misc_isScreensaverDisabled)
        InputManager::setScreensaver(true);

    // save current game/profile association
    ConfigIO::setGameAssocation(g_pConfig->getCurrentProfileId(), g_pConfig->gen_gameId);
    // restore default config profile for next use
    g_pConfig->useDefaultProfile();
    g_pConfig->gen_gameId = "";

    return PSE_SUCCESS;
}

/// <summary>Activity update (called every vsync)</summary>
void CALLBACK GPUupdateLace()
{
    // interlacing (if CC game fix, done in GPUreadStatus)
    if (g_pConfig->getCurrentProfile()->sync_hasFixInterlace == false)
        g_pMemory->mem_vramImage.oddFrame ^= 1;

    // change window or stretching mode
    if (InputManager::m_isWindowModeChangePending)
    {
        if (InputManager::m_isStretchingChangePending == false) // window change
            g_pConfig->dsp_isFullscreen = !(g_pConfig->dsp_isFullscreen);

        InputManager::stopListener();
        g_pDisplayManager->changeQuery(); // reload renderer
        FramerateManager::resetFrameSkipping();
        InputManager::initListener();
        return;
    }
    // change config profile (if requested)
    if (InputManager::m_isProfileChangePending)
    {
        try
        {
            g_pConfig->useProfile(InputManager::m_menuIndex); // set profile
            g_pMemory->dsp_displayWidths[4] = (g_pConfig->getCurrentProfile()->dsp_hasFixExpandScreen) ? 384 : 368;
            InputManager::m_isProfileChangePending = false;

            g_pDisplayManager->changeQuery(); // reload renderer
            FramerateManager::resetFrameSkipping();
            return;
        }
        catch (std::exception& exc) 
        {
            try { LogUtility::getInstance()->writeErrorEntry("GPUupdateLace", exc.what()); } catch (...) {}
            InputManager::m_isProfileChangePending = false;
        }
    }
    // fast forward -> skip 2 frames out of 4
    if (InputManager::m_isFastForward)
    {
        static int ffCount = 0;
        if (++ffCount < 3) return;
        else if (ffCount == 4)
            ffCount = 0;
    }

    // display current frame (if not skipped)
    if (FramerateManager::isFrameSkipped(g_pMemory->mem_vramImage.oddFrame != 0) == false)
        g_pDisplayManager->drawQuery();

    FramerateManager::waitFrameTime(InputManager::m_isSlowMotion); // frame sync
    FramerateManager::checkFrameStarted(); // wait until frame data is read
}



//________________________________________________________________________________

// -- PLUGIN DIALOG INTERFACE -- -----------------------------------------------

/// <summary>Plugin - Open the configuration dialog box</summary>
/// <returns>Success indicator</returns>
long CALLBACK GPUconfigure()
{
    // create config dialog
    ConfigDialogVM* dial = NULL;
    try
    {
        dial = new ConfigDialogVM();
        if (dial == NULL)
            return PSE_ERR_FATAL;
    }
    catch (std::exception exc)
    {
        try { LogUtility::getInstance()->writeErrorEntry("GPUconfigure", exc.what()); }
        catch (...) {}
        return PSE_ERR_FATAL;
    }

    // show dialog GUI
    dial->showDialog();
    delete dial;
    return PSE_SUCCESS;
}

/// <summary>Plugin - Open the 'about' dialog box</summary>
void CALLBACK GPUabout()
{
    // create about dialog
    AboutDialogVM* dial = NULL;
    try
    {
        dial = new AboutDialogVM();
        if (dial == NULL)
            return;
    }
    catch (std::exception exc)
    {
        return;
    }

    // show dialog GUI
    dial->showDialog();
    delete dial;
}

/// <summary>Plugin - Load test</summary>
/// <returns>Success indicator (0 = ok, 1 = warning or -1 = error)</returns>
long CALLBACK GPUtest()
{
    return PSE_GPU_SUCCESS; // always OK
}



//________________________________________________________________________________

// -- PLUGIN LIBRARY INFO -- ---------------------------------------------------

/// <summary>Get PSemu library type</summary>
/// <returns>Library type (GPU)</returns>
unsigned long CALLBACK PSEgetLibType() 
{ 
    return PSE_LT_GPU; 
}

/// <summary>Get library title</summary>
/// <returns>Library name</returns>
char* CALLBACK PSEgetLibName() 
{ 
    return s_pLibName;
}

/// <summary>Get library version</summary>
/// <returns>Full library version identifier</returns>
unsigned long CALLBACK PSEgetLibVersion() 
{ 
    return (_PPDK_HEADER_VERSION << 16) | (PLUGIN_VERSIONMAJ << 8) | PLUGIN_VERSIONMIN;
}

#ifndef _WINDOWS
/// <summary>Get library infos</summary>
char* GPUgetLibInfos()
{
    return s_pLibInfo;
}
#endif


// -- GETTERS - SETTERS -- -----------------------------------------------------

/// <summary>Set game executable ID (for config profiles associations)</summary>
/// <param name="pGameId">Newly started game identifier</param>
void CALLBACK GPUsetExeName(char* pGameId)
{
    if (g_pConfig == NULL)
        return;

    // set game executable ID
    if (pGameId != NULL)
    {
        std::string copyStr(pGameId);
        g_pConfig->gen_gameId = copyStr;
    }
    else
        g_pConfig->gen_gameId = "";
}

/// <summary>Enable/disable frame limit from emulator</summary>
/// <param name="option">Status (1 = limit / 0 = none)</param>
void CALLBACK GPUsetframelimit(unsigned long option)
{
    g_pConfig->sync_isFrameLimit = (option == 1);
    FramerateManager::setFramerate(true);
}


/// <summary>Get PSemu transfer mode</summary>
/// <returns>Image transfer mode</returns>
long CALLBACK GPUgetMode()
{ 
    long imageTransfer = 0;
    if (g_pMemory->mem_vramWriteMode == DR_VRAMTRANSFER)
        imageTransfer |= 0x1;
    if (g_pMemory->mem_vramReadMode == DR_VRAMTRANSFER)
        imageTransfer |= 0x2;
    return imageTransfer;
}

/// <summary>Set PSemu transfer mode (deprecated)</summary>
/// <param name="gdataMode">Image transfer mode</param>
void CALLBACK GPUsetMode(unsigned long gdataMode)
{
    // g_pMemory->mem_vramWriteMode = (gdataMode&0x1) ? DR_VRAMTRANSFER : DR_NORMAL;
    // g_pMemory->mem_vramReadMode  = (gdataMode&0x2) ? DR_VRAMTRANSFER : DR_NORMAL;
}

/// <summary>Set special display flags</summary>
/// <param name="dwFlags">Display flags</param>
void CALLBACK GPUdisplayFlags(unsigned long dwFlags)
{
    // display flags for GPU menu
    g_pMemory->dsp_displayFlags = dwFlags; // 00 -> digital, 01 -> analog, 02 -> mouse, 03 -> gun
}

/// <summary>Set custom fixes from emulator</summary>
/// <param name="fixBits">Fixes (bits)</param>
void CALLBACK GPUsetfix(unsigned long fixBits)
{
    // 'GPU busy' emulation hack
    g_pMemory->st_hasFixBusyEmu = ((fixBits&0x0001) != 0 || (fixBits&0x20000) != 0);
}



//________________________________________________________________________________

// -- STATUS REGISTER CONTROL -- -----------------------------------------------

/// <summary>Read data from GPU status register</summary>
/// <returns>GPU status register data</returns>
unsigned long CALLBACK GPUreadStatus()
{ 
    // interlacing CC game fix
    if (g_pConfig->isProfileSet() && g_pConfig->getCurrentProfile()->sync_hasFixInterlace)
    {
        static int readCount = 0;
        if (++readCount >= 2) // interlace bit toggle - every second read
        {
            readCount = 0;
            g_pMemory->mem_vramImage.oddFrame ^= 1;
        }
    }

    // fake busy status fix -> busy/idle sequence (while drawing)
    if (g_pMemory->st_fixBusyEmuSequence)
    {
        g_pMemory->st_fixBusyEmuSequence--;
        if (g_pMemory->st_fixBusyEmuSequence & 1) // busy
        {
            g_pMemory->unsetStatus(GPUSTATUS_IDLE);
            g_pMemory->unsetStatus(GPUSTATUS_READYFORCOMMANDS);
        }
        else // idle
        {
            g_pMemory->setStatus(GPUSTATUS_IDLE);
            g_pMemory->setStatus(GPUSTATUS_READYFORCOMMANDS);
        }
    }
    return g_pMemory->st_statusReg;
}


/// <summary>Process data sent to GPU status register</summary>
/// <param name="gdata">Status register command</param>
void CALLBACK GPUwriteStatus(unsigned long gdata)
{
    // get command indicator
    unsigned long command = getGpuCommand(gdata);
    g_pMemory->st_pStatusControl[command] = gdata; // store command (for save-states)

    switch (command)
    {
        // set VRAM transfer mode
        case CMD_SETTRANSFERMODE: 
        {
            gdata &= 0x3; // extract last 2 bits (LSB+1, LSB)
            g_pMemory->mem_vramWriteMode = (gdata == 0x2) ? DR_VRAMTRANSFER : DR_NORMAL;
            g_pMemory->mem_vramReadMode = (gdata == 0x3) ? DR_VRAMTRANSFER : DR_NORMAL;
            // set DMA bits
            g_pMemory->unsetStatus(GPUSTATUS_DMABITS); // clear current settings
            g_pMemory->setStatus(gdata << 29); // set based on received data (MSB-1, MSB-2)
            return;
        }
        // check GPU information (version, draw info, ...)
        case CMD_GPUREQUESTINFO: 
        {
            gdata &= 0x0FF; // extract last 8 bits
            g_pMemory->cmdGpuQuery(gdata, (zincGPUVersion==2)); return;
        }
        // enable/disable display
        case CMD_TOGGLEDISPLAY: g_pMemory->cmdSetDisplay((gdata&0x1)!=0); return;
        // reset GPU info
        case CMD_RESETGPU:      g_pMemory->cmdReset(); return;
        // set display
        case CMD_SETDISPLAYPOSITION: 
        {
            if (g_isZincEmu && zincGPUVersion == 2)
                g_pMemory->cmdSetDisplayPosition((short)(gdata&0x3FF), (short)((gdata>>12)&0x3FF));
            else
                g_pMemory->cmdSetDisplayPosition((short)(gdata&0x3FF), (short)((gdata>>10)&0x3FF));
            return;
        }
        case CMD_SETDISPLAYWIDTH:  g_pMemory->cmdSetWidth((short)(gdata&0x7FF), (short)((gdata>>12)&0x0FFF)); return;
        case CMD_SETDISPLAYHEIGHT: g_pMemory->cmdSetHeight((short)(gdata&0x3FF), (short)((gdata>>10)&0x3FF)); return;
        case CMD_SETDISPLAYINFO:   g_pMemory->cmdSetDisplayInfo(gdata); return;
    }
}


// -- DATA TRANSFER INTERFACE -- -----------------------------------------------

/// <summary>Read data from video memory (vram)</summary>
/// <returns>Raw GPU data</returns>
unsigned long CALLBACK GPUreadData()
{ 
    unsigned long gdata;
    GPUreadDataMem(&gdata, 1);
    return g_pMemory->mem_gpuDataTransaction;
}

/// <summary>Read entire chunk of data from video memory (vram)</summary>
/// <param name="pDwMem">Pointer to chunk of data (destination)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUreadDataMem(unsigned long* pDwMem, int size)
{
    if (g_pMemory->mem_vramReadMode != DR_VRAMTRANSFER) // check read mode
        return;
    g_pMemory->readDataMem(pDwMem, size);
}


/// <summary>Process and send data to video data register</summary>
/// <param name="gdata">Written data</param>
void CALLBACK GPUwriteData(unsigned long gdata)
{
    GPUwriteDataMem(&gdata, 1);
}

/// <summary>Process and send chunk of data to video data register</summary>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUwriteDataMem(unsigned long* pDwMem, int size)
{
    g_pMemory->writeDataMem(pDwMem, size);
}

/// <summary>Direct memory access chain</summary>
/// <param name="pDwBaseAddress">Pointer to memory chain</param>
/// <param name="offset">Memory offset</param>
/// <returns>Success indicator</returns>
long CALLBACK GPUdmaChain(unsigned long* pDwBaseAddress, unsigned long offset)
{ 
    unsigned char* pByteBaseAddress = (unsigned char*)pDwBaseAddress;
    unsigned int dmaCommandCounter = 0;
    unsigned long dmaOffset;
    short currentCount;

    g_pMemory->unsetStatus(GPUSTATUS_IDLE); // busy

    // direct memory access loop
    g_pMemory->resetDmaCheck();
    do
    {
        // prevent out of range memory access
        if (g_isZincEmu == false)
            offset &= PSXVRAM_MASK;
        if (dmaCommandCounter++ > PSXVRAM_THRESHOLD || g_pMemory->checkDmaEndlessChain(offset))
            break;

        // read and process data (if not empty)
        currentCount = pByteBaseAddress[(sizeof(offset) - 1) + offset];
        dmaOffset = offset / sizeof(offset); // convert address to dword array index
        if (currentCount > 0)
            GPUwriteDataMem(&pDwBaseAddress[1uL + dmaOffset], currentCount);

        offset = pDwBaseAddress[dmaOffset] & THREEBYTES_MASK; // follow chain
    } 
    while (offset != THREEBYTES_MASK);

    g_pMemory->setStatus(GPUSTATUS_IDLE); // idle
    return PSE_GPU_SUCCESS;
}



//________________________________________________________________________________

// -- SAVE-STATES - SNAPSHOTS -- -----------------------------------------------

/// <summary>Save/load current state</summary>
/// <param name="dataMode">Transaction type (0 = setter / 1 = getter / 2 = slot selection)</param>
/// <param name="pMem">Save-state structure pointer (to read or write)</param>
/// <returns>Success/compatibility indicator</returns>
long CALLBACK GPUfreeze(unsigned long dataMode, GPUFreeze_t* pMem)
{ 
    // process save-state request
    switch (dataMode)
    {
        // select save slot (for display)
        case SAVESTATE_INFO:
        {
            long slotNumber = *((long *)pMem);
            if (slotNumber < 0 || slotNumber > 8)
                return GPUFREEZE_ERR;

            g_pMemory->st_selectedSlot = slotNumber + 1;
            return GPUFREEZE_SUCCESS;
        }
        // save data
        case SAVESTATE_SAVE:
        {
            if (pMem == NULL || pMem->freezeVersion != 1) // check version
                return GPUFREEZE_ERR;

            // copy data into destination memory
            pMem->status = g_pMemory->st_statusReg;
            memcpy(pMem->pControlReg, g_pMemory->st_pStatusControl, STATUSCTRL_SIZE*sizeof(unsigned long));
            memcpy(pMem->pPsxVram, g_pMemory->mem_vramImage.pByte, g_pMemory->mem_vramImage.bufferSize * 2);
            return GPUFREEZE_SUCCESS;
        }
        // load data
        case SAVESTATE_LOAD:
        {
            if (pMem == NULL || pMem->freezeVersion != 1) // check version
                return GPUFREEZE_ERR;
            
            // read data from source memory
            g_pMemory->st_statusReg = pMem->status;
            memcpy(g_pMemory->st_pStatusControl, pMem->pControlReg, STATUSCTRL_SIZE*sizeof(unsigned long));
            memcpy(g_pMemory->mem_vramImage.pByte, pMem->pPsxVram, g_pMemory->mem_vramImage.bufferSize * 2);

            /*! ResetTextureArea(true);//opengl */

            // set status register, based on new status control
            GPUwriteStatus(g_pMemory->st_pStatusControl[0]);
            GPUwriteStatus(g_pMemory->st_pStatusControl[1]);
            GPUwriteStatus(g_pMemory->st_pStatusControl[2]);
            GPUwriteStatus(g_pMemory->st_pStatusControl[3]);
            GPUwriteStatus(g_pMemory->st_pStatusControl[8]);
            GPUwriteStatus(g_pMemory->st_pStatusControl[6]);
            GPUwriteStatus(g_pMemory->st_pStatusControl[7]);
            GPUwriteStatus(g_pMemory->st_pStatusControl[5]);
            GPUwriteStatus(g_pMemory->st_pStatusControl[4]);
            return GPUFREEZE_SUCCESS;
        }
        default: return GPUFREEZE_ERR;
    }
}


/// <summary>Request snapshot (on next display)</summary>
void CALLBACK GPUmakeSnapshot()
{
}

/// <summary>Get screen picture</summary>
/// <param name="pMem">allocated screen picture container 128x96 px (24b/px: 8-8-8 bit BGR, no header)</param>
void CALLBACK GPUgetScreenPic(unsigned char* pMem)
{
}

/// <summary>Store and display screen picture</summary>
/// <param name="pMem">screen picture data 128x96 px (24b/px: 8-8-8 bit BGR, no header)</param>
void CALLBACK GPUshowScreenPic(unsigned char* pMem)
{
}


// -- MISCELLANEOUS -- ---------------------------------------------------------

/// <summary>Display debug text</summary>
/// <param name="pText">Text string</param>
void CALLBACK GPUdisplayText(char* pText)
{
}

/// <summary>Set gun cursor display and position</summary>
/// <param name="player">Player with current cursor (0-7)</param>
/// <param name="x">Cursor horizontal position (0-511)</param>
/// <param name="y">Cursor vertical position (0-255)</param>
void CALLBACK GPUcursor(int player, int x, int y)
{
}

/// <summary>Trigger screen vibration</summary>
/// <param name="smallVbr">Small rumble value</param>
/// <param name="bigVbr">Big rumble value (if != 0, smallVbr will be ignored)</param>
void CALLBACK GPUvisualVibration(unsigned long smallVbr, unsigned long bigVbr)
{
}
