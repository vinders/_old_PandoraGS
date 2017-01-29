/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : GPU library entry point
*******************************************************************************/
#ifdef _WINDOWS
#include "globals.h"
#include <cstdlib>
#include <Windows.h>
#include <tchar.h>
#include "pandoraGS.h"
using namespace std;

HINSTANCE PandoraGS::s_hInstance = NULL; ///< Executed instance handle


/// @brief Main library entry point
/// @return Success indicator
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
    PandoraGS::setInstance(&hModule);
    return TRUE;
}

/// @brief Initialize instance handle
/// @param hModule Module handle
void PandoraGS::setInstance(HANDLE* hModule)
{
    s_hInstance = (HINSTANCE)*hModule;
}
#endif
