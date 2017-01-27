/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : drawing line primitive (line / poly-line)
*******************************************************************************/
#pragma once

#include "primitive_common.h"

/// @namespace command
/// GPU commands management
namespace command
{
    /// @namespace command.primitive
    /// Drawing primitive management
    namespace primitive
    {
        // -- primitive units - lines -- ---------------------------------------

        /// @struct line_f2_t
        /// @brief Flat-shaded line
        typedef struct 
        {
            /// @brief Process primitive
            /// @param pData Raw primitive data pointer
            static void process(cmd_block_t* pData);

            rgb24_t color;       ///< Primitive ID (pad) + line color (RGB)
            vertex_f1_t vertex0; ///< Vertex coordinates
            vertex_f1_t vertex1; ///< Vertex coordinates

            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        } line_f2_t;


        /// @struct line_g2_t
        /// @brief Gouraud-shaded line
        typedef struct 
        {
            /// @brief Process primitive
            /// @param pData Raw primitive data pointer
            static void process(cmd_block_t* pData);

            vertex_g1_t vertex0; ///< Primitive ID (pad) + vertex color/coordinates
            vertex_g1_t vertex1; ///< Vertex color/coordinates

            static inline size_t size() { return 4; } ///< Length (32-bit blocks)
        } line_g2_t;


        // -- primitive units - poly-lines -- ----------------------------------

        /// @struct line_fp_t
        /// @brief Flat-shaded poly-line
        typedef struct 
        {
            /// @brief Process primitive
            /// @param pData Raw primitive data pointer
            static void process(cmd_block_t* pData);

            rgb24_t color;       ///< Primitive ID (pad) + line color (RGB)
            vertex_f1_t vertex0; ///< Vertex coordinates
            vertex_f1_t vertex1; ///< Vertex coordinates
            vertex_f1_t vertex2; ///< Vertex coordinates OR end code (0x55555555)

            static inline size_t maxSize() { return 255; } ///< Maximum length (1 color + up to 254 vertices / end code)
            static inline size_t minSize() { return 3; }   ///< Minimum length (at least 1 color + 2 vertices)
            static inline bool isEndCode(cmd_block_t data) ///< End code verification
            { 
                return ((data & 0xF000F000) == 0x50005000); 
            }
            static inline bool isLineEndable(size_t position) ///< Check if line data block can be the last
            {
                return (position >= minSize());
            }
        } line_fp_t;


        /// @struct line_gp_t
        /// @brief Gouraud-shaded poly-line
        typedef struct 
        {
            /// @brief Process primitive
            /// @param pData Raw primitive data pointer
            static void process(cmd_block_t* pData);

            vertex_g1_t vertex0; ///< Primitive ID (pad) + vertex color/coordinates
            vertex_g1_t vertex1; ///< Vertex color/coordinates
            vertex_g1_t vertex2; ///< Vertex color/coordinates OR end code (0x55555555)

            static inline size_t maxSize() { return 255; } ///< Maximum length (up to 127 vertices/colors + end code)
            static inline size_t minSize() { return 4; }   ///< Minimum length (at least 2 colors + 2 vertices)
            static inline bool isEndCode(cmd_block_t data) ///< End code verification
            {
                return ((data & 0xF000F000) == 0x50005000);
            }
            static inline bool isLineEndable(size_t position) ///< Check if line data block can be the last
            {
                return (position >= minSize() && (position & 0x1) == 0); // N*(color+vertex) -> even number
            }
        } line_gp_t;


        // -- primitive units - iterators -- -----------------------------------

        /// @struct line_fp_iterator
        /// @brief Flat-shaded poly-line iterator
        typedef struct LINE_FP_IT
        {
        private:
            size_t m_count;                ///< Read units count
            cmd_block_t* m_pCurrentVertex; ///< Current vertex reference

        public:
            /// @brief Create iterator for flat poly-line
            /// @param dataSource Reference to poly-line container
            LINE_FP_IT(line_fp_t& dataSource) : m_count(1), m_pCurrentVertex(&(dataSource.vertex0.raw)) {}

            /// @brief Select next element (if available)
            /// @return Success (or end of line)
            inline bool next()
            {
                if (++m_count < line_fp_t::maxSize() 
                 && (m_count < line_fp_t::minSize() || line_fp_t::isEndCode(*m_pCurrentVertex) == false))
                {
                    ++m_pCurrentVertex;
                    return true;
                }
                return false;
            }

            /// @brief Extract current value
            /// @return Reference to current value
            inline vertex_f1_t* read()
            {
                return (vertex_f1_t*)(m_pCurrentVertex);
            }
        } line_fp_iterator;


        /// @struct line_gp_iterator
        /// @brief Gouraud-shaded poly-line iterator
        typedef struct LINE_GP_IT
        {
        private:
            size_t m_count;                ///< Read units count
            cmd_block_t* m_pCurrentVertex; ///< Current vertex reference

        public:
            /// @brief Create iterator for flat poly-line
            /// @param dataSource Reference to poly-line container
            LINE_GP_IT(line_gp_t& dataSource) : m_count(0), m_pCurrentVertex(&(dataSource.vertex0.color.raw)) {}

            /// @brief Select next element (if available)
            /// @return Success (or end of line)
            inline bool next()
            {
                m_count += vertex_g1_t::size();
                if (m_count < line_gp_t::maxSize() 
                 && (m_count < line_gp_t::minSize() || line_gp_t::isEndCode(*m_pCurrentVertex) == false))
                {
                    m_pCurrentVertex += vertex_g1_t::size();
                    return true;
                }
                return false;
            }

            /// @brief Extract current value
            /// @return Reference to current value
            inline vertex_g1_t* read()
            {
                return (vertex_g1_t*)(m_pCurrentVertex);
            }
        } line_gp_iterator;


        /*
        Size Restriction: The maximum distance between two vertices is 1023 horizontally, and 511 vertically.
          Polygons and lines that are exceeding that dimensions are NOT rendered.
          If portions of the polygon/line/rectangle are located outside of the drawing area, then the hardware renders only the portion that is inside of the drawing area.
        */
    }
}
