/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include <memory/memory_pool.h>
#include "./control_status_register.h"

namespace psx { 
  /// @class VideoMemory
  /// @brief PlayStation GPU VRAM (frame buffer, texture pages, palettes...)
  class VideoMemory final {
  public:
    VideoMemory() = default;
    VideoMemory(const VideoMemory&) = delete;
    VideoMemory(VideoMemory&&) = default;
    VideoMemory& operator=(const VideoMemory&) = delete;
    VideoMemory& operator=(VideoMemory&&) = default;
    ~VideoMemory() = default;
    
    static constexpr inline size_t VramWidth() noexcept { return size_t{ 2048u }; }
    static constexpr inline size_t VramHeight() noexcept { return size_t{ 512u }; }
    
    
    
  private:
    pandora::memory::MemoryPool<VramWidth()*VramHeight(), 512u*1024u, 
                                pandora::memory::MemoryPoolAllocation::onHeap, false> _memoryPool;
                                
    //framebuffers: usually 2: display area + drawing area -> horizontal coordinates: 4bit/8bit/16bit/24bit/32bit
    //texture page(s): when using textures
    //texture palette(s): when using 4bit/8bit textures
  };
}
