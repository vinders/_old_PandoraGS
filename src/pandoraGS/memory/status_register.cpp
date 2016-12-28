/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   status_register.h
Description : status register and status information
*******************************************************************************/
using namespace std;
#include "status_register.h"
uint32_t StatusRegister::s_statusReg = 0;   // GPU status register
long StatusRegister::s_busyEmuSequence = 0L;// 'GPU busy' emulation hack - sequence value
std::string StatusRegister::s_gameId("");   // game executable ID
