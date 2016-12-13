/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   status_register.cpp
Description : status register and status information
*******************************************************************************/
using namespace std;
#include "status_register.h"

// emulated gpu status
int32_t StatusRegister::s_statusReg = 0;       // GPU status register
std::string StatusRegister::s_gameId("");      // game executable ID
long StatusRegister::s_selectedSaveSlot = 0L;  // selected save-state slot

/// <summary>Initialize status</summary>
void StatusRegister::init()
{
    s_statusReg = GPUSTATUS_INIT;
    setStatus(GPUSTATUS_IDLE | GPUSTATUS_READYFORCOMMANDS);
}
