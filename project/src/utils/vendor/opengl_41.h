/*******************************************************************************
Description : opengl specific includes
*******************************************************************************/
// dependencies : opengl32.lib;glu32.lib; 
#pragma once

#if !defined(_OPENGL_REVISION_NUMBER) or _OPENGL_REVISION_NUMBER < 41
#   define _OPENGL_REVISION_NUMBER 41
#endif

#ifdef _WINDOWS
#   define GLEW_STATIC // static linking    (warning : constant also defined in glew.h)
#   define GLEW_BUILD  // build DLL library (warning : constant also defined in glew.h)
#   include "opengl_41/glew.h"   // 1.9.0
#   include "opengl_41/wglew.h"
#else
#   include "opengl_41/glew.h"   // 1.9.0
#   include <GLFW/glfw3.h> // glfw3.lib
#endif
