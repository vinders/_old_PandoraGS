/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   shader_vertex.hpp
Description : vertex shader builder (polygon management)
*******************************************************************************/
#ifndef _SHADER_VERTEX_HPP
#define _SHADER_VERTEX_HPP
#include <string>

namespace VertexShader
{
    /// <summary>Create shader source with headers and beginning of main function</summary>
    inline std::string* beginMain()
    {
        std::string* pShaderSource = new std::string( // alloc to avoid stack overflow
            "#version 410                               \n"
            "layout(location = 0) in vec3 vertex_pos;   \n"
            "layout(location = 1) in vec3 vertex_color; \n"
            "out vec3 color;                            \n"
            " "
            "void main() {");
        return pShaderSource;
    }
    /// <summary>Append end of main function</summary>
    inline void endMain(std::string& shaderSource)
    {
        shaderSource +=
            "    color = vertex_color;"
            "    gl_Position = vec4(vertex_pos, 1.0);"
            "}\n";
    }

    //... 1 fonction pour chaque type d'appel du main

    //... 1 fonction pour chaque type de fonctionnalité (avec paramètre pour choix, ex: type de filtre)
}

#endif
