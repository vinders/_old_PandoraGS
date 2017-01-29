/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : rendering shader builder
*******************************************************************************/
#include "../globals.h"
#include "shader.h"
#include "effects/vertex_shader_definition.h"
#include "effects/fragment_shader_definition.h"
using namespace display;


/// @brief Create and load shader
/// @param programId Rendering pipeline program
/// @param type Type of shader
Shader::Shader(GLuint& programId, shadertype_t type)
{
    switch (type)
    {
        case shadertype_t::vertex: setVertexShader(); break;
        case shadertype_t::fragment: setFragmentShader(); break;
    }

    m_programId = programId;
    glAttachShader(programId, m_shader);
}

/// @brief Disable and destroy shader
Shader::~Shader()
{
    glDetachShader(m_programId, m_shader);
    glDeleteShader(m_shader);
}

/// @brief Create custom vertex shader (based on current config)
void Shader::setVertexShader()
{
    // create shader definition (based on current config)
    display::effects::VertexShaderDefinition shaderDefBuilder;

    // load string definition into shader
    const char* shaderDef = shaderDefBuilder.getDefinition();
    m_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_shader, 1, &shaderDef, NULL);
    glCompileShader(m_shader);
}

/// @brief Create custom fragment shader (based on current config)
void Shader::setFragmentShader()
{
    // create shader definition (based on current config)
    display::effects::FragmentShaderDefinition shaderDefBuilder;

    // load string definition into shader
    const char* shaderDef = shaderDefBuilder.getDefinition();
    m_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_shader, 1, &shaderDef, NULL);
    glCompileShader(m_shader);
}
