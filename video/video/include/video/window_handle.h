/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
--------------------------------------------------------------------------------
Description : Window management native handles (Windows / Mac OS / Linux / BSD)
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>

namespace pandora {
  namespace video {
#   ifdef _WINDOWS
      using InstanceHandle = void*;
      using WindowHandle   = void*;
      using ResourceHandle = void*;
#   else
      using InstanceHandle = int32_t; ///< App instance management handle
      using WindowHandle   = int32_t; ///< Window management handle
      using ResourceHandle = int32_t; ///< Resource management handle
#   endif
  }
}
