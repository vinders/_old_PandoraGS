/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : unit testing utility
*******************************************************************************/
#include "globals.h"
#include <string>
using namespace std::literals::string_literals;
#include "psemu_main.h"
#include "pandoraGS.h"
#include "unit_tests.h"
using namespace std;


/// @brief Plugin - full unit testing
/// @param pWinData  Window handle reference
/// @returns Success indicator
long CALLBACK GPUtestUnits(void* pWinData)
{

    return PSE_SUCCESS;
}

/// @brief Plugin - primitive testing
/// @param pData      Primitive raw data
/// @param len        Primitive data length (number of 32bits blocks)
/// @param isFlipped  Flip indicator (only for rectangles)
void CALLBACK GPUtestPrimitive(unsigned long* pData, int len, bool isFlipped)
{

}
