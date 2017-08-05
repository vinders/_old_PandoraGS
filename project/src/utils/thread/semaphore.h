/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : simple semaphore (only shared between threads, not processes)
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <thread>
#include <mutex>
#include <condition_variable>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.thread
    /// Thread management utilities
    namespace thread
    {
        /// @class Semaphore
        /// @brief Simple semaphore (only shared between threads, not processes)
        class Semaphore
        {
        public:
            /// @brief Create empty instance
            Semaphore() : m_count(0u) {}
            /// @brief Create instance with initial count value
            Semaphore(uint32_t count) : m_count(count) {}
            /// @brief Move instance
            /// @param[in] other  Other instance to move
            explicit Semaphore(Semaphore&& other) : m_count(other.m_count) {}
            // no copy allowed
            Semaphore(const Semaphore& other) = delete;
            
            
            // -- Lock management --
            
            /// @brief Decrement semaphore count (if count > 0), or wait until the count is bigger than 0
            inline void wait() noexcept
            {
                std::unique_lock<mutex> guard(m_lock);
                while (m_count == 0u)
                {
                    m_condition.wait(guard);
                }
                --m_count;
            }
            /// @brief Try to lock, if not already locked
            /// @returns Success (true) or not
            inline bool tryWait() noexcept
            {
                std::unique_lock<mutex> guard(m_lock);
                if (m_count > 0u)
                {
                    --m_count;
                    return true;
                }
                return false;
            }
            /// @brief Increment semaphore count
            inline void notify() noexcept
            {
                std::unique_lock<mutex> guard(m_lock);
                ++m_count;
                m_condition.notify_one();
            }
            

        private:
            uint32_t m_count;  ///< Semaphore count (if 0, wait() will wait until notify() is called)
            std::mutex m_lock; ///< Internal lock system
            std::condition_variable m_condition; ///< Condition variable
        };
    }
}
