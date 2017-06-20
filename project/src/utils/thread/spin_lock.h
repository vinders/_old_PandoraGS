/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : spin-lock (single lock) : very fast mutex (if no wait -> only use if rare concurrency)
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <thread>
#include <atomic>
#include <chrono>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.thread
    /// Thread management utilities
    namespace thread
    {
        /// @class SpinLock
        /// @brief Spin-lock mutex
        class SpinLock
        {
        public:
            /// @brief Create unlocked instance
            SpinLock() : m_status(ATOMIC_FLAG_INIT) {}
            /// @brief Move instance
            /// @param[in] other  Other instance to move
            explicit SpinLock(SpinLock&& other) : m_status(other.m_status) {}
            // no copy allowed
            SpinLock(const SpinLock& other) = delete;
            
            
            // -- Utilities --
            
            /// @brief Compare 2 instances (equality)
            /// @param[in] other  Other instance
            /// @returns Equal (true) or not (false)
            inline bool operator==(const SpinLock& other) const noexcept
            {
                return (m_status == other.m_status);
            }
            /// @brief Compare 2 instances (difference)
            /// @param[in] other  Other instance
            /// @returns Different (true) or not (false)
            inline bool operator!=(const SpinLock& other) const noexcept
            {
                return (m_status != other.m_status);
            }
            
            
            // -- Lock management --
            
            /// @brief Wait until instance is unlocked, then lock it
            inline void lock() noexcept
            {
                while (m_status.test_and_set(std::memory_order_acquire))
                {
                    std::this_thread::yield();
                }
            }
            /// @brief Wait until instance unlocked or timeout
            /// @param[in] timeout  Timeout (milliseconds)
            /// @returns Success (true) or timeout
            inline bool lock(uint32_t timeout) noexcept
            {
                if (m_status.test_and_set(std::memory_order_acquire)) // if already locked
                {
                    // set timeout reference
                    std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
                    std::chrono::time_point<std::chrono::system_clock> timeoutTime = currentTime + std::chrono::milliseconds(timeout);
                    
                    while (m_status.test_and_set(std::memory_order_acquire))
                    {
                        currentTime = std::chrono::system_clock::now();
                        if (currentTime > timeoutTime) // check for timeout
                            return false;
                        
                        std::this_thread::yield();
                    }
                }
                return true;
            }
            /// @brief Try to lock, if not already locked
            /// @returns Success (true) or not
            inline bool tryLock() noexcept
            {
                return !m_status.test_and_set(std::memory_order_acquire); // return opposite of previous value
            }
            /// @brief Unlock instance
            inline void unlock() noexcept
            {
                m_status.clear(std::memory_order_release);
            }
            

        private:
            std::atomic_flag m_status;  ///< Lock status (true = locked)
        };
    }
}
