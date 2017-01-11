/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   primitive_line.cpp
Description : primitive processing - lines
*******************************************************************************/
using namespace std;
#include "primitive_line.h"

namespace Primitive
{
    /// <summary>Create line primitive - flat-shaded line</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmLineF(unsigned char* pData)
    {
        line_f2_t* pPrim = (line_f2_t*)pData;
        //...
    }

    /// <summary>Create line primitive - gouraud-shaded line</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmLineG(unsigned char* pData)
    {
        line_g2_t* pPrim = (line_g2_t*)pData;
        //...
    }


    /// <summary>Create line primitive - flat-shaded poly-line</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmPolyLineF(unsigned char* pData)
    {
        line_fp_t* pPrim = (line_fp_t*)pData;
        line_fp_iterator it(*pPrim);
        //...
        
        do
        {
            it.get();//...
        }
        while (it.next());
    }

    /// <summary>Create line primitive - gouraud-shaded poly-line</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmPolyLineG(unsigned char* pData)
    {
        line_gp_t* pPrim = (line_gp_t*)pData;
        line_gp_iterator it(*pPrim);
        //...

        do
        {
            it.get();//...
        } 
        while (it.next());
    }
}
