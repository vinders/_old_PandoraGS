/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   shader_fragment.hpp
Description : fragment shader builder (pixel management)
*******************************************************************************/
#ifndef _SHADER_FRAGMENT_HPP
#define _SHADER_FRAGMENT_HPP
#include <string>

namespace FragmentShader
{
    /// <summary>Create shader source with headers and beginning of main function</summary>
    inline std::string* startMain()
    {
        std::string* pShaderSource = new std::string( // alloc to avoid stack overflow
            "#version 410         \n"
            "in vec3 color;       \n"
            "out vec4 frag_color; \n"
            " "
            "void main() {");
        return pShaderSource;
    }
    /// <summary>Append end of main function</summary>
    inline void endMain(std::string& shaderSource)
    {
        shaderSource +=
            "    frag_color = vec4(color, 1.0);"
            "}\n";
    }

    //... 1 fonction pour chaque type d'appel du main

    //... 1 fonction pour chaque type de fonctionnalité (avec paramètre pour choix, ex: type de filtre)
}

#endif
