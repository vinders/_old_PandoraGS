/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   gpu_main.cpp
    Description : main driver interface (PSEmu Pro spec)
*******************************************************************************/
using namespace std;
#include <string.h>
#include <ctime>

/* headers - globals/interface */
#define _INCLUDE_DIALOGAPI
#include "globals.h"
#include "lang/lang.h"
#include "gpu_memory.h"
#include "gpu_main.h"

/* headers - config/input */
#include "config/config_manager.h"
#include "input/keyinput_manager.h"

/* headers - rendering/dialogs */
#include "dialog/dialog_about.h"
#include "dialog/dialog_config.h"
#include "display/framerate.h"
#include "display/renderer.h"
#include "display/menu.h"


// GLOBAL VARIABLES ------------------------------------------------------------
/* specification */
const unsigned char lib_version = 1;  //PSEmu 1.x
static char*        lib_name    = PLUGIN_NAME;

/* plugin variables */
char*     gpu_gameExeName;
Renderer* gpu_gameRenderer;
unsigned long emuFixes = 0;
#if _SYSTEM == _WINDOWS
    HWND  gpu_window;
#endif



// MAIN DRIVER INTERFACE
//______________________________________________________________________________

/* driver init/constructor (called once) ------------------------------------ */
long CALLBACK GPUinit()
{
    // initialize global/static vars
    #if _SYSTEM == _WINDOWS
        gpu_window  = NULL;
    #endif
    gpu_gameExeName  = NULL;
    gpu_gameRenderer = NULL;
    ConfigManager::init();
    
    // initialize base config (no profile yet)
    ConfigManager::setConfig();  // set global singleton
    Config* globalConfig = ConfigManager::getInstance();
    if (globalConfig == NULL)
        return -1;
    Framerate::initFramerateConfig();
    
    // initialize renderer and psx/emulator variables
    gpu_gameRenderer = Renderer::getInstance(globalConfig->getRenderer()); // factory
    if (gpu_gameRenderer == NULL)
        return -1;   
    return gpu_gameRenderer->init();
}


/* driver shutdown/destructor (called once) --------------------------------- */
long CALLBACK GPUshutdown()
{   
    // release allocated memory (init)
    if (gpu_gameRenderer)
        delete gpu_gameRenderer;
    gpu_gameRenderer = NULL;
    ConfigManager::unsetConfig();
    return 0;
}


/* driver open (everytime a game starts/stops) ------------------------------ */
//   windows : ARGS = HWND hWindow
//   other OS: ARGS = unsigned long* displayId, char* caption, char* configFile
long CALLBACK GPUopen_ARGS_
{
    // read associated config profile
    int profile = ConfigManager::getGameProfile(gpu_gameExeName);
    ConfigManager::setProfile(profile, true);
    if (ConfigManager::getInstance()->getProfile() == NULL)
        return -1;
    
    // start keyboard input handler
    KeyInputManager::setKeyInput();     // set global singleton
    if (KeyInputManager::getInstance() == NULL)
        return -1;
    
    // set default framerate settings 
    Framerate::setFramerateConfig(gpu_gameRenderer->consoleDisplay.gameLocalization, 
                                  gpu_gameRenderer->consoleDisplay.gameInterlacing);
    
    // start graphic rendering pipeline
    #if _SYSTEM == _WINDOWS
        gpu_window  = hWindow;
    #endif
    return gpu_gameRenderer->setPipeline(); 
}


/* driver close (everytime a game stops) ------------------------------------ */
long CALLBACK GPUclose()
{
    // save config association
    ConfigManager::setGameProfile(gpu_gameExeName);
    // reset display and menu
    gpu_gameRenderer->unsetPipeline();

    // release allocated memory (open)
    if (gpu_gameExeName)
        delete [] gpu_gameExeName;
    gpu_gameExeName  = NULL;
    KeyInputManager::unsetKeyInput();
    ConfigManager::unsetProfile();
    return 0;
}


/* update activity (called every vsync) ------------------------------------- */
void CALLBACK GPUupdateLace(void)
{
    if (gpu_gameRenderer->isApiSet() == false)  // check graphic API status
        return;
    
    // set interlacing
    if((ConfigManager::getInstance()->bugFixes&FIX_ODDEVENBIT) == 0)                               
        gpu_gameRenderer->gpuRawStatus ^= GPUSTATUS_ODDLINES; // interlace bit toggle
    
    // check and adjust framerate
    if((ConfigManager::getInstance()->bugFixes&FIX_FPSLIMIT) == 0)
        Framerate::checkFramerate();
    
    // update graphic rendering
    if (Framerate::framesToSkip <= 0)
        gpu_gameRenderer->render();
    else
        Framerate::framesToSkip--;
}
 
 

// DIALOG CALL INTERFACE
//______________________________________________________________________________

/* open config dialog ------------------------------------------------------- */
long CALLBACK GPUconfigure()
{
    // create config dialog
    DialogConfig* configDial = new DialogConfig();
    if (configDial == NULL)
        return -1;
    
    // show dialog
    long code = configDial->showDialog();
    delete configDial;
    return code;
}

/* open about dialog -------------------------------------------------------- */
void CALLBACK GPUabout()
{
    // create and display about dialog
    DialogAbout* aboutDial = new DialogAbout();
    if (aboutDial)
    {
        aboutDial->showDialog();
        delete aboutDial;
    }
}

/* basic plugin test -------------------------------------------------------- */
long CALLBACK GPUtest()
{
    // always OK
    return 0; // positive = warning / negative = error
}



// GETTERS/SETTERS INTERFACE
//______________________________________________________________________________

