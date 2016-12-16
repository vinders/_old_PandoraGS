/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   unit_tests.h
Description : plugin unit testing
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <string>
using namespace std;
#include "unit_tests.h"
#include "video_memory.h"
#include "gpu_main.h"

// result messages
#define printSuccess() printf("SUCCESS\n");
#define printError(error) printf("FAILED : %s\n", error);


/// <summary>Plugin - full unit testing</summary>
/// <param name="pWinData">Window handle reference</param>
/// <returns>Success indicator</returns>
long CALLBACK GPUtestUnits(void* pWinData)
{
    #ifdef _WINDOWS
    HWND* phWindow = NULL;
    if (pWinData != NULL)
        phWindow = static_cast<HWND*>(pWinData);
    #endif

    // init plugin
    printf("GPUinit(): ");
    if (GPUinit() != PSE_SUCCESS)
    {
        printError("init failure");
        return false;
    }
    else
        printSuccess();
    // open plugin
    #ifdef _WINDOWS
    printf("GPUopen(hWindow): ");
    if (GPUopen(*phWindow) != PSE_SUCCESS)
    #else
    printf("GPUopen(NULL,NULL,NULL): ");
    if (GPUopen(NULL,NULL,NULL) != PSE_SUCCESS)
    #endif
    {
        printError("open failure");
        GPUshutdown();
        return false;
    }
    else
        printSuccess();

    // unit testing
    bool isSuccess = testUnit(Unit_geometry) 
                  && testUnit(Unit_logger)
                  && testUnit(Unit_system_tools) 
                  && testUnit(Unit_timer) 
                  && testUnit(Unit_input_reader) 
                  && testUnit(Unit_status_register) 
                  && testUnit(Unit_video_memory) 
                  && testUnit(Unit_drawing_prim)
                  && testUnit(Unit_lang)
                  && testUnit(Unit_config_io) 
                  && testUnit(Unit_config_profile) 
                  && testUnit(Unit_config)
                  && testUnit(Unit_line_prim) 
                  && testUnit(Unit_sprite_prim) 
                  && testUnit(Unit_poly_prim)
                  && testUnit(Unit_shader)
                  && testUnit(Unit_render_api)
                  && testUnit(Unit_display_state)
                  && testUnit(Unit_memory_dispatcher)
                  && testUnit(Unit_gpu_main);

    // close plugin
    printf("GPUclose(): ");
    if (GPUclose() != PSE_SUCCESS)
        printError("close failure");
    else
        printSuccess();
    // shutdown plugin
    printf("GPUshutdown(): ");
    if (GPUshutdown() != PSE_SUCCESS)
        printError("close failure");
    else
        printSuccess();
    return isSuccess;
}

/// <summary>Test one unit</summary>
/// <param name="unit">Unit to test</param>
/// <returns>Success indicator</returns>
bool testUnit(unit_id_t unit)
{
    bool isSuccess = true;
    switch (unit)
    {
        case Unit_geometry: break;
        case Unit_logger: break;
        case Unit_system_tools: break;
        case Unit_timer: break;
        case Unit_input_reader: break;
        case Unit_status_register: break;
        case Unit_video_memory:
        {
            printf("\nVIDEO_MEMORY UNIT\n---\n");
            VideoMemory* pData = new VideoMemory();
            try
            {
                printf("* VideoMemory(): ");
                VideoMemory* pData = new VideoMemory();
                if (pData == NULL)
                    throw std::exception("Null result (no instance)");
                printSuccess();

                printf("* init(): ");
                pData->init();
                printSuccess();

                printf("* init(true): ");
                pData->init(true);
                printSuccess();

                printf("* init(false): ");
                pData->init(false);
                printSuccess();

                //...

                printf("* close(): ");
                pData->close();
                printSuccess();

                printf("* \x7EVideoMemory(): ");
                delete pData;
                printSuccess();
            }
            catch (std::exception exc)
            {
                printError(exc.what());
                isSuccess = false;
                if (pData != NULL)
                    delete pData;
            }
            break;
        }
        case Unit_drawing_prim: break;
        case Unit_lang: break;
        case Unit_config_io: break;
        case Unit_config_profile: break;
        case Unit_config: break;
        case Unit_line_prim: break;
        case Unit_sprite_prim: break;
        case Unit_poly_prim: break;
        case Unit_shader: break;
        case Unit_render_api: break;
        case Unit_display_state: break;
        case Unit_memory_dispatcher: break;
        case Unit_gpu_main: break;
    }
    return isSuccess;
}
