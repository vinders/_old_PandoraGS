/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Description : PSEmu GPU library interface
*******************************************************************************/
#pragma once

#include "globals.h"

/*  PSEmu Plugin Developer Kit Header definition - (C)1998 Vision Thing
This file can be used only to develop PSEmu Plugins. Other usage is highly prohibited.
*/
#define PSX_BIOSADDR        0xBFC03720 // BIOS - virtual hardware address (last bits after memory)
#define _PPDK_HEADER_VERSION       1uL //PSEmu 1.x

// plugin type returned by PSEgetLibType (types can be merged)
#define PSE_LT_CDR  1L
#define PSE_LT_GPU  2L
#define PSE_LT_SPU  4L
#define PSE_LT_PAD  8L

// plugin return codes
#define PSE_SUCCESS                 0L // every interface function should return this value if successful
#define PSE_ERR_FATAL              -1L // undefined error but fatal one, that kills all functionality
#define PSE_INIT_SUCCESS            0L // initialization went OK
#define PSE_INIT_ERR_NOTCONFIGURED -2L // this driver is not configured
#define PSE_INIT_ERR_NOHARDWARE    -3L // this driver can not operate properly on this hardware or hardware is not detected
#define SAVESTATE_SUCCESS           1L
#define SAVESTATE_ERR               0L

// GPU test return values
#define PSE_GPU_SUCCESS			    0L // success, everything configured, and went OK
#define PSE_GPU_ERR_NOTCONFIGURED	1L // this driver is not configured
// gpuQueryS.flags
#define PSE_GPU_FLAGS_WINDOWED		1L // this driver requests windowed mode
// gpuQueryS.status
#define PSE_GPU_STATUS_WINDOWWRONG	1L // this driver cannot operate in this windowed mode

// CDR test return values
#define PSE_CDR_SUCCESS             0L  // success, everything configured, and went OK
#define PSE_CDR_ERR                -40L // driver error
#define PSE_CDR_ERR_NOTCONFIGURED   PSE_CDR_ERR - 0 // this driver is not configured
#define PSE_CDR_ERR_NOREAD          PSE_CDR_ERR - 1 // this driver is unable to read data from medium
#define PSE_CDR_WARN                40L // driver warnings
// if this driver emulates lame mode ie. can read only 2048 tracks and sector header is emulated
// this might happen to CDROMS that do not support RAW mode reading
#define PSE_CDR_WARN_LAMECD         PSE_CDR_WARN + 0


/// @struct GPUFreeze_t
/// @brief Save-state data structure
typedef struct GPUFREEZETAG
{
    unsigned long freezeVersion;    ///< System version = 1 (set by emulator)
    unsigned long status;           ///< GPU emulated status
    unsigned long pControlReg[256]; ///< Control register value
    unsigned char pPsxVram[1024 * 1024 * 2]; ///< Video memory image
} GPUFreeze_t;


// -- driver base interface -- -------------------------------------------------

/// @brief Driver init (called once)
/// @returns Success indicator
long CALLBACK GPUinit();
/// @brief Driver shutdown (called once)
/// @returns Success indicator
long CALLBACK GPUshutdown();

#ifdef _WINDOWS
/// @brief Driver opening/reopening (game started)
/// @param hWindow  Emulator main window handle
/// @returns Success indicator
long CALLBACK GPUopen(HWND hWindow);
#else
/// @brief Driver opening/reopening (game started)
/// @param pDisplayId   Display identifier
/// @param pCaption     Window caption
/// @param pConfigFile  Config file path
/// @returns Success indicator
long CALLBACK GPUopen(unsigned long* pDisplayId, char* pCaption, char* pConfigFile);
#endif
/// @brief Driver closed (game stopped)
/// @returns Success indicator
long CALLBACK GPUclose();

/// @brief Activity update (called on every vsync)
void CALLBACK GPUupdateLace();


// -- status control -- --------------------------------------------------------

/// @brief Read data from GPU status register
/// @returns GPU status register data
unsigned long CALLBACK GPUreadStatus();
/// @brief Process data sent to GPU status register
/// @param gdata  Status register command
void CALLBACK GPUwriteStatus(unsigned long gdata);


// -- data transfers -- --------------------------------------------------------

