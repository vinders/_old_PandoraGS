/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : PSEmu GPU library interface
*******************************************************************************/
#include "globals.h"
#include <cstdlib>
#include <string>
#include "pandoraGS.h"
#include "config/config.h"
#include "events/listener.h"
#include "events/menu.h"
#include "events/utils/logger.h"
#include "command/dispatcher.h"
#include "display/engine.h"
#include "psemu_main.h"
using namespace std;


// -- driver base interface -- -------------------------------------------------

/// @brief Driver init (called once)
/// @returns Success indicator
long CALLBACK GPUinit()
{
    try
    {
        // initialize config
        config::Config::init();

        // apply settings
        //...lang
        //...memory + status
        //...timer

        // open debug window
        if (config::Config::events.isDebugMode)
        {
            //...console output window
        }
    }
    catch (const std::runtime_error& runExc)
    {
        events::utils::Logger::getInstance()->writeErrorEntry("GPUinit"s, runExc.what());
        GPUshutdown();
        return PSE_INIT_ERR_NOTCONFIGURED;
    }
    catch (const std::exception& exc)
    {
        events::utils::Logger::getInstance()->writeErrorEntry("GPUinit"s, exc.what());
        GPUshutdown();
        return PSE_ERR_FATAL;
    }
    return PSE_INIT_SUCCESS;
}

/// @brief Driver shutdown (called once)
/// @returns Success indicator
long CALLBACK GPUshutdown()
{
    // close debug window
    if (config::Config::events.isDebugMode)
    {
        //...close console output window
    }

    // close renderer
    //...engine
    //...memory + status
    //...lang

    // close config
    config::Config::close();
    events::utils::Logger::closeInstance();
    return PSE_SUCCESS;
}


#ifdef _WINDOWS
/// @brief Driver opening/reopening (game started)
/// @param hWindow  Emulator main window handle
/// @returns Success indicator
long CALLBACK GPUopen(HWND hWindow)
#else
/// @brief Driver opening/reopening (game started)
/// @param pDisplayId   Display identifier
/// @param pCaption     Window caption
/// @param pConfigFile  Config file path
/// @returns Success indicator
long CALLBACK GPUopen(unsigned long* pDisplayId, char* pCaption, char* pConfigFile)
#endif
{
    try
    {

        //...

    }
    catch (const std::runtime_error& runExc)
    {
        events::utils::Logger::getInstance()->writeErrorEntry("GPUopen"s, runExc.what());
        GPUclose();
        return PSE_GPU_ERR_NOTCONFIGURED;
    }
    catch (const std::exception& exc)
    {
        events::utils::Logger::getInstance()->writeErrorEntry("GPUopen"s, exc.what());
        GPUclose();
        return PSE_ERR_FATAL;
    }
    return PSE_GPU_SUCCESS;
}


/// @brief Driver closed (game stopped)
/// @returns Success indicator
long CALLBACK GPUclose()
{

    return PSE_SUCCESS;
}


/// @brief Activity update (called on every vsync)
void CALLBACK GPUupdateLace()
{

}



// -- status control -- --------------------------------------------------------

/// @brief Read data from GPU status register
/// @returns GPU status register data
unsigned long CALLBACK GPUreadStatus()
{

    return 0;//...
}


/// @brief Process data sent to GPU status register
/// @param gdata  Status register command
void CALLBACK GPUwriteStatus(unsigned long gdata)
{

}



// -- data transfers -- --------------------------------------------------------

/// @brief Get data transfer mode
/// @returns Image transfer mode
long CALLBACK GPUgetMode()
{

    return 0;//...
}

/// @brief Set data transfer mode (deprecated)
/// @param transferMode  Image transfer mode
void CALLBACK GPUsetMode(unsigned long transferMode)
{

}


/// @brief Read data from video memory (vram)
/// @returns Raw GPU data
unsigned long CALLBACK GPUreadData()
{

    return 0;//...
}

/// @brief Read entire chunk of data from video memory (vram)
/// @param pMem  Pointer to chunk of data (destination)
/// @param size  Memory chunk size
void CALLBACK GPUreadDataMem(unsigned long* pMem, int size)
{

}


/// @brief Process and send data to video data register
/// @param gdata  Written data
void CALLBACK GPUwriteData(unsigned long gdata)
{

}

