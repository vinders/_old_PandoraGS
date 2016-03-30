/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   gpu_main.h
    Description : main driver interface (PSEmu Pro spec)
*******************************************************************************/
#ifndef _GPU_MAIN_H
#define _GPU_MAIN_H

/* main driver interface */
long CALLBACK GPUinit();
long CALLBACK GPUshutdown();
#if _SYSTEM == _WINDOWS
    #define GPUopen_ARGS_ GPUopen(HWND hWindow)
  #else
    #define GPUopen_ARGS_ GPUopen(unsigned long* displayId,char* caption,char* configFile)
#endif
long CALLBACK GPUopen_ARGS_;
long CALLBACK GPUclose();
void CALLBACK GPUupdateLace();

/* dialog call interface */
long CALLBACK GPUconfigure();
void CALLBACK GPUabout();
long CALLBACK GPUtest();

/* getters/setters interface */
unsigned long CALLBACK PSEgetLibType();
unsigned long CALLBACK PSEgetLibVersion();
char* CALLBACK PSEgetLibName();
long  CALLBACK GPUgetMode();
void  CALLBACK GPUsetMode(unsigned long);
void  CALLBACK GPUsetExeName(char*);
void CALLBACK GPUsetframelimit(unsigned long);
void CALLBACK GPUsetfix(unsigned long);

/* pictures interface */
void CALLBACK GPUmakeSnapshot();
void CALLBACK GPUgetScreenPic(unsigned char*);
void CALLBACK GPUshowScreenPic(unsigned char*);

/* miscellaneous interface */
void CALLBACK GPUdisplayText(char*);
void CALLBACK GPUdisplayFlags(unsigned long);
void CALLBACK GPUcursor(int, int, int);
void CALLBACK GPUvisualVibration(unsigned long, unsigned long);

#endif
