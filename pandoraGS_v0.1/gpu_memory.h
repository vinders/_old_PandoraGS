/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   gpu_memory.h
    Description : memory management interface (PSEmu Pro spec)
*******************************************************************************/
#ifndef _GPU_MEMORY_H
#define _GPU_MEMORY_H

/* save-states structure */
typedef struct
{
    unsigned long freezeVersion; // should always be 1 for now (set by main emu)
    unsigned long status;        // current gpu status
    unsigned long control[PSSTATUSCONTROL_SIZE]; // latest control register values
    unsigned char consoleVram[PSVRAM_SIZE*2];    // current VRam image
} GPUFreeze_t;


/* status register control interface functions */
unsigned long CALLBACK GPUreadStatus();
void CALLBACK GPUwriteStatus(unsigned long);
unsigned long CALLBACK GPUreadData();

/* data communication interface functions */
void CALLBACK GPUreadDataMem(unsigned long*, int);
void CALLBACK GPUwriteData(unsigned long);
void CALLBACK GPUwriteDataMem(unsigned long*, int);
long CALLBACK GPUdmaChain(unsigned long*, unsigned long);

/* misc interface functions */
long CALLBACK GPUfreeze(unsigned long, GPUFreeze_t*);

#endif
