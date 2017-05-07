/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : virtual raw-memory image + iterator
*******************************************************************************/
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include "virtual_memory.h"
using namespace utils::memory;


/// @brief Copy virtual memory
/// @param[in] other  Other instance
VirtualMemory::VirtualMemory(const VirtualMemory& other) : 
    m_pRawMemoryImage(nullptr), m_memorySize(0u), m_offsetSize(0u), m_pBegin(nullptr), m_pEnd(nullptr)
{
    init(other.m_memorySize, other.m_offsetSize);
    memcpy(m_pBegin, 0x0, other.m_offsetSize);
}

/// @brief Move virtual memory
/// @param[in] other  Other instance
VirtualMemory::VirtualMemory(VirtualMemory&& other) : 
    m_pRawMemoryImage(other.m_pRawMemoryImage), m_memorySize(other.m_memorySize), m_offsetSize(other.m_offsetSize), m_pBegin(other.m_pBegin), m_pEnd(other.m_pEnd)
{
    other.m_pRawMemoryImage = other.m_pBegin = other. m_pEnd = nullptr;
    other.m_memorySize = other.m_offsetSize = 0u;
}


// -- Setters --
            
/// @brief Initialize virtual memory image (re-allocate if necessary + set to zero)
/// @param[in] memorySize  Usable memory size
/// @param[in] offsetSize  Offset before and after usable memory (extra security)
/// @throws runtime_error  Memory allocation failure
void VirtualMemory::init(const size_t memorySize, const size_t offsetSize)
{
    if (memorySize == m_memorySize && offsetSize == m_offsetSize)
    {
        clear(); // same size -> no reallocation
        return;
    }
    
    // remove previous allocation
    if (m_pRawMemoryImage != nullptr)
    {
        free(m_pRawMemoryImage);
        m_pRawMemoryImage = m_pBegin = m_pEnd = nullptr;
    }
    
    m_memorySize = memorySize;
    m_offsetSize = offsetSize;
    if (memorySize > 0u)
    {
        // new allocation
        size_t totalSize = memorySize + (offsetSize << 1);
        m_pRawMemoryImage = (uint8_t*)malloc(totalSize);
        if (m_pRawMemoryImage == nullptr)
        {
            m_memorySize = 0u;
            m_offsetSize = 0u;
            throw std::runtime_error("VirtualMemory.init: allocation failure");
        }
        m_pBegin = m_pRawMemoryImage + offsetSize;
        m_pEnd   = m_pBegin + memorySize;
        
        // clear virtual memory + clear offsets
        memset(m_pRawMemoryImage, 0x0, totalSize);
    }
}

/// @brief Copy assignment
/// @param[in] other  Instance to copy
/// @returns Reference to instance
VirtualMemory& VirtualMemory::operator=(const VirtualMemory& other)
{
    init(other.m_memorySize, other.m_offsetSize);
    memcpy(m_pBegin, 0x0, other.m_offsetSize);
    return *this;
}

/// @brief Move assignment
/// @param[in] other  Instance to move
/// @returns Reference to instance
VirtualMemory& VirtualMemory::operator=(VirtualMemory&& other)
{
    if (m_pRawMemoryImage != nullptr)
        free(m_pRawMemoryImage);
    // move values
    m_pRawMemoryImage = other.m_pRawMemoryImage;
    m_pBegin = other.m_pBegin;
    m_pEnd = other.m_pEnd;
    m_memorySize = other.m_memorySize;
    m_offsetSize = other.m_offsetSize;
    // remove from source
    other.m_pRawMemoryImage = other.m_pBegin = other. m_pEnd = nullptr;
    other.m_memorySize = other.m_offsetSize = 0u;
    return *this;
}
