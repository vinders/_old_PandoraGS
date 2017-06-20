/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : system/CPU architecture
*******************************************************************************/
#pragma once

// -- Operating system --

#define _OS_MICROSOFT_  1
#define _OS_APPLE_      2
#define _OS_LINUX_      3
#define _OS_UNIX_       4
#define _OS_SONY_       5
#define _OS_NINTENDO_   6
#define _OS_UNKNOWN_    0

#define _OS_MICROSOFT_WINDOWS_DESKTOP_  1
#define _OS_MICROSOFT_WINDOWS_MOBILE_   2
#define _OS_MICROSOFT_XBOX_ONE_         3
#define _OS_APPLE_MACOS_DESKTOP_        4
#define _OS_APPLE_MACOS_LEGACY_         5
#define _OS_APPLE_IOS_MOBILE_           6
#define _OS_GNULINUX_DESKTOP_           7
#define _OS_ANDROID_MOBILE_             8
#define _OS_UNIX_DESKTOP_               9
#define _OS_UNIX_LEGACY_                10
#define _OS_SONY_PLAYSTATION4_          11
#define _OS_NINTENDO_SWITCH_            12

#define __OPERATING_SYSTEM_FAMILY__ _OS_UNKNOWN_
#define __OPERATING_SYSTEM__ _OS_UNKNOWN_

// Windows
#ifndef _WINDOWS
#   if defined(_WIN32) or defined(_WIN64) or defined(_WINNT) or defined(_MSC_VER)
#       define __OPERATING_SYSTEM_FAMILY__ _OS_MICROSOFT_
#       ifndef __WINDOWS__
#           define __WINDOWS__
#       endif
#       undef __APPLE__
#       undef __ANDROID__
#       undef __LINUX__

#       if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#           define __OPERATING_SYSTEM__ _OS_MICROSOFT_WINDOWS_DESKTOP_
#       else
#           define __OPERATING_SYSTEM__ _OS_MICROSOFT_WINDOWS_MOBILE_
#       endif
#   endif
#else
#   undef __WINDOWS__
    
// Apple
#   ifdef __APPLE__
#       define __OPERATING_SYSTEM_FAMILY__ _OS_APPLE_
#       include "TargetConditionals.h"
#       undef __ANDROID__
#       undef __LINUX__

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
#       define __OPERATING_SYSTEM_FAMILY__ _OS_LINUX_
#       define __OPERATING_SYSTEM__ _OS_ANDROID_MOBILE_
#       undef __UNIX__
#       undef __APPLE__
#   endif

// Linux
#   ifdef __linux__
#       define __OPERATING_SYSTEM_FAMILY__ _OS_LINUX_
#       ifndef __LINUX__
#           define __LINUX__
#       endif
#       undef __UNIX__
#       undef __APPLE__

#       define __OPERATING_SYSTEM__ _OS_GNULINUX_DESKTOP_

// UNIX
#   elif defined(__unix__)
#       define __OPERATING_SYSTEM_FAMILY__ _OS_UNIX_
#       ifndef __UNIX__
#           define __UNIX__
#       endif
#       undef __LINUX__
#       undef __ANDROID__

#       define __OPERATING_SYSTEM__ _OS_UNIX_DESKTOP_

#   elif defined(_POSIX_VERSION)
#       define __OPERATING_SYSTEM_FAMILY__ _OS_UNIX_
#       ifndef __UNIX__
#           define __UNIX__
#       endif
#       undef __LINUX__
#       undef __ANDROID__

#       define __OPERATING_SYSTEM__ _OS_UNIX_LEGACY_
#   endif
#endif


// -- CPU architecture --

#define _CPU_32_BIT_     0
#define _CPU_64_BIT_     1
#define _CPU_128_BIT_    2

#define _CPU_ARCH_X86_     1
#define _CPU_ARCH_ARM_     2
#define _CPU_ARCH_PPC_     3
#define _CPU_ARCH_SPARC_   4

#define __CPU_ARCHITECTURE_FAMILY__ _CPU_ARCH_X86_
#define __CPU_ARCHITECTURE_BITS__ _CPU_32_BIT_

// Intel/AMD x86_64
#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64) || defined(_M_X64) || defined(_M_I64) || defined(_M_IX64) || defined(_M_AMD64) || defined(__IA64__)
#   define __CPU_ARCHITECTURE_FAMILY__ _CPU_ARCH_X86_
#   define __CPU_ARCHITECTURE_BITS__ _CPU_64_BIT_

// Intel/AMD x86
#elif defined(i386) || defined(__i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) || defined(__IA32__) || defined(_M_I86) || defined(_M_IX86) || defined(__X86__) || defined(_X86_) || defined (__I86__)
#   define __CPU_ARCHITECTURE_FAMILY__ _CPU_ARCH_X86_
#   define __CPU_ARCHITECTURE_BITS__ _CPU_32_BIT_

// ARM
#elif defined(__arm__) || defined(_M_ARM) || defined(__arm) || defined(_ARM) || defined(_M_ARMT) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB)
#   define __CPU_ARCHITECTURE_FAMILY__ _CPU_ARCH_ARM_
#   ifdef __ARM_ARCH_7__
#       define __CPU_ARCHITECTURE_BITS__ _CPU_32_BIT_
#   elif defined(__ARM_ARCH_8__)
#       define __CPU_ARCHITECTURE_BITS__ _CPU_64_BIT_
#   elif defined(__ARM_ARCH_9__)
#       define __CPU_ARCHITECTURE_BITS__ _CPU_64_BIT_
#   else
#       define __CPU_ARCHITECTURE_BITS__ _CPU_32_BIT_
#   endif

// PowerPC
#elif defined(__powerpc) || defined(__powerpc__) || defined(_M_PPC) || defined(_ARCH_PPC) || defined(_ARCH_PPC64) || defined(__powerpc64__)
#   define __CPU_ARCHITECTURE_FAMILY__ _CPU_ARCH_PPC_
#   if defined(_ARCH_PPC64) || defined(__powerpc64__)
#       define __CPU_ARCHITECTURE_BITS__ _CPU_64_BIT_
#   else
#       define __CPU_ARCHITECTURE_BITS__ _CPU_32_BIT_
#   endif

// SPARC
#elif defined(__sparc__) || defined(__sparc)
#   define __CPU_ARCHITECTURE_FAMILY__ _CPU_ARCH_SPARC_
#   define __CPU_ARCHITECTURE_BITS__ _CPU_32_BIT_
#endif
