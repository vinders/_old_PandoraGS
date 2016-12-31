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
#include "geometry.hpp"
#include "logger.h"
#include "system_tools.h"
#include "timer.h"
#include "input_reader.h"
#include "status_register.h"
#include "video_memory.h"
#include "lang.h"
#include "config_io.h"
#include "config_profile.h"
#include "config.h"
#include "primitive_builder.h"
#include "shader.h"
#include "render_api.h"
#include "display_state.h"
#include "dispatcher.h"
#include "gpu_main.h"

/// <summary>Success result message</summary>
inline void printSuccess()
{
    printf("SUCCESS\n");
}
/// <summary>Error result message</summary>
/// <param name="error">Error message</param>
inline void printError(const char* error)
{
    printf("\n\t  FAILED : %s\n", error);
}


/// <summary>Plugin - primitive testing</summary>
/// <param name="pData">Primitive raw data</param>
/// <param name="len">Primitive data length (number of 32bits blocks)</param>
/// <param name="isFlipped">Flip indicator (only for rectangles)</param>
void CALLBACK GPUtestPrimitive(unsigned char* pData, int len, bool isFlipped)
{
    //... reset draw area/offsets/size/...

    if (isFlipped)
        ;//... flip XY
    PrimitiveBuilder::processSinglePrimitive(pData, len);

    if (pData[0] < 0x80)
        ;//... display
    else
        ;//... fill draw zone with white blank rectangle
}

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

    // unit testing
    bool isSuccess = testUnit(Unit_geometry)
        && testUnit(Unit_logger)
        && testUnit(Unit_system_tools)
        && testUnit(Unit_timer)
        && testUnit(Unit_input_reader, pWinData)
        && testUnit(Unit_status_register)
        && testUnit(Unit_video_memory)
        && testUnit(Unit_config_profile)
        && testUnit(Unit_config)
        && testUnit(Unit_config_io)
        && testUnit(Unit_lang)
        && testUnit(Unit_display_state)
        && testUnit(Unit_shader)
        && testUnit(Unit_render_api)
        && testUnit(Unit_dispatcher);
    return isSuccess;
}

