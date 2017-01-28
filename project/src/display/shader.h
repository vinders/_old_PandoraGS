/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : rendering shader builder
*******************************************************************************/
#pragma once

#include <cstdint>
#include "../vendor/opengl.h" // openGL includes

/// @namespace display
/// Display management
namespace display
{
    /// @enum shadertype_t
    /// @brief Rendering shader type
    enum class shadertype_t : uint32_t
    {
        undefined = 0u,
        vertex = 1u,
        fragment = 2u
    };


    /// @class Shader
    /// @brief Rendering shader builder
    class Shader
    {
    private:
        GLuint m_programId; ///< Rendering pipeline program identifier
        GLuint m_shader;    ///< Shader identifier

    public:
        /// @brief Create and load shader
        /// @param programId Rendering pipeline program
        /// @param type Type of shader
        Shader(GLuint& programId, shadertype_t type);
        /// @brief Disable and destroy shader
        ~Shader();

        /// @brief Create custom vertex shader (based on current config)
        void setVertexShader();
        /// @brief Create custom fragment shader (based on current config)
        void setFragmentShader();
    };
}
