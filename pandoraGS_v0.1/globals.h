/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   globals.h
    Description : global plugin constants and variables
*******************************************************************************/
#ifndef _GLOBALS_H
#define _GLOBALS_H

/* emulator specification */
#include "psemu.h"

/* plugin identification */
#define PLUGIN_VERSIONMAJ  0
#define PLUGIN_VERSIONMIN  1
#define PLUGIN_STRVERSION "0.1"
#define PLUGIN_NAME     "PandoraGS Driver"

/* plugin description */
#define PLUGIN_AUTHOR      "Romain Vinders"
#define PLUGIN_DATE        "2015"
#define PLUGIN_ABOUT_TITLE "About PandoraGS GPU renderer"
#define PLUGIN_SUPPORT_GL  "Software (DX11 warp), OpenGL 4.1"

/* system compilation */
#include <stddef.h>
#define _WINDOWS  1
#define _LINUX    2
#define _MACOS    3
#define _ANDROID  4
#ifndef _SYSTEM
    #define _SYSTEM _WINDOWS     // default target system
#endif

/* form dialogs API */
#define _WINAPI     1
#define _QTAPI      2
#if _SYSTEM == _WINDOWS
    #define _DIALOGAPI _WINAPI   // windows dialog/form API
#endif
#ifndef _DIALOGAPI
    #define _DIALOGAPI _QTAPI    // cross-platform dialog/form API
#endif


// OPERATING SYSTEM ------------------------------------------------------------

/* system specific inclusions */
#if _SYSTEM == _WINDOWS
    #include "windows_afx.h"    // todo: disable precompiled headers in VS
#endif
#if _DIALOGAPI == _QTAPI
    #ifdef _INCLUDE_DIALOGAPI
        //...
    #endif
#endif

/* system compatibility */
#if _SYSTEM == _WINDOWS
    #define snprintf _snprintf
  #else
    #define FALSE 0
    #define TRUE  1
    #define BOOL  unsigned short

    #define LOWORD(l)           ((unsigned short)(l))
    #define HIWORD(l)           ((unsigned short)(((unsigned long)(l) >> 16) & 0xFFFF))
    #define max(a,b)            (((a) > (b)) ? (a) : (b))
    #define min(a,b)            (((a) < (b)) ? (a) : (b))
    #define DWORD unsigned long
#endif
#ifndef CALLBACK
    #define CALLBACK __stdcall
#endif


// CONVENIENCE CONSTANTS -------------------------------------------------------

/* memory size constants */
// standard bit sizes
#define BYTE_BITSIZE         8
#define WORD_BITSIZE         16
#define DWORD_BITSIZE        (sizeof(long)*8)
// standard byte sizes
#define BYTE_SIZE            1
#define WORD_SIZE            2
#define DWORD_SIZE           sizeof(long)
// kilobyte VRAM sizes
#define PSVRAM_SIZE          512
#define ZINCVRAM_SIZE        1024
#define PSVRAM_SECURE_EXTRA  1024
#define PSVRAM_SECURE_OFFSET 512
// array sizes
#define PSRAWINFO_SIZE       16
#define PSSTATUSCONTROL_SIZE 256
// bytes masks
#define DOUBLEBYTES_MASK     0xFFFF
#define TRIPLEBYTES_MASK     0xFFFFFF
#define PSVRAM_MASK          0x1FFFFC // 2097148

/* renderer constants */
// raw values
#define GPUDATA_INIT        0x400   // initial value
// array indexes
#define GPUINFO_TW          0
#define GPUINFO_DRAWSTART   1
#define GPUINFO_DRAWEND     2
#define GPUINFO_DRAWOFF     3
// framerate settings values
#define FPSMODE_PERFCOUNTER true
#define FPSMODE_GETTIME     false
#define FPS_USERDEFAULT     59.94f
#define FPS_NTSC            59.82750f   // 54.94
#define FPS_NTSC_IL         59.94146f
#define FPS_NTSC_FIX        30.0f
#define FPS_NTSC_FIXIL      60.0f
#define FPS_PAL             49.76351f   // 50.0
#define FPS_PAL_IL          50.00238f
#define FPS_PAL_FIX         25.0f
#define FPS_PAL_FIXIL       50.0f
// display settings values
#define DISPLAY_BASEWIDTH   320
#define DISPLAY_BASEHEIGHT  240
#define DISPLAY_RGB15       0
#define DISPLAY_RGB24       1
#define DISPLAY_NTSC        0
#define DISPLAY_PAL         1
// special fixes
#define FIX_FPSLIMIT        0x0080
#define FIX_FPSOLDSKIP      0x0100
#define FIX_ODDEVENBIT      0x0200
#define FIX_NOFPSCOUNTER    0x1000
#define FIX_DISPWIDTHS      0x2000

