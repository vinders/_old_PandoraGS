/*******************************************************************************
Description : windows general API specific includes
*******************************************************************************/
#pragma once

#ifdef _WINDOWS
    // win32 manifest
#   pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

    // enable all features available on platform versions up to the one specified
#   include <WinSDKVer.h>
#   if !defined(WINVER) OR WINVER < 0x0600 // required platform
#       define WINVER 0x0600
#   endif
#   if !defined(_WIN32_WINNT) OR _WIN32_WINNT < 0x0600 // required NT platform
#       define _WIN32_WINNT 0x0600
#   endif
#   if !defined(_WIN32_WINDOWS) OR _WIN32_WINDOWS < 0x0600 // required DOS-based platform
#      define _WIN32_WINDOWS 0x0600  
#   endif
#   ifndef _WIN32_IE // required browser is Internet Explorer 7.0
#       define _WIN32_IE 0x0700 
#   endif
#   include <SDKDDKVer.h>

    // disable unwanted features
#   ifndef NOMINMAX // no min/max macros
#       define NOMINMAX
#   endif
#   ifndef VC_EXTRALEAN // exclude rare MFC libraries
#       define VC_EXTRALEAN
#   endif
#   ifndef WIN32_LEAN_AND_MEAN // exclude rare libraries
#       define WIN32_LEAN_AND_MEAN
#   endif

    // windows API
#   include <windef.h>
#   include <Windows.h>
#endif
