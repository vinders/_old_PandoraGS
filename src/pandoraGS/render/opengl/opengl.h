/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
File name :   opengl.h
Description : opengl specific includes
*******************************************************************************/
// dependencies : opengl32.lib;glu32.lib; 
#ifndef _OPENGL_H_
#define _OPENGL_H_

#ifdef _WINDOWS
#define GLEW_STATIC // static linking (constant also defined in glew.h)
#define GLEW_BUILD  // build DLL library (constant also defined in glew.h)
#include "glew.h"   // 1.9.0
#include "wglew.h"
#else
#include "glew.h"   // 1.9.0
#include <GLFW/glfw3.h> // glfw3.lib
#endif

#endif
