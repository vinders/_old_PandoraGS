/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : multi-flag register
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.memory
    /// Memory management utilities
    namespace memory
    {
        /// @class FlagSet
        /// @brief Multi-flag register (template : integer type, default value)
        template <typename U = uint32_t, U emptyData = 0u>
        class FlagSet
        {
        public:
            /// @brief Create empty instance
            FlagSet() : m_flags(emptyData) {}
            /// @brief Copy instance
            /// @param[in] other  Other instance to copy
            explicit FlagSet(const FlagSet& other) : m_flags(other.data()) {}
            /// @brief Move instance
            /// @param[in] other  Other instance to move
            explicit FlagSet(FlagSet&& other) : m_flags(other.data()) {}
            /// @brief Create initialized instance (value)
            /// @param[in] value  Register value
            FlagSet(U value) : m_flags(value) {}
            /// @brief Create initialized instance (list)
            /// @param[in] flags  List of flags to enable
            explicit FlagSet(std::initializer_list<U> flags) : m_flags(emptyData)
            {
                for (U flag : flags)
                {
                    setFlag(flag);
                }
            }
            
            
            // -- Utilities --
            
            /// @brief Swap 2 instances 
            /// @param[in] other  Other instance
            void swap(FlagSet& other) noexcept
            {
                m_flags ^= other.m_flags;
                other.m_flags ^= m_flags;
                m_flags ^= other.m_flags;
            }
            
            /// @brief Get flag set capacity
            /// @returns Max number of flags
            inline size_t size() const noexcept
            {
                return (static_cast<size_t>(8u) * sizeof(m_flags));
            }
            /// @brief Get number of active flags
            /// @returns Number of active flags
            inline uint32_t count() const noexcept
            {
                uint32_t activeFlagsNumber = 0;
                if (isEmpty() == false)
                {
                    uint32_t containerSize = static_cast<uint32_t>(size());
                    for (uint32_t i = 0; i < containerSize; ++i)
                    {
                        if (m_flags & (1 << i))
                            ++activeFlagsNumber;
                    }
                }
                return activeFlagsNumber;
            }
            
            /// @brief Compare 2 instances (equality)
            /// @param[in] other  Other instance
            /// @returns Equal (true) or not (false)
            inline bool operator==(const FlagSet& other) const noexcept
            {
                return (m_flags == other.data());
            }
            /// @brief Compare 2 instances (difference)
            /// @param[in] other  Other instance
            /// @returns Different (true) or not (false)
            inline bool operator!=(const FlagSet& other) const noexcept
            {
                return (m_flags != other.data());
            }
            
            
            // -- Setters --
            
            /// @brief Copy assignment
            /// @param[in] other  Instance to copy
            /// @returns Reference to instance
            inline FlagSet& operator=(const FlagSet& other) noexcept
            {
                m_flags = other.data();
            }
            /// @brief Move assignment
            /// @param[in] other  Instance to move
            /// @returns Reference to instance
            inline FlagSet& operator=(FlagSet&& other)
            {
                m_flags = other.data();
            }

            /// @brief Remove all flags
            inline void clear() noexcept
            {
                m_flags = emptyData;
            }
            /// @brief Assign instance value
            /// @param[in] value  Register value
            /// @returns Reference to instance
            inline FlagSet& operator=(const U value) noexcept
            {
                m_flags = value;
                return *this;
            }
            /// @brief Activate flag
            /// @param[in] flag  Flag(s) value(s) to enable
            inline void setFlag(const U flag) noexcept
            {
                m_flags |= flag;
            }
            /// @brief Remove flag
            /// @param[in] flag  Flag(s) value(s) to disable
            inline void unsetFlag(const U flag) noexcept
            {
                m_flags &= ~flag;
            }
            
            
            // -- Getters --
            
            /// @brief Check if flag set is empty
            /// @returns Empty register (true) or flags enabled (false)
            inline bool isEmpty() const noexcept
            {
                return (m_flags == emptyData);
            }
            /// @brief Get full register value
            /// @returns All flags
            inline U data() const noexcept
            {
                return m_flags;
            }
            /// @brief Extract register value for specified flag(s)
            /// @param[in] flags  Flags to extract
            /// @returns All values for specified flags
            inline U at(const U flags) const noexcept
            {
                return (m_flags & flags);
            }
            /// @brief Check if all specified flags are active
            /// @param[in] flags  Flags to check
            /// @returns All active (true) or not (false)
            inline bool isFullFlag(const U flag) const noexcept
            {
                return ((m_flags & flag) == flag);
            }
            /// @brief Check if any of specified flags is active
            /// @param[in] flags  Flags to check
            /// @returns At least one active (true) or none (false)
            inline bool isAnyFlag(const U flags) const noexcept
            {
                return ((m_flags & flag) != emptyData);
            }
            

        private:
            U m_flags;  ///< Flag container (bit-map)
        };
    }
}
