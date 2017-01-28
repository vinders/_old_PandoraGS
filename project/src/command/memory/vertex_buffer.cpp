/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : vertex data buffer
*******************************************************************************/
#include <cstdlib>
#include <cstdint>
#include "vertex_buffer.h"
using namespace command;


/// @brief Create and configure vertex buffer
/// @param baseCapacity Base capacity (pre-allocated number of vertices)
/// @param isTextured Use texture data
memory::VertexBuffer::VertexBuffer(uint32_t baseCapacity, bool isTextured)
{
    // coords
    m_pVertexCoords = new std::vector<float>();
    m_pVertexCoords->reserve(baseCapacity * 3);

    // colors
    m_pVertexColors = new std::vector<uint32_t>();
    m_pVertexColors->reserve(baseCapacity);

    // texture coords
    if (isTextured)
    {
        m_pTextureInfos = new std::vector<uint32_t>();
        m_pTextureInfos->reserve(baseCapacity);
    }
    else
        m_pTextureInfos = NULL;
}

/// @brief Destroy vertex buffer
memory::VertexBuffer::~VertexBuffer()
{
    delete m_pVertexCoords;
    delete m_pVertexColors;
    if (m_pTextureInfos != NULL)
        delete m_pTextureInfos;
}