/// <summary>Test one unit</summary>
/// <param name="unit">Unit to test</param>
/// <param name="pWinData">Window handle reference (optional)</param>
/// <returns>Success indicator</returns>
bool testUnit(unit_id_t unit, void* pWinData)
{
    bool isSuccess = true;
    switch (unit)
    {
        case Unit_geometry: 
        {
            printf("\nGEOMETRY UNIT\n---\n");
            try
            {
                printf("\t* nothing to test...\n");
            }
            catch (const std::exception& exc)
            {
                printError(exc.what());
                isSuccess = false;
            }
            break;
        }
        case Unit_logger:
        {
            printf("\nLOGGER UNIT\n---\n");
            Logger* pData = NULL;
            try
            {
                printf("\t* getInstance(): ");
                pData = Logger::getInstance();
                if (pData == NULL)
                    throw std::exception("Null result (no instance)");
                printSuccess();

                printf("\t* writeEntry(\"orig\",\"type\",\"message\"): ");
                pData->writeEntry("orig","type","message");
                printSuccess();

                printf("\t* writeErrorEntry(\"orig\",\"message\"): ");
                pData->writeErrorEntry("orig","message");
                printSuccess();

                printf("\t* removeLogFile(): ");
                pData->removeLogFile();
                printSuccess();

                printf("\t* closeInstance(): ");
                Logger::closeInstance();
                printSuccess();
            }
            catch (const std::exception& exc)
            {
                printError(exc.what());
                isSuccess = false;
                if (pData != NULL)
                    delete pData;
            }
            break;
        }
        case Unit_system_tools:
        {
            printf("\nSYSTEM_TOOLS UNIT\n---\n");
            try
            {
                printf("\t* setScreensaver(false): ");
                SystemTools::setScreensaver(false);
                printSuccess();

                printf("\t* setScreensaver(true): ");
                SystemTools::setScreensaver(true);
                printSuccess();

                printf("\t* getWritableFilePath(): ");
                SystemTools::getWritableFilePath();
                printSuccess();
            }
            catch (const std::exception& exc)
            {
                printError(exc.what());
                isSuccess = false;
            }
            break;
        }
        case Unit_timer:
        {
            printf("\nTIMER UNIT\n---\n");
            try
            {
                printf("\t* setTimeMode(Timemode_multimediaClock): ");
                Timer::setTimeMode(Timemode_multimediaClock);
                printSuccess();

                printf("\t* setTimeMode(Timemode_highResCounter): ");
                Timer::setTimeMode(Timemode_highResCounter);
                printSuccess();

                printf("\t* setFrequency(60.0f, Regionmode_undefined, false): ");
                Timer::setFrequency(60.0f, Regionsync_undefined, false);
                printSuccess();

                printf("\t* setFrequency(0.0f, Regionmode_ntsc_pcfix, true): ");
                Timer::setFrequency(0.0f, Regionsync_ntsc_pcfix, true);
                printSuccess();

                printf("\t* setFrequency(0.0f, Regionmode_pal_pcfix, false): ");
                Timer::setFrequency(0.0f, Regionsync_pal_pcfix, false);
                printSuccess();

                printf("\t* setFrequency(0.0f, Regionmode_ntsc, false): ");
                Timer::setFrequency(0.0f, Regionsync_ntsc, false);
                printSuccess();

                printf("\t* setFrequency(0.0f, Regionmode_pal, true): ");
                Timer::setFrequency(0.0f, Regionsync_pal, true);
                printSuccess();

                printf("\t* resetTimeReference(): ");
                Timer::resetTimeReference();
                printSuccess();

                printf("\t* wait(Speed_normal): ");
                Timer::wait(Speed_normal, true);
                Timer::wait(Speed_normal, false);
                Timer::wait(Speed_normal, true);
                Timer::wait(Speed_normal, false);
                printSuccess();

                printf("\t* wait(Speed_slow): ");
                Timer::wait(Speed_slow, true);
                Timer::wait(Speed_slow, false);
                printSuccess();

                printf("\t* wait(Speed_fast): ");
                Timer::wait(Speed_fast, true);
                Timer::wait(Speed_fast, false);
                printSuccess();

                printf("\t* calcFrequency(): ");
                Timer::calcFrequency();
                printSuccess();
            }
            catch (const std::exception& exc)
            {
                printError(exc.what());
                isSuccess = false;
            }
            break;
        }
        case Unit_input_reader:
        {
            printf("\nINPUT_READER UNIT\n---\n");
            try
            {
                printf("\t* start(handle, ctrlKeys, 0, false, false): ");
                char ctrlKeys[CTRLKEYS_LENGTH];
                memset(ctrlKeys, 0x0, CTRLKEYS_LENGTH);
                #ifdef _WINDOWS
                HWND* phWindow = NULL;
                if (pWinData != NULL)
                    phWindow = static_cast<HWND*>(pWinData);
                InputReader::start(*phWindow, ctrlKeys, 0, false, false);
                #else
                InputReader::start(ctrlKeys, 0, false);
                #endif
                printSuccess();

                printf("\t* stop(): ");
                InputReader::stop();
                printSuccess();
            }
            catch (const std::exception& exc)
            {
                printError(exc.what());
                isSuccess = false;
            }
            break;
        }
        case Unit_status_register:
        {
            printf("\nSTATUS_REGISTER UNIT\n---\n");
            try
            {
                printf("\t* init(): ");
                StatusRegister::init();
                printSuccess();

                printf("\t* setStatus(): ");
                StatusRegister::setStatus(0x4);
                if (StatusRegister::getStatus(0x4))
                {
                    printSuccess();

                    printf("\t* unsetStatus(): ");
                    StatusRegister::unsetStatus(0x4);
                    if (StatusRegister::getStatus(0x4) == false)
                        printSuccess();
                    else
                        printError("status not unset");
                }
                else
                    printError("status not set");

                printf("\t* initFakeBusySequence(): ");
                StatusRegister::initFakeBusySequence();
                printSuccess();

                printf("\t* setFakeBusyStep(): ");
                StatusRegister::setFakeBusyStep();
                printSuccess();
            }
            catch (const std::exception& exc)
            {
                printError(exc.what());
                isSuccess = false;
            }
            break;
        }
        case Unit_video_memory:
        {
            printf("\nVIDEO_MEMORY UNIT\n---\n");
            VideoMemory* pData = NULL;
            try
            {
                printf("\t* VideoMemory(): ");
                pData = new VideoMemory();
                if (pData == NULL)
                    throw std::exception("Null result (no instance)");
                printSuccess();

                printf("\t* init(): ");
                pData->init();
                printSuccess();

                printf("\t* init(true): ");
                pData->init(true);
                printSuccess();

                printf("\t* init(false): ");
                pData->init(false);
                printSuccess();

                printf("\t* resetDmaCheck(): ");
                pData->resetDmaCheck();
                printSuccess();

                printf("\t* checkDmaEndlessChain(): ");
                pData->checkDmaEndlessChain(0x0FFFF);
                pData->resetDmaCheck();
                printSuccess();

                printf("\t* VideoMemory::iterator: ");
                VideoMemory::iterator it = pData->begin();
                it.getValue();
                while (it.getPos() < pData->end())
                    ++it;
                while (it.getPos() > pData->rend())
                    --it;
                printSuccess();

                printf("\t* close(): ");
                pData->close();
                printSuccess();

                printf("\t* \x7EVideoMemory(): ");
                delete pData;
                printSuccess();
            }
            catch (const std::exception& exc)
            {
                printError(exc.what());
                isSuccess = false;
                if (pData != NULL)
                    delete pData;
            }
            break;
        }
        case Unit_config_profile:
        {
            break;
        }
        case Unit_config:
        {
            break;
        }
        case Unit_config_io:
        {
            break;
        }
        case Unit_lang:
        {
            printf("\nLANG UNIT\n---\n");
            LanguageDialogResource* pData = NULL;
            try
            {
                printf("\t* LanguageGameMenuResource::setLanguage(internal): ");
                LanguageGameMenuResource::setLanguage(Langcode_english);
                printSuccess();

                printf("\t* LanguageGameMenuResource::setLanguage(external): ");
                LanguageGameMenuResource::setLanguage(Langcode_customFile);
                printSuccess();

                printf("\t* LanguageGameMenuResource::setLanguage(external, L\"valid\"): ");
                LanguageGameMenuResource::setLanguage(Langcode_customFile, L"pandoraGS.lang");
                printSuccess();

                printf("\t* LanguageDialogResource(): ");
                pData = new LanguageDialogResource();
                if (pData == NULL)
                    throw std::exception("Null result (no instance)");
                printSuccess();

                printf("\t* setLanguage(Langcode_english): ");
                pData->setLanguage(Langcode_english);
                printSuccess();

                printf("\t* setLanguage(Langcode_customFile, L\"invalid\"): ");
                try
                {
                    pData->setLanguage(Langcode_customFile, L"nonexistingfile");
                    printError("Non existing file: should throw exception");
                    isSuccess = false;
                }
                catch (...) 
                { 
                    printSuccess(); 
                    Logger::getInstance()->removeLogFile();
                    Logger::getInstance()->closeInstance();
                }
                delete pData;
            }
            catch (const std::exception& exc)
            {
                printError(exc.what());
                isSuccess = false;
                if (pData != NULL)
                    delete pData;
            }
            break;
        }
        case Unit_shader:
        {
            break;
        }
        case Unit_render_api:
        {
            break;
        }
        case Unit_display_state:
        {
            break;
        }
        case Unit_dispatcher:
        {
            break;
        }
    }
    return isSuccess;
}
