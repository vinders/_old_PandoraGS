/*******************************************************************************
Description : opengl specific includes
*******************************************************************************/
// dependencies : opengl32.lib;glu32.lib; 
#pragma once

#ifdef _WINDOWS
#define GLEW_STATIC // static linking    (warning : constant also defined in glew.h)
#define GLEW_BUILD  // build DLL library (warning : constant also defined in glew.h)
#include "opengl/glew.h"   // 1.9.0
#include "opengl/wglew.h"
#else
#include "opengl/glew.h"   // 1.9.0
#include <GLFW/glfw3.h> // glfw3.lib
#endif
