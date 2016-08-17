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
#include "core_memory.h"
#include "framerate_manager.h"
#include "input_manager.h"
#include "render_opengl.h"
#include "render_directx.h"

#include "about_dialog.h"
#include "config_dialog.h"
#include "log_utility.h"
#include "service_main.h"

// global data
static char* s_pLibName = PLUGIN_NAME;
#ifndef _WINDOWS
static char* s_pLibInfo = PLUGIN_INFO;
#endif
extern bool     g_isZincEmu;
extern uint32_t zincGPUVersion;


// -- DRIVER INIT INTERFACE -- -------------------------------------------------

/// <summary>Driver init (called once)</summary>
/// <returns>Success indicator</returns>
long CALLBACK GPUinit()
{
    try
    {
        // initialize config container
        Config::init(); // default config
        ConfigIO::loadConfig(true, false); // user config

        // initialize memory container
        CoreMemory::initMemory();
        LanguageGameMenuResource::setLanguage((LangCode)Config::gen_langCode);
    }
    catch (std::exception exc) // init failure
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
    // stop renderer
    Render::closeApi();
    LogUtility::closeInstance();
    // close memory container
    CoreMemory::closeMemory();
    // release config container
    Config::close();

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
        ConfigIO::loadFrameLimitConfig();
        // load associated profile (if not already loaded)
        if (CoreMemory::gen_isFirstOpening || Config::getGenFix(GEN_FIX_RELOAD_CFG_AFTER_OPEN))
        {
            Config::useProfile(ConfigIO::getGameAssociation(CoreMemory::gen_gameId));
            CoreMemory::gen_isFirstOpening = false;

            if (Config::getCurrentProfile()->getFix(CFG_FIX_EXPAND_SCREEN))
                CoreMemory::dsp_displayWidths[4] = 384;
            else
                CoreMemory::dsp_displayWidths[4] = 368;
        }

        // create rendering window
        #ifdef _WINDOWS
        CoreMemory::gen_hWindow = hWindow;
        #endif
        Render::setWindow(true);
        CoreMemory::dsp_isDisplaySet = false;

        // disable screensaver (if possible)
        if (Config::misc_isScreensaverDisabled)
            InputManager::setScreensaver(false);

        // configure framerate manager (default)
        FramerateManager::initFramerate();
        FramerateManager::setFramerate(false);
        // start user input tracker
        InputManager::initListener();
    }
    catch (std::exception exc) // allocation failure
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

    // close window and rendering pipeline
    Render::setWindow(false);
    #ifdef _WINDOWS
    // enable screensaver (if disabled)
    if (Config::misc_isScreensaverDisabled)
        InputManager::setScreensaver(true);
    #endif

    // save current game/profile association
    ConfigIO::setGameAssocation(Config::getCurrentProfileId(), CoreMemory::gen_gameId);
    // restore default config profile for next use
    if (Config::getGenFix(GEN_FIX_RELOAD_CFG_AFTER_OPEN))
        Config::useDefaultProfile();
    CoreMemory::gen_gameId = "";

    return PSE_SUCCESS;
}

