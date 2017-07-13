/*******************************************************************************
Description : OpenGL 4.1 specific includes - static link
*******************************************************************************/
// dependencies : opengl32.lib;glu32.lib; 
#pragma once

#if !defined(_OPENGL_REVISION_NUMBER) or _OPENGL_REVISION_NUMBER < 41
#   define _OPENGL_REVISION_NUMBER 41

#   ifdef _WINDOWS
#       define GLEW_STATIC // static linking    (warning : constant also to define in glew.h)
#       define GLEW_BUILD  // build DLL library (warning : constant also to define in glew.h)
#       include "../../vendor/opengl_41/glew.h" // add glew.cpp to the project
#       include "../../vendor/opengl_41/wglew.h"
#   else
#       include "../../vendor/opengl_41/glew.h"  // add glew.cpp to the project
#       include "../../vendor/opengl_41/glfw3.h" // add glfw3.cpp to the project
#   endif
#endif
