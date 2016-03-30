/*******************************************************************************
PANDORAGS project - PS1 GPU renderer
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   pandoraGS.cpp
Description : main dll library instance
*******************************************************************************/
using namespace std;
#include "globals.h"
#include "pandoraGS.h"
#include "gpu_main.h"
#include "gpu_memory.h"

HINSTANCE PandoraGS::hInstance = NULL;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	PandoraGS::setInstance(&hModule);
	return TRUE;
}

HINSTANCE PandoraGS::getInstance()
{
    return hInstance;
}

void PandoraGS::setInstance(HANDLE* hModule)
{
    hInstance = (HINSTANCE)*hModule;
}
