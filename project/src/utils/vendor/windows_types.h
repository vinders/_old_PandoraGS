/*******************************************************************************
Description : windows type definitions specific includes
*******************************************************************************/
#pragma once

#ifdef _WINDOWS
    // disable unwanted features
#   ifndef NOMINMAX // no min/max macros
#       define NOMINMAX
#   endif
#   ifndef WIN32_LEAN_AND_MEAN // exclude rare libraries
#       define WIN32_LEAN_AND_MEAN
#   endif

    // windows type definitions
#   include <windef.h>

#   undef min
#   undef max
#endif
