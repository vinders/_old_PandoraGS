/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : fragment shader definition builder
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
        /// @class FragmentShaderDefinition
        /// @brief Fragment shader definition builder
        class FragmentShaderDefinition
        {
        private:
            std::string* m_pDefinition;

        public:
            /// @brief Create fragment shader definition (based on current config)
            FragmentShaderDefinition();
            /// @brief Destroy fragment shader definition
            ~FragmentShaderDefinition();

            /// @brief Get fragment shader definition
            /// @return Shader definition string
            inline const char* getDefinition()
            {
                return m_pDefinition->c_str();
            }
        };
    }
}
