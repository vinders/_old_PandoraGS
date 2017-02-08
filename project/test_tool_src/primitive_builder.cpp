/*******************************************************************************
PANDORAGS project - PS1 GPU driver test app
------------------------------------------------------------------------
Description : PandoraGS test utility -- primitive builder
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <stdexcept>
using namespace std;
#include "primitive_builder.h"

#define PRIMITIVE_STP_BIT 0x2000000


/// @brief Create primitive - fill area
/// @param[out] ppOutData  Primitive data chain
/// @param[in] color       Fill color
/// @returns Data length (number of 32-bit blocks)
size_t PrimitiveBuilder::PrimitiveBuilder::createAreaFiller(unsigned long** ppOutData, const uint32_t color)
{
    if (ppOutData == nullptr)
        return 0;
    if (*ppOutData != nullptr)
        delete [] (*ppOutData);
    
    *ppOutData = new unsigned long[2];
    **ppOutData = 0x2000000 | color;
    return 1;
}

/// @brief Create primitive - polygon
/// @param[out] ppOutData  Primitive data chain
/// @param[in] descr       Primitive descriptor
/// @returns Data length (number of 32-bit blocks)
size_t PrimitiveBuilder::createPolygon(unsigned long** ppOutData, const poly_descriptor_t& descr)
{
    if (ppOutData == nullptr)
        return 0;
    if (*ppOutData != nullptr)
        delete [] (*ppOutData);
    
    unsigned long primId = (static_cast<unsigned long>(primitive_category_t::polygon) | static_cast<unsigned long>(descr.type) | static_cast<unsigned long>(descr.rendering)) << 24;
    if (descr.isSemiTransparent)
        primId |= PRIMITIVE_STP_BIT;
    
    size_t len = 0;
    unsigned long* pPrim = nullptr;
    switch (descr.type)
    {
        case shape_type_t::flat_base:
            if (descr.rendering == shape_rendering_t::textured || descr.rendering == shape_rendering_t::texturedBlended)
            {
                len = 7;
                pPrim = new unsigned long[7];
                pPrim[0] = descr.colors[0] | primId;
                pPrim[1] = descr.coords[0].x & 0x0FFFF | ((descr.coords[0].y & 0x0FFFF) << 16);
                pPrim[3] = descr.coords[1].x & 0x0FFFF | ((descr.coords[1].y & 0x0FFFF) << 16);
                pPrim[5] = descr.coords[2].x & 0x0FFFF | ((descr.coords[2].y & 0x0FFFF) << 16);
                pPrim[2] = pPrim[4] = pPrim[6] = 0;
            }
            else
            {
                len = 4;
                pPrim = new unsigned long[4];
                pPrim[0] = descr.colors[0] | primId;
                pPrim[1] = descr.coords[0].x & 0x0FFFF | ((descr.coords[0].y & 0x0FFFF) << 16);
                pPrim[2] = descr.coords[1].x & 0x0FFFF | ((descr.coords[1].y & 0x0FFFF) << 16);
                pPrim[3] = descr.coords[2].x & 0x0FFFF | ((descr.coords[2].y & 0x0FFFF) << 16);
            }
            break;
        case shape_type_t::flat_multiple:
            if (descr.rendering == shape_rendering_t::textured || descr.rendering == shape_rendering_t::texturedBlended)
            {
                len = 9;
                pPrim = new unsigned long[9];
                pPrim[0] = descr.colors[0] | primId;
                pPrim[1] = descr.coords[0].x & 0x0FFFF | ((descr.coords[0].y & 0x0FFFF) << 16);
                pPrim[3] = descr.coords[1].x & 0x0FFFF | ((descr.coords[1].y & 0x0FFFF) << 16);
                pPrim[5] = descr.coords[2].x & 0x0FFFF | ((descr.coords[2].y & 0x0FFFF) << 16);
                pPrim[7] = descr.coords[3].x & 0x0FFFF | ((descr.coords[3].y & 0x0FFFF) << 16);
                pPrim[2] = pPrim[4] = pPrim[6] = pPrim[8] = 0;
            }
            else
            {
                len = 5;
                pPrim = new unsigned long[5];
                pPrim[0] = descr.colors[0] | primId;
                pPrim[1] = descr.coords[0].x & 0x0FFFF | ((descr.coords[0].y & 0x0FFFF) << 16);
                pPrim[2] = descr.coords[1].x & 0x0FFFF | ((descr.coords[1].y & 0x0FFFF) << 16);
                pPrim[3] = descr.coords[2].x & 0x0FFFF | ((descr.coords[2].y & 0x0FFFF) << 16);
                pPrim[4] = descr.coords[3].x & 0x0FFFF | ((descr.coords[3].y & 0x0FFFF) << 16);
            }
            break;
        case shape_type_t::gouraud_base:
            if (descr.rendering == shape_rendering_t::textured || descr.rendering == shape_rendering_t::texturedBlended)
            {
                len = 9;
                pPrim = new unsigned long[9];
                pPrim[0] = descr.colors[0] | primId;
                pPrim[3] = descr.colors[1];
                pPrim[6] = descr.colors[2];
                pPrim[1] = descr.coords[0].x & 0x0FFFF | ((descr.coords[0].y & 0x0FFFF) << 16);
                pPrim[4] = descr.coords[1].x & 0x0FFFF | ((descr.coords[1].y & 0x0FFFF) << 16);
                pPrim[7] = descr.coords[2].x & 0x0FFFF | ((descr.coords[2].y & 0x0FFFF) << 16);
                pPrim[2] = pPrim[5] = pPrim[8] = 0;
            }
            else
            {
                len = 6;
                pPrim = new unsigned long[6];
                pPrim[0] = descr.colors[0] | primId;
                pPrim[2] = descr.colors[1];
                pPrim[4] = descr.colors[2];
                pPrim[1] = descr.coords[0].x & 0x0FFFF | ((descr.coords[0].y & 0x0FFFF) << 16);
                pPrim[3] = descr.coords[1].x & 0x0FFFF | ((descr.coords[1].y & 0x0FFFF) << 16);
                pPrim[5] = descr.coords[2].x & 0x0FFFF | ((descr.coords[2].y & 0x0FFFF) << 16);
            }
            break;
        case shape_type_t::gouraud_multiple:
            if (descr.rendering == shape_rendering_t::textured || descr.rendering == shape_rendering_t::texturedBlended)
            {
                len = 12;
                pPrim = new unsigned long[12];
                pPrim[0] = descr.colors[0] | primId;
                pPrim[3] = descr.colors[1];
                pPrim[6] = descr.colors[2];
                pPrim[9] = descr.colors[3];
                pPrim[1] = descr.coords[0].x & 0x0FFFF | ((descr.coords[0].y & 0x0FFFF) << 16);
                pPrim[4] = descr.coords[1].x & 0x0FFFF | ((descr.coords[1].y & 0x0FFFF) << 16);
                pPrim[7] = descr.coords[2].x & 0x0FFFF | ((descr.coords[2].y & 0x0FFFF) << 16);
                pPrim[10] = descr.coords[3].x & 0x0FFFF | ((descr.coords[3].y & 0x0FFFF) << 16);
                pPrim[2] = pPrim[5] = pPrim[8] = pPrim[11] = 0;
            }
            else
            {
                len = 8;
                pPrim = new unsigned long[8];
                pPrim[0] = descr.colors[0] | primId;
                pPrim[2] = descr.colors[1];
                pPrim[4] = descr.colors[2];
                pPrim[6] = descr.colors[3];
                pPrim[1] = descr.coords[0].x & 0x0FFFF | ((descr.coords[0].y & 0x0FFFF) << 16);
                pPrim[3] = descr.coords[1].x & 0x0FFFF | ((descr.coords[1].y & 0x0FFFF) << 16);
                pPrim[5] = descr.coords[2].x & 0x0FFFF | ((descr.coords[2].y & 0x0FFFF) << 16);
                pPrim[7] = descr.coords[3].x & 0x0FFFF | ((descr.coords[3].y & 0x0FFFF) << 16);
            }
            break;
    }
    *ppOutData = pPrim;
    return len;
}

/// @brief Create primitive - line
/// @param[out] ppOutData  Primitive data chain
/// @param[in] descr       Primitive descriptor
/// @returns Data length (number of 32-bit blocks)
size_t PrimitiveBuilder::createLine(unsigned long** ppOutData, const line_descriptor_t& descr)
{
    if (ppOutData == nullptr)
        return 0;
    if (*ppOutData != nullptr)
        delete [] (*ppOutData);
    
    unsigned long primId = (static_cast<unsigned long>(primitive_category_t::line) | static_cast<unsigned long>(descr.type)) << 24;
    if (descr.isSemiTransparent)
        primId |= PRIMITIVE_STP_BIT;
    
    size_t len = 0;
    unsigned long* pPrim = nullptr;
    switch (descr.type)
    {
        case shape_type_t::flat_base:
            len = 3;
            pPrim = new unsigned long[3];
            pPrim[0] = descr.colors[0] | primId;
            pPrim[1] = descr.coords[0].x & 0x0FFFF | ((descr.coords[0].y & 0x0FFFF) << 16);
            pPrim[2] = descr.coords[1].x & 0x0FFFF | ((descr.coords[1].y & 0x0FFFF) << 16);
            break;
        case shape_type_t::flat_multiple:
            len = 6;
            pPrim = new unsigned long[6];
            pPrim[0] = descr.colors[0] | primId;
            pPrim[1] = descr.coords[0].x & 0x0FFFF | ((descr.coords[0].y & 0x0FFFF) << 16);
            pPrim[2] = descr.coords[1].x & 0x0FFFF | ((descr.coords[1].y & 0x0FFFF) << 16);
            pPrim[3] = descr.coords[2].x & 0x0FFFF | ((descr.coords[2].y & 0x0FFFF) << 16);
            pPrim[4] = descr.coords[3].x & 0x0FFFF | ((descr.coords[3].y & 0x0FFFF) << 16);
            pPrim[5] = 0x55555555;
            break;
        case shape_type_t::gouraud_base:
            len = 4;
            pPrim = new unsigned long[4];
            pPrim[0] = descr.colors[0] | primId;
            pPrim[2] = descr.colors[1];
            pPrim[1] = descr.coords[0].x & 0x0FFFF | ((descr.coords[0].y & 0x0FFFF) << 16);
            pPrim[3] = descr.coords[1].x & 0x0FFFF | ((descr.coords[1].y & 0x0FFFF) << 16);
            break;
        case shape_type_t::gouraud_multiple:
            len = 9;
            pPrim = new unsigned long[9];
            pPrim[0] = descr.colors[0] | primId;
            pPrim[2] = descr.colors[1];
            pPrim[4] = descr.colors[2];
            pPrim[6] = descr.colors[3];
            pPrim[1] = descr.coords[0].x & 0x0FFFF | ((descr.coords[0].y & 0x0FFFF) << 16);
            pPrim[3] = descr.coords[1].x & 0x0FFFF | ((descr.coords[1].y & 0x0FFFF) << 16);
            pPrim[5] = descr.coords[2].x & 0x0FFFF | ((descr.coords[2].y & 0x0FFFF) << 16);
            pPrim[7] = descr.coords[3].x & 0x0FFFF | ((descr.coords[3].y & 0x0FFFF) << 16);
            pPrim[8] = 0x55555555;
            break;
    }
    *ppOutData = pPrim;
    return len;
}

/// @brief Create primitive - rectangle
/// @param[out] ppOutData  Primitive data chain
/// @param[in] descr       Primitive descriptor
/// @returns Data length (number of 32-bit blocks)
size_t PrimitiveBuilder::createRectangle(unsigned long** ppOutData, const rect_descriptor_t& descr)
{
    if (ppOutData == nullptr)
        return 0;
    if (*ppOutData != nullptr)
        delete [] (*ppOutData);
    
    unsigned long primId = (static_cast<unsigned long>(primitive_category_t::rectangle) | static_cast<unsigned long>(descr.type) | static_cast<unsigned long>(descr.rendering)) << 24;
    if (descr.isSemiTransparent)
        primId |= PRIMITIVE_STP_BIT;
    
    size_t len = 0;
    unsigned long* pPrim = nullptr;
    if (descr.type == rectangle_type_t::freeSize)
    {
        if (descr.rendering == shape_rendering_t::textured || descr.rendering == shape_rendering_t::texturedBlended)
        {
            len = 4;
            pPrim = new unsigned long[4];
            pPrim[2] = 0;
            pPrim[3] = descr.coords[1].x & 0x0FFFF | ((descr.coords[1].y & 0x0FFFF) << 16);
        }
        else
        {
            len = 3;
            pPrim = new unsigned long[3];
            pPrim[2] = descr.coords[1].x & 0x0FFFF | ((descr.coords[1].y & 0x0FFFF) << 16);
        }
    }
    else
    {
        if (descr.rendering == shape_rendering_t::textured || descr.rendering == shape_rendering_t::texturedBlended)
        {
            len = 3;
            pPrim = new unsigned long[3];
            pPrim[2] = 0;
        }
        else
        {
            len = 2;
            pPrim = new unsigned long[2];
        }
    }
    pPrim[0] = descr.color | primId;
    pPrim[1] = descr.coords[0].x & 0x0FFFF | ((descr.coords[0].y & 0x0FFFF) << 16);
    
    *ppOutData = pPrim;
    return len;
}
