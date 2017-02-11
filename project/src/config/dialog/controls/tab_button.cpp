/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab button for tab control
*******************************************************************************/
#include "../../../globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#endif
#include "../../../res/resource.h"
#include "tab_button.h"
using namespace config::dialog::controls;


/// @brief Trigger control drawing
/// @param[in] args  Event arguments
void TabButton::paint(paint_control_event_args_t& args)
#if _DIALOGAPI == DIALOGAPI_WIN32
{

}
#else
{

}
#endif