/* dialog draw constants */
// dialog sizes
#define LOGO_WIDTH          140
#define LOGO_HEIGHT         44
#define ICONS_SIZE          48
#define PAGETAB_INTERVAL    6
#define PAGETAB_FIRST_Y     46
#define PAGETAB_HEIGHT      90
#define PAGETAB_TEXT_Y      64
// dialog colors
#define COLOR_PAGE          255
#define COLOR_BORDER_R      210
#define COLOR_BORDER_G      216
#define COLOR_BORDER_B      220
#define COLOR_MENUTOP_R     247
#define COLOR_MENUTOP_G     250
#define COLOR_MENUTOP_B     251
#define COLOR_MENUBTM_R     217
#define COLOR_MENUBTM_G     227
#define COLOR_MENUBTM_B     236
#define COLOR_MENUBRD_R     227
#define COLOR_MENUBRD_G     232
#define COLOR_MENUBRD_B     236


// LOW-LEVEL CONSTANTS ---------------------------------------------------------

/* memory management constants */
// transfer modes
#define DR_NORMAL                   0
#define DR_VRAMTRANSFER             1
// status register
#define GPUSTATUS_INIT              0x14802000 // initial values
#define GPUSTATUS_ODDLINES          0x80000000
#define GPUSTATUS_DMABITS           0x60000000 // 2 bits
#define GPUSTATUS_READYFORCOMMANDS  0x10000000
#define GPUSTATUS_READYFORVRAM      0x08000000
#define GPUSTATUS_IDLE              0x04000000
#define GPUSTATUS_DISPLAYDISABLED   0x00800000
#define GPUSTATUS_INTERLACED        0x00400000
#define GPUSTATUS_RGB24             0x00200000
#define GPUSTATUS_PAL               0x00100000
#define GPUSTATUS_DOUBLEHEIGHT      0x00080000
#define GPUSTATUS_WIDTHBITS         0x00070000 // 3 bits
#define GPUSTATUS_MASKENABLED       0x00001000
#define GPUSTATUS_MASKDRAWN         0x00000800
#define GPUSTATUS_DRAWINGALLOWED    0x00000400
#define GPUSTATUS_DITHER            0x00000200
// status change macros
#define setGpuStatus(STATUSREG, STATUSVAL)    (STATUSREG |= STATUSVAL)
#define unsetGpuStatus(STATUSREG, STATUSVAL)  (STATUSREG &= ~STATUSVAL)

/* emulator commands constants */
// get commands macro
#define getGpuCommand(DATA)   ((DATA>>24)&0x0FF) // get only bits 25 and 26
// setter commands
#define CMD_RESETGPU            0x00
#define CMD_TOGGLEDISPLAY       0x03
#define CMD_SETTRANSFERMODE     0x04
#define CMD_SETDISPLAYPOSITION  0x05
#define CMD_SETDISPLAYWIDTH     0x06
#define CMD_SETDISPLAYHEIGHT    0x07
#define CMD_SETDISPLAYINFO      0x08
#define CMD_GPUREQUEST          0x10
// request codes
#define REQ_TW                  0x02
#define REQ_DRAWSTART           0x03
#define REQ_DRAWEND             0x04
#define REQ_DRAWOFFSET1         0x05
#define REQ_DRAWOFFSET2         0x06
#define REQ_GPUTYPE             0x07
#define REQ_BIOSADDR1           0x08
#define REQ_BIOSADDR2           0x0F
#define SAVESTATE_SET           0
#define SAVESTATE_GET           1
#define SAVESTATE_INFO          2
// request replies
#define GPUBIOSADDR             0xBFC03720
#define INFO_TW                 0
#define INFO_DRAWSTART          1
#define INFO_DRAWEND            2
#define INFO_DRAWOFF            3


// GENERAL STRUCTURES ----------------------------------------------------------

/* graphic components */
typedef struct
{
    short x;
    short y;
} WPoint_t;
typedef struct
{
    long x;
    long y;
} DwPoint_t;

typedef struct
{
    short left;
    short top;
    short right;
    short bottom;
} WRect_t;
typedef struct
{
    short x;
    short y;
    short width;
    short height;
} WRectSize_t;

#endif
