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

uint32_t StatusRegister::s_statusReg;  // GPU status register
long StatusRegister::s_busyEmuSequence;// 'GPU busy' emulation hack - sequence value