/* PSEmu library info ------------------------------------------------------- */
char* CALLBACK PSEgetLibName()
{
    return lib_name;
}
unsigned long CALLBACK PSEgetLibType()
{
    return PSE_LT_GPU;
}
unsigned long CALLBACK PSEgetLibVersion()
{
    return ((unsigned long)lib_version<<WORD_BITSIZE)|(PLUGIN_VERSIONMAJ<<BYTE_BITSIZE)|PLUGIN_VERSIONMIN;
}

/* PSEmu mode info ---------------------------------------------------------- */
void CALLBACK GPUsetMode(unsigned long gdata) {} // outdated unused function
long CALLBACK GPUgetMode()
{
    long imageTransfer = 0;
    if(gpu_gameRenderer->outputMode == DR_VRAMTRANSFER) 
        imageTransfer = 0x1;
    if(gpu_gameRenderer->inputMode == DR_VRAMTRANSFER) 
        imageTransfer |= 0x2;
    return imageTransfer;
}

/* game executable name ----------------------------------------------------- */
void CALLBACK GPUsetExeName(char* pName)
{
    // release previously allocated memory
    if (gpu_gameExeName)
        delete [] gpu_gameExeName;
    gpu_gameExeName = new char[strlen(pName) + 1];
    
    // copy executable name
    if (gpu_gameExeName)
    {
        gpu_gameExeName[strlen(pName)] = '\0';
        strcpy(gpu_gameExeName, pName);
    }
}

/* enable/disable frame limit from emulator --------------------------------- */
void CALLBACK GPUsetframelimit(unsigned long option)
{
    ConfigManager::getInstance()->bFramerateLimit = (option == 1);
    Framerate::setFramerateConfig(gpu_gameRenderer->consoleDisplay.gameLocalization, 
                                  gpu_gameRenderer->consoleDisplay.gameInterlacing);
}

/* set custom fixes from emulator ------------------------------------------- */
void CALLBACK GPUsetfix(unsigned long fixBits)
{
    emuFixes = fixBits;
}



// PICTURES INTERFACE
//______________________________________________________________________________

/* trigger snapshot creation (on next display) ------------------------------ */
void CALLBACK GPUmakeSnapshot()
{
    //...
}

/* screen picture creation -------------------------------------------------- */
//   pMem = 128x96 px (3 bytes/px : 8-8-8 bit BGR screen data, without header)
void CALLBACK GPUgetScreenPic(unsigned char * pMem)    
{
    ////////////////////////////////////////////////////////////////////////
    // the main emu allocs 128x96x3 bytes, and passes a ptr
    // to it in pMem... the plugin has to fill it with
    // 8-8-8 bit BGR screen data (Win 24 bit BMP format 
    // without header). 
    // Beware: the func can be called at any time,
    // so you have to use the frontbuffer to get a fully
    // rendered picture
}

/* screen picture storage and display --------------------------------------- */
//   pMem = 128x96 px (3 bytes/px : 8-8-8 bit BGR screen data, without header)
void CALLBACK GPUshowScreenPic(unsigned char * pMem)
{
    ////////////////////////////////////////////////////////////////////////
    // func will be called with 128x96x3 BGR data.
    // the plugin has to store the data and display
    // it in the upper right corner.
    // If the func is called with a NULL ptr, you can
    // release your picture data and stop displaying
    // the screen pic
}



// MISCELLANEOUS INTERFACE
//______________________________________________________________________________
extern bool zincEmulation;

/* debug text display ------------------------------------------------------- */
void CALLBACK GPUdisplayText(char * pText)
{
    //...
}

/* special information display ---------------------------------------------- */
void CALLBACK GPUdisplayFlags(unsigned long dwFlags)
{
    // currently supported flags: 
    // bit 0 -> Analog pad mode activated
    // bit 1 -> PSX mouse mode activated
    // display this info in the gpu menu/fps display
}

/* define gun cursor -------------------------------------------------------- */
void CALLBACK GPUcursor(int player,int x,int y) // player=0-7, x=0-511, y=0-255
{
    if (player < 0 || player > 7)
        return;
    if (gpu_gameRenderer == NULL)
        return;
    ConfigProfile* gameConfigProfile = ConfigManager::getInstance()->getProfile();
    if (gameConfigProfile == NULL)
        return;
    
    // turn on gun cursor (if not already active)
    if (gameConfigProfile->gunCursor == -1)
        gameConfigProfile->gunCursor = CFG_GUN_CROSSHAIR;
    
    // set cursor position
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (y > 255) y = 255;
    if ((unsigned long)x >= gpu_gameRenderer->vramSize) 
        x = gpu_gameRenderer->vramSize - 1;
    gpu_gameRenderer->playersCursorsActive |= (1<<player);
    gpu_gameRenderer->playersCursors[player].x = x;
    gpu_gameRenderer->playersCursors[player].y = y;
}

/* screen vibration --------------------------------------------------------- */
void CALLBACK GPUvisualVibration(unsigned long small, unsigned long big)
{
    if (gpu_gameRenderer == NULL) // security check
        return;
        
    // initialize random generator (for buffer swap)
    srand((unsigned int)time(0));
    // set screen vibration
    if (ConfigManager::getInstance()->isFullscreen())
    {
        gpu_gameRenderer->consoleDisplay.setVisualVibration(small, big, 
                                         ConfigManager::getInstance()->fullResX);
    }
    else
    {
        gpu_gameRenderer->consoleDisplay.setVisualVibration(small, big, 
                                         ConfigManager::getInstance()->windowResX);
    }
}
