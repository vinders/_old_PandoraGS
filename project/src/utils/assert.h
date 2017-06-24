/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : debug assertions (assert, verify, trace)
*******************************************************************************/
#pragma once

#include <cstddef>
#include "system/system_info.h"

#if defined(_DEBUG) || !defined(NDEBUG)
// -- DEBUG MODE --

// portable __debugbreak
#ifdef _WINDOWS
#   include <intrin.h>
#   ifndef __debugbreak
#       define __debugbreak() __asm{int 0x03}
#   endif
#else
#   if __CPU_ARCHITECTURE_FAMILY__ == _CPU_ARCH_ARM_
#       include <armintrin.h>
#   elif __CPU_ARCHITECTURE_FAMILY__ == _CPU_ARCH_X86_
#       include <x86intrin.h>
#   endif
#   ifndef __debugbreak
#       ifdef __builtin_trap
#           define __debugbreak() __builtin_trap()
#       else
#           include <cstdlib>
#           define __debugbreak() abort(3)
#       endif
#   endif
#endif

#include <cstdio>

// debug assertion / verification
#define ASSERT(cond)       if(!(cond)){__debugbreak();}       // check condition value (completely removed in release mode)
#define VERIFY(cmd)        if(!(cmd)){__debugbreak();}        // check condition command (command kept in release mode)
#define VERIFY_EQ(cmd,val) if((cmd)==(val)){__debugbreak();}  // check command equality (command kept in release mode)
#define VERIFY_NE(cmd,val) if((cmd)!=(val)){__debugbreak();}  // check command difference (command kept in release mode)
   
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
