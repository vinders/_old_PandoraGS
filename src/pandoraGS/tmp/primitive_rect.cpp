/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   primitive_rect.cpp
Description : primitive processing - rectangles/sprites
*******************************************************************************/
using namespace std;
#include "primitive_rect.h"

namespace Primitive
{
    /// <summary>Create rectangle primitive - custom-sized tile</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmTile(unsigned char* pData)
    {
        tile_f_t* pPrim = (tile_f_t*)pData;
        //...
    }

    /// <summary>Create rectangle primitive - 1x1 tile (point)</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmTile1(unsigned char* pData)
    {
        tile_f1_t* pPrim = (tile_f1_t*)pData;
        //...
    }

    /// <summary>Create rectangle primitive - 8x8 tile</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmTile8(unsigned char* pData)
    {
        tile_f8_t* pPrim = (tile_f8_t*)pData;
        //...
    }

    /// <summary>Create rectangle primitive - 16x16 tile</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmTile16(unsigned char* pData)
    {
        tile_f16_t* pPrim = (tile_f16_t*)pData;
        //...
    }


    /// <summary>Create rectangle primitive - custom-sized sprite</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmSprite(unsigned char* pData)
    {
        sprite_f_t* pPrim = (sprite_f_t*)pData;
        //...
    }

    /// <summary>Create rectangle primitive - 1x1 sprite (pixel)</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmSprite1(unsigned char* pData)
    {
        sprite_f1_t* pPrim = (sprite_f1_t*)pData;
        //...
    }

    /// <summary>Create rectangle primitive - 8x8 sprite</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmSprite8(unsigned char* pData)
    {
        sprite_f8_t* pPrim = (sprite_f8_t*)pData;
        //...
    }

    /// <summary>Create rectangle primitive - 16x16 sprite</summary>
    /// <param name="pData">Raw primitive data</param>
    void cmSprite16(unsigned char* pData)
    {
        sprite_f16_t* pPrim = (sprite_f16_t*)pData;
        //...
    }
}
