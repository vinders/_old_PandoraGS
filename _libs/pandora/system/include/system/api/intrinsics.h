/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
--------------------------------------------------------------------------------
System-dependant includes of intrinsics
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cassert>

// -- windows includes --

#if defined(__MINGW32__) || defined(_WINDOWS) || defined(_WIN32) || defined(WIN32) || defined(_WIN64)
# if defined(M_ARM64)
#   include <arm64intr.h>
#   include <arm64_neon.h>
# elif defined(M_ARM)
#   include <armintr.h>
#   include <arm_neon.h>
# else
#   include <intrin.h>
# endif
  
// -- linux / unix includes --

#elif defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
# if !defined(__clang__)
#   include <fcntl.h>
#   include <unistd.h>
#   if defined(__linux__)
#     include <elf.h>
#     include <linux/auxvec.h>
#   endif
# endif
# if defined(__APPLE__)
#   include <TargetConditionals.h>
# endif

# if defined(__aarch64__)
#   if (!defined(__APPLE__) || !defined(TARGET_OS_IPHONE) || !TARGET_OS_IPHONE)
#     include <arm64intr.h>
#   endif
#   include <arm64_neon.h>
# elif defined(__ARM_NEON__)
#   if (!defined(__APPLE__) || !defined(TARGET_OS_IPHONE) || !TARGET_OS_IPHONE)
#     include <armintr.h>
#   endif
#   include <arm_neon.h>
# elif defined(__IWMMXT__)
#   include <mmintrin.h>
# elif defined(__VEC__) || defined(__ALTIVEC__)
#   include <altivec.h>
# elif defined(__SPE__)
#   include <spe.h>
# else
#   include <x86intrin.h>
# endif
#endif
