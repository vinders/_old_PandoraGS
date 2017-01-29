/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : fragment shader definition builder
*******************************************************************************/
#include "../../globals.h"
#include <string>
#include "../../config/config.h"
#include "fragment_shader_definition.h"
using namespace display::effects;


/// @brief Create fragment shader definition (based on current config)
FragmentShaderDefinition::FragmentShaderDefinition()
{
    // input/output + main function
    m_pDefinition = new std::string(
        "#version 410          \n"
        "in vec3 color;        \n" // R, G, B
        "in uint transparency; \n" // semi-transparency
        " "
        "out vec4 frag_color;  \n"
        " "
        "void main() {");

    //...

    *m_pDefinition +=
        "    frag_color = vec4(color, 1.0);"
        "}\n";

    // function blocks
    //...
}
/// @brief Destroy fragment shader definition
FragmentShaderDefinition::~FragmentShaderDefinition()
{
    delete m_pDefinition;
}
