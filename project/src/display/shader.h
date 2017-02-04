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
        /// @brief Load external shader
        /// @param[in] programId  Rendering pipeline program
        /// @param[in] type       Type of shader
        /// @param[in] path       File path (if external shader) or empty value (built-in)
        Shader(const GLuint& programId, const shadertype_t type, const std::string& path = ""s);
        /// @brief Disable and destroy shader
        ~Shader();

    private:
        /// @brief Create custom vertex shader (based on current config)
        /// @param[in] path  File path (if external shader) or empty (built-in)
        void setVertexShader(const std::string& path);
        /// @brief Create custom fragment shader (based on current config)
        /// @param[in] path  File path (if external shader) or empty (built-in)
        void setFragmentShader(const std::string& path);

        /// @brief Load external shader from file
        /// @param[in] path  File path
        /// @returns Success (validity)
        bool loadShaderFromFile(const std::string& path) noexcept;
    };
}
