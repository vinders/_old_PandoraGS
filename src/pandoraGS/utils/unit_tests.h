/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   unit_tests.h
Description : plugin unit testing
*******************************************************************************/
#ifndef _UNIT_TESTS_H
#define _UNIT_TESTS_H
#include "globals.h"

// data types
enum unit_id_t : unsigned long // unit IDs
{
    Unit_geometry,
    Unit_logger,
    Unit_system_tools,
    Unit_timer,
    Unit_input_reader,
    Unit_status_register,
    Unit_video_memory,
    Unit_lang,
    Unit_config_profile,
    Unit_config,
    Unit_config_io,
    Unit_shader,
    Unit_display_state,
    Unit_engine,
    Unit_dispatcher
};

/// <summary>Plugin - full unit testing</summary>
/// <param name="pWinData">Window handle reference</param>
/// <returns>Success indicator</returns>
long CALLBACK GPUtestUnits(void* pWinData);

/// <summary>Test one unit</summary>
/// <param name="unit">Unit to test</param>
/// <param name="pWinData">Window handle reference (optional)</param>
/// <returns>Success indicator</returns>
bool testUnit(unit_id_t unit, void* pWinData = NULL);

/// <summary>Plugin - primitive testing</summary>
/// <param name="pData">Primitive raw data</param>
/// <param name="len">Primitive data length (number of 32bits blocks)</param>
/// <param name="isFlipped">Flip indicator (only for rectangles)</param>
void CALLBACK GPUtestPrimitive(unsigned char* pData, int len, bool isFlipped);

#endif
