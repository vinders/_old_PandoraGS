/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   shader.h
Description : rendering shader manager
*******************************************************************************/
#ifndef _SHADER_H
#define _SHADER_H

#include "opengl.h" // openGL includes

// data types
enum shadertype_t : unsigned int // shader types
{
    Shadertype_vertex = 1u,
    Shadertype_fragment = 2u
};


// Rendering shader manager
class Shader
{
private:
    GLuint m_prog;   // rendering pipeline program identifier
    GLuint m_shader; // shader identifier


public:
    /// <summary>Create and load shader</summary>
    /// <param name="prog">Rendering pipeline program</summary>
    /// <param name="type">Type of shader</summary>
    Shader(GLuint& prog, shadertype_t type);
    /// <summary>Disable and destroy shader</summary>
    ~Shader();

    /// <summary>Create custom vertex shader, based on config</summary>
    void setVertexShader();
    /// <summary>Create custom fragment shader, based on config</summary>
    void setFragmentShader();
};

#endif
