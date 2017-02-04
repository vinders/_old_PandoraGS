/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : fifo GPU command buffer
*******************************************************************************/
#pragma once

#include <cstdint>
#include "memory/vertex_buffer.h"
#include "frame_buffer_settings.h"

/// @namespace command
/// GPU commands management
namespace command
{
    /// @class CommandBuffer
    /// @brief FIFO GPU command buffer
    class CommandBuffer
    {
    private:
        FrameBufferSettings m_drawSettings; ///< Frame buffer drawing settings
        memory::VertexBuffer m_lineBuffer;  ///< FIFO buffer for line vertices
        memory::VertexBuffer m_polyBuffer;  ///< FIFO buffer for polygon vertices
        uint32_t m_currentPrimitiveCount;   ///< Total primitive count since last rendering
        bool m_isBusy;


    private:
        /// @brief Set "busy" status
        void lock() noexcept;
        /// @brief Set "available" status
        void unlock() noexcept;

    public:
        /// @brief Create GPU command buffer
        CommandBuffer()
        {
            // PS1 GPU limits :
            //  - max 360000 flat-shaded polygons per second -> 12000/frame
            //  - max 180000 textured/gouraud-shaded polygons per second -> 6000/frame
            m_lineBuffer.init(12000 * 2, false); // pre-allocate buffer for lines
            m_polyBuffer.init(10000 * 3, true);  // pre-allocate buffer for polygons

            //! ne pas vider contenu de frame avant de dessiner -> garder "restes" de la frame précédente -> effets de tornade de certains jeux (crash 3, ff 7, ...)
            //! ajout option pour vider frame avant dessin
        }
    };
}
