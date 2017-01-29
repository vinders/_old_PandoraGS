/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : polygon primitive (triangle / quad)
*******************************************************************************/
#include "../../globals.h"
#include "primitive_facade.h"
#include "poly_primitive.h"
using namespace command::primitive;


// -- primitive units - flat polygons -- -------------------------------

/// @brief Process flat-shaded triangle
/// @param pData Raw primitive data pointer
void poly_f3_t::process(cmd_block_t* pData)
{
    poly_f3_t* pPrim = (poly_f3_t*)pData;

    //...
}

/// @brief Process flat-shaded quad
/// @param pData Raw primitive data pointer
void poly_f4_t::process(cmd_block_t* pData)
{
    poly_f4_t* pPrim = (poly_f4_t*)pData;

    //...
}

/// @brief Process flat-shaded texture-mapped triangle
/// @param pData Raw primitive data pointer
void poly_ft3_t::process(cmd_block_t* pData)
{
    poly_ft3_t* pPrim = (poly_ft3_t*)pData;

    //...
}

/// @brief Process flat-shaded texture-mapped quad
/// @param pData Raw primitive data pointer
void poly_ft4_t::process(cmd_block_t* pData)
{
    poly_ft4_t* pPrim = (poly_ft4_t*)pData;

    //...
}


// -- primitive units - shaded polygons -- -----------------------------

/// @brief Process gouraud-shaded triangle
/// @param pData Raw primitive data pointer
void poly_g3_t::process(cmd_block_t* pData)
{
    poly_g3_t* pPrim = (poly_g3_t*)pData;

    //...
}

/// @brief Process gouraud-shaded quad
/// @param pData Raw primitive data pointer
void poly_g4_t::process(cmd_block_t* pData)
{
    poly_g4_t* pPrim = (poly_g4_t*)pData;

    //...
}

/// @brief Process gouraud-shaded texture-mapped triangle
/// @param pData Raw primitive data pointer
void poly_gt3_t::process(cmd_block_t* pData)
{
    poly_gt3_t* pPrim = (poly_gt3_t*)pData;

    //...
}

/// @brief Process gouraud-shaded texture-mapped quad
/// @param pData Raw primitive data pointer
void poly_gt4_t::process(cmd_block_t* pData)
{
    poly_gt4_t* pPrim = (poly_gt4_t*)pData;

    //...
}
