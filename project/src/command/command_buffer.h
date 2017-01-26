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
#include <thread>
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
        FrameBufferSettings m_drawSettings;
        memory::VertexBuffer* m_pLineBuffer;
        memory::VertexBuffer* m_pPolyBuffer;
        memory::VertexBuffer* m_pTxPolyBuffer;
        uint32_t m_currentPrimitiveCount;

        bool m_isBusy;


    private:
        void lock()
        {
            while (m_isBusy)
                std::this_thread::yield();
            m_isBusy = true;
        }

        void release()
        {
            m_isBusy = false;
        }

    public:
        CommandBuffer()
        {
            // PS1 GPU limits :
            //  - theoretically 360000 flat-shaded polygons per second -> 12000/frame
            //  - theoretically 180000 textured/gouraud-shaded polygons per second -> 6000/frame
            // lignes:    7 = { X, Y, z, R, G, B, stp }
            m_pLineBuffer = new memory::VertexBuffer(9000 * 2, false);
            // triangles: 7 = { X, Y, z, R, G, B, stp }
            m_pPolyBuffer = new memory::VertexBuffer(9000 * 3, false);
            // triangles: 9 = { X, Y, z, R, G, B, stp, U, V }
            m_pTxPolyBuffer = new memory::VertexBuffer(6000 * 3, false);
        }
    };
}
