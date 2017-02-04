/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : vertex shader definition builder
*******************************************************************************/
#include "../../globals.h"
#include <string>
#include "../../config/config.h"
#include "vertex_shader_definition.h"
using namespace display::effects;


/// @brief Create vertex shader definition (based on current config)
VertexShaderDefinition::VertexShaderDefinition()
{
    // input/output + main function
    m_definition.reserve(8000);
    m_definition =
        "#version 410                              \n"
        "layout(location = 0) in vec3 vtx_pos;     \n" // X, Y, Z
        "layout(location = 1) in uint vtx_color;   \n" // St-Bb-Gg-Rr
        "layout(location = 2) in uint vtx_texture; \n" // Id-id-Vv-Uu
        " "
        "out vec3 color;                           \n" // R, G, B
        "out uint transparency;                    \n" // semi-transparency
        " "
        "void main() {\n"s;

    //...

    m_definition +=
        "    vec3i colorInt = vec3i(vtx_color&0x0FF, (vtx_color>>8)&0x0FF, (vtx_color>>16)&0x0FF);"
        "    color = vec3(colorInt[0]/255.0f, colorInt[1]/255.0f, colorInt[2]/255.0f);"
        "    transparency = ((vtx_color>>24)&0x0FF);"
        "    gl_Position = vec4(vtx_pos, 1.0);"
        "}\n"s;

    // function blocks
    //...
}
