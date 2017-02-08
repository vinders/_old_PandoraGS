/*******************************************************************************
PANDORAGS project - PS1 GPU driver test app
------------------------------------------------------------------------
Description : PandoraGS test utility -- primitive builder
*******************************************************************************/
#pragma once

#include <cstdint>

/// @enum primitive_category_t
/// @brief Type of primitive to build
enum class primitive_category_t : uint32_t
{
    polygon = 0x20,
    line = 0x40,
    rectangle = 0x60
};

/// @enum shape_type_t
/// @brief Type of shape to build
enum class shape_type_t : uint32_t
{
    flat_base = 0x00,
    flat_multiple = 0x08,
    gouraud_base = 0x10,
    gouraud_multiple = 0x18
};

/// @enum rectangle_type_t
/// @brief Type of rectangle to build
enum class rectangle_type_t : uint32_t
{
    freeSize = 0x00,
    dot = 0x08,
    size8x8 = 0x10,
    size16x16 = 0x18
};

/// @enum shape_rendering_t
/// @brief Rendering type of shape to build
enum class shape_rendering_t : uint32_t
{
    shaded = 0x0,
    textured = 0x4,
    shadedBlended = 0x1,
    texturedBlended = 0x5
};


/// @struct coord_t
/// @brief Coordinates
struct coord_t
{
    int32_t x;
    int32_t y;
};

/// @struct poly_descriptor_t
/// @brief Polygon descriptor
struct poly_descriptor_t
{
    shape_type_t type;
    shape_rendering_t rendering;
    bool isSemiTransparent;
    coord_t coords[4];
    uint32_t colors[4];
};

/// @struct line_descriptor_t
/// @brief Line descriptor
struct line_descriptor_t
{
    shape_type_t type;
    bool isSemiTransparent;
    coord_t coords[4];
    uint32_t colors[4];
};

/// @struct rect_descriptor_t
/// @brief Rectangle descriptor
struct rect_descriptor_t
{
    rectangle_type_t type;
    shape_rendering_t rendering;
    bool isSemiTransparent;
    coord_t coords[2];
    uint32_t color;
};


/// @class PrimitiveBuilder
/// @brief Primitive drawing tool
class PrimitiveBuilder
{
public:    
    /// @brief Create primitive - fill area
    /// @param[out] ppOutData  Primitive data chain
    /// @param[in] color       Fill color
    /// @returns Data length (number of 32-bit blocks)
    static size_t PrimitiveBuilder::createAreaFiller(unsigned long** ppOutData, const uint32_t color);
    /// @brief Create primitive - polygon
    /// @param[out] ppOutData  Primitive data chain
    /// @param[in] descr       Primitive descriptor
    /// @returns Data length (number of 32-bit blocks)
    static size_t PrimitiveBuilder::createPolygon(unsigned long** ppOutData, const poly_descriptor_t& descr);
    /// @brief Create primitive - line
    /// @param[out] ppOutData  Primitive data chain
    /// @param[in] descr       Primitive descriptor
    /// @returns Data length (number of 32-bit blocks)
    static size_t PrimitiveBuilder::createLine(unsigned long** ppOutData, const line_descriptor_t& descr);
    /// @brief Create primitive - rectangle
    /// @param[out] ppOutData  Primitive data chain
    /// @param[in] descr       Primitive descriptor
    /// @returns Data length (number of 32-bit blocks)
    static size_t PrimitiveBuilder::createRectangle(unsigned long** ppOutData, const rect_descriptor_t& descr);
};