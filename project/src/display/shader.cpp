/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : rendering shader builder
*******************************************************************************/
#include "../globals.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <stdexcept>
using namespace std::literals::string_literals;
#include "../events/utils/logger.h"
#include "effects/vertex_shader_definition.h"
#include "effects/fragment_shader_definition.h"
#include "shader.h"
using namespace display;


/// @brief Load external shader
/// @param[in] programId  Rendering pipeline program
/// @param[in] type       Type of shader
/// @param[in] path       File path (if external shader) or empty value (built-in)
Shader::Shader(const GLuint& programId, const shadertype_t type, const std::string& path)
{
    switch (type)
    {
        case shadertype_t::vertex: setVertexShader(path); break;
        case shadertype_t::fragment: setFragmentShader(path); break;
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
/// @param[in] path  File path (if external shader) or empty (built-in)
void Shader::setVertexShader(const std::string& path)
{
    m_shader = glCreateShader(GL_VERTEX_SHADER);
    if (path.empty() || loadShaderFromFile(path) == false) // built-in or external (if valid file)
    {
        // create shader definition (based on current config)
        display::effects::VertexShaderDefinition shaderDefBuilder;

        // load string definition into shader
        const char* shaderDef = shaderDefBuilder.getDefinition();
        glShaderSource(m_shader, 1, &shaderDef, NULL);
        glCompileShader(m_shader);

        // check validity
        GLint status = 0;
        glGetShaderiv(m_shader, GL_COMPILE_STATUS, &status);
        if (!status)
            events::utils::Logger::getInstance()->writeErrorEntry("Shader.setVertexShader"s, "Built-in shader compilation failure"s);
    }
}

/// @brief Create custom fragment shader (based on current config)
/// @param[in] path  File path (if external shader) or empty (built-in)
void Shader::setFragmentShader(const std::string& path)
{
    m_shader = glCreateShader(GL_FRAGMENT_SHADER);
    if (path.empty() || loadShaderFromFile(path) == false) // built-in or external (if valid file)
    {
        // create shader definition (based on current config)
        display::effects::FragmentShaderDefinition shaderDefBuilder;

        // load string definition into shader
        const char* shaderDef = shaderDefBuilder.getDefinition();
        glShaderSource(m_shader, 1, &shaderDef, NULL);
        glCompileShader(m_shader);

        // check validity
        GLint status = 0;
        glGetShaderiv(m_shader, GL_COMPILE_STATUS, &status);
        if (!status)
            events::utils::Logger::getInstance()->writeErrorEntry("Shader.setVertexShader"s, "Built-in shader compilation failure"s);
    }
}


/// @brief Load external shader from file
/// @param[in] path  File path
/// @returns Success (validity)
bool Shader::loadShaderFromFile(const std::string& path) noexcept
{
    try
    {
        // open file
		FILE* shaderFd = nullptr;
		#ifdef _WINDOWS
		if (fopen_s(&shaderFd, path.c_str(), "rb") != 0)
			throw std::invalid_argument("Shader file not found or corrupted.");
		#else
		shaderFd = fopen(path.c_str(), "rb");
        if (shaderFd == nullptr)
            throw std::invalid_argument("Shader file not found or corrupted.");
		#endif
        // get file size
        fseek(shaderFd, 0, SEEK_END);
        size_t fileSize = ftell(shaderFd);
        fseek(shaderFd, 0, SEEK_SET);

        // read entire file
        char* pShaderDef = new char[fileSize + 1];
        if (pShaderDef == nullptr)
        {
            fclose(shaderFd);
            throw std::runtime_error("Shader definition allocation failure. File may be too big?");
        }
        fread(pShaderDef, 1, fileSize, shaderFd);
		pShaderDef[fileSize] = '\0';
        fclose(shaderFd);

        // load string definition into shader
		const char* pShaderConstDef = pShaderDef;
        glShaderSource(m_shader, 1, &pShaderConstDef, NULL);
        glCompileShader(m_shader);
        delete [] pShaderDef;

        // check validity
        GLint status = 0;
        glGetShaderiv(m_shader, GL_COMPILE_STATUS, &status);
        if (!status)
            throw std::invalid_argument("Shader file compilation failure. File contains invalid code.");
    }
    catch (const std::exception& exc)
    {
        events::utils::Logger::getInstance()->writeErrorEntry("Shader.loadShaderFromFile : "s + path, exc.what());
        return false;
    }
    return true;
}
