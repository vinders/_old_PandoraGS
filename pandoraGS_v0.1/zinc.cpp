/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   zinc.cpp
    Description : zinc specification interface
*******************************************************************************/
using namespace std;
#include "globals.h"
#include "gpu_main.h"
#include "gpu_memory.h"
#if _SYSTEM != _WINDOWS
    void CALLBACK GPUkeypressed(int keycode);
#endif

/* Zinc GPU interface ------------------------------------------------------- */

bool          zincEmulation  = false;
unsigned long zincGPUVersion = 0;
int           zincTileFix    = 0;

/* configuration structure -------------------------------------------------- */

typedef struct GPUOTAG
{
    unsigned long  Version;        // Version of structure - currently 1
    long           hWnd;           // Window handle
    unsigned long  ScreenRotation; // 0 = 0CW, 1 = 90CW, 2 = 180CW, 3 = 270CW = 90CCW
    unsigned long  GPUVersion;     // 0 = a, 1 = b, 2 = c
    const char*    GameName;       // NULL terminated string
    const char*    CfgFile;        // NULL terminated string
} GPUConfiguration_t;

/* functions equivalents ---------------------------------------------------- */

long CALLBACK ZN_GPUinit() // we always set the vram size to 2MB, if the ZN interface is used
{
    zincEmulation = true;
    return GPUinit();
}

// --------------------------------------------------- //

long CALLBACK ZN_GPUopen(void * vcfg)
{
    GPUConfiguration_t * cfg = (GPUConfiguration_t *)vcfg;
    if(!cfg || cfg->Version != 1)
        return -1;
    
    zincTileFix = 1; // tile erase bug fix
    zincGPUVersion = cfg->GPUVersion;

    #if _SYSTEM == _WINDOWS
        return GPUopen((HWND)cfg->hWnd);
      #else
        return GPUopen(&cfg->hWnd,cfg->GameName,cfg->CfgFile);
    #endif
}

// --------------------------------------------------- //

long CALLBACK ZN_GPUclose()
{
    return GPUclose();
}

// --------------------------------------------------- // 

long CALLBACK ZN_GPUshutdown()
{
    return GPUshutdown();
}

// --------------------------------------------------- // 

void CALLBACK ZN_GPUupdateLace()
{
    GPUupdateLace();
}

// --------------------------------------------------- // 

void CALLBACK ZN_GPUdisplayFlags(unsigned long dwFlags)
{
    GPUdisplayFlags(dwFlags);
}

// --------------------------------------------------- //

void CALLBACK ZN_GPUmakeSnapshot()
{
    GPUmakeSnapshot();
}

// --------------------------------------------------- //

unsigned long CALLBACK ZN_GPUreadStatus()
{
    return GPUreadStatus();
}

// --------------------------------------------------- // 

void CALLBACK ZN_GPUwriteStatus(unsigned long gdata)
{
    GPUwriteStatus(gdata);
}

// --------------------------------------------------- // 

long CALLBACK ZN_GPUdmaSliceOut(unsigned long *baseAddrL, unsigned long addr, unsigned long iSize)
{
    GPUreadDataMem(baseAddrL+addr, iSize);
    return 0;
}

// --------------------------------------------------- // 

unsigned long CALLBACK ZN_GPUreadData()
{
    return GPUreadData();
}

// --------------------------------------------------- // 

void CALLBACK ZN_GPUsetMode(unsigned long gdata)
{
    GPUsetMode(gdata);
}

// --------------------------------------------------- // 

long CALLBACK ZN_GPUgetMode()
{
    return GPUgetMode();
}

// --------------------------------------------------- // 

long CALLBACK ZN_GPUdmaSliceIn(unsigned long *baseAddrL, unsigned long addr, unsigned long iSize)
{
    GPUwriteDataMem(baseAddrL+addr, iSize);
    return 0;
}
// --------------------------------------------------- // 

void CALLBACK ZN_GPUwriteData(unsigned long gdata)
{
    GPUwriteDataMem(&gdata,1);
}

// --------------------------------------------------- // 

long CALLBACK ZN_GPUdmaChain(unsigned long * baseAddrL, unsigned long addr)
{
    return GPUdmaChain(baseAddrL, addr);
}

// --------------------------------------------------- // 

long CALLBACK ZN_GPUtest()
{
    return GPUtest();
}

// --------------------------------------------------- // 

long CALLBACK ZN_GPUfreeze(unsigned long ulGetFreezeData, void * pF)
{
    GPUFreeze_t* pFreeze = (GPUFreeze_t*)pF;
    return GPUfreeze(ulGetFreezeData, pFreeze);
}

// --------------------------------------------------- // 

void CALLBACK ZN_GPUgetScreenPic(unsigned char * pMem)
{
    GPUgetScreenPic(pMem);
}

// --------------------------------------------------- // 

void CALLBACK ZN_GPUshowScreenPic(unsigned char * pMem)
{
    GPUshowScreenPic(pMem);
}

// --------------------------------------------------- // 
                              
#if _SYSTEM != _WINDOWS
    void CALLBACK ZN_GPUkeypressed(int keycode)
    {
        GPUkeypressed(keycode);
    }
#endif
