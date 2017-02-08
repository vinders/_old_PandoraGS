/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : unit testing utility
*******************************************************************************/
#pragma once

#include "globals.h"

#ifdef _WINDOWS
/// @brief Plugin - full unit testing
/// @param hWindow  Main window handle
/// @returns Success indicator
long CALLBACK GPUtestUnits(HWND hWindow);
#else
/// @brief Plugin - full unit testing
/// @param pDisplayId   Display identifier
/// @param pCaption     Window caption
/// @param pConfigFile  Config file path
/// @returns Success indicator
long CALLBACK GPUtestUnits(unsigned long* pDisplayId, char* pCaption, char* pConfigFile);
#endif

/// @brief Plugin - primitive testing
/// @param pData      Primitive raw data
/// @param len        Primitive data length (number of 32bits blocks)
void CALLBACK GPUtestPrimitive(unsigned long* pData, int len);
