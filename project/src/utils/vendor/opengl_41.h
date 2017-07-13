/*******************************************************************************
Description : OpenGL 4.1 specific includes - library link
*******************************************************************************/
// dependencies : opengl32.lib;glu32.lib;glew32.lib
#pragma once

#if !defined(_OPENGL_REVISION_NUMBER) or _OPENGL_REVISION_NUMBER < 41
#   define _OPENGL_REVISION_NUMBER 41

#   ifdef _WINDOWS
#       include <GL/glew.h>
#       include <GL/wglew.h>
#   else
#       include <GL/glew.h>
#       include <GLFW/glfw3.h>
#   endif
#endif
