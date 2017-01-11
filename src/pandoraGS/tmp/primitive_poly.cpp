/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   primitive_poly.cpp
Description : primitive processing - polygons
*******************************************************************************/
using namespace std;
#include "primitive_poly.h"

namespace Primitive
{
    /// <summary>Create polygon primitive - flat-shaded triangle</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmTriangleF(unsigned char* pData)
    {
        poly_f3_t* pPrim = (poly_f3_t*)pData;
        //...
    }

    /// <summary>Create polygon primitive - gouraud-shaded triangle</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmTriangleG(unsigned char* pData)
    {
        poly_g3_t* pPrim = (poly_g3_t*)pData;
        //...
    }

    /// <summary>Create polygon primitive - texture-mapped triangle</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmTriangleFT(unsigned char* pData)
    {
        poly_ft3_t* pPrim = (poly_ft3_t*)pData;
        //...
    }

    /// <summary>Create polygon primitive - texture-mapped gouraud-shaded triangle</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmTriangleGT(unsigned char* pData)
    {
        poly_gt3_t* pPrim = (poly_gt3_t*)pData;
        //...
    }


    /// <summary>Create polygon primitive - flat-shaded quad</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmQuadF(unsigned char* pData)
    {
        poly_f4_t* pPrim = (poly_f4_t*)pData;
        //...
    }

    /// <summary>Create polygon primitive - gouraud-shaded quad</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmQuadG(unsigned char* pData)
    {
        poly_g4_t* pPrim = (poly_g4_t*)pData;
        //...
    }

    /// <summary>Create polygon primitive - texture-mapped quad</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmQuadFT(unsigned char* pData)
    {
        poly_ft4_t* pPrim = (poly_ft4_t*)pData;
        //...
    }

    /// <summary>Create polygon primitive - texture-mapped gouraud-shaded quad</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmQuadGT(unsigned char* pData)
    {
        poly_gt4_t* pPrim = (poly_gt4_t*)pData;
        //...
    }
}
