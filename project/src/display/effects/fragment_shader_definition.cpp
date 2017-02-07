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
using namespace std::literals::string_literals;
#include "../../config/config.h"
#include "fragment_shader_definition.h"
using namespace display::effects;


/// @brief Create fragment shader definition (based on current config)
FragmentShaderDefinition::FragmentShaderDefinition()
{
    // input/output + main function
    m_definition.reserve(32000);
    m_definition =
        "#version 410          \n"
        "in vec3 color;        \n" // R, G, B
        "in uint transparency; \n" // semi-transparency
        " "
        "out vec4 frag_color;  \n"
        " "
        "void main() {"s;

    //...

    m_definition +=
        "    frag_color = vec4(color, 1.0);"
        "}\n"s;

    // function blocks
    //...
}
