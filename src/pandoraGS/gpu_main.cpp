/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   gpu_main.cpp
Description : driver service interface (PSEmu)
*******************************************************************************/
#include <string>
#include <string.h>
using namespace std;

#include "logger.h"
#include "timer.h"
#include "input_reader.h"
#include "lang.h"
#include "config.h"
#include "config_io.h"
#include "dispatcher.h"
#include "engine.h"

#include "about_dialog.h"
#include "config_dialog.h"
#include "gpu_main.h"

// global data
static char* g_pLibName = PLUGIN_NAME;
#ifndef _WINDOWS
static char* g_pLibInfo = PLUGIN_INFO;
#else
HWND  g_hWindow = NULL; // main emulator window handle
#endif


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
        LanguageGameMenuResource::setLanguage((langcode_t)Config::gen_langCode, Config::gen_langFilePath);

        // configure frame rate (default)
        Dispatcher::initFrameRate();
        // initialize memory container
        Dispatcher::init();

        // open debug window
        if (Config::rnd_isDebugMode)
            SystemTools::createOutputWindow();
    }
    catch (const std::exception& exc) // init failure
    {
        Logger::getInstance()->writeErrorEntry("GPUinit", exc.what());
        GPUshutdown();
        return PSE_INIT_ERR_NOHARDWARE;
    }
    return PSE_INIT_SUCCESS;
}

/// <summary>Driver shutdown (called once)</summary>
/// <returns>Success indicator</returns>
long CALLBACK GPUshutdown()
{
    // close debug window
    if (Config::rnd_isDebugMode)
        SystemTools::closeOutputWindow();

    // stop renderer
    Engine::close();
    Logger::closeInstance();
    // close memory container
    Dispatcher::close();
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
        // load associated profile (if not already loaded)
        if (Dispatcher::st_isFirstOpen || Config::getGenFix(GEN_FIX_RELOAD_CFG_AFTER_OPEN))
        {
            Config::useProfile(ConfigIO::getGameAssociation(Dispatcher::getGameId()));
            if (Dispatcher::getGameId().compare("UNITTEST.001") == 0) // testing -> force window mode
            {
                Config::dsp_isFullscreen = false;
                Config::dsp_windowResX = 800u;
                Config::dsp_windowResY = 600u;
            }
            Dispatcher::st_isFirstOpen = false;

            if (Config::getCurrentProfile()->getFix(CFG_FIX_EXPAND_SCREEN))
                DisplayState::s_displayWidths[4] = 384;
            else
                DisplayState::s_displayWidths[4] = 368;
        }

        // create rendering window
        #ifdef _WINDOWS
        g_hWindow = hWindow;
        SystemTools::createDisplayWindow(g_hWindow, Config::dsp_isFullscreen, Config::dsp_isWindowResizable, 
                                         Config::dsp_windowResX, Config::dsp_windowResY);
        // disable screensaver (if possible)
        if (Config::misc_isScreensaverDisabled)
            SystemTools::setScreensaver(false);
        #else
        SystemTools::createDisplayWindow();
        #endif

        // reset frame skipping
        Timer::setSkippingMode(Config::sync_isFrameSkip, Config::getCurrentProfile()->getFix(CFG_FIX_HALF_SKIPPING));
        Timer::resetTimeReference();
        // fill window + allow rendering (unlock GPUupdateLace)
        Engine::initScreen();
        Dispatcher::st_displayState.set(false);

        // start user input tracker
        #ifdef _WINDOWS
        InputReader::start(g_hWindow, Config::misc_gpuKeys, (menu_t)Config::countProfiles() - 1,
                           Config::dsp_isFullscreen, Config::misc_isScreensaverDisabled);
        #else
        InputReader::start(Config::misc_gpuKeys, (menu_t)Config::countProfiles() - 1, Config::dsp_isFullscreen);
        #endif
        Timer::resetTimeReference(); // reset again (in case GPUupdateLace was called meanwhile)
    }
    catch (const std::exception& exc) // allocation failure
    {
        Logger::getInstance()->writeErrorEntry("GPUopen", exc.what());
        return PSE_ERR_FATAL;
    }
    return PSE_SUCCESS;
}

/// <summary>Driver closed (game stopped)</summary>
/// <returns>Success indicator</returns>
long CALLBACK GPUclose()
{
    // stop user input tracker
    InputReader::stop();

    // debug output
    if (Config::rnd_isDebugMode)
    {
        Dispatcher::printDebugSummary();
        Dispatcher::exportData();
    }

    // close renderer
    Engine::close();
    #ifdef _WINDOWS
    // close window
    SystemTools::closeDisplayWindow(g_hWindow);
    // re-enable screensaver (if disabled)
    if (Config::misc_isScreensaverDisabled)
        SystemTools::setScreensaver(true);
    #else
    SystemTools::closeDisplayWindow();
    #endif

    // save current game/profile association
    ConfigIO::setGameAssocation(Config::getCurrentProfileId(), Dispatcher::getGameId());
    // restore default config profile for next use
    if (Config::getGenFix(GEN_FIX_RELOAD_CFG_AFTER_OPEN))
        Config::useDefaultProfile();
    Dispatcher::setGameId("");

    return PSE_SUCCESS;
}

