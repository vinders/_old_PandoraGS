/*******************************************************************************
PANDORAGS project - PS1 GPU driver test app
------------------------------------------------------------------------
Description : PandoraGS test utility -- entry point and test functions
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <Windows.h>
#include <tchar.h>
#include <string>
using namespace std;
#include "../src/psemu_main.h" // plugin PSEmu interface
#include "window_manager.h"
#include "plugin_loader.h"
#include "tool_main.h"


/// @brief Test utility - window creation
/// @returns Exit code
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    return createMenuWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

/// @brief Plugin dialog testing
/// @param type  Dialog type
void openDialog(plugin_dialog_t type)
{
    if (type == plugin_dialog_t::config)
        GPUconfigure();
    else
        GPUabout();
}

/// @brief Launch plugin and start demonstration sequence
/// @param hWindow  Main window handle
void startDemoSequence(HWND hWindow)
{
    try
    {
        PluginLoader loader(hWindow);
        loader.playDemoSequence();
    }
    catch (const std::exception& exc)
    {
        printf("%s", exc.what());
    }
}

/// @brief Load plugin unit testing system
/// @param hWindow  Main window handle
void startUnitTesting(HWND hWindow)
{
    try
    {
        PluginLoader loader(hWindow);
        if (loader.checkUnits())
            printf("\nSUCCESS: all the tests were successful.\n");
        else
            printf("\nWARNING: some tests have failed...\n");

        fflush(stdout);
        system("pause");
    }
    catch (const std::exception& exc)
    {
        printf("%s", exc.what());
    }
}

/// @brief Custom primitive testing
/// @param hWindow  Main window handle
void startPrimitiveTesting(HWND hWindow)
{
    try
    {
        PluginLoader loader(hWindow);
        /*listPrimitives();
        do
        {
            // choose primitive type
            primitiveId = 0;
            char inputBuffer = 0;
            bool isFlipped = false;
            printf("Primitive: ");
            fflush(stdin);
            while ((inputBuffer = getchar()) != EOF && inputBuffer != '\n')
            {
                if (inputBuffer >= '0' && inputBuffer <= '9')
                    primitiveId = (primitiveId << 4) + (inputBuffer - '0');
                else if (inputBuffer >= 'A' && inputBuffer <= 'F')
                    primitiveId = (primitiveId << 4) + ((inputBuffer - 'A') + 10);
                else if (inputBuffer == '-')
                    isFlipped = true;
            }

            // display primitive
            if (primitiveId != 0)
            {
                if (renderPrimitive(primitiveId, isFlipped))
                    listPrimitives();
                else
                    printf("Unknown primitive type: %d\n", primitiveId);
            }
        }
        while (primitiveId != 0);*/
    }
    catch (const std::exception& exc)
    {
        printf("%s", exc.what());
    }
}
