/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   gpu_main.h
Description : driver service interface (PSEmu)
*******************************************************************************/
#ifndef _GPU_MAIN_H
#define _GPU_MAIN_H
#include "globals.h"


// -- DRIVER INIT INTERFACE -- -------------------------------------------------

/// <summary>Driver init (called once)</summary>
/// <returns>Success indicator</returns>
long CALLBACK GPUinit();
/// <summary>Driver shutdown (called once)</summary>
/// <returns>Success indicator</returns>
long CALLBACK GPUshutdown();

#ifdef _WINDOWS
#define GPUopen_PARAM_ GPUopen(HWND hWindow)
#else
#define GPUopen_PARAM_ GPUopen(unsigned long* displayId,char* caption,char* configFile)
#endif
/// <summary>Driver opening/reopening (game started)</summary>
/// <param name="hWindow">Emulator main window handle</param>
/// <returns>Success indicator</returns>
long CALLBACK GPUopen_PARAM_;
/// <summary>Driver closed (game stopped)</summary>
/// <returns>Success indicator</returns>
long CALLBACK GPUclose();

/// <summary>Activity update (called every vsync)</summary>
void CALLBACK GPUupdateLace();


// -- PLUGIN DIALOG INTERFACE -- -----------------------------------------------

/// <summary>Plugin - Open the configuration dialog box</summary>
/// <returns>Success indicator</returns>
long CALLBACK GPUconfigure();
/// <summary>Plugin - Open the 'about' dialog box</summary>
void CALLBACK GPUabout();

/// <summary>Plugin - Load unit tests</summary>
/// <returns>Success indicator (0 = ok, 1 = warning or -1 = error)</returns>
long CALLBACK GPUtest();


// -- PLUGIN LIBRARY INFO -- ---------------------------------------------------

/// <summary>Get PSemu library type</summary>
/// <returns>Library type (GPU)</returns>
unsigned long CALLBACK PSEgetLibType();
/// <summary>Get library title</summary>
/// <returns>Library name</returns>
char* CALLBACK PSEgetLibName();
/// <summary>Get library version</summary>
/// <returns>Full library version identifier</returns>
unsigned long CALLBACK PSEgetLibVersion();


// -- SETTERS -- ---------------------------------------------------------------

/// <summary>Enable/disable frame limit from emulator</summary>
/// <param name="option">Status (1 = limit / 0 = none)</param>
void CALLBACK GPUsetframelimit(unsigned long option);
/// <summary>Set custom fixes from emulator</summary>
/// <param name="fixBits">Fixes (bits)</param>
void CALLBACK GPUsetfix(unsigned long fixBits);


// -- SNAPSHOTS -- -------------------------------------------------------------

/// <summary>Request snapshot (on next display)</summary>
void CALLBACK GPUmakeSnapshot();
/// <summary>Get screen picture</summary>
/// <param name="pMem">allocated screen picture container 128x96 px (24b/px: 8-8-8 bit BGR, no header)</param>
void CALLBACK GPUgetScreenPic(unsigned char* pMem);
/// <summary>Store and display screen picture</summary>
/// <param name="pMem">screen picture data 128x96 px (24b/px: 8-8-8 bit BGR, no header)</param>
void CALLBACK GPUshowScreenPic(unsigned char* pmem);


// -- DATA TRANSFER INTERFACE -- -----------------------------------------------

// see "memory_dispatcher.h"


// -- MISCELLANEOUS -- ---------------------------------------------------------

/// <summary>Display debug text</summary>
/// <param name="pText">Text string</param>
void CALLBACK GPUdisplayText(char* pText);
/// <summary>Set gun cursor display and position</summary>
/// <param name="player">Player with current cursor (0-7)</param>
/// <param name="x">Cursor horizontal position (0-511)</param>
/// <param name="y">Cursor vertical position (0-255)</param>
void CALLBACK GPUcursor(int player, int x, int y);
/// <summary>Trigger screen vibration</summary>
/// <param name="smallVbr">Small rumble value</param>
/// <param name="bigVbr">Big rumble value (if != 0, smallVbr will be ignored)</param>
void CALLBACK GPUvisualVibration(unsigned long smallVbr, unsigned long bigVbr);


// -- UNIT TESTING -- ----------------------------------------------------------

// see "unit_tests.h"

#endif