/// <summary>Activity update (called every vsync)</summary>
void CALLBACK GPUupdateLace()
{
#if _TRACE_CALLS == 1
    Logger::getInstance()->writeEntry("GPUupdateLace", "trace", "");
#endif
    if (Engine::isReady() == false) // skip first frames (requested while GPUopen is still loading...)
        return;

    // interlacing (if CC game fix, done in GPUreadStatus)
    if (Config::getCurrentProfile()->getNotFix(CFG_FIX_STATUS_INTERLACE))
        Dispatcher::st_displayState.toggleOddFrameFlag();

    // debug output
    if (Config::rnd_isDebugMode)
    {
        static int debugFrameCount = 0;
        if (++debugFrameCount >= 16) // every 16 frames, refresh debug data
        {
            Dispatcher::printDebugSummary();
            debugFrameCount = 0;
        }
    }

    // user input event
    if (InputReader::isEvent())
    {
        // toggle FPS display
        if (InputReader::getEvents(EVT_FPSCHANGE))
            Config::rnd_isFpsDisplayed = (Config::rnd_isFpsDisplayed == false);

        // change config profile
        if (InputReader::getEvents(EVT_PROFILECHANGE))
        {
            InputReader::lock();
            InputReader::resetEvents();
            Dispatcher::st_displayState.set(false);

            try
            {
                Config::useProfile(InputReader::getCurrentMenuIndex()); // set profile
                if (Config::getCurrentProfile()->getFix(CFG_FIX_EXPAND_SCREEN))
                    DisplayState::s_displayWidths[4] = 384;
                else
                    DisplayState::s_displayWidths[4] = 368;

                Engine::loadPipeline(); // rebuild rendering pipeline
                return;
            }
            catch (std::exception exc)
            {
                Logger::getInstance()->writeErrorEntry("GPUupdateLace", exc.what());
            }

            InputReader::unlock();
            Timer::setSkippingMode(Config::sync_isFrameSkip, Config::getCurrentProfile()->getFix(CFG_FIX_HALF_SKIPPING));
            Timer::resetTimeReference();
            return;
        }
        // change ratio or window size
        else if (InputReader::getEvents(EVT_WINDOWSIZE | EVT_WINDOWRATIO))
        {
            InputReader::lock();
            Dispatcher::st_displayState.set(false);

            if (EVT_WINDOWRATIO)
            {
                Config::getCurrentProfile()->dsp_ratioType = (Config::getCurrentProfile()->dsp_ratioType == CFG_ratio_aspect) ? CFG_ratio_pixel : CFG_ratio_aspect;
            }
            Engine::setViewport(InputReader::getEvents(EVT_WINDOWSIZE));

            InputReader::resetEvents();
            InputReader::unlock();
            Timer::resetTimeReference();
            return;
        } 
        // change window mode
        else if (InputReader::getEvents(EVT_WINDOWMODE))
        {
            InputReader::stop();
            Dispatcher::st_displayState.set(false);
            Engine::close();

            Config::dsp_isFullscreen = !(Config::dsp_isFullscreen);
            #ifdef _WINDOWS
            SystemTools::closeDisplayWindow(g_hWindow);
            SystemTools::createDisplayWindow(g_hWindow, Config::dsp_isFullscreen, Config::dsp_isWindowResizable,
                                             Config::dsp_windowResX, Config::dsp_windowResY);
            #else
            SystemTools::closeDisplayWindow();
            SystemTools::createDisplayWindow();
            #endif
            Engine::initScreen();
            Engine::initGL();

            // restart input tracker for new window
            #ifdef _WINDOWS
            InputReader::start(g_hWindow, Config::misc_gpuKeys, (menu_t)Config::countProfiles() - 1,
                               Config::dsp_isFullscreen, Config::misc_isScreensaverDisabled);
            #else
            InputReader::start(Config::misc_gpuKeys, (menu_t)Config::countProfiles() - 1, Config::dsp_isFullscreen);
            #endif
            Timer::resetTimeReference();
            return;
        }
    }

    // frame limiting/skipping + display current frame (if not skipped)
    if (Config::rnd_isFpsDisplayed)
    {
        static unsigned int laceCount = 16;
        if (++laceCount >= 16)
        {
            Timer::calcFrequency();
            laceCount = 0;
        }
    }
    bool isSkipped = Timer::isPeriodSkipped();
    Timer::wait(Config::sync_isFrameLimit, InputReader::getSpeedStatus(), (Dispatcher::st_displayState.getOddFrameFlag() != 0));
    if (isSkipped == false)
        Engine::render();
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
        Logger::getInstance()->writeErrorEntry("GPUconfigure", exc.what());
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
    return PSE_SUCCESS; // always ok
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
    return g_pLibName;
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
    return g_pLibInfo;
}
#endif


// -- SETTERS -- ---------------------------------------------------------------

/// <summary>Enable/disable frame limit from emulator</summary>
/// <param name="option">Status (1 = limit / 0 = none)</param>
void CALLBACK GPUsetframelimit(unsigned long option)
{
    Config::sync_isFrameLimit = (option == 1);
    Dispatcher::setFrameRate();
}

/// <summary>Set custom fixes from emulator</summary>
/// <param name="fixBits">Fixes (bits)</param>
void CALLBACK GPUsetfix(unsigned long fixBits)
{
    Config::misc_emuFixBits = fixBits;
}


// -- MEMORY MANAGEMENT INTERFACE -- -------------------------------------------

// see "dispatcher.h"



//________________________________________________________________________________

// -- SNAPSHOTS -- -------------------------------------------------------------

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


// -- UNIT TESTING -- ----------------------------------------------------------

// see "unit_tests.h"
