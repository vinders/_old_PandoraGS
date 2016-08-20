/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   core_memory_commands.h
Description : vram commands and status register values
*******************************************************************************/
#ifndef _CORE_MEMORY_COMMANDS_H
#define _CORE_MEMORY_COMMANDS_H

// dma access masks
#define THREEBYTES_MASK       0x0FFFFFF
#define PSXVRAM_MASK          0x1FFFFC // 2097148
#define PSXVRAM_THRESHOLD     2000000

// status bits masks
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
// data transfer modes
#define DR_NORMAL               0
#define DR_VRAMTRANSFER         1
// data transaction codes
#define GPUDATA_INIT            0x400
#define GPUDATA_BIOSADDR        0xBFC03720
#define GPUINFO_TW              0
#define GPUINFO_DRAWSTART       1
#define GPUINFO_DRAWEND         2
#define GPUINFO_DRAWOFF         3

// packet information access (read commands)
#define getGpuCommand(DATA)    ((DATA>>24)&0x0FF) // get only bits 25 and 26
// commands
#define CMD_RESETGPU            0x00
#define CMD_TOGGLEDISPLAY       0x03
#define CMD_SETTRANSFERMODE     0x04
#define CMD_SETDISPLAYPOSITION  0x05
#define CMD_SETDISPLAYWIDTH     0x06
#define CMD_SETDISPLAYHEIGHT    0x07
#define CMD_SETDISPLAYINFO      0x08
#define CMD_GPUREQUESTINFO      0x10
// info request codes
#define REQ_TW                  0x02
#define REQ_DRAWSTART           0x03
#define REQ_DRAWEND             0x04
#define REQ_DRAWOFFSET1         0x05
#define REQ_DRAWOFFSET2         0x06
#define REQ_GPUTYPE             0x07
#define REQ_BIOSADDR1           0x08
#define REQ_BIOSADDR2           0x0F
#define SAVESTATE_LOAD          0
#define SAVESTATE_SAVE          1
#define SAVESTATE_INFO          2
// info request replies
#define INFO_GPUBIOSADDR        0xBFC03720 // BIOS - virtual hardware address (last bits after memory)
#define INFO_TW                 0 // draw info - array index
#define INFO_DRAWSTART          1
#define INFO_DRAWEND            2
#define INFO_DRAWOFF            3

#endif
