// UiLauncher.cpp : Defines the entry point for the console application.
//

#include<Windows.h>
#include<tchar.h>
#include "service_main.h"


int _tmain(int argc, _TCHAR* argv[])
{
    GPUconfigure();
    GPUabout();
	return 0;
}

