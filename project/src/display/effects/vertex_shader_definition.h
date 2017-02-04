/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : vertex shader definition builder
*******************************************************************************/
#pragma once

#include <string>

/// @namespace display
/// Display management
namespace display
{
    /// @namespace display.effects
    /// Display effects
    namespace effects
    {
        /// @class VertexShaderDefinition
        /// @brief Vertex shader definition builder
        class VertexShaderDefinition
        {
        private:
            std::string m_definition; ///< Generated shader code

        public:
            /// @brief Create vertex shader definition (based on current config)
            VertexShaderDefinition();

            /// @brief Get vertex shader definition
            /// @returns Shader definition string
            inline const char* getDefinition() const
            {
                return m_definition.c_str();
            }
        };
    }
}