/// <summary>Activity update (called every vsync)</summary>
void CALLBACK GPUupdateLace()
{
    // interlacing (if CC game fix, done in GPUreadStatus)
    if (Config::getCurrentProfile()->getNotFix(CFG_FIX_STATUS_INTERLACE))
        CoreMemory::mem_vramImage.oddFrame ^= 1;

    // change window or stretching mode
    if (InputManager::m_isWindowModeChangePending)
    {
        InputManager::stopListener();
        CoreMemory::dsp_isDisplaySet = false;
        if (InputManager::m_isStretchingChangePending == false) // toggle window mode
        {
            Config::dsp_isFullscreen = !(Config::dsp_isFullscreen);
            Render::changeWindowMode();
        }
        else // toggle stretching mode
        {
            Render::setDrawingSize(InputManager::m_isSizeChangePending);
            InputManager::m_isStretchingChangePending = false;
            InputManager::m_isSizeChangePending = false;
        }
        InputManager::m_isWindowModeChangePending = false;
        FramerateManager::resetFrameTime();
        InputManager::initListener();
        return;
    }
    // change config profile (if requested)
    if (InputManager::m_isProfileChangePending)
    {
        InputManager::m_isProfileChangePending = false;
        CoreMemory::dsp_isDisplaySet = false;
        try
        {
            Config::useProfile(InputManager::m_menuIndex); // set profile
            if (Config::getCurrentProfile()->getFix(CFG_FIX_EXPAND_SCREEN))
                CoreMemory::dsp_displayWidths[4] = 384;
            else
                CoreMemory::dsp_displayWidths[4] = 368;
            InputManager::m_isProfileChangePending = false;

            Render::reloadApi(); // rebuild rendering pipeline
            InputManager::updateListener();
            FramerateManager::resetFrameTime();
            return;
        }
        catch (std::exception exc) 
        {
            try { LogUtility::getInstance()->writeErrorEntry("GPUupdateLace", exc.what()); } catch (...) {}
            InputManager::m_isProfileChangePending = false;
        }
    }

    // frame limiting/skipping + display current frame (if not skipped)
    if (Config::getCurrentProfile()->getFix(CFG_FIX_REACTIVE_FPSLIMIT)) // reactive display mode -> draw then wait
    {
        if (FramerateManager::isFrameSkipped() == false) // drawing starts earlier + skipping is more effective
            Render::drawFrame();
        FramerateManager::waitFrameTime(InputManager::m_frameSpeed, CoreMemory::mem_vramImage.oddFrame != 0);
    }
    else // steady display mode -> wait then draw
    {
        static bool isSkipped;
        isSkipped = FramerateManager::isFrameSkipped();
        FramerateManager::waitFrameTime(InputManager::m_frameSpeed, CoreMemory::mem_vramImage.oddFrame != 0);
        if (isSkipped == false) // drawing starts at regular intervals
            Render::drawFrame();
    }
}



//________________________________________________________________________________

// -- PLUGIN DIALOG INTERFACE -- -----------------------------------------------

