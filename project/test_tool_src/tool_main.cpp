/*******************************************************************************
PANDORAGS project - PS1 GPU driver test app
------------------------------------------------------------------------
Description : PandoraGS test utility -- entry point and test functions
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
#include <tchar.h>
#include <string>
using namespace std;
#include "windowManager.h"
#include "primitive.h"
#include "demo.h"
#include "tool_main.h"

#include "../src/psemu_main.h" // plugin PSEmu interface
#include "../src/unit_tests.h" // plugin test interface


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                       _In_ LPTSTR    lpCmdLine, _In_ int       nCmdShow)
{
    return 0;
}
