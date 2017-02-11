/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : advanced tab control
*******************************************************************************/
#include "../../../globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#endif
#include "../../../res/resource.h"
#include "dialog.h"
#include "tab_button.h"
#include "tab_page.h"
#include "tab_control.h"
using namespace config::dialog::controls;


/// @brief Trigger control drawing
/// @param[in] args  Event arguments
void TabControl::paint(paint_event_args_t args)
#if _DIALOGAPI == DIALOGAPI_WIN32
{

}
#else
{

}
#endif
