/*******************************************************************************
PANDORAGS project - PS1 GPU driver test app
------------------------------------------------------------------------
Description : PandoraGS test utility -- GPU plugin loader
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <stdexcept>
using namespace std;
#include "../src/psemu_main.h"
#include "primitive_builder.h"
#include "plugin_loader.h"


/// @brief Initialize tested plugin
/// @param[in] hWindow  Main window handle
/// @throws runtime_error  Init failure
PluginLoader::PluginLoader(HWND hWindow)
{
    // initialize plugin
    if (GPUinit())
        throw std::runtime_error("GPUinit command failed");
    // start renderer
    GPUsetExeName("UNITTEST.001");
    if (GPUopen(hWindow))
        throw std::runtime_error("GPUopen command failed");
    m_hWindow = hWindow;
}

/// @brief Shutdown tested plugin
PluginLoader::~PluginLoader()
{
    // close renderer
    GPUclose();
    // close plugin
    GPUshutdown();
}


/// @brief Load unit testing
void PluginLoader::checkUnits()
{
    GPUtestUnits((void*)m_hWindow);
}

/// @brief Process and display primitive
/// @param[in] pData  Primitive data chain
/// @param[in] len    Data length (number of 32-bit blocks)
void PluginLoader::processPrimitive(unsigned long* pData, size_t len)
{
    if (len > 0)
        GPUtestPrimitive(pData, static_cast<int>(len));
}


/// @brief Draw polygon primitive
/// @param[in] type               Type of shape (triangle/quad, flat/gouraud)
/// @param[in] rendering          Type of rendering (texturing, blending)
/// @param[in] isSemiTransparent  Use semi-transparency or not
void PluginLoader::drawPolygon(shape_type_t type, shape_rendering_t rendering, bool isSemiTransparent)
{
    size_t len = 0;
    unsigned long* pData = nullptr;
    
    // clear display
    len = PrimitiveBuilder::createAreaFiller(&pData, 0);
    processPrimitive(pData, len);
    
    // clear texture cache
    //...
    // set texture page
    //...
    // set semi-transparency mode
    //...
    
    // draw back shape
    poly_descriptor_t shape1{ type, rendering, isSemiTransparent, 
                              { coord_t{30,30}, coord_t{90,40}, coord_t{40,60}, coord_t{100,70} }, 
                              { 0xFF0000, 0xFFFF00, 0x00FF00, 0x00FFFF } };
    len = PrimitiveBuilder::createPolygon(&pData, shape1);
    processPrimitive(pData, len);
    // draw front shape
    poly_descriptor_t shape2{ type, rendering, isSemiTransparent, 
                              { coord_t{60,40}, coord_t{120,50}, coord_t{70,70}, coord_t{130,80} }, 
                              { 0x0000FF, 0x00FFFF, 0xFF00FF, 0x00FF00 } };
    len = PrimitiveBuilder::createPolygon(&pData, shape2);
    processPrimitive(pData, len);
    
    if (pData != nullptr)
        delete [] pData;
}

/// @brief Draw line primitive
/// @param[in] type               Type of shape (line/poly-line, flat/gouraud)
/// @param[in] isSemiTransparent  Use semi-transparency or not
void PluginLoader::drawLine(shape_type_t type, bool isSemiTransparent)
{
    size_t len = 0;
    unsigned long* pData = nullptr;
    
    // clear display
    len = PrimitiveBuilder::createAreaFiller(&pData, 0);
    processPrimitive(pData, len);
    
    // set semi-transparency mode
    //...
    
    // draw back shape
    line_descriptor_t shape1{ type, isSemiTransparent, 
                              { coord_t{30,30}, coord_t{90,40}, coord_t{40,60}, coord_t{100,70} }, 
                              { 0xFF0000, 0xFFFF00, 0x00FF00, 0x00FFFF } };
    len = PrimitiveBuilder::createLine(&pData, shape1);
    processPrimitive(pData, len);
    // draw front shape
    line_descriptor_t shape2{ type, isSemiTransparent, 
                              { coord_t{60,40}, coord_t{120,50}, coord_t{70,70}, coord_t{130,80} }, 
                              { 0x0000FF, 0x00FFFF, 0xFF00FF, 0x00FF00 } };
    len = PrimitiveBuilder::createLine(&pData, shape2);
    processPrimitive(pData, len);
    
    if (pData != nullptr)
        delete [] pData;
}

/// @brief Draw rectangle primitive
/// @param[in] type               Type of shape (size)
/// @param[in] rendering          Type of rendering (texturing, blending)
/// @param[in] isSemiTransparent  Use semi-transparency or not
void PluginLoader::drawRectangle(rectangle_type_t type, shape_rendering_t rendering, bool isSemiTransparent, bool isFlipped)
{
    size_t len = 0;
    unsigned long* pData = nullptr;
    
    // clear display
    len = PrimitiveBuilder::createAreaFiller(&pData, 0);
    processPrimitive(pData, len);
    
    // clear texture cache
    //...
    // set texture page
    //...
    // set semi-transparency mode
    //...
    
    // draw back shape
    rect_descriptor_t shape1{ type, rendering, isSemiTransparent, 
                              { coord_t{30,30}, coord_t{60,50} }, 
                              0xFF0000 };
    len = PrimitiveBuilder::createRectangle(&pData, shape1);
    processPrimitive(pData, len);
    // draw front shape
    rect_descriptor_t shape2{ type, rendering, isSemiTransparent, 
                              { coord_t{60,40}, coord_t{60,50} }, 
                              0x0000FF };
    len = PrimitiveBuilder::createRectangle(&pData, shape2);
    processPrimitive(pData, len);
    
    if (pData != nullptr)
        delete [] pData;
}


/// @brief Play plugin demonstration sequence
void PluginLoader::playDemoSequence()
{
    //...
}
