/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   shader.cpp
Description : rendering shader manager
*******************************************************************************/
using namespace std;
#include "config.h"
#include "shader.h"

/// <summary>Create and load shader</summary>
/// <param name="prog">Rendering pipeline program</summary>
/// <param name="type">Type of shader</summary>
Shader::Shader(GLuint& prog, shadertype_t type)
{
    if (type == Shadertype_vertex)
    { 
        m_shader = glCreateShader(GL_VERTEX_SHADER);
        setVertexShader();
    }
    else // Shadertype_fragment
    {
        m_shader = glCreateShader(GL_FRAGMENT_SHADER);
        setFragmentShader();
    }
    m_prog = prog;
    glAttachShader(prog, m_shader);
}

/// <summary>Disable and destroy shader</summary>
Shader::~Shader()
{
    glDetachShader(m_prog, m_shader);
    glDeleteShader(m_shader);
}

/// <summary>Create custom vertex shader, based on config</summary>
void Shader::setVertexShader()
{
    // create shader definition
    std::string shaderStr;

    //...
    shaderStr = "";
    //...

    // load string definition into shader
    const char* shaderDef = shaderStr.c_str();
    glShaderSource(m_shader, 1, &shaderDef, NULL);
    glCompileShader(m_shader);
}

/// <summary>Create custom fragment shader, based on config</summary>
void Shader::setFragmentShader()
{
    // create shader definition
    std::string shaderStr;

    //...
    shaderStr = "";
    //...

    // load string definition into shader
    const char* shaderDef = shaderStr.c_str();
    glShaderSource(m_shader, 1, &shaderDef, NULL);
    glCompileShader(m_shader);
}
