/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   renderer.h
    Description : graphic API rendering engine (abstract factory)
*******************************************************************************/
#ifndef _RENDERER_H
#define _RENDERER_H
#include "../config/config_manager.h"
#include "../input/keyinput_manager.h"
#include "../lang/lang.h"
#include "console_display.h"

/* graphic API codes */
#define GLAPICODE_SOFTD3D 1               // do not change
#define GLAPICODE_OGL4    4
//#define GLAPICODE_DX11    11
#define GLAPI_DEFAULTCODE GLAPICODE_SOFTD3D // base config

/* API versions per label */
#define GLAPI_MINOR41   1                 // do not change
//#define GLAPI_MINOR45  5
//#define GLAPI_MINOR11  0
#define GLAPI_DEFAULTMINOR 0 // base config

/* API combobox labels + index */
#define GLAPI_LABEL1  "Software (DX11 warp)"
#define GLAPI_LABEL41 "OpenGL 4.1"
//#define GLAPI_LABEL45 "OpenGL 4.5"
//#define GLAPI_LABEL11 "DirectX 11.0"
#if _SYSTEM == _WINDOWS
    #define GLAPI_INDEX1  0
    #define GLAPI_INDEX41 1
    //#define GLAPI_INDEX11 2
    //#define GLAPI_INDEX45 3
  #else
    #define GLAPI_INDEX41 1
    //#define GLAPI_INDEX45 2
#endif

/* Vram data structure */
typedef struct
{
    WRectSize_t area;       // limits
    short remainingRows;    // remaining lines
    short remainingCols;    // remaining columns
    unsigned short *vramCursor; // memory pointer
} ConsoleVramIO_t;


/* GRAPHIC RENDERER (common abstract) --------------------------------------- */
class Renderer
{
  protected:
    #if _SYSTEM == _WINDOWS
        HMENU _hEmulatorMenu;    // emulator menu backup
        DWORD _dwOriginalStyle;  // window style backup to restore window
    #endif
    // internal variables
    bool      _bApiState;           // graphics api started or not
    LangMenu* _langContent;         // menu translated labels
    unsigned char*  _consoleVram;   // allocated space  
    
    
    // common constructor/destructor (no instanciation allowed)
    Renderer();              // init static values
    void destroyRenderer();  // remove allocated values
    // misc graphic methods
    void setScreenSaver(bool);
    long displayBaseBackground();
    #if _SYSTEM == _WINDOWS
        virtual long setPixelFormat(HDC) = 0;
      #else
        virtual long setPixelFormat() = 0;
    #endif
    
    
  public:
    static Renderer* getInstance(unsigned int); // factory pattern
    bool isApiSet()
    {
        return _bApiState;
    }
    // graphic pipeline methods
    virtual long init() = 0;            // init allocated values
            long initCommon();
    virtual long setPipeline() = 0;     // prepare graphic pipeline
            void setPipelineCommon(bool);
    virtual void unsetPipeline() = 0;   // remove graphic pipeline and API
            void unsetPipelineCommon();
    virtual long setGraphicApi()   = 0; // start graphics API
    virtual void unsetGraphicApi() = 0;
    virtual void render() = 0;          // render display frame
    void setRenderFixes();
    // status change methods
    void resetRenderer();               // set renderer default values
    void setTransferMode(unsigned long);// set data IO mode
    void setDisplayState(unsigned long);
    void setDisplayInfo(unsigned long);
    void setDisplayPosition(unsigned long);
    void processRequest(unsigned long);
    // data IO methods
    void readDataInput(unsigned long*, int);
    void writeDataOutput(unsigned long*, int);
    bool writeNormalData(unsigned long*, int, unsigned long*, int*);
    
    // public display variables
    ConsoleDisplay consoleDisplay; // current/previous display settings
    short    playersCursorsActive; // show/hide gun cursor(s)
    WPoint_t playersCursors[8];    
    // public special items
    bool     bVsyncReady;
    bool     bWriteUpload;
    long     busySequence;
    long     saveSlot;
    // playstation VRAM image (different type modes)
    unsigned char*  consoleVramByte;    // 8 bits
    unsigned short* consoleVramWord;    // 16 bits
    unsigned long*  consoleVramDword;   // 32 bits
    unsigned short* consoleVramEOM;     // end of memory
    unsigned long   vramSize;
    // public VRAM access properties
    ConsoleVramIO_t consoleVramOutput; // write
    ConsoleVramIO_t consoleVramInput; // read
    int outputMode;
    int inputMode;
    // low-level plugin variables
    unsigned long gpuRawData;    // data shared with emulator
    unsigned long gpuRawStatus;  // gpu status information
    unsigned long gpuRawInfo[PSRAWINFO_SIZE];          // gpu information data
    unsigned long statusControl[PSSTATUSCONTROL_SIZE]; // save-states control field
};

#endif
