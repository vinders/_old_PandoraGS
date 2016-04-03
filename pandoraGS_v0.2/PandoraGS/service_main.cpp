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
        g_pMemory->ps_displayWidths[4] = (g_pConfig->getCurrentProfile()->dsp_hasFixExpandScreen) ? 384 : 368;

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
    // interlacing
    if (g_pConfig->getCurrentProfile()->sync_hasFixInterlace == false)
        g_pMemory->mem_vramImage.oddFrame ^= 1;

    // change window or stretching mode
    if (InputManager::m_isWindowModeChangePending)
    {
        if (InputManager::m_isStretchingChangePending == false) // window change
            g_pConfig->dsp_isFullscreen = (g_pConfig->dsp_isFullscreen == false);

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
            g_pMemory->ps_displayWidths[4] = (g_pConfig->getCurrentProfile()->dsp_hasFixExpandScreen) ? 384 : 368;
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
    // fast forward -> skip 2 frames out of 3
    if (InputManager::m_isFastForward)
    {
        static int ffCount = 0;
        if (++ffCount < 3) return;
        else ffCount = 0;
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
    // g_pMemory->mem_vramWriteMode = (gdata&1) ? DR_VRAMTRANSFER : DR_NORMAL;
    // g_pMemory->mem_vramReadMode  = (gdata&2) ? DR_VRAMTRANSFER : DR_NORMAL;
}

/// <summary>Set special display flags</summary>
/// <param name="dwFlags">Display flags</param>
void CALLBACK GPUdisplayFlags(unsigned long dwFlags)
{
}

/// <summary>Set custom fixes from emulator</summary>
/// <param name="fixBits">Fixes (bits)</param>
void CALLBACK GPUsetfix(unsigned long fixBits)
{
    //emuFixes = fixBits;
}



//________________________________________________________________________________

// -- STATUS REGISTER CONTROL -- -----------------------------------------------

/// <summary>Read data from GPU status register</summary>
/// <returns>GPU status register data</returns>
unsigned long CALLBACK GPUreadStatus()
{ 
    return 0; 
}

/// <summary>Process data sent to GPU status register</summary>
/// <param name="gdata">Status register command</param>
void CALLBACK GPUwriteStatus(unsigned long gdata)
{
}


// -- DATA TRANSFER INTERFACE -- -----------------------------------------------

/// <summary>Read data from video memory (vram)</summary>
/// <returns>Raw GPU data</returns>
unsigned long CALLBACK GPUreadData()
{ 
    return 0; 
}

/// <summary>Read entire chunk of data from video memory (vram)</summary>
/// <param name="pDwMem">Pointer to chunk of data (destination)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUreadDataMem(unsigned long* pDwMem, int size)
{
}


/// <summary>Process and send data to video data register</summary>
/// <param name="gdata">Written data</param>
void CALLBACK GPUwriteData(unsigned long gdata)
{
}

/// <summary>Process and send chunk of data to video data register</summary>
/// <param name="pDwMem">Pointer to chunk of data (source)</param>
/// <param name="size">Memory chunk size</param>
void CALLBACK GPUwriteDataMem(unsigned long* pDwMem, int size)
{
}

/// <summary>Give a direct core memory access chain to GPU driver</summary>
/// <param name="pDwBaseAddress">Pointer to memory chain</param>
/// <param name="offset">Memory offset</param>
/// <returns>Success indicator</returns>
long CALLBACK GPUdmaChain(unsigned long* pDwBaseAddress, unsigned long offset)
{ 
    return PSE_GPU_SUCCESS; 
}



//________________________________________________________________________________

// -- SAVE-STATES - SNAPSHOTS -- -----------------------------------------------

/// <summary>Set custom fixes from emulator</summary>
/// <param name="dataMode">Transaction type (0 = setter / 1 = getter / 2 = slot selection)</param>
/// <param name="pMem">Save-state structure pointer (to read or write)</param>
/// <returns>Success/compatibility indicator</returns>
long CALLBACK GPUfreeze(unsigned long dataMode, GPUFreeze_t* pMem)
{ 
    //...
    return GPUFREEZE_SUCCESS; 
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
