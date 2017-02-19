/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : data table control
*******************************************************************************/
#include "../../../globals.h"
#include <cstdint>
#include <string>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#endif
#include "../../../res/resource.h"
#include "common.h"
#include "data_table.h"
using namespace config::dialog;
using namespace config::dialog::controls;

