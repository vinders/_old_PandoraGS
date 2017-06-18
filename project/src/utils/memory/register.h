/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : CPU/memory registers
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include "../assert.h"

#pragma pack(push, 1)

typedef Register<uint8_t>  register8_t;
typedef Register<uint16_t> register16_t;
typedef Register<uint32_t> register32_t;
typedef Register<uint64_t> register64_t;

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.memory
    /// Memory management utilities
    namespace memory
    {
        /// @class Register
        /// @brief CPU/memory register
        template <typename T>
        class Register
        {
        public:
            /// @brief Create empty register
            Register() : m_bits(0) {}
            /// @brief Create initialized register (value)
            /// @param[in] value  Register value
            Register(T value) : m_bits(value) {}
            /// @brief Copy register
            /// @param[in] other  Other instance to copy
            explicit Register(const Register<T>& other) : m_bits(other.data()) {}
            /// @brief Move register
            /// @param[in] other  Other instance to move
            explicit Register(Register<T>&& other) : m_bits(other.data()) {}
            
            
            // -- Utilities --
            
            /// @brief Swap 2 instances 
            /// @param[in] other  Other instance
            void swap(Register<T>& other) noexcept
            {
                m_bits ^= other.m_bits;
                other.m_bits ^= m_bits;
                m_bits ^= other.m_bits;
            }
            /// @brief Get register capacity (bytes)
            /// @returns Number of bytes
            inline const size_t size() const noexcept { return sizeof(T); }
            
            /// @brief Compare 2 instances
            /// @param[in] other  Other instance
            /// @returns Equal (true) or not (false)
            inline bool equals(const Register<T>& other) const noexcept
            {
                return (m_bits == other.data());
            }
            /// @brief Compare 2 instances (equality)
            /// @param[in] other  Other instance
            /// @returns Equal (true) or not (false)
            inline bool operator==(const Register<T>& other) const noexcept
            {
                return (m_bits == other.data());
            }
            /// @brief Compare 2 instances (difference)
            /// @param[in] other  Other instance
            /// @returns Different (true) or not (false)
            inline bool operator!=(const Register<T>& other) const noexcept
            {
                return (m_bits != other.data());
            }
            
            
            // -- Setters --
            
            /// @brief Copy assignment
            /// @param[in] other  Instance to copy
            /// @returns Reference to instance
            inline Register<T>& operator=(const Register<T>& other) noexcept
            {
                m_bits = other.data();
                return *this;
            }
            /// @brief Move assignment
            /// @param[in] other  Instance to move
            /// @returns Reference to instance
            inline Register<T>& operator=(Register<T>&& other)
            {
                m_bits = other.data();
                return *this;
            }

            /// @brief Clear register
            inline void clear() noexcept
            {
                m_bits = 0;
            }
            /// @brief Assign register value
            /// @param[in] value  Register value
            /// @returns Reference to instance
            inline Register<T>& operator=(const T value) noexcept
            {
                m_bits = value;
                return *this;
            }
            /// @brief Set register bit
            /// @param[in] bit    Bit to set (0 - number of bits / 0 - 7 for 8-bit / 0 - 15 for 16-bit / 0 - 31 for 32-bit / 0 - 63 for 64-bit)
            /// @param[in] value  Value to set (0 - 1)
            inline void setBit(const uint32_t bit, const bool value) noexcept
            {
                ASSERT(bit < (8u * sizeof(T)));
                if (value)
                    m_bits |= (1u << bit);
                else
                    m_bits &= ~(1u << bit)
            }
            
            
            // -- Getters --
            
            /// @brief Check if register is empty
            /// @returns Empty register or not
            inline bool isEmpty() const noexcept
            {
                return (m_bits == 0);
            }
            /// @brief Check if all bits are 1
            /// @returns All set to 1 (true) or not (false)
            inline bool isFull() const noexcept
            {
                T empty = 0;
                return (m_bits == (~empty));
            }
            
            /// @brief Get full register value
            /// @returns All bits
            inline T data() const noexcept
            {
                return m_bits;
            }
            /// @brief Extract register value for specified bit
            /// @param[in] bit  Bit to extract (bits / 0 - 7 for 8-bit / 0 - 15 for 16-bit / 0 - 31 for 32-bit / 0 - 63 for 64-bit)
            /// @returns Bit value (0 - 1)
            inline bool getBit(const uint32_t bit) const noexcept
            {
                ASSERT(bit < (8u * sizeof(T)));
                return ((m_bits & (1u << bit)) != 0);
            }
            /// @brief Extract register value for specified byte
            /// @param[in] byte  Byte to extract (bytes / 0 for 8-bit / 0 - 1 for 16-bit / 0 - 3 for 32-bit / 0 - 7 for 64-bit)
            /// @returns Byte value
            inline uint8_t getByte(const uint32_t byte) const noexcept
            {
                ASSERT(byte < sizeof(T));
                return static_cast<uint8_t>((static_cast<uint32_t>(m_bits) >> byte) & 0xFFu);
            }
            /// @brief Extract register value for specified word (2 bytes)
            /// @param[in] word  Word to extract (words / 0 for 8-bit / 0 for 16-bit / 0 - 1 for 32-bit / 0 - 3 for 64-bit)
            /// @returns Word value (2 bytes)
            inline uint16_t getWord(const uint32_t word) const noexcept
            {
                ASSERT(word == 0u || word < (sizeof(T) >> 1));
                return static_cast<uint16_t>((static_cast<uint32_t>(m_bits) >> (word << 1)) & 0xFFFFu);
            }
            /// @brief Extract register value for specified double-word (4 bytes)
            /// @param[in] word  Double-word to extract (dwords / 0 for 8-bit / 0 for 16-bit / 0 for 32-bit / 0 - 1 for 64-bit)
            /// @returns Double-word value (4 bytes)
            inline uint32_t getDword(const uint32_t dword) const noexcept
            {
                ASSERT(dword == 0u || dword < (sizeof(T) >> 2));
                return ((static_cast<uint32_t>(m_bits) >> (dword << 2)) & 0xFFFFFFFFu);
            }
            

        private:
            T m_bits;
        };
    }
}

#pragma pack(pop)
