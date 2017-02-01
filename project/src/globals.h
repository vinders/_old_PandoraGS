/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : global constants and settings
*******************************************************************************/
#pragma once

// plugin information
#define PLUGIN_VERSIONMAJ  0
#define PLUGIN_VERSIONMIN  3
#define PLUGIN_VERSIONSTR  "0.3.1"
#define PLUGIN_NAME        "PandoraGS Driver"
#define PLUGIN_INFO        "PandoraGS GPU Driver 0.3\nBy Romain Vinders"
#define PLUGIN_DESCRIPTION "GPU plugin for PSEmu compatible emulators"
#define PLUGIN_AUTHOR      "Romain Vinders"
#define PLUGIN_DATE        "2017"
#define PLUGIN_LANGUAGES   "English, Spanish, French, German"
#define PLUGIN_TECH        "C++11 (VC++ runtimes 2013)"
#define PLUGIN_API         "OpenGL 4.1"

// other information
#define PLUGIN_ABOUT_TITLE "About PandoraGS..."
#ifdef _WINDOWS
#define PLUGIN_THANKS      "No$psx, Doomed, J.Walker - for providing useful docs\
 \r\nPete, Tapeq, Asmodean - for sharing public sources\
 \r\nePSXe team - for being helpful and adding game ID spec\
 \r\nMednafen - for sharing useful links and tools\
 \r\nThe OpenGL SuperBible, Open.gl - for teaching me OpenGL"
#else
#define PLUGIN_THANKS      "No$psx, Doomed, J.Walker - for providing useful docs\
 \nPete, Tapeq, Asmodean - for sharing public sources\
 \nePSXe team - for being helpful and adding game ID spec\
 \nMednafen - for sharing useful links and tools\
 \nThe OpenGL SuperBible, Open.gl - for teaching me OpenGL"
#endif

// compilation settings - dialog API
#define DIALOGAPI_WIN32  1
#define DIALOGAPI_QT     2
#ifdef _WINDOWS
#define _DIALOGAPI       DIALOGAPI_WIN32
#else
#define _DIALOGAPI       DIALOGAPI_QT // cross-platform
#endif
// compilation settings - trace psemu calls
#define _TRACE_CALLS     0 // 0 - disabled / 1 - enabled


// -- SYSTEM COMPATIBILITY - WINDOWS -- ----------------------------------------

#ifdef _WINDOWS
// win32 manifest
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
// exclude rarely-used libraries
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

// functions / macros
#define snprintf _snprintf

// win32 resources identifiers
#include "res/targetver.h"
#include "res/resource.h"

// C++11 alternatives
#ifdef _WINDOWS
    #ifdef _MSC_VER 
        #if _MSC_VER <= 1800
        #define NOEXCEPT
        #else
        #define NOEXCEPT noexcept
        #endif
    #else
    #define NOEXCEPT noexcept
    #endif
#else
#define NOEXCEPT noexcept
#endif


// -- SYSTEM COMPATIBILITY - LINUX/UNIX -- -------------------------------------

#else 
// functions / macros
#ifndef LOWORD
#define LOWORD(l)   ((unsigned short)(l))
#endif
#ifndef HIWORD
#define HIWORD(l)   ((unsigned short)(((unsigned long)(l) >> 16) & 0xFFFF))
#endif
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#ifndef strcpy_s
#define strcpy_s(a,size,b) strcpy(a,b)
#endif
#endif

#ifndef CALLBACK
#define CALLBACK __stdcall
#endif
