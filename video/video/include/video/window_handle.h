/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
--------------------------------------------------------------------------------
Description : Window management native handles (Windows / Mac OS / Linux / BSD)
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#ifdef _WINDOWS
# include <system/api/windows_api.h>
#endif

namespace pandora {
  namespace video {
#   ifdef _WINDOWS
      using InstanceHandle = HINSTANCE; ///< App instance management handle
      using WindowHandle   = HWND;      ///< Window management handle
      using ResourceHandle = HANDLE;    ///< Resource management handle
#   else
      using InstanceHandle = int32_t; ///< App instance management handle
      using WindowHandle   = int32_t; ///< Window management handle
      using ResourceHandle = int32_t; ///< Resource management handle
#   endif
  }
}
