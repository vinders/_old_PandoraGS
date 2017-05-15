/*******************************************************************************
Description : windows general API specific includes
*******************************************************************************/
#pragma once

#ifdef _WINDOWS
#   if !defined(WINVER) OR WINVER < 0x0600
#       define WINVER 0x0600
#   endif
#   if !defined(_WIN32_WINNT) OR _WIN32_WINNT < 0x0600
#       define _WIN32_WINNT 0x0600
#   endif

#   define NOMINMAX
#   define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#   ifndef WINVER
#   include <Windows.h>
#endif
