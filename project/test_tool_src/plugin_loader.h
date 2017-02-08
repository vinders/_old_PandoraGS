/*******************************************************************************
PANDORAGS project - PS1 GPU driver test app
------------------------------------------------------------------------
Description : PandoraGS test utility -- GPU plugin loader
*******************************************************************************/
#pragma once

#include <cstdint>
#include "primitive_builder.h"

/// @class PluginLoader
/// @brief GPU plugin loader
class PluginLoader
{
private:
    HWND m_hWindow; ///< Main window handle

public:
    /// @brief Initialize tested plugin
    /// @param[in] hWindow  Main window handle
    /// @throws runtime_error  Init failure
    PluginLoader(HWND hWindow);
    /// @brief Shutdown tested plugin
    ~PluginLoader();

    /// @brief Load unit testing
    /// @returns Success
    bool checkUnits();
    
    /// @brief Draw polygon primitive
    /// @param[in] type               Type of shape (triangle/quad, flat/gouraud)
    /// @param[in] rendering          Type of rendering (texturing, blending)
    /// @param[in] isSemiTransparent  Use semi-transparency or not
    void drawPolygon(shape_type_t type, shape_rendering_t rendering, bool isSemiTransparent);
    /// @brief Draw line primitive
    /// @param[in] type               Type of shape (line/poly-line, flat/gouraud)
    /// @param[in] isSemiTransparent  Use semi-transparency or not
    void drawLine(shape_type_t type, bool isSemiTransparent);
    /// @brief Draw rectangle primitive
    /// @param[in] type               Type of shape (size)
    /// @param[in] rendering          Type of rendering (texturing, blending)
    /// @param[in] isSemiTransparent  Use semi-transparency or not
    void drawRectangle(rectangle_type_t type, shape_rendering_t rendering, bool isSemiTransparent, bool isFlipped);
    
    /// @brief Play plugin demonstration sequence
    void playDemoSequence();
    
private:
    /// @brief Process and display primitive
    /// @param[in] pData  Primitive data chain
    /// @param[in] len    Data length (number of 32-bit blocks)
    void processPrimitive(unsigned long* pData, size_t len);
};