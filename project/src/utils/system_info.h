/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : system/CPU architecture
*******************************************************************************/
#pragma once

// -- Operating system --

#define _OS_MICROSOFT_WINDOWS_DESKTOP_  1
#define _OS_MICROSOFT_WINDOWS_MOBILE_   2
#define _OS_APPLE_MACOS_DESKTOP_        3
#define _OS_APPLE_MACOS_LEGACY_         4
#define _OS_APPLE_IOS_MOBILE_           5
#define _OS_GNULINUX_DESKTOP_           6
#define _OS_UNIX_DESKTOP_               7
#define _OS_UNIX_LEGACY_                8
#define _OS_ANDROID_MOBILE_             9
#define _OS_UNKNOWN_                    0
#define __OPERATING_SYSTEM__ _OS_UNKNOWN_

// Windows
#ifndef _WINDOWS
#   if defined(_WIN32) or defined(_WIN64) or defined(_WINNT) or defined(_MSC_VER)
#       ifndef __WINDOWS__
#           define __WINDOWS__
#       endif
#       if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#           define __OPERATING_SYSTEM__ _OS_MICROSOFT_WINDOWS_DESKTOP_
#       else
#           define __OPERATING_SYSTEM__ _OS_MICROSOFT_WINDOWS_MOBILE_
#       endif
#   endif
#endif

#ifndef _WINDOWS
// Apple
#   ifdef __APPLE__
#       include "TargetConditionals.h"
#       ifdef TARGET_IPHONE_SIMULATOR
#           define __OPERATING_SYSTEM__ _OS_APPLE_IOS_MOBILE_
#       elif defined(TARGET_OS_IPHONE)
#           define __OPERATING_SYSTEM__ _OS_APPLE_IOS_MOBILE_
#       elif defined(TARGET_OS_MAC)
#           define __OPERATING_SYSTEM__ _OS_APPLE_MACOS_DESKTOP_
#       else
#           define __OPERATING_SYSTEM__ _OS_APPLE_MACOS_LEGACY_
#       endif
#   endif

// Android
#   ifdef __ANDROID__
#       define __OPERATING_SYSTEM__ _OS_ANDROID_MOBILE_
#   endif

// Linux
#   ifdef __linux__
#       ifndef __LINUX__
#           define __LINUX__
#       endif
#       define __OPERATING_SYSTEM__ _OS_GNULINUX_DESKTOP_

// UNIX
#   elif defined(__unix__)
#       ifndef __UNIX__
#           define __UNIX__
#       endif
#       define __OPERATING_SYSTEM__ _OS_UNIX_DESKTOP_
#   elif defined(_POSIX_VERSION)
#       ifndef __UNIX__
#           define __UNIX__
#       endif
#       define __OPERATING_SYSTEM__ _OS_UNIX_LEGACY_
#   endif
#endif


// -- CPU architecture --

#define _CPU_ARCH_x86_     1
#define _CPU_ARCH_x86_64_  2
#define _CPU_ARCH_ARM_     3
#define _CPU_ARCH_ARM_64_  4
#define _CPU_ARCH_PPC_     5
#define _CPU_ARCH_PPC_64_  6
#define _CPU_ARCH_SPARC_   7
#define __CPU_ARCHITECTURE__ _CPU_ARCH_x86_

// Intel/AMD x86_64
#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64) || defined(_M_X64) || defined(_M_I64) || defined(_M_IX64) || defined(_M_AMD64) || defined(__IA64__)
#   define __CPU_ARCHITECTURE__ _CPU_ARCH_x86_64_

// Intel/AMD x86_32
#elif defined(i386) || defined(__i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) || defined(__IA32__) || defined(_M_I86) || defined(_M_IX86) || defined(__X86__) || defined(_X86_) || defined (__I86__)
#   define __CPU_ARCHITECTURE__ _CPU_ARCH_x86_

// ARM
#elif defined(__arm__) || defined(_M_ARM) || defined(__arm) || defined(_ARM) || defined(_M_ARMT) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB)
#   ifdef __ARM_ARCH_7__
#       define __CPU_ARCHITECTURE__ _CPU_ARCH_ARM_
#   elif defined(__ARM_ARCH_8__)
#       define __CPU_ARCHITECTURE__ _CPU_ARCH_ARM_64_
#   elif defined(__ARM_ARCH_9__)
#       define __CPU_ARCHITECTURE__ _CPU_ARCH_ARM_64_
#   else
#       define __CPU_ARCHITECTURE__ _CPU_ARCH_ARM_
#   endif

// PowerPC
#elif defined(__powerpc) || defined(__powerpc__) || defined(_M_PPC) || defined(_ARCH_PPC) || defined(_ARCH_PPC64) || defined(__powerpc64__)
#   if defined(_ARCH_PPC64) || defined(__powerpc64__)
#       define __CPU_ARCHITECTURE__ _CPU_ARCH_PPC_64_
#   else
#       define __CPU_ARCHITECTURE__ _CPU_ARCH_PPC_
#   endif

// SPARC
#elif defined(__sparc__) || defined(__sparc)
#   define __CPU_ARCHITECTURE__ _CPU_ARCH_SPARC_
#endif