/// <summary>Plugin - Open the configuration dialog box</summary>
/// <returns>Success indicator</returns>
long CALLBACK GPUconfigure()
{
    // create config dialog
    ConfigDialog* dial = NULL;
    try
    {
        dial = new ConfigDialog();
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
    AboutDialog* dial = NULL;
    try
    {
        dial = new AboutDialog();
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
    // set game executable ID
    if (pGameId != NULL)
    {
        std::string copyStr(pGameId);
        CoreMemory::gen_gameId = copyStr;
    }
    else
        CoreMemory::gen_gameId = "";
}

/// <summary>Enable/disable frame limit from emulator</summary>
/// <param name="option">Status (1 = limit / 0 = none)</param>
void CALLBACK GPUsetframelimit(unsigned long option)
{
    Config::sync_isFrameLimit = (option == 1);
    FramerateManager::setFramerate(true);
}


/// <summary>Get PSemu transfer mode</summary>
/// <returns>Image transfer mode</returns>
long CALLBACK GPUgetMode()
{ 
    long imageTransfer = 0L;
    if (CoreMemory::mem_vramWriteMode == DR_VRAMTRANSFER)
        imageTransfer |= 0x1;
    if (CoreMemory::mem_vramReadMode == DR_VRAMTRANSFER)
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
    CoreMemory::dsp_displayFlags = dwFlags; // 00 -> digital, 01 -> analog, 02 -> mouse, 03 -> gun
}

/// <summary>Set custom fixes from emulator</summary>
/// <param name="fixBits">Fixes (bits)</param>
void CALLBACK GPUsetfix(unsigned long fixBits)
{
    Config::misc_emuFixBits = fixBits;
}



//________________________________________________________________________________

// -- STATUS REGISTER CONTROL -- -----------------------------------------------

/// <summary>Read data from GPU status register</summary>
/// <returns>GPU status register data</returns>
unsigned long CALLBACK GPUreadStatus()
{ 
    // interlacing CC game fix
    if (Config::isProfileSet() && Config::getCurrentProfile()->getFix(CFG_FIX_STATUS_INTERLACE))
    {
        static int readCount = 0;
        if (++readCount >= 2) // interlace bit toggle - every second read
        {
            readCount = 0;
            CoreMemory::mem_vramImage.oddFrame ^= 1;
        }
    }

    // fake busy status fix -> busy/idle sequence (while drawing)
    if (CoreMemory::st_fixBusyEmuSequence)
    {
        CoreMemory::st_fixBusyEmuSequence--;
        if (CoreMemory::st_fixBusyEmuSequence & 0x1) // busy if odd number
        {
            CoreMemory::unsetStatus(GPUSTATUS_IDLE);
            CoreMemory::unsetStatus(GPUSTATUS_READYFORCOMMANDS);
        }
        else // idle
        {
            CoreMemory::setStatus(GPUSTATUS_IDLE);
            CoreMemory::setStatus(GPUSTATUS_READYFORCOMMANDS);
        }
    }
    return CoreMemory::st_statusReg;
}


/// <summary>Process data sent to GPU status register</summary>
/// <param name="gdata">Status register command</param>
void CALLBACK GPUwriteStatus(unsigned long gdata)
{
    // get command indicator
    unsigned long command = getGpuCommand(gdata);
    CoreMemory::st_pStatusControl[command] = gdata; // store command (for save-states)

    switch (command)
    {
        // set VRAM transfer mode
        case CMD_SETTRANSFERMODE: 
        {
            gdata &= 0x3; // extract last 2 bits (LSB+1, LSB)
            CoreMemory::mem_vramWriteMode = (gdata == 0x2) ? DR_VRAMTRANSFER : DR_NORMAL;
            CoreMemory::mem_vramReadMode = (gdata == 0x3) ? DR_VRAMTRANSFER : DR_NORMAL;
            // set DMA bits
            CoreMemory::unsetStatus(GPUSTATUS_DMABITS); // clear current settings
            CoreMemory::setStatus(gdata << 29); // set based on received data (MSB-1, MSB-2)
            return;
        }
        // check GPU information (version, draw info, ...)
        case CMD_GPUREQUESTINFO: 
        {
            gdata &= 0x0FF; // extract last 8 bits
            CoreMemory::cmdGpuQuery(gdata, (zincGPUVersion == 2)); return;
        }
        // enable/disable display
        case CMD_TOGGLEDISPLAY: CoreMemory::cmdSetDisplay((gdata & 0x1) != 0); return;
        // reset GPU info
        case CMD_RESETGPU:      CoreMemory::cmdReset(); return;
        // set display
        case CMD_SETDISPLAYPOSITION: 
        {
            if (g_isZincEmu && zincGPUVersion == 2)
                CoreMemory::cmdSetDisplayPosition((short)(gdata & 0x3FF), (short)((gdata >> 12) & 0x3FF));
            else
                CoreMemory::cmdSetDisplayPosition((short)(gdata & 0x3FF), (short)((gdata >> 10) & 0x3FF));
            return;
        }
        case CMD_SETDISPLAYWIDTH:  CoreMemory::cmdSetWidth((short)(gdata & 0x7FF), (short)((gdata >> 12) & 0x0FFF)); return;
        case CMD_SETDISPLAYHEIGHT: CoreMemory::cmdSetHeight((short)(gdata & 0x3FF), (short)((gdata >> 10) & 0x3FF)); return;
        case CMD_SETDISPLAYINFO:
        {
            CoreMemory::cmdSetDisplayInfo(gdata);
            if (Config::sync_framerateLimit <= 0.05f) // == 0.0 (with float error offset) = auto-detect
                FramerateManager::setFramerate(true);
            //...màj
            return;
        }
    }
}


// -- DATA TRANSFER INTERFACE -- -----------------------------------------------

/// <summary>Read data from video memory (vram)</summary>
/// <returns>Raw GPU data</returns>
unsigned long CALLBACK GPUreadData()
{ 
    unsigned long gdata;
    GPUreadDataMem(&gdata, 1);
    return CoreMemory::mem_gpuDataTransaction;
}

/// <summary>Process and send data to video data register</summary>
/// <param name="gdata">Written data</param>
void CALLBACK GPUwriteData(unsigned long gdata)
{
    GPUwriteDataMem(&gdata, 1);
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

    CoreMemory::unsetStatus(GPUSTATUS_IDLE); // busy

    // direct memory access loop
    CoreMemory::resetDmaCheck();
    do
    {
        // prevent out of range memory access
        if (g_isZincEmu == false)
            offset &= PSXVRAM_MASK;
        if (dmaCommandCounter++ > PSXVRAM_THRESHOLD || CoreMemory::checkDmaEndlessChain(offset))
            break;

        // read and process data (if not empty)
        currentCount = pByteBaseAddress[(sizeof(offset) - 1) + offset];
        dmaOffset = offset / sizeof(offset); // convert address to dword array index
        if (currentCount > 0)
            GPUwriteDataMem(&pDwBaseAddress[1uL + dmaOffset], currentCount);

        offset = pDwBaseAddress[dmaOffset] & THREEBYTES_MASK; // follow chain
    } 
    while (offset != THREEBYTES_MASK);

    CoreMemory::setStatus(GPUSTATUS_IDLE); // idle
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

            CoreMemory::st_selectedSlot = slotNumber + 1;
            return GPUFREEZE_SUCCESS;
        }
        // save data
        case SAVESTATE_SAVE:
        {
            if (pMem == NULL || pMem->freezeVersion != 1) // check version
                return GPUFREEZE_ERR;

            // copy data into destination memory
            pMem->status = CoreMemory::st_statusReg;
            memcpy(pMem->pControlReg, CoreMemory::st_pStatusControl, STATUSCTRL_SIZE*sizeof(unsigned long));
            memcpy(pMem->pPsxVram, CoreMemory::mem_vramImage.pByte, CoreMemory::mem_vramImage.bufferSize * 2);
            return GPUFREEZE_SUCCESS;
        }
        // load data
        case SAVESTATE_LOAD:
        {
            if (pMem == NULL || pMem->freezeVersion != 1) // check version
                return GPUFREEZE_ERR;
            
            // read data from source memory
            CoreMemory::st_statusReg = pMem->status;
            memcpy(CoreMemory::st_pStatusControl, pMem->pControlReg, STATUSCTRL_SIZE*sizeof(unsigned long));
            memcpy(CoreMemory::mem_vramImage.pByte, pMem->pPsxVram, CoreMemory::mem_vramImage.bufferSize * 2);

            //... reset textures API

            // set status register, based on new status control
            GPUwriteStatus(CoreMemory::st_pStatusControl[0]);
            GPUwriteStatus(CoreMemory::st_pStatusControl[1]);
            GPUwriteStatus(CoreMemory::st_pStatusControl[2]);
            GPUwriteStatus(CoreMemory::st_pStatusControl[3]);
            GPUwriteStatus(CoreMemory::st_pStatusControl[8]);
            GPUwriteStatus(CoreMemory::st_pStatusControl[6]);
            GPUwriteStatus(CoreMemory::st_pStatusControl[7]);
            GPUwriteStatus(CoreMemory::st_pStatusControl[5]);
            GPUwriteStatus(CoreMemory::st_pStatusControl[4]);
            return GPUFREEZE_SUCCESS;
        }
        default: return GPUFREEZE_ERR;
    }
}


/// <summary>Snapshot request (on next display)</summary>
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
