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
#include <vector>
#include <thread>
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
        std::vector<float> m_lineBuffer;
        std::vector<float> m_polygonBuffer;
        uint32_t m_currentFramePolygonCount;

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
            m_lineBuffer.reserve(9000 * 2 * 7);
            // triangles: 7 = { X, Y, z, R, G, B, stp }
            // triangles: 9 = { X, Y, z, R, G, B, stp, U, V }
            m_polygonBuffer.reserve(6000 * 3 * 9);

            // !!! peu fiable -> préférer fifo buffer de trois commandes ??? -> vidé au fur et à mesure ???
        }
    };
}
