/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : virtual raw-memory image + iterator
*******************************************************************************/
#pragma once

#include <cstdlib>
#include <cstdint>
#include <cstddef>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.memory
    /// Memory management utilities
    namespace memory
    {
        /// @class VirtualMemory
        /// @brief Virtual raw-memory image
        class VirtualMemory
        {
        public:
            /// @brief Create unallocated virtual memory
            VirtualMemory() : m_pRawMemoryImage(nullptr), m_memorySize(0u), m_offsetSize(0u), m_pBegin(nullptr), m_pEnd(nullptr) {}
            /// @brief Create virtual memory
            /// @param[in] memorySize  Usable memory size
            /// @param[in] offsetSize  Offset before and after usable memory (extra security)
            VirtualMemory(const size_t memorySize, const size_t offsetSize) : 
                m_pRawMemoryImage(nullptr), m_memorySize(0u), m_offsetSize(0u), m_pBegin(nullptr), m_pEnd(nullptr)
            {
                init(memorySize, offsetSize);
            }
            /// @brief Copy virtual memory
            /// @param[in] other  Other instance
            VirtualMemory(const VirtualMemory& other);
            /// @brief Move virtual memory
            /// @param[in] other  Other instance
            VirtualMemory(VirtualMemory&& other);
            
            /// @brief Destroy virtual memory
            ~VirtualMemory()
            {
                init(0u, 0u);
            }

            
            // -- Getters --
            
            /// @brief Get virtual memory size
            /// @returns Usable memory size
            inline size_t size() const noexcept
            {
                return m_memorySize;
            }
            /// @brief Get direct access to memory
            /// @returns Pointer at beginning of usable memory
            inline uint8_t* data() const noexcept
            {
                return m_pBegin;
            }
            
            /// @brief Compare 2 instances (equality)
            /// @param[in] other  Other instance
            /// @returns Equal (true) or not (false)
            inline bool operator==(const VirtualMemory& other) const noexcept
            {
                return (m_memorySize == other.m_memorySize && memcmp(m_pBegin, other.m_pBegin, m_memorySize) == 0);
            }
            /// @brief Compare 2 instances (difference)
            /// @param[in] other  Other instance
            /// @returns Different (true) or not (false)
            inline bool operator!=(const VirtualMemory& other) const noexcept
            {
                return (m_memorySize != other.m_memorySize || memcmp(m_pBegin, other.m_pBegin, m_memorySize) != 0);
            }
            
            
            // -- Setters --
            
            /// @brief Initialize virtual memory image (re-allocate if necessary + set to zero)
            /// @param[in] memorySize  Usable memory size
            /// @param[in] offsetSize  Offset before and after usable memory (extra security)
            /// @throws runtime_error  Memory allocation failure
            void init(const size_t memorySize, const size_t offsetSize);
            
            /// @brief Set usable memory to zero
            inline void clear() noexcept
            {
                if (m_pRawMemoryImage != nullptr)
                    memset(m_pBegin, 0x0, m_memorySize);
            }
            
            /// @brief Copy assignment
            /// @param[in] other  Instance to copy
            /// @returns Reference to instance
            inline VirtualMemory& operator=(const VirtualMemory& other) noexcept;
            /// @brief Move assignment
            /// @param[in] other  Instance to move
            /// @returns Reference to instance
            inline VirtualMemory& operator=(VirtualMemory&& other);

            
            
        public:
            /// @class VirtualMemory.iterator
            /// @brief Virtual raw-memory image iterator
            template <typename T = uint8_t>
            class iterator
            {
            public:
                /// @brief Create iterator
                /// @param[in] memory  Virtual memory image
                /// @param[in] target  Target position in memory
                iterator(const VirtualMemory& memory, const T* pTarget) : it_memory(memory), it_pos(pTarget) 
                {
                    if (memory.size() == 0u)
                        throw std::logic_error("VirtualMemory.iterator: can't iterate through empty/unallocated memory");
                }
                /// @brief Copy iterator
                /// @param[in] other  Iterator to copy
                iterator(const VirtualMemory::iterator<T>& other) noexcept : it_memory(other.it_memory), it_pos(other.it_pos) {}
                
                // -- Getters --
                
                /// @brief Get current position
                inline T* getPosition() noexcept { return it_pos; }
                /// @brief Get current position
                inline T* operator->() noexcept  { return it_pos; }
                /// @brief Get value at current position
                inline T data() const { return *it_pos; }
                /// @brief Get referenced value at current position
                inline T& operator*() { return *it_pos; }
                
                /// @brief Compare 2 iterators (equality)
                inline bool operator==(const VirtualMemory::iterator<T>& other) const noexcept { return (it_pos == other.it_pos); }
                /// @brief Compare 2 iterators (difference)
                inline bool operator!=(const VirtualMemory::iterator<T>& other) const noexcept { return (it_pos != other.it_pos); }
                /// @brief Compare 2 iterators (lower)
                inline bool operator<(const VirtualMemory::iterator<T>& other) const noexcept  { return (it_pos < other.it_pos); }
                /// @brief Compare 2 iterators (lower or equal)
                inline bool operator<=(const VirtualMemory::iterator<T>& other) const noexcept { return (it_pos <= other.it_pos); }
                /// @brief Compare 2 iterators (higher)
                inline bool operator>(const VirtualMemory::iterator<T>& other) const noexcept  { return (it_pos > other.it_pos); }
                /// @brief Compare 2 iterators (higher or equal)
                inline bool operator>=(const VirtualMemory::iterator<T>& other) const noexcept { return (it_pos >= other.it_pos); }
                
                /// @brief Compare iterator and position (equality)
                inline bool operator==(const T* other) const noexcept
                { 
                    if (it_pos >= it_memory.end<T>())
                        return (it_memory.end<T>() == other);
                    if (it_pos <= it_memory.rend<T>())
                        return (it_memory.rend<T>() == other);
                    return (it_pos == other); 
                }
                /// @brief Compare iterator and position (difference)
                inline bool operator!=(const T* other) const noexcept
                { 
                    if (it_pos >= it_memory.end<T>())
                        return (it_memory.end<T>() != other);
                    if (it_pos <= it_memory.rend<T>())
                        return (it_memory.rend<T>() != other);
                    return (it_pos == other); 
                }
                /// @brief Compare iterator and position (lower)
                inline bool operator<(const T* other) const noexcept  { return (it_pos < other); }
                /// @brief Compare iterator and position (lower or equal)
                inline bool operator<=(const T* other) const noexcept { return (it_pos <= other); }
                /// @brief Compare iterator and position (higher)
                inline bool operator>(const T* other) const noexcept  { return (it_pos > other); }
                /// @brief Compare iterator and position (higher or equal)
                inline bool operator>=(const T* other) const noexcept { return (it_pos >= other); }
                
                // -- Setters --
                
                /// @brief Copy assignment
                inline T* operator=(const VirtualMemory::iterator<T>& other) noexcept { it_memory = other.it_memory; it_pos = other.it_pos; return it_pos; }
                /// @brief Value assignment at current position
                inline T operator=(const T value) { *it_pos = value; return value; }
                /// @brief Value assignment at current position
                inline void setData(const T value) { *it_pos = value; }
                
                /// @brief Reset position at beginning
                inline void begin(const T value) noexcept { it_pos = it_memory.rend<T>() + 1 }
                /// @brief Pre-increment position
                inline T* operator++() noexcept    { return ++it_pos; }
                /// @brief Post-increment position
                inline T* operator++(int) noexcept { T copy = it_pos; ++it_pos; return copy; }
                /// @brief Pre-decrement position
                inline T* operator--() noexcept    { return --it_pos; }
                /// @brief Post-decrement position
                inline T* operator--(int) noexcept { T copy = it_pos; --it_pos; return copy; }
                
                /// @brief Calculate new position - positive offset
                inline T* operator+(const int offset) noexcept    { return it_pos + offset; }
                /// @brief Change position - positive offset
                inline void operator+=(const int offset) noexcept { it_pos += offset; }
                /// @brief Calculate new position - negative offset
                inline T* operator-(const int offset) noexcept    { return it_pos - offset; }
                /// @brief Change position - negative offset
                inline void operator-=(const int offset) noexcept { it_pos -= offset; }
                
            private:
                const VirtualMemory& it_memory; ///< Iterated virtual memory image
                T* it_pos;               ///< Iterator position
            };
            
            
            // -- Iteration --

            /// @brief Create iterator at beginning of usable memory zone
            /// @returns Template (byte/16-bit/32-bit) iterator
            /// @throws logic_error  Unallocated memory
            template <typename T = uint8_t>
            inline VirtualMemory::iterator<T> begin() const
            {
                return VirtualMemory::iterator<T>(*this, reinterpret_cast<T*>(m_pBegin));
            }
            /// @brief Create iterator at last block of usable memory zone
            /// @returns Template (byte/16-bit/32-bit) iterator
            /// @throws logic_error  Unallocated memory
            template <typename T = uint8_t>
            inline VirtualMemory::iterator<T> rbegin() const
            {
                size_t multipleSize = (m_memorySize / sizeof(T)) * sizeof(T);
                return VirtualMemory::iterator<T>(*this, reinterpret_cast<T*>(m_pBegin) + (multipleSize - 1));
            }
            
            /// @brief Get pointer after the end of usable memory zone
            /// @returns End of memory
            template <typename T = uint8_t>
            inline T* end() const noexcept
            {
                return reinterpret_cast<T*>(m_pEnd);
            }
            /// @brief Get pointer before the beginning of usable memory zone
            /// @returns Reversed end of memory
            template <typename T = uint8_t>
            inline T* rend() const noexcept
            {
                return reinterpret_cast<T*>(m_pBegin) - 1;
            }
            
            
        private:
            uint8_t*  m_pRawMemoryImage; ///< Raw memory image (usable memory + offsets)    
            size_t    m_offsetSize;      ///< Offset before and after usable memory (extra security)
            
        protected:
            size_t    m_memorySize;      ///< Usable memory size
            uint8_t*  m_pBegin;   ///< Usable memory origin (byte mode)
            uint8_t* m_pEnd;      ///< End of usable memory (byte mode)
        };
    }
}
