/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : drawing rectangle primitive (tile / sprite)
*******************************************************************************/
#include "primitive_factory.h"
#include "rect_primitive.h"
using namespace command::primitive;


// -- primitive units - tiles -- ---------------------------------------

/// @brief Fill blank area
/// @param pData Raw primitive data pointer
void fill_area_t::process(cmd_block_t* pData)
{
    if (pData == NULL)
        return;
    fill_area_t* pPrim = (fill_area_t*)pData;

    //...
}

/// @brief Process tile of any desired size
/// @param pData Raw primitive data pointer
void tile_f_t::process(cmd_block_t* pData)
{
    if (pData == NULL)
        return;
    tile_f_t* pPrim = (tile_f_t*)pData;

    //...
}

/// @brief Process 1x1 fixed-size tile
/// @param pData Raw primitive data pointer
void tile_f1_t::process(cmd_block_t* pData)
{
    if (pData == NULL)
        return;
    tile_f1_t* pPrim = (tile_f1_t*)pData;

    //...
}

/// @brief Process 8x8 fixed-size tile
/// @param pData Raw primitive data pointer
void tile_f8_t::process(cmd_block_t* pData)
{
    if (pData == NULL)
        return;
    tile_f8_t* pPrim = (tile_f8_t*)pData;

    //...
}

/// @brief Process 16x16 fixed-size tile
/// @param pData Raw primitive data pointer
void tile_f16_t::process(cmd_block_t* pData)
{
    if (pData == NULL)
        return;
    tile_f16_t* pPrim = (tile_f16_t*)pData;

    //...
}


// -- primitive units - sprites -- -------------------------------------

/// @brief Process sprite of any desired size
/// @param pData Raw primitive data pointer
void sprite_f_t::process(cmd_block_t* pData)
{
    if (pData == NULL)
        return;
    sprite_f_t* pPrim = (sprite_f_t*)pData;

    //...
}

/// @brief Process 1x1 fixed-size sprite
/// @param pData Raw primitive data pointer
void sprite_f1_t::process(cmd_block_t* pData)
{
    if (pData == NULL)
        return;
    sprite_f1_t* pPrim = (sprite_f1_t*)pData;

    //...
}

/// @brief Process 8x8 fixed-size sprite
/// @param pData Raw primitive data pointer
void sprite_f8_t::process(cmd_block_t* pData)
{
    if (pData == NULL)
        return;
    sprite_f8_t* pPrim = (sprite_f8_t*)pData;

    //...
}

/// @brief Process 16x16 fixed-size sprite
/// @param pData Raw primitive data pointer
void sprite_f16_t::process(cmd_block_t* pData)
{
    if (pData == NULL)
        return;
    sprite_f16_t* pPrim = (sprite_f16_t*)pData;

    //...
}
