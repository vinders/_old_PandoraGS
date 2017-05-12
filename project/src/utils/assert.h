/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : debug assertions (assert, verify, trace)
*******************************************************************************/
#pragma once

#include <cstddef>

#if defined(_DEBUG) || !defined(NDEBUG)
// -- DEBUG MODE --

// portable __debugbreak
#ifdef _MSC_VER
#   include <intrin.h>
#   ifndef __debugbreak
#       define __debugbreak() __asm{int 0x03}
#   endif
#else
#   if defined(__arm__) || defined(_M_ARM) || defined(__arm) || defined(_ARM) || defined(_M_ARMT) || defined(__thumb__)
#       include <armintrin.h>
#   else
#       include <x86intrin.h>
#   endif
#   ifndef __debugbreak
#       define __debugbreak() __builtin_trap()
#   endif
#endif

#include <string>
#ifdef _WINDOWS
#   include <Windows.h> // OutputDebugString
#else
#   include <cstdio>    // printf
#endif

// debug assertion / verification
#define ASSERT(cond)       if(!(cond)){__debugbreak();}       // check condition value (completely removed in release mode)
#define VERIFY(cmd)        if(!(cmd)){__debugbreak();}        // check condition command (command kept in release mode)
#define VERIFY_EQ(val,cmd) if((cmd)==(val)){__debugbreak();}  // check command equality (command kept in release mode)
#define VERIFY_NE(val,cmd) if((cmd)!=(val)){__debugbreak();}  // check command difference (command kept in release mode)
   
// debug tracer
#define TRACE(format,...) fprintf(stderr, "%s(%d): " format "\n", __FILE__, __LINE__, __VA_ARGS__)


#else
// -- RELEASE MODE --
  
#ifndef __noop
#   define __noop ((void)0)
#endif

// ignore debug messages
#define ASSERT(cond)       __noop  // no assertion
#define VERIFY(cmd)        (cmd)   // no verification but command executed
#define VERIFY_EQ(cmd,val) (cmd)   // no verification but command executed
#define VERIFY_NE(cmd,val) (cmd)   // no verification but command executed
#define TRACE(format,...)  __noop  // no tracer

#endif