/// @brief Get data transfer mode
/// @returns Image transfer mode
long CALLBACK GPUgetMode();
/// @brief Set data transfer mode (deprecated)
/// @param transferMode  Image transfer mode
void CALLBACK GPUsetMode(unsigned long transferMode);

/// @brief Read data from video memory (vram)
/// @returns Raw GPU data
unsigned long CALLBACK GPUreadData();
/// @brief Read entire chunk of data from video memory (vram)
/// @param pMem  Pointer to chunk of data (destination)
/// @param size  Memory chunk size
void CALLBACK GPUreadDataMem(unsigned long* pMem, int size);

/// @brief Process and send data to video data register
/// @param gdata  Written data
void CALLBACK GPUwriteData(unsigned long gdata);
/// @brief Process and send chunk of data to video data register
/// @param pMem  Pointer to chunk of data (source)
/// @param size  Memory chunk size
void CALLBACK GPUwriteDataMem(unsigned long* pMem, int size);
/// @brief Direct core memory chain transfer to GPU driver
/// @param pBaseAddress  Pointer to memory chain
/// @param offset        Memory offset
/// @returns Success indicator
long CALLBACK GPUdmaChain(unsigned long* pBaseAddress, unsigned long offset);

/// @brief Save/load current state
/// @param dataMode  Transaction type (0 = setter / 1 = getter / 2 = slot selection)
/// @param pMem      Save-state structure pointer (to read or write)
/// @returns Success/compatibility indicator
long CALLBACK GPUfreeze(unsigned long dataMode, GPUFreeze_t* pMem);


// -- plugin dialog interface -- -----------------------------------------------

/// @brief Open plugin configuration dialog box
/// @returns Success indicator
long CALLBACK GPUconfigure();
/// @brief Open plugin 'about' dialog box
void CALLBACK GPUabout();

/// @brief Check if plugin works
/// @returns Success indicator (0 = ok, 1 = warning or -1 = error)
long CALLBACK GPUtest();


// -- plugin library info -- ---------------------------------------------------

/// @brief Get library title
/// @returns Library name
char* CALLBACK PSEgetLibName();
/// @brief Get PSemu library type
/// @returns Library type (GPU)
unsigned long CALLBACK PSEgetLibType();
/// @brief Get library version
/// @returns Full library version identifier
unsigned long CALLBACK PSEgetLibVersion();


// -- runtime settings -- ------------------------------------------------------

/// @brief Set special display flags
/// @param flags  Display flags
void CALLBACK GPUdisplayFlags(unsigned long flags);
/// @brief Enable/disable frame limit from emulator
/// @param option  Status (1 = limit / 0 = none)
void CALLBACK GPUsetframelimit(unsigned long option);
/// @brief Set custom fixes from emulator
/// @param fixBits  Fixes (bits)
void CALLBACK GPUsetfix(unsigned long fixBits);
/// @brief Set game executable ID (for config profiles associations)
/// @param pGameId  Newly started game identifier
void CALLBACK GPUsetExeName(char* pGameId);


// -- snapshots -- -------------------------------------------------------------

/// @brief Request snapshot (on next display)
void CALLBACK GPUmakeSnapshot();
/// @brief Get screen picture
/// @param pMem  allocated screen picture container 128x96 px (24b/px: 8-8-8 bit BGR, no header)
void CALLBACK GPUgetScreenPic(unsigned char* pMem);
/// @brief Store and display screen picture
/// @param pMem  screen picture data 128x96 px (24b/px: 8-8-8 bit BGR, no header)
void CALLBACK GPUshowScreenPic(unsigned char* pMem);


// -- miscellaneous -- ---------------------------------------------------------

/// @brief Display debug text
/// @param pText  Text string
void CALLBACK GPUdisplayText(char* pText);
/// @brief Set gun cursor display and position
/// @param player  Player with current cursor (0-7)
/// @param x       Cursor horizontal position (0-511)
/// @param y       Cursor vertical position (0-255)
void CALLBACK GPUcursor(int player, int x, int y);
/// @brief Trigger screen vibration
/// @param small  Small rumble value
/// @param big    Big rumble value (if != 0, 'small' will be ignored)
void CALLBACK GPUvisualVibration(unsigned long small, unsigned long big);


// -- unit testing -- ----------------------------------------------------------

// see "unit_tests.h"
