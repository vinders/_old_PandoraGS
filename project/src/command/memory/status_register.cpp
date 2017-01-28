/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : GPU status register
*******************************************************************************/
#include "status_register.h"
using namespace command::memory;

uint32_t StatusRegister::s_statusReg;   // GPU status register
long StatusRegister::s_busyEmuSequence; // 'GPU busy' emulation hack - sequence value
