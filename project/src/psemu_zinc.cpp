/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : PSEmu-Zinc GPU library interface
*******************************************************************************/
#include "globals.h"
#include <string>
using namespace std::literals::string_literals;
#include "psemu_main.h"
using namespace std;

/// @struct GPUConfiguration_t
/// @brief Zinc configuration structure
typedef struct GPUOTAG
{
	unsigned long version;        // version of structure - currently 1
	long          hWindow;        // window handle
	unsigned long screenRotation; // 0 = 0CW, 1 = 90CW, 2 = 180CW, 3 = 270CW = 90CCW
	unsigned long gpuVersion;     // 0 = a, 1 = b, 2 = c
	const char*   pGameName;      // game title string
	const char*   pCfgFile;       // config filepath string
} GPUConfiguration_t;


// -- driver base interface -- -------------------------------------------------

/// @brief Driver init (called once)
/// @returns Success indicator
long CALLBACK ZN_GPUinit() // we always set the vram size to 2MB, if the ZN interface is used
{
	
	//...

	return GPUinit();
}
/// @brief Driver shutdown (called once)
/// @returns Success indicator
long CALLBACK ZN_GPUshutdown()
{
	return GPUshutdown();
}

/// @brief Driver opening/reopening (game started)
/// @param pCfg  Configuration pointer
/// @returns Success indicator
long CALLBACK ZN_GPUopen(void* pCfg)
{
	GPUConfiguration_t* cfg = (GPUConfiguration_t *)pCfg;
	if (!cfg || cfg->version != 1)
		return -1L;

	//... tile fix
	//... set version

	#ifdef _WINDOWS
	return GPUopen((HWND)cfg->hWindow);
	#else
	return GPUopen(&cfg->hWnd, cfg->GameName, cfg->CfgFile);
	#endif
}
/// @brief Driver closed (game stopped)
/// @returns Success indicator
long CALLBACK ZN_GPUclose()
{
	return GPUclose();
}

/// @brief Activity update (called every vsync)
void CALLBACK ZN_GPUupdateLace()
{
	GPUupdateLace();
}


// -- status control -- --------------------------------------------------------

/// @brief Read data from GPU status register
/// @returns GPU status register data
unsigned long CALLBACK ZN_GPUreadStatus()
{
	return GPUreadStatus();
}
/// @brief Process data sent to GPU status register
/// @param gdata  Status register command
void CALLBACK ZN_GPUwriteStatus(unsigned long gdata)
{
	GPUwriteStatus(gdata);
}


// -- data transfers -- --------------------------------------------------------

/// @brief Get PSemu transfer mode
/// @returns Image transfer mode
long CALLBACK ZN_GPUgetMode()
{
	return GPUgetMode();
}
/// @brief Set PSemu transfer mode (deprecated)
/// @param transferMode  Image transfer mode
void CALLBACK ZN_GPUsetMode(unsigned long transferMode)
{
	GPUsetMode(transferMode);
}

/// @brief Read data from video memory (vram)
/// @returns Raw GPU data
unsigned long CALLBACK ZN_GPUreadData()
{
	return GPUreadData();
}
/// @brief Read entire chunk of data from video memory (vram)
/// @param pBaseAddress  Pointer to memory chain
/// @param offset        Memory offset
/// @param size          Memory chunk size
long CALLBACK ZN_GPUdmaSliceOut(unsigned long* pBaseAddress, unsigned long offset, unsigned long size)
{
	GPUreadDataMem(pBaseAddress + offset, size);
	return 0L;
}

/// @brief Process and send data to video data register
/// @param gdata  Written data
void CALLBACK ZN_GPUwriteData(unsigned long gdata)
{
	GPUwriteDataMem(&gdata, 1);
}
/// @brief Process and send chunk of data to video data register
/// @param pBaseAddress  Pointer to memory chain
/// @param offset        Memory offset
/// @param size          Memory chunk size
long CALLBACK ZN_GPUdmaSliceIn(unsigned long* pBaseAddress, unsigned long offset, unsigned long size)
{
	GPUwriteDataMem(pBaseAddress + offset, size);
	return 0L;
}
/// @brief Give a direct core memory access chain to GPU driver
/// @param pBaseAddress  Pointer to memory chain
/// @param offset        Memory offset
/// @returns Success indicator
long CALLBACK ZN_GPUdmaChain(unsigned long* pBaseAddress, unsigned long offset)
{
	return GPUdmaChain(pBaseAddress, offset);
}

/// @brief Set custom fixes from emulator
/// @param dataMode  Transaction type (0 = setter / 1 = getter / 2 = slot selection)
/// @param pMem      Save-state structure pointer (to read or write)
/// @returns Success/compatibility indicator
long CALLBACK ZN_GPUfreeze(unsigned long dataMode, void * pMem)
{
	GPUFreeze_t* pFreeze = (GPUFreeze_t*)pMem;
	return GPUfreeze(dataMode, pFreeze);
}


// -- runtime settings -- ------------------------------------------------------

/// @brief Plugin - Load test
/// @returns Success indicator (0 = ok, 1 = warning or -1 = error)
long CALLBACK ZN_GPUtest()
{
	return GPUtest();
}
/// @brief Set special display flags
/// @param flags  Display flags
void CALLBACK ZN_GPUdisplayFlags(unsigned long flags)
{
	GPUdisplayFlags(flags);
}


// -- snapshots -- -------------------------------------------------------------

/// @brief Request snapshot (on next display)
void CALLBACK ZN_GPUmakeSnapshot()
{
	GPUmakeSnapshot();
}
/// @brief Get screen picture
/// @param pMem  allocated screen picture container 128x96 px (24b/px: 8-8-8 bit BGR, no header)
void CALLBACK ZN_GPUgetScreenPic(unsigned char * pMem)
{
	GPUgetScreenPic(pMem);
}
/// @brief Store and display screen picture
/// @param pMem  screen picture data 128x96 px (24b/px: 8-8-8 bit BGR, no header)
void CALLBACK ZN_GPUshowScreenPic(unsigned char * pMem)
{
	GPUshowScreenPic(pMem);
}


// -- miscellaneous -- ---------------------------------------------------------

#ifndef _WINDOWS
/// @brief Keyboard event management
/// @param keycode  Event code
void CALLBACK ZN_GPUkeypressed(int keycode)
{
	InputManager::GPUkeypressed(keycode);
}
#endif
