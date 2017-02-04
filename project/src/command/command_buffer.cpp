/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : fifo GPU command buffer
*******************************************************************************/
#include "../globals.h"
#include <cstdint>
#include <thread>
#include "command_buffer.h"
using namespace command;


/// @brief Set "busy" status
void CommandBuffer::lock() noexcept
{
    while (m_isBusy)
        std::this_thread::yield();
    m_isBusy = true;
}

/// @brief Set "available" status
void CommandBuffer::unlock() noexcept
{
    m_isBusy = false;
}
