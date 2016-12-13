/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   video_memory.h
Description : video memory (vram) image
*******************************************************************************/
using namespace std;
#include "video_memory.h"

// -- MEMORY ALLOCATION -- -------------------------------------------------

/// <summary>Initialize null pointers</summary>
VideoMemory::VideoMemory()
{
    m_pVramImage = NULL;
}

/// <summary>Release memory allocations</summary>
VideoMemory::~VideoMemory()
{
    close();
}

/// <summary>Memory allocation and initialization</summary>
/// <param name="isDoubledBufSize">Use doubled buffer size (for Zinc emu)</exception>
/// <exception cref="std::exception">Memory allocation failure</exception>
void VideoMemory::init(bool isDoubledBufSize = false)
{
    if (m_pVramImage != NULL)
        close();

    //...

    // alloc double buffered PSX VRAM image
    m_isDoubledBufSize = isDoubledBufSize;
    m_vramBufferSize = (isDoubledBufSize) ? VRAM_SIZE * 2048 : VRAM_SIZE * 1024;
    m_vramTotalSize = (m_vramBufferSize * 2) + (VRAM_SECURITY_OFFSET * 2048); // extra security for drawing API
    if ((m_pVramImage = (uint8_t*)malloc(m_vramTotalSize)) == NULL)
        throw new std::exception("VideoMemory.init: VRAM allocation failure");

    // initialize VRAM
    memset(m_pVramImage, 0x0, m_vramTotalSize);
    m_pByte = m_pVramImage + (VRAM_SECURITY_OFFSET * 1024); // start limit
    m_pWord = (uint16_t*)m_pByte;
    m_pDword = (uint32_t*)m_pByte;
    m_pEnd = m_pWord + m_vramBufferSize; // end limit

    //...
}

/// <summary>Release memory allocations</summary>
void VideoMemory::close()
{
    if (m_pVramImage != NULL)
    {
        free(m_pVramImage);
        m_pVramImage = NULL;
    }
}


// -- MEMORY ITERATION -- --------------------------------------------------

/// <summary>Create iterator at the beginning of the effective memory zone</summary>
/// <returns>New iterator</returns>
/// <exception cref="std::exception">Uninitialized memory</exception>
VideoMemory::iterator VideoMemory::begin()
{
    if (m_pVramImage == NULL)
        throw std::exception("VideoMemory.begin: can't get iterator from uninitialized memory");
    return VideoMemory::iterator(*this);
}
