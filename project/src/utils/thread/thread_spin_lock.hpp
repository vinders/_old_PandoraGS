/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : spin-lock with multi-lock per thread : very fast mutex (if no wait -> only use if rare concurrency)
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <thread>
#include <atomic>
#include <chrono>
#include <thread>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.thread
    /// Thread management utilities
    namespace thread
    {
        /// @class ThreadSpinLock
        /// @brief Spin-lock mutex (locked once per thread)
        class ThreadSpinLock
        {
        public:
            /// @brief Create unlocked instance
            SpinLock() : m_status(ATOMIC_FLAG_INIT) {}
            /// @brief Move instance
            /// @param[in] other  Other instance to move
            explicit SpinLock(SpinLock&& other) : m_status(other.m_status), m_lockThreadId(other.m_lockThreadId) {}
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
                std::thread::id threadId(std::this_thread::get_id());
                if (threadId == m_lockThreadId) // same thread -> increase lock count
                {
                    ++m_lockCount;
                    return;
                }
                
                while (m_status.test_and_set(std::memory_order_acquire))
                {
                    std::this_thread::yield();
                }
                m_lockThreadId = threadId;
            }
            /// @brief Wait until instance unlocked or timeout
            /// @returns Success (true) or timeout
            inline bool lock(uint32_t timeout) noexcept
            {
                std::thread::id threadId(std::this_thread::get_id());
                if (threadId == m_lockThreadId) // same thread -> increase lock count
                {
                    ++m_lockCount;
                    return true;
                }
                
                if (m_status.test_and_set(std::memory_order_acquire)) // if already locked
                {
                    // set timeout reference
                    std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::steady_clock::now();
                    std::chrono::time_point<std::chrono::steady_clock> timeoutTime = currentTime + std::chrono::milliseconds(timeout);
                    
                    while (m_status.test_and_set(std::memory_order_acquire))
                    {
                        currentTime = std::chrono::steady_clock::now();
                        if (currentTime > timeoutTime) // check for timeout
                            return false;
                        
                        std::this_thread::yield();
                    }
                }
                m_lockThreadId = threadId;
                return true;
            }
            /// @brief Try to lock, if not already locked
            /// @returns Success (true) or not
            inline bool tryLock() noexcept
            {
                std::thread::id threadId(std::this_thread::get_id());
                if (threadId == m_lockThreadId) // same thread -> increase lock count
                {
                    ++m_lockCount;
                    return true;
                }
                
                if (!m_status.test_and_set(std::memory_order_acquire)) // return opposite of previous value
                {
                    m_lockThreadId = threadId;
                    return true;
                }
                return false;
            }
            
            /// @brief Unlock instance if locked by same thread - prefered method if lock/unlock are not called in same function
            /// @returns Success (same thread and currently in locked state) or not
            inline bool safeUnlock() noexcept
            {
                if (std::this_thread::get_id() != m_lockThreadId) // different thread or unlocked
                    return false;
                unlock();
                return true;
            }
            /// @brief Unlock instance without checking thread ID - safe if lock/unlock are called in same function
            inline void unlock() noexcept
            {
                if (m_lockCount == 0u)
                {
                    m_lockThreadId = std::thread::id();
                    m_status.clear(std::memory_order_release);
                }
                else
                {
                    --m_lockCount;
                }
            }
            

        private:
            std::thread::id m_lockThreadId; ///< Locking thread ID
            std::atomic_flag m_status;      ///< Lock status (true = locked)
            uint32_t m_lockCount;           ///< Number of locks made by same thread (if multiple locks, multiple unlocks required)
        };
    }
}
