/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   pandoraGS.cpp
Description : main DLL entry point (Windows)
*******************************************************************************/
#include <Windows.h>
using namespace std;
#include "pandoraGS.h"

HINSTANCE PandoraGS::s_hInstance = NULL; // executed instance handle


/// <summary>Main library entry point</summary>
/// <returns>Success indicator</returns>
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
    PandoraGS::setInstance(&hModule);
    return TRUE;
}


// -- GETTER / SETTER -- -------------------------------------------------------

/// <summary>Initialize instance handle</summary>
/// <param name="hModule">Handle module</param>
void PandoraGS::setInstance(HANDLE* hModule)
{
    s_hInstance = (HINSTANCE)*hModule;
}

/// <summary>Get main instance handle</summary>
/// <returns>Instance handle</returns>
HINSTANCE PandoraGS::getInstance()
{
    return s_hInstance;
}
