/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   pandoraGS.h
Description : main DLL entry point (Windows)
*******************************************************************************/
#ifdef _WINDOWS
#ifndef _PANDORAGS_H
#define _PANDORAGS_H

#include "globals.h"


// PandoraGS GPU Driver - entry point
class PandoraGS
{
private:
    static HINSTANCE s_hInstance; // executed instance handle

public:
    // -- GETTER / SETTER -- ---------------------------------------------------

    /// <summary>Initialize instance handle</summary>
    /// <param name="hModule">Handle module</param>
    static void setInstance(HANDLE* hModule);
    /// <summary>Get main instance handle</summary>
    /// <returns>Instance handle</returns>
    static HINSTANCE getInstance();
};

#endif
#endif
