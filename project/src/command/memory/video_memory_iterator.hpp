/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : GPU video memory (vram) iterator
*******************************************************************************/
#pragma once

#include "video_memory.h"

/// @brief Get current position
/// @return Pointer data
inline uint16_t* command::memory::VideoMemory::iterator::getPos()
{
    return it_pCurrentPos;
}
/// @brief Get value at current position
/// @return Pointed value
inline uint16_t command::memory::VideoMemory::iterator::getValue()
{
    return *it_pCurrentPos;
}
/// @brief Set value at current position
/// <param name="val">Value
inline void command::memory::VideoMemory::iterator::setValue(uint16_t val)
{
    *it_pCurrentPos = val;
}
/// @brief Pre-increment
/// @return Current iterator
inline command::memory::VideoMemory::iterator& command::memory::VideoMemory::iterator::operator++()
{
    ++(this->it_pCurrentPos);
    return *this;
}
/// @brief Post-increment (slower)
/// @return Copy with old value
inline command::memory::VideoMemory::iterator command::memory::VideoMemory::iterator::operator++(int)
{
    VideoMemory::iterator copy(*this);
    ++(this->it_pCurrentPos);
    return copy;
}
/// @brief Pre-decrement
/// @return Current iterator
inline command::memory::VideoMemory::iterator& command::memory::VideoMemory::iterator::operator--()
{
    --(this->it_pCurrentPos);
    return *this;
}
/// @brief Post-decrement (slower)
/// @return Copy with old value
inline command::memory::VideoMemory::iterator command::memory::VideoMemory::iterator::operator--(int)
{
    VideoMemory::iterator copy(*this);
    --(this->it_pCurrentPos);
    return copy;
}
/// @brief Add offset
/// @param off Offset
/// @return Copy with new value
inline command::memory::VideoMemory::iterator command::memory::VideoMemory::iterator::operator+(const int off)
{
    VideoMemory::iterator copy(*(this->it_pTarget));
    copy.it_pCurrentPos += off;
    return copy;
}
/// @brief Add offset (faster)
/// @param off Offset
/// @return Current iterator
inline command::memory::VideoMemory::iterator& command::memory::VideoMemory::iterator::operator+=(const int off)
{
    this->it_pCurrentPos += off;
    return *this;
}
/// @brief Remove offset
/// @param off Offset
/// @return Copy with new value
inline command::memory::VideoMemory::iterator command::memory::VideoMemory::iterator::operator-(const int off)
{
    VideoMemory::iterator copy(*(this->it_pTarget));
    copy.it_pCurrentPos -= off;
    return copy;
}
/// @brief Remove offset (faster)
/// @param off Offset
/// @return Current iterator
inline command::memory::VideoMemory::iterator& command::memory::VideoMemory::iterator::operator-=(const int off)
{
    this->it_pCurrentPos += off;
    return *this;
}
