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
            /// @param[in] baseCapacity  Base capacity (pre-allocated number of vertices)
            /// @param[in] isTextured    Use texture data
            VertexBuffer(const uint32_t baseCapacity, const bool isTextured);
            /// @brief Destroy vertex buffer
            ~VertexBuffer();

            /// @brief Configure vertex buffer
            /// @param[in] baseCapacity  Base capacity (pre-allocated number of vertices)
            /// @param[in] isTextured    Use texture data
            void init(const uint32_t baseCapacity, const bool isTextured);

            /// @brief Clear vertex buffer (remove content)
            inline void clear() noexcept
            {
                m_pVertexCoords->clear();
                m_pVertexColors->clear();
                if (m_pTextureInfos != nullptr)
                    m_pTextureInfos->clear();
            }

            /// @brief Add non-textured elements at the end of the buffer
            /// @param[in] pCoords  Vertex coordinates to insert
            /// @param[in] color    Vertex color to insert
            inline void push(const vertex_pos_t& coords, const vertex_color_t color)
            {
                m_pVertexCoords->push_back(coords.x);
                m_pVertexCoords->push_back(coords.y);
                m_pVertexCoords->push_back(coords.z);
                m_pVertexColors->push_back(color);
            }

            /// @brief Add non-textured elements at the end of the buffer
            /// @param[in] pCoords  Vertex coordinates to insert
            /// @param[in] color    Vertex color to insert
            /// @param[in] tex      Texture information to insert
            inline void push(const vertex_pos_t& coords, const vertex_color_t& color, const vertex_texture_t& tex)
            {
                push(coords, color);
                if (m_pTextureInfos != nullptr)
                    m_pTextureInfos->push_back(tex);
            }

            /// @brief Extract simple vertices as arrays (coordinates, colors)
            /// @param[out] pOutCoords  Destination for array of vertex coordinates
            /// @param[out] pOutColors  Destination for array of vertex colors
            inline size_t read(float*& pOutCoords, uint32_t*& pOutColors) noexcept
            {
                if (m_pVertexColors->size() > 0)
                {
                    pOutCoords = &((*m_pVertexCoords)[0]);
                    pOutColors = &((*m_pVertexColors)[0]);

                    return m_pVertexColors->size();
                }
                return 0;
            }

            /// @brief Extract textured vertices as arrays (coordinates, colors, texture information)
            /// @param[out] pOutCoords    Destination for array of vertex coordinates
            /// @param[out] pOutColors    Destination for array of vertex colors
            /// @param[out] pOutTextures  Destination for array of vertex texture information
            inline size_t read(float*& pOutCoords, uint32_t*& pOutColors, uint32_t*& pOutTextures) noexcept
            {
                if (m_pVertexColors->size() > 0)
                {
                    pOutCoords = &((*m_pVertexCoords)[0]);
                    pOutColors = &((*m_pVertexColors)[0]);
                    if (m_pTextureInfos != nullptr)
                        pOutTextures = &((*m_pTextureInfos)[0]);
                    else
                        pOutTextures = nullptr;

                    return m_pVertexColors->size();
                }
                return 0;
            }
        };
    }
}
