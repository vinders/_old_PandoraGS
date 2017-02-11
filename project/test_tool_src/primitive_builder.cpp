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


/// @brief Print list of primitive categories
/// @returns Number of items
uint32_t PrimitiveBuilder::listPrimitiveCategories()
{
    printf(" 1 - Polygon (triangle / quad)\n");
    printf(" 2 - Line (line / poly-line)\n");
    printf(" 3 - Rectangle (tile / sprite)\n");
    return 3u;
}
/// @brief Convert user input to primitive category
/// @param[in] input  Input value
/// @returns Category
primitive_category_t PrimitiveBuilder::getPrimitiveCategory(const int32_t input)
{
    primitive_category_t val;
    switch (input)
    {
        case 2:  val = primitive_category_t::line; break;
        case 3:  val = primitive_category_t::rectangle; break;
        default: val = primitive_category_t::polygon; break;
    }
    return val;
}

/// @brief Print list of shape types/sizes
/// @param[in] category  Primitive category
/// @returns Number of items
uint32_t PrimitiveBuilder::listShapeTypes(const primitive_category_t category)
{
    switch (category)
    {
        case primitive_category_t::polygon: 
            printf(" 1 - Flat-shaded triangle\n");
            printf(" 2 - Flat-shaded quad\n");
            printf(" 3 - Gouraud-shaded triangle\n");
            printf(" 4 - Gouraud-shaded quad\n");
            break;
        case primitive_category_t::line:
            printf(" 1 - Flat-shaded line\n");
            printf(" 2 - Flat-shaded poly-line\n");
            printf(" 3 - Gouraud-shaded line\n");
            printf(" 4 - Gouraud-shaded poly-line\n");
            break;
        case primitive_category_t::rectangle:
            printf(" 1 - Custom sized\n");
            printf(" 2 - 1x1\n");
            printf(" 3 - 8x8\n");
            printf(" 4 - 16x16\n");
            printf(" Negative value : flip XY\n");
            break;
    }
    return 4u;
}
/// @brief Convert user input to shape type
/// @param[in] input  Input value
/// @returns Shape type
shape_type_t PrimitiveBuilder::getShapeType(const int32_t input)
{
    shape_type_t val;
    switch (input)
    {
        case 2:  val = shape_type_t::flat_multiple; break;
        case 3:  val = shape_type_t::gouraud_base; break;
        case 4:  val = shape_type_t::gouraud_multiple; break;
        default: val = shape_type_t::flat_base; break;
    }
    return val;
}
/// @brief Convert user input to shape type
/// @param[in] input  Input value
/// @returns Shape type
rectangle_type_t PrimitiveBuilder::getRectangleType(const int32_t input)
{
    rectangle_type_t val;
    switch (input)
    {
        case -2:
        case 2:  val = rectangle_type_t::dot; break;
        case -3:
        case 3:  val = rectangle_type_t::size8x8; break;
        case -4:
        case 4:  val = rectangle_type_t::size16x16; break;
        default: val = rectangle_type_t::freeSize; break;
    }
    return val;
}

/// @brief Print list of primitive rendering types
/// @returns Number of items
uint32_t PrimitiveBuilder::listRenderingTypes()
{
    printf(" 1 - Shaded\n");
    printf(" 2 - Textured\n");
    printf(" 3 - Shaded with blending\n");
    printf(" 4 - Textured with blending\n");
    return 4u;
}
/// @brief Convert user input to rendering type
/// @param[in] input  Input value
/// @returns Rendering type
shape_rendering_t PrimitiveBuilder::getRenderingType(const int32_t input)
{
    shape_rendering_t val;
    switch (input)
    {
        case 2:  val = shape_rendering_t::textured; break;
        case 3:  val = shape_rendering_t::shadedBlended; break;
        case 4:  val = shape_rendering_t::texturedBlended; break;
        default: val = shape_rendering_t::shaded; break;
    }
    return val;
}
