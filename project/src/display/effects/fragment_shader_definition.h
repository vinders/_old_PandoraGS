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
            std::string m_definition; ///< Generated shader code

        public:
            /// @brief Create fragment shader definition (based on current config)
            FragmentShaderDefinition();

            /// @brief Get fragment shader definition
            /// @returns Shader definition string
            inline const char* getDefinition() const
            {
                return m_definition.c_str();
            }
        };
    }
}
