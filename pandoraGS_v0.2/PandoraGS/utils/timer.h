/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   timer.h
Description : frame time management
*******************************************************************************/
#ifndef _TIMER_H
#define _TIMER_H
#include <cstdint>

// data types
enum timemode_t : uint32_t // timing modes
{
    Timemode_multimediaClock = 0, // low res, steady
    Timemode_highResCounter = 1   // high res, unstable
};



#endif
