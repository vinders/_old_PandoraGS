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
            std::string* m_pDefinition;

        public:
            /// @brief Create vertex shader definition (based on current config)
            VertexShaderDefinition();
            /// @brief Destroy vertex shader definition
            ~VertexShaderDefinition();

            /// @brief Get vertex shader definition
            /// @return Shader definition string
            inline const char* getDefinition()
            {
                return m_pDefinition->c_str();
            }
        };
    }
}
