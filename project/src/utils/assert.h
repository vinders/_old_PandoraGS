/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : debug assertions (assert, verify, trace)
*******************************************************************************/
#pragma once

#include <cstddef>

#ifdef _DEBUG
// -- DEBUG MODE --

#ifdef _x86
#include <x86intrin.h>
#else
    #ifdef _ARM
    #include <armintrin.h>
    #else
    #include <intrin.h> // __debugbreak
    #endif
#endif
#ifndef __debugbreak
#define __debugbreak() __asm{int 0x03}
#endif 

#include <string>
#ifdef _WINDOWS
#include <Windows.h> // OutputDebugString
#else
#include <cstdio>    // printf
#endif

// debug assertion / verification
#define ASSERT(cond)       if(!(cond)){__debugbreak();}       // check condition value (completely removed in release mode)
#define VERIFY(cmd)        if(!(cmd)){__debugbreak();}        // check condition command (command kept in release mode)
#define VERIFY_EQ(cmd,val) if((cmd)==(val)){__debugbreak();}  // check command equality (command kept in release mode)
#define VERIFY_NE(cmd,val) if((cmd)!=(val)){__debugbreak();}  // check command difference (command kept in release mode)

// debug tracer
inline void TRACE(std::string msg)
{
    #ifdef _WINDOWS
    OutputDebugString(msg.c_str());
    #else
    printf("%s", msg.c_str());
    #endif
}


#else
// -- RELEASE MODE --
  
#ifndef __noop
#define __noop ((void)0)
#endif

// ignore debug messages
#define ASSERT(cond)       __noop  // no verification
#define VERIFY(cmd)        (cmd)   // no verification but command executed
#define VERIFY_EQ(cmd,val) (cmd)   // no verification but command executed
#define VERIFY_NE(cmd,val) (cmd)   // no verification but command executed
#define TRACE(msg)         __noop  // no tracer

#endif
