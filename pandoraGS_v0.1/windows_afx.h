// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

// Windows constants -----------------------------------------------------------

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN   // exclude rarely-used stuff from Windows headers
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN  // exclude rarely-used stuff from Windows headers
#endif

#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include "windows_target.h"

#include <windows.h>
#include <tchar.h>

#include "resource.h"
