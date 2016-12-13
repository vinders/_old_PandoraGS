/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   video_memory_iterator.hpp
Description : video memory (vram) image - iterator
*******************************************************************************/
#ifndef _VIDEO_MEMORY_ITERATOR_HPP
#define _VIDEO_MEMORY_ITERATOR_HPP
#ifndef _VIDEO_MEMORY_H
#include "video_memory.h"
#endif

/// <summary>Get current position</summary>
/// <returns>Pointer data</returns>
inline uint16_t* VideoMemory::iterator::getPos()
{
    return it_pWord;
}
/// <summary>Get value at current position</summary>
/// <returns>Pointed value</returns>
inline uint16_t VideoMemory::iterator::getValue()
{
    return *it_pWord;
}
/// <summary>Pre-increment</summary>
/// <returns>Current iterator</returns>
inline VideoMemory::iterator& VideoMemory::iterator::operator++()
{
    ++(this->it_pWord);
    if (this->it_pWord >= it_pTarget->end())
        this->it_pWord -= it_pTarget->size();
    return *this;
}
/// <summary>Post-increment (slower)</summary>
/// <returns>Copy with old value</returns>
inline VideoMemory::iterator VideoMemory::iterator::operator++(int)
{
    VideoMemory::iterator copy(*this);
    ++(this->it_pWord);
    if (this->it_pWord >= it_pTarget->end())
        this->it_pWord -= it_pTarget->size();
    return copy;
}
/// <summary>Pre-decrement</summary>
/// <returns>Current iterator</returns>
inline VideoMemory::iterator& VideoMemory::iterator::operator--()
{
    --(this->it_pWord);
    if (this->it_pWord < it_pTarget->rend())
        this->it_pWord += it_pTarget->size();
    return *this;
}
/// <summary>Post-decrement (slower)</summary>
/// <returns>Copy with old value</returns>
inline VideoMemory::iterator VideoMemory::iterator::operator--(int)
{
    VideoMemory::iterator copy(*this);
    --(this->it_pWord);
    if (this->it_pWord < it_pTarget->rend())
        this->it_pWord += it_pTarget->size();
    return copy;
}
/// <summary>Add offset</summary>
/// <returns>Copy with new value</returns>
inline VideoMemory::iterator VideoMemory::iterator::operator+(const int off)
{
    VideoMemory::iterator copy(*(this->it_pTarget));
    copy.it_pWord += off;
    return copy;
}
/// <summary>Add offset (faster)</summary>
/// <returns>Current iterator</returns>
inline VideoMemory::iterator& VideoMemory::iterator::operator+=(const int off)
{
    this->it_pWord += off;
    return *this;
}
/// <summary>Remove offset</summary>
/// <returns>Copy with new value</returns>
inline VideoMemory::iterator VideoMemory::iterator::operator-(const int off)
{
    VideoMemory::iterator copy(*(this->it_pTarget));
    copy.it_pWord -= off;
    return copy;
}
/// <summary>Remove offset (faster)</summary>
/// <returns>Current iterator</returns>
inline VideoMemory::iterator& VideoMemory::iterator::operator-=(const int off)
{
    this->it_pWord += off;
    return *this;
}

#endif