/// @brief Process and send chunk of data to video data register
/// @param pMem  Pointer to chunk of data (source)
/// @param size  Memory chunk size
void CALLBACK GPUwriteDataMem(unsigned long* pMem, int size)
{

}


/// @brief Direct core memory chain transfer to GPU driver
/// @param pBaseAddress  Pointer to memory chain
/// @param offset        Memory offset
/// @returns Success indicator
long CALLBACK GPUdmaChain(unsigned long* pBaseAddress, unsigned long offset)
{
    return PSE_SUCCESS;
}


/// @brief Save/load current state
/// @param dataMode  Transaction type (0 = setter / 1 = getter / 2 = slot selection)
/// @param pMem      Save-state structure pointer (to read or write)
/// @returns Success/compatibility indicator
long CALLBACK GPUfreeze(unsigned long dataMode, GPUFreeze_t* pMem)
{

    return SAVESTATE_SUCCESS;
}



// -- plugin dialog interface -- -----------------------------------------------

/// @brief Open plugin configuration dialog box
/// @returns Success indicator
long CALLBACK GPUconfigure()
{

    return PSE_SUCCESS;
}

/// @brief Open plugin 'about' dialog box
void CALLBACK GPUabout()
{

}

/// @brief Check if plugin works
/// @returns Success indicator (0 = ok, 1 = warning or -1 = error)
long CALLBACK GPUtest()
{
    return PSE_GPU_SUCCESS; // always ok
}



// -- plugin library info -- ---------------------------------------------------

static char g_pLibName[] = PLUGIN_NAME;

/// @brief Get library title
/// @returns Library name
char* CALLBACK PSEgetLibName()
{
    return g_pLibName;
}

/// @brief Get PSemu library type
/// @returns Library type (GPU)
unsigned long CALLBACK PSEgetLibType()
{
    return PSE_LT_GPU;
}

/// @brief Get library version
/// @returns Full library version identifier
unsigned long CALLBACK PSEgetLibVersion()
{
    return (_PPDK_HEADER_VERSION << 16) | (PLUGIN_VERSIONMAJ << 8) | PLUGIN_VERSIONMIN;
}

#ifndef _WINDOWS
static char g_pLibInfo[] = PLUGIN_INFO;

/// @brief Get library infos
char* GPUgetLibInfos()
{
    return g_pLibInfo;
}
#endif



// -- runtime settings -- ------------------------------------------------------

/// @brief Set special display flags
/// @param flags  Display flags
void CALLBACK GPUdisplayFlags(unsigned long flags)
{
    
}

/// @brief Enable/disable frame limit from emulator
/// @param option  Status (1 = limit / 0 = none)
void CALLBACK GPUsetframelimit(unsigned long option)
{
    //...indicateur séparé pour ne pas "écraser" config user ? -> utilisé si limite auto ou désactivée ?
}

/// @brief Set custom fixes from emulator
/// @param fixBits  Fixes (bits)
void CALLBACK GPUsetfix(unsigned long fixBits)
{
    config::Config::runtimeFixBits = fixBits;
}

/// @brief Set game executable ID (for config profiles associations)
/// @param pGameId  Newly started game identifier
void CALLBACK GPUsetExeName(char* pGameId)
{

}



// -- snapshots -- -------------------------------------------------------------

/// @brief Request snapshot (on next display)
void CALLBACK GPUmakeSnapshot()
{

}


/// @brief Get screen picture
/// @param pMem  allocated screen picture container 128x96 px (24b/px: 8-8-8 bit BGR, no header)
void CALLBACK GPUgetScreenPic(unsigned char* pMem)
{

}

/// @brief Store and display screen picture
/// @param pMem  screen picture data 128x96 px (24b/px: 8-8-8 bit BGR, no header)
void CALLBACK GPUshowScreenPic(unsigned char* pMem)
{

}



// -- miscellaneous -- ---------------------------------------------------------

/// @brief Display debug text
/// @param pText  Text string
void CALLBACK GPUdisplayText(char* pText)
{

}

/// @brief Set gun cursor display and position
/// @param player  Player with current cursor (0-7)
/// @param x       Cursor horizontal position (0-511)
/// @param y       Cursor vertical position (0-255)
void CALLBACK GPUcursor(int player, int x, int y)
{

}

/// @brief Trigger screen vibration
/// @param small  Small rumble value
/// @param big    Big rumble value (if != 0, 'small' will be ignored)
void CALLBACK GPUvisualVibration(unsigned long small, unsigned long big)
{

}
