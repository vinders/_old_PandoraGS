/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : vertex data buffer
*******************************************************************************/
#pragma once

#include <cstdlib>
#include <cstdint>
#include <vector>

/// @namespace command
/// GPU commands management
namespace command
{
    /// @struct vertex_pos_t
    /// @brief Vertex position
    struct vertex_pos_t
    {
        float x; ///< Vertex X coordinate
        float y; ///< Vertex Y coordinate
        float z; ///< Vertex Z depth layer

        static inline size_t size() { return 3; } ///< Length (number of variables)
    };
    /// @struct vertex_color_t
    /// @brief Vertex color (24-bit RGB + semi-transparency mode)
    typedef uint32_t vertex_color_t;
    /// @struct vertex_texture_t
    /// @brief Vertex texturing information (16-bit texture ID + UV coordinates)
    typedef uint32_t vertex_texture_t;


    /// @namespace command.primitive
    /// GPU memory management
    namespace memory
    {
        /// @class VertexBuffer
        /// @brief Vertex data buffer
        class VertexBuffer
        {
        private:
            std::vector<float>* m_pVertexCoords;    ///< Vertex coordinates : X, Y, Z
            std::vector<uint32_t>* m_pVertexColors; ///< Vertex colors
            std::vector<uint32_t>* m_pTextureInfos; ///< Vertex texture informations

        public:
            /// @brief Create default vertex buffer
            VertexBuffer();
            /// @brief Create and configure vertex buffer
            /// @param baseCapacity Base capacity (pre-allocated number of vertices)
            /// @param isTextured Use texture data
            VertexBuffer(uint32_t baseCapacity, bool isTextured);
            /// @brief Destroy vertex buffer
            ~VertexBuffer();

            /// @brief Configure vertex buffer
            /// @param baseCapacity Base capacity (pre-allocated number of vertices)
            /// @param isTextured Use texture data
            void init(uint32_t baseCapacity, bool isTextured);

            /// @brief Clear vertex buffer (remove content)
            inline void clear()
            {
                m_pVertexCoords->clear();
                m_pVertexColors->clear();
                if (m_pTextureInfos != nullptr)
                    m_pTextureInfos->clear();
            }

            /// @brief Add non-textured elements at the end of the buffer
            /// @param pCoords Vertex coordinates to insert
            /// @param color Vertex color to insert
            inline void push(vertex_pos_t& pCoords, vertex_color_t color)
            {
                m_pVertexCoords->push_back(pCoords.x);
                m_pVertexCoords->push_back(pCoords.y);
                m_pVertexCoords->push_back(pCoords.z);
                m_pVertexColors->push_back(color);
            }

            /// @brief Add non-textured elements at the end of the buffer
            /// @param pCoords Vertex coordinates to insert
            /// @param color Vertex color to insert
            /// @param tex Texture information to insert
            inline void push(vertex_pos_t& pCoords, vertex_color_t& color, vertex_texture_t& tex)
            {
                push(pCoords, color);
                if (m_pTextureInfos != nullptr)
                {
                    m_pTextureInfos->push_back(tex);
                }
            }

            /// @brief Extract simple vertices as arrays (coordinates, colors)
            /// @param ppOutCoords Destination for array of vertex coordinates
            /// @param ppOutColors Destination for array of vertex colors
            inline size_t read(float** ppOutCoords, uint32_t** ppOutColors)
            {
                if (m_pVertexColors->size() > 0)
                {
                    *ppOutCoords = &((*m_pVertexCoords)[0]);
                    *ppOutColors = &((*m_pVertexColors)[0]);

                    return m_pVertexColors->size();
                }
                return 0;
            }

            /// @brief Extract textured vertices as arrays (coordinates, colors, texture information)
            /// @param ppOutCoords Destination for array of vertex coordinates
            /// @param ppOutColors Destination for array of vertex colors
            /// @param ppOutColors Destination for array of vertex texture information
            inline size_t read(float** ppOutCoords, uint32_t** ppOutColors, uint32_t** ppOutTextures)
            {
                if (m_pVertexColors->size() > 0)
                {
                    *ppOutCoords = &((*m_pVertexCoords)[0]);
                    *ppOutColors = &((*m_pVertexColors)[0]);
                    if (m_pTextureInfos != nullptr)
                        *ppOutTextures = &((*m_pTextureInfos)[0]);
                    else
                        *ppOutTextures = nullptr;

                    return m_pVertexColors->size();
                }
                return 0;
            }
        };
    }
}
