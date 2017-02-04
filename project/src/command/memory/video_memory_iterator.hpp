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
/// @returns Pointer data
inline uint16_t* command::memory::VideoMemory::iterator::getPos() const noexcept
{
    return it_pCurrentPos;
}
/// @brief Get value at current position
/// @returns Pointed value
inline uint16_t command::memory::VideoMemory::iterator::getValue() const
{
    return *it_pCurrentPos;
}
/// @brief Set value at current position
/// @param[in] val  Value
inline void command::memory::VideoMemory::iterator::setValue(uint16_t val)
{
    *it_pCurrentPos = val;
}
/// @brief Pre-increment
/// @returns Current iterator
inline command::memory::VideoMemory::iterator& command::memory::VideoMemory::iterator::operator++() noexcept
{
    ++(this->it_pCurrentPos);
    return *this;
}
/// @brief Post-increment (slower)
/// @returns Copy with old value
inline command::memory::VideoMemory::iterator command::memory::VideoMemory::iterator::operator++(int) noexcept
{
    VideoMemory::iterator copy(*this);
    ++(this->it_pCurrentPos);
    return copy;
}
/// @brief Pre-decrement
/// @returns Current iterator
inline command::memory::VideoMemory::iterator& command::memory::VideoMemory::iterator::operator--() noexcept
{
    --(this->it_pCurrentPos);
    return *this;
}
/// @brief Post-decrement (slower)
/// @returns Copy with old value
inline command::memory::VideoMemory::iterator command::memory::VideoMemory::iterator::operator--(int) noexcept
{
    VideoMemory::iterator copy(*this);
    --(this->it_pCurrentPos);
    return copy;
}
/// @brief Add offset
/// @param[in] off  Offset
/// @returns Copy with new value
inline command::memory::VideoMemory::iterator command::memory::VideoMemory::iterator::operator+(const int off) noexcept
{
    VideoMemory::iterator copy(this->it_target);
    copy.it_pCurrentPos += off;
    return copy;
}
/// @brief Add offset (faster)
/// @param[in] off  Offset
/// @returns Current iterator
inline command::memory::VideoMemory::iterator& command::memory::VideoMemory::iterator::operator+=(const int off) noexcept
{
    this->it_pCurrentPos += off;
    return *this;
}
/// @brief Remove offset
/// @param[in] off  Offset
/// @returns Copy with new value
inline command::memory::VideoMemory::iterator command::memory::VideoMemory::iterator::operator-(const int off) noexcept
{
    VideoMemory::iterator copy(this->it_target);
    copy.it_pCurrentPos -= off;
    return copy;
}
/// @brief Remove offset (faster)
/// @param[in] off  Offset
/// @returns Current iterator
inline command::memory::VideoMemory::iterator& command::memory::VideoMemory::iterator::operator-=(const int off) noexcept
{
    this->it_pCurrentPos += off;
    return *this;
}
