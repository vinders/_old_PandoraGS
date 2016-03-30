/*******************************************************************************
PANDORAGS project - PS1 GPU renderer
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   pandoraGS.h
Description : main dll library instance
*******************************************************************************/
#ifndef _PANDORAGS_H
#define _PANDORAGS_H

/*#ifdef PANDORAGS_EXPORTS
#define PANDORAGS_API __declspec(dllexport)
#else
#define PANDORAGS_API __declspec(dllimport)
#endif*/

#if _SYSTEM == _WINDOWS

    #define INSTANCE_SINGLETON
    #include "resource.h" // main symbols

    /* main plugin class ---------------------------------------------------- */
    class PandoraGS
    {
      private:
        static HINSTANCE hInstance;
        
      public:
        static HINSTANCE getInstance();  //singleton pattern
        static void setInstance(HANDLE*);
    };

#endif
#endif
