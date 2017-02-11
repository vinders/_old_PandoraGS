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
#define PLUGIN_TECH        "C++14 (VC++ runtimes 2015), OpenGL 4.1"

// other information
#define LOGO_WIDTH   140
#define LOGO_HEIGHT  44
#define PLUGIN_ABOUT_TITLE "About PandoraGS..."
#ifdef _WINDOWS
#define PLUGIN_THANKS      "No$psx, Doomed - for providing useful docs\
 \r\nPete, Tapeq - for sharing public sources\
 \r\nePSXe team - for supporting game ID spec\
 \r\nMednafen - for sharing useful links/tools\
 \r\nThe OpenGL SuperBible and website open.gl"
#else
#define PLUGIN_THANKS      "No$psx, Doomed - for providing useful docs\
 \nPete, Tapeq - for sharing public sources\
 \nePSXe team - for supporting game ID spec\
 \nMednafen - for sharing useful links/tools\
 \nThe OpenGL SuperBible and website open.gl"
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
#define _UNITTEST_APP_NAME "UNITTEST.001"


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

// win32 resources identifiers
#include "res/targetver.h"
#include "res/resource.h"


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
