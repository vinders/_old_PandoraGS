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
#include "../../event/utils/std_exception.hpp"


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
                VideoMemory* it_pTarget;  ///< Memory target
                uint16_t* it_pCurrentPos; ///< Iterator position (16-bit block access mode)

            public:
                /// @brief Create uninitialized iterator
                iterator() : it_pTarget(NULL), it_pCurrentPos(NULL) {}
                /// @brief Create iterator
                /// @param target Memory target
                iterator(VideoMemory& target) : it_pTarget(&target), it_pCurrentPos(target.m_pBuffer16) {}
                /// @brief Copy constructor
                /// @param copy Iterator to copy
                iterator(const VideoMemory::iterator& copy) : it_pTarget(copy.it_pTarget), it_pCurrentPos(copy.it_pCurrentPos) {}
                // Getters / setters
                inline uint16_t* getPos();  ///< Current position
                inline uint16_t getValue(); ///< Value at current position
                inline void iterator::setValue(uint16_t val); ///< Define value at current position
                // Change iterator position
                inline VideoMemory::iterator& operator++();
                inline VideoMemory::iterator operator++(int);
                inline VideoMemory::iterator& operator--();
                inline VideoMemory::iterator operator--(int);
                inline VideoMemory::iterator operator+(const int off);
                inline VideoMemory::iterator& operator+=(const int off);
                inline VideoMemory::iterator operator-(const int off);
                inline VideoMemory::iterator& operator-=(const int off);
            };


        public:
            // -- memory allocation -- -------------------------------------------------

            /// @brief Create uninitialized memory image
            VideoMemory();
            /// @brief Destroy memory image
            ~VideoMemory();

            /// @brief Memory allocation and initialization
            /// @param isDoubledSize Use doubled buffer size (for Zinc)
            /// @throw Memory allocation failure
            void init(bool isDoubledSize = false);

            /// @brief Destroy memory image
            void close();


            // -- memory iteration -- --------------------------------------------------

            /// @brief Create iterator at the beginning of the effective memory zone
            /// @return New iterator
            /// @throw Uninitialized memory
            inline VideoMemory::iterator begin()
            {
                if (m_pVramImage == NULL)
                    throw event::utils::StdException("VideoMemory.begin: can't iterate through uninitialized memory");
                return VideoMemory::iterator(*this);
            }
            /// @brief Get pointer after the end of the last buffer
            /// @return End of memory
            inline uint16_t* end()
            {
                return m_pEnd;
            }
            /// @brief Get pointer to the beginning of the first buffer
            /// @return Start of memory
            inline uint16_t* rend()
            {
                return m_pBuffer16;
            }
            /// @brief Get pointer to the beginning of the first buffer
            /// @return Start of memory
            inline uint8_t* get()
            {
                return m_pBuffer8;
            }
            /// @brief Get the size of a single memory buffer
            /// @return New iterator
            inline size_t size()
            {
                return m_bufferSize;
            }
            /// @brief Check if doubled size is used
            /// @return Size doubled or not
            inline bool isDoubledSize()
            {
                return m_isDoubledSize;
            }
        };
    }
}
#include "video_memory_iterator.hpp"
