/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   shader.h
Description : rendering API shader
*******************************************************************************/
#ifndef _SHADER_H
#define _SHADER_H
#include "globals.h"
#include "config.h"

// different types of shaders
enum ShaderType
{
    ShaderType_Vertex = 0,
    ShaderType_Geometry = 1,
    ShaderType_Fragment = 2,
    ShaderType_Compute = 3
};


// Rendering API shader
class Shader
{
protected:
    unsigned int renderApi;
    ShaderType   type;
    //...

public:
    /// <summary>shader construction</summary>
    /// <param name="type">Shader type</param>
    Shader(ShaderType type);
    /// <summary>shader destruction</summary>
    ~Shader();

    //...
    //prototypes communs, Warp, OpenGl, DirectX
    //-> switch dans chaque fonction (+ rapide que virtual)
    //...
};

#endif
