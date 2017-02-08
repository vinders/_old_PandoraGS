/*******************************************************************************
PANDORAGS project - PS1 GPU driver test app
------------------------------------------------------------------------
Description : PandoraGS test utility -- entry point and test functions
*******************************************************************************/
#pragma once

#include <Windows.h>
#include "res/resource.h"

/// @enum plugin_dialog_t
/// @brief Plugin dialog types
enum class plugin_dialog_t : uint32_t
{
    config = 0,
    about = 1
};


/// @brief Plugin dialog testing
/// @param type  Dialog type
void openDialog(plugin_dialog_t type);

/// @brief Launch plugin and start demonstration sequence
/// @param hWindow  Main window handle
void startDemoSequence(HWND hWindow);

/// @brief Load plugin unit testing system
/// @param hWindow  Main window handle
void startUnitTesting(HWND hWindow);

/// @brief Custom primitive testing
/// @param hWindow  Main window handle
void startPrimitiveTesting(HWND hWindow);
