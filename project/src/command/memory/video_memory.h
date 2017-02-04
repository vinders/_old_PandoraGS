/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : GPU video memory (vram) image
*******************************************************************************/
#pragma once

#include <cstdlib>
#include <cstdint>
#include <stdexcept>

/// @namespace command
/// GPU commands management
namespace command
{
    /// @namespace command.primitive
    /// GPU memory management
    namespace memory
    {
        /// @class VideoMemory
        /// @brief GPU video memory (vram) image
        class VideoMemory
        {
        private:
            // memory image
            uint8_t*  m_pVramImage;    ///< Allocated memory image
            bool      m_isDoubledSize; ///< Memory image contains two buffers (Zinc) or one
            size_t    m_bufferSize;    ///< Vram buffer size (single buffer)
            size_t    m_totalSize;     ///< Total allocated memory (buffers + offsets)

            // memory access
            uint8_t*  m_pBuffer8;  ///< Buffer origin (8-bit mode)
            uint16_t* m_pBuffer16; ///< Buffer origin (16-bit mode)
            uint32_t* m_pBuffer32; ///< Buffer origin (32-bit mode)
            uint16_t* m_pEnd;      ///< End of last buffer

            
        public:
            /// @class VideoMemory::iterator
            /// @brief GPU video memory (vram) iterator
            class iterator
            {
            private:
                const VideoMemory& it_target; ///< Memory target
                uint16_t* it_pCurrentPos;     ///< Iterator position (16-bit block access mode)

            public:
                /// @brief Create iterator
                /// @param[in] target  Memory target
                iterator(const VideoMemory& target) noexcept : it_target(target), it_pCurrentPos(target.m_pBuffer16) {}
                /// @brief Copy constructor
                /// @param[in] copy  Iterator to copy
                iterator(const VideoMemory::iterator& copy) noexcept : it_target(copy.it_target), it_pCurrentPos(copy.it_pCurrentPos) {}
                // Getters / setters
                inline uint16_t* getPos() const noexcept;     ///< Current position
                inline uint16_t getValue() const;             ///< Value at current position
                inline void iterator::setValue(uint16_t val); ///< Define value at current position
                // Change iterator position
                inline VideoMemory::iterator& operator++() noexcept;
                inline VideoMemory::iterator operator++(int) noexcept;
                inline VideoMemory::iterator& operator--() noexcept;
                inline VideoMemory::iterator operator--(int) noexcept;
                inline VideoMemory::iterator operator+(const int off) noexcept;
                inline VideoMemory::iterator& operator+=(const int off) noexcept;
                inline VideoMemory::iterator operator-(const int off) noexcept;
                inline VideoMemory::iterator& operator-=(const int off) noexcept;
            };


        public:
            // -- memory allocation -- -------------------------------------------------

            /// @brief Create uninitialized memory image
            VideoMemory() noexcept;
            /// @brief Destroy memory image
            ~VideoMemory();

            /// @brief Memory allocation and initialization
            /// @param[in] isDoubledSize  Use doubled buffer size (for Zinc)
            /// @throws runtime_error  Memory allocation failure
            void init(const bool isDoubledSize = false);

            /// @brief Destroy memory image
            void close();


            // -- memory iteration -- --------------------------------------------------

            /// @brief Create iterator at the beginning of the effective memory zone
            /// @returns New iterator
            /// @throws logic_error  Uninitialized memory
            inline VideoMemory::iterator begin() const
            {
                if (m_pVramImage == nullptr)
                    throw std::logic_error("VideoMemory.begin: can't iterate through uninitialized memory");
                return VideoMemory::iterator(*this);
            }
            /// @brief Get pointer after the end of the last buffer
            /// @returns End of memory
            inline uint16_t* end() const noexcept
            {
                return m_pEnd;
            }
            /// @brief Get pointer to the beginning of the first buffer
            /// @returns Start of memory
            inline uint16_t* rend() const noexcept
            {
                return m_pBuffer16;
            }
            /// @brief Get pointer to the beginning of the first buffer
            /// @returns Start of memory
            inline uint8_t* get() const noexcept
            {
                return m_pBuffer8;
            }
            /// @brief Get the size of a single memory buffer
            /// @returns New iterator
            inline size_t size() const noexcept
            {
                return m_bufferSize;
            }
            /// @brief Check if doubled size is used
            /// @returns Size doubled or not
            inline bool isDoubledSize() const noexcept
            {
                return m_isDoubledSize;
            }
        };
    }
}
#include "video_memory_iterator.hpp"
