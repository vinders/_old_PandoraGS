/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : rectangle primitive (tile / sprite)
*******************************************************************************/
#include "../../globals.h"
#include "primitive_facade.h"
#include "rect_primitive.h"
using namespace command::primitive;
#pragma pack(push, 4)


// -- primitive units - tiles -- ---------------------------------------

/// @brief Fill blank area
/// @param[in] pData  Raw primitive data pointer
void fill_area_t::process(command::cmd_block_t* pData)
{
    fill_area_t* pPrim = (fill_area_t*)pData;

    //...
}

/// @brief Process tile of any desired size
/// @param[in] pData  Raw primitive data pointer
void tile_f_t::process(command::cmd_block_t* pData)
{
    tile_f_t* pPrim = (tile_f_t*)pData;

    //...
}

/// @brief Process 1x1 fixed-size tile
/// @param[in] pData  Raw primitive data pointer
void tile_f1_t::process(command::cmd_block_t* pData)
{
    tile_f1_t* pPrim = (tile_f1_t*)pData;

    //...
}

/// @brief Process 8x8 fixed-size tile
/// @param[in] pData  Raw primitive data pointer
void tile_f8_t::process(command::cmd_block_t* pData)
{
    tile_f8_t* pPrim = (tile_f8_t*)pData;

    //...
}

/// @brief Process 16x16 fixed-size tile
/// @param[in] pData  Raw primitive data pointer
void tile_f16_t::process(command::cmd_block_t* pData)
{
    tile_f16_t* pPrim = (tile_f16_t*)pData;

    //...
}


// -- primitive units - sprites -- -------------------------------------

/// @brief Process sprite of any desired size
/// @param[in] pData  Raw primitive data pointer
void sprite_f_t::process(command::cmd_block_t* pData)
{
    sprite_f_t* pPrim = (sprite_f_t*)pData;

    //...
}

/// @brief Process 1x1 fixed-size sprite
/// @param[in] pData  Raw primitive data pointer
void sprite_f1_t::process(command::cmd_block_t* pData)
{
    sprite_f1_t* pPrim = (sprite_f1_t*)pData;

    //...
}

/// @brief Process 8x8 fixed-size sprite
/// @param[in] pData  Raw primitive data pointer
void sprite_f8_t::process(command::cmd_block_t* pData)
{
    sprite_f8_t* pPrim = (sprite_f8_t*)pData;

    //...
}

/// @brief Process 16x16 fixed-size sprite
/// @param[in] pData  Raw primitive data pointer
void sprite_f16_t::process(command::cmd_block_t* pData)
{
    sprite_f16_t* pPrim = (sprite_f16_t*)pData;

    //...
}

#pragma pack(pop)
