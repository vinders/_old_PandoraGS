/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
File name :   opengl.h
Description : opengl specific includes
*******************************************************************************/
#ifndef _OPENGL_H_
#define _OPENGL_H_

#define GLEW_STATIC //#define GLEW_BUILD // ???
#include <GL/glew.h> // 1.9.0 ([%VS%]/VC/lib & [%VS%]/VC/include)
#include <GL/wglew.h>
// dependencies : opengl32.lib;glu32.lib;glew32s.lib;
#ifndef _WINDOWS
#include <GLFW/glfw3.h> // glfw3.lib
#endif

#endif
