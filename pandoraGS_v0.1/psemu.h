/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   psemu.h
    Description : PSEmu Pro specification constants
*******************************************************************************/
#ifndef _PSEMU_H
#define _PSEMU_H

/*  PSEmu Plugin Developer Kit Header definition
	(C)1998 Vision Thing
	This file can be used only to develop PSEmu Plugins
	Other usage is highly prohibited. 
*/

// header version
#define _PPDK_HEADER_VERSION		1

// plugin type returned by PSEgetLibType (types can be merged if plugin is multi type!)
#define PSE_LT_CDR					1
#define PSE_LT_GPU					2
#define PSE_LT_SPU					4
#define PSE_LT_PAD					8

// every function in DLL if completed sucessfully should return this value
#define PSE_ERR_SUCCESS				0
// undefined error but fatal one, that kills all functionality
#define PSE_ERR_FATAL				-1

// initialization went OK
#define PSE_INIT_ERR_SUCCESS		0
// this driver is not configured
#define PSE_INIT_ERR_NOTCONFIGURED	-2
// this driver can not operate properly on this hardware or hardware is not detected
#define PSE_INIT_ERR_NOHARDWARE		-3


/* GPU PlugIn --------------------------------------------------------------- */

//  GPU_Test return values
// success, everything configured, and went OK.
#define PSE_GPU_ERR_SUCCESS			0

// this driver is not configured
#define PSE_GPU_ERR_NOTCONFIGURED	1

// gpuQueryS.flags
// this driver requests windowed mode,
#define PSE_GPU_FLAGS_WINDOWED		1

// gpuQueryS.status
// this driver cannot operate in this windowed mode
#define PSE_GPU_STATUS_WINDOWWRONG	1

//  GPU_Query	End	- will be implemented in v2


/* CDR PlugIn --------------------------------------------------------------- */

//	CDR_Test return values
// success, everything configured, and went OK.
#define PSE_CDR_ERR_SUCCESS 0

// ERRORS
#define PSE_CDR_ERR -40
// this driver is not configured
#define PSE_CDR_ERR_NOTCONFIGURED	PSE_CDR_ERR - 0
// if this driver is unable to read data from medium
#define PSE_CDR_ERR_NOREAD			PSE_CDR_ERR - 1

// WARNINGS
#define PSE_CDR_WARN 40
// if this driver emulates lame mode ie. can read only 2048 tracks and sector header is emulated
// this might happen to CDROMS that do not support RAW mode reading - surelly it will kill many games
#define PSE_CDR_WARN_LAMECD			PSE_CDR_WARN + 0


#endif
