// The minimum required platform is the earliest version of Windows, IE etc. 
// that has the necessary features to run your application.  The macros work by 
// enabling all features available on platform versions up to the one specified.
// Versions : Windows 7 (0x601), Vista (0x0600), XP (0x501), 98 (0x0410).
#ifndef WINVER           // minimum required platform.
#define WINVER 0x0600
#endif
#ifndef _WIN32_WINNT     // minimum required NT platform.
#define _WIN32_WINNT 0x0600    
#endif
#ifndef _WIN32_WINDOWS   // minimum required DOS-based platform.
#define _WIN32_WINDOWS 0x0600  
#endif
#ifndef _WIN32_IE        // minimum required platform is Internet Explorer 7.0.
#define _WIN32_IE 0x0700 
#endif

// Including SDKDDKVer.h defines the highest available Windows platform.
// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <SDKDDKVer.h>
