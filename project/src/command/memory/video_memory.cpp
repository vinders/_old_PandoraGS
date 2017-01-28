/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : GPU video memory (vram) image
*******************************************************************************/
#include <cstdlib>
#include <cstdint>
#include "../../event/utils/std_exception.hpp"
#include "video_memory.h"
using namespace command::memory;

#define VRAM_BUFFER_SIZE 512 // VRAM buffer size (kilobytes)
#define VRAM_OFFSET_SIZE VRAM_BUFFER_SIZE // security offset (for drawing functions security)


/// @brief Create uninitialized memory image
VideoMemory::VideoMemory()
{
    m_pVramImage = NULL;
    m_bufferSize = VRAM_BUFFER_SIZE * 1024;
}

/// @brief Destroy memory image
VideoMemory::~VideoMemory()
{
    close();
}

/// @brief Memory allocation and initialization
/// @param isDoubledSize Use doubled buffer size (for Zinc)
/// @throw Memory allocation failure
void VideoMemory::init(bool isDoubledSize)
{
    // allocate VRAM image
    if (m_pVramImage == NULL)
    {
        m_isDoubledSize = isDoubledSize;
        m_bufferSize = (isDoubledSize) ? (VRAM_BUFFER_SIZE * 2 * 1024) : (VRAM_BUFFER_SIZE * 1024);
        m_totalSize = (m_bufferSize * 2) + (VRAM_OFFSET_SIZE * 2 * 1024); // 2 buffers + security offset
        if ((m_pVramImage = (uint8_t*)malloc(m_totalSize)) == NULL)
            throw event::utils::StdException("VideoMemory.init: VRAM allocation failure");
    }

    // initialize VRAM
    memset(m_pVramImage, 0x0, m_totalSize);
    m_pBuffer8 = m_pVramImage + (VRAM_OFFSET_SIZE * 1024); // position of first buffer
    m_pBuffer16 = (uint16_t*)m_pBuffer8;
    m_pBuffer32 = (uint32_t*)m_pBuffer8;
    m_pEnd = m_pBuffer16 + m_bufferSize; // end of last buffer (16-bit mode: adds 2 bytes at once)
}

/// @brief Destroy memory image
void VideoMemory::close()
{
    if (m_pVramImage != NULL)
    {
        free(m_pVramImage);
        m_pVramImage = NULL;
    }
}
