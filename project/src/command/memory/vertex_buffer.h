/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : vertex data buffer
*******************************************************************************/
#pragma once

#include <cstdint>
#include <vector>
#include "../primitive/i_primitive.h"

/// @namespace command
/// GPU commands management
namespace command
{
    /// @namespace command.primitive
    /// GPU memory management
    namespace memory
    {
        /// @struct vertex_info_t
        /// @brief Vertex data
        typedef struct
        {
            float pCoords[3];                  ///< Vertex coordinates : X, Y, Z
            primitive::rgb24_t color;          ///< RGB color data
            primitive::stp_t semiTransparency; ///< Semi-transparency mode
        } vertex_info_t;

        /// @struct vertex_tx_info_t
        /// @brief Textured vertex data
        typedef struct
        {
            float pCoords[3];                  ///< Vertex coordinates : X, Y, Z
            primitive::rgb24_t color;          ///< RGB color data
            primitive::stp_t semiTransparency; ///< Semi-transparency mode
            uint32_t textureId;                ///< Texture identifier
            short pTextureCoords[3];           ///< Texture coordinates : U, V, upscaling factor
        } vertex_tx_info_t;


        /// @class VertexBuffer
        /// @brief Vertex data buffer
        class VertexBuffer
        {
        private:
            std::vector<float>* m_pVertexCoords;  ///< Vertex coordinates : X, Y, Z
            std::vector<short>* m_pVertexColors;  ///< Vertex colors : R, G, B, transparency-mode
            std::vector<short>* m_pTextureCoords; ///< Vertex texture information : texture-ID, U, V, upscaling

        public:
            /// @brief Create and configure vertex buffer
            /// @param baseCapacity Base capacity (pre-allocated number of vertices)
            /// @param isTextured Use texture data
            VertexBuffer(uint32_t baseCapacity, bool isTextured)
            {
                // coords
                m_pVertexCoords = new std::vector<float>();
                m_pVertexCoords->reserve(baseCapacity * 3);

                // colors
                m_pVertexColors = new std::vector<short>();
                m_pVertexColors->reserve(baseCapacity << 2);

                // texture coords
                if (isTextured)
                {
                    m_pTextureCoords = new std::vector<short>();
                    m_pTextureCoords->reserve(baseCapacity << 2);
                }
                else
                    m_pTextureCoords = NULL;
            }

            /// @brief Destroy vertex buffer
            ~VertexBuffer()
            {
                delete m_pVertexCoords;
                delete m_pVertexColors;
                if (m_pTextureCoords != NULL)
                    delete m_pTextureCoords;
            }

            /// @brief Clear vertex buffer (remove content)
            void clear()
            {
                m_pVertexCoords->clear();
                m_pVertexColors->clear();
                if (m_pTextureCoords != NULL)
                    m_pTextureCoords->clear();
            }

            /// @brief Add simple elements at the end of the buffer
            /// @param pVertices Array of vertices to insert
            /// @param length Array length
            void push(vertex_info_t* pVertices, size_t length)
            {
                //...
            }

            /// @brief Add textured elements at the end of the buffer
            /// @param pVertices Array of textured vertices to insert
            /// @param length Array length
            void push(vertex_tx_info_t* vertices, size_t length)
            {
                //...
                if (m_pTextureCoords != NULL)
                {

                }
            }

            /// @brief Extract simple vertices as arrays (coordinates, colors)
            /// @param ppOutCoords Destination for array of vertex coordinates
            /// @param ppOutColors Destination for array of vertex colors
            size_t read(float** ppOutCoords, short** ppOutColors)
            {
                *ppOutCoords = &((*m_pVertexCoords)[0]);
                *ppOutColors = &((*m_pVertexColors)[0]);
            }

            /// @brief Extract textured vertices as arrays (coordinates, colors, texture information)
            /// @param ppOutCoords Destination for array of vertex coordinates
            /// @param ppOutColors Destination for array of vertex colors
            /// @param ppOutColors Destination for array of vertex texture information
            size_t read(float** ppOutCoords, short** ppOutColors, short** ppOutTextures)
            {
                *ppOutCoords = &((*m_pVertexCoords)[0]);
                *ppOutColors = &((*m_pVertexColors)[0]);
                if (m_pTextureCoords != NULL)
                    *ppOutTextures = &((*m_pTextureCoords)[0]);
                else
                    *ppOutTextures = NULL;
            }
        };
    }
}
