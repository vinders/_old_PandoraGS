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
int32_t StatusRegister::s_statusReg = 0;   // GPU status register
std::string StatusRegister::s_gameId("");  // game executable ID
