/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : double-buffered virtual memory image
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include "virtual_memory.h"

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.memory
    /// Memory management utilities
    namespace memory
    {
        /// @class DoubleBufferedMemory
        /// @brief Double-buffered virtual memory image
        /// @see VirtualMemory
        class DoubleBufferedMemory : public VirtualMemory
        {
        public:
            /// @brief Create unallocated memory
            DoubleBufferedMemory() noexcept : VirtualMemory(), m_pBeginSecondBuffer(nullptr) {}
            /// @brief Create double-buffered virtual memory
            /// @param[in] bufferSize  Size of a single buffer
            /// @param[in] offsetSize  Offset before and after usable memory (extra security)
            DoubleBufferedMemory(const size_t bufferSize, const size_t offsetSize) : 
                VirtualMemory((bufferSize << 1), offsetSize), m_pBeginSecondBuffer(nullptr) 
            {
                if (m_memorySize > 0u)
                    m_pBeginSecondBuffer = m_pBegin + bufferSize;
            }
            /// @brief Copy double-buffered virtual memory
            /// @param[in] other  Other instance
            DoubleBufferedMemory(const DoubleBufferedMemory& other) :
                VirtualMemory(other), m_pBeginSecondBuffer(other.m_pBeginSecondBuffer) {}
            /// @brief Move double-buffered virtual memory
            /// @param[in] other  Other instance
            DoubleBufferedMemory(DoubleBufferedMemory&& other) :
                VirtualMemory(other), m_pBeginSecondBuffer(other.m_pBeginSecondBuffer) 
            {
                other.m_pBeginSecondBuffer = nullptr;
            }
            
            
            // -- Getters --
            
            /// @brief Get virtual memory buffer size
            /// @returns Single buffer size
            inline size_t bufferSize() const noexcept
            {
                return (m_memorySize >> 1);
            }
            /// @brief Get direct access to first buffer
            /// @returns Pointer at beginning of first buffer
            inline uint8_t* front() const noexcept
            {
                return m_pBegin;
            }
            /// @brief Get direct access to second buffer
            /// @returns Pointer at beginning of second buffer
            inline uint8_t* back() const noexcept
            {
                return m_pBeginSecondBuffer;
            }
            
            
            // -- Setters --
            
            /// @brief Initialize double-buffered virtual memory image (+ set to zero)
            /// @param[in] bufferSize  Size of a single buffer
            /// @param[in] offsetSize  Offset before and after usable memory (extra security)
            /// @throws runtime_error  Memory allocation failure
            void init(const size_t bufferSize, const size_t offsetSize)
            {
                m_pBeginSecondBuffer = nullptr;
                VirtualMemory::init((bufferSize << 1), offsetSize);
                if (m_memorySize > 0u)
                    m_pBeginSecondBuffer = m_pBegin + bufferSize;
            }
            
            /// @brief Set first buffer to zero
            inline void clearFront() noexcept
            {
                if (m_memorySize > 0u)
                    memset(m_pBegin, 0x0, bufferSize());
            }
            /// @brief Set second buffer to zero
            inline void clearBack() noexcept
            {
                if (m_memorySize > 0u)
                    memset(m_pBeginSecondBuffer, 0x0, bufferSize());
            }
            
            /// @brief Copy assignment
            /// @param[in] other  Instance to copy
            /// @returns Reference to instance
            inline DoubleBufferedMemory& operator=(const DoubleBufferedMemory& other)
            {
                VirtualMemory::operator=(other);
                m_pBeginSecondBuffer = other.m_pBeginSecondBuffer;
                return *this;
            }
            /// @brief Move assignment
            /// @param[in] other  Instance to move
            /// @returns Reference to instance
            inline DoubleBufferedMemory& operator=(DoubleBufferedMemory&& other)
            {
                VirtualMemory::operator=(other);
                m_pBeginSecondBuffer = other.m_pBeginSecondBuffer;
                other.m_pBeginSecondBuffer = nullptr;
                return *this;
            }
            
            
            // -- Iteration --

            /// @brief Create iterator at beginning of first buffer
            /// @returns Template (byte/16-bit/32-bit) iterator
            /// @throws logic_error  Unallocated memory
            template <typename T = uint8_t>
            inline VirtualMemory::iterator<T> beginFront() const
            {
                return VirtualMemory::iterator<T>(*this, reinterpret_cast<T*>(m_pBegin));
            }
            /// @brief Create iterator at beginning of second buffer
            /// @returns Template (byte/16-bit/32-bit) iterator
            /// @throws logic_error  Unallocated memory
            template <typename T = uint8_t>
            inline VirtualMemory::iterator<T> beginBack() const
            {
                return VirtualMemory::iterator<T>(*this, reinterpret_cast<T*>(m_pBeginSecondBuffer));
            }
            
            /// @brief Create iterator at last block of first buffer
            /// @returns Template (byte/16-bit/32-bit) iterator
            /// @throws logic_error  Unallocated memory
            template <typename T = uint8_t>
            inline VirtualMemory::iterator<T> rbeginFront() const
            {
                size_t multipleSize = (bufferSize() / sizeof(T));
                if (multipleSize > 0u)
                {
                    multipleSize *= sizeof(T); // truncate to integer multiplier
                    return VirtualMemory::iterator<T>(*this, reinterpret_cast<T*>(m_pBegin) + (multipleSize - 1));
                }
                else
                    return beginFront<T>();
            }
            /// @brief Create iterator at last block of second buffer
            /// @returns Template (byte/16-bit/32-bit) iterator
            /// @throws logic_error  Unallocated memory
            template <typename T = uint8_t>
            inline VirtualMemory::iterator<T> rbeginBack() const
            {
                size_t multipleSize = (m_memorySize / sizeof(T));
                if (multipleSize > 0u)
                {
                    multipleSize *= sizeof(T); // truncate to integer multiplier
                    return VirtualMemory::iterator<T>(*this, reinterpret_cast<T*>(m_pBegin) + (multipleSize - 1));
                }
                else
                    return beginBack<T>();
            }
            
            /// @brief Get pointer after the end of first buffer
            /// @returns End of first buffer
            template <typename T = uint8_t>
            inline T* endFront() const noexcept
            {
                return reinterpret_cast<T*>(m_pBeginSecondBuffer);
            }
            /// @brief Get pointer after the end of second buffer
            /// @returns End of second buffer (= end of memory)
            template <typename T = uint8_t>
            inline T* endBack() const noexcept
            {
                return reinterpret_cast<T*>(m_pEnd);
            }
            /// @brief Get pointer before the beginning of first buffer
            /// @returns Reversed end of first buffer (= reversed end of memory)
            template <typename T = uint8_t>
            inline T* rendFront() const noexcept
            {
                return reinterpret_cast<T*>(m_pBegin) - 1;
            }
            /// @brief Get pointer before the beginning of second buffer
            /// @returns Reversed end of second buffer
            template <typename T = uint8_t>
            inline T* rendBack() const noexcept
            {
                return reinterpret_cast<T*>(m_pBeginSecondBuffer) - 1;
            }

                
        private:
            uint8_t*  m_pBeginSecondBuffer;   ///< Beginning of second buffer / end of first buffer
        };
    }
}
