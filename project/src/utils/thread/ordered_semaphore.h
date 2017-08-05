/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : semaphore with guaranteed order (only shared between threads, not processes)
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <thread>
#include <unordered_set>
#include <queue>
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
        /// @class OrderedSemaphore
        /// @brief Semaphore with guaranteed order (only shared between threads, not processes)
        class OrderedSemaphore
        {
        public:
            /// @brief Create empty instance
            OrderedSemaphore() : m_count(0u) {}
            /// @brief Create instance with initial count value
            OrderedSemaphore(uint32_t count) : m_count(count) {}
            /// @brief Move instance
            /// @param[in] other  Other instance to move
            explicit OrderedSemaphore(OrderedSemaphore&& other) : m_count(other.m_count), m_pendingTickets(std::move(other.m_pendingTickets)), m_notifiedTickets(std::move(other.m_notifiedTickets)) {}
            // no copy allowed
            OrderedSemaphore(const OrderedSemaphore& other) = delete;
            
            /// @brief Assign moved instance
            /// @param[in] other  Other instance to move
            OrderedSemaphore& operator=(OrderedSemaphore&& other) 
            { 
                m_count = other.m_count; 
                m_pendingTickets = std::move(other.m_pendingTickets); 
                m_notifiedTickets = std::move(other.m_notifiedTickets); 
            }
            // no copy assignment allowed
            OrderedSemaphore& operator=(const OrderedSemaphore& other) = delete;
            
            
            // -- Lock management --
            
            /// @brief Decrement semaphore count (if count > 0), or wait until the count is bigger than 0
            inline void wait() noexcept
            {
                std::unique_lock<mutex> guard(m_lock);
                if (m_count == 0u)
                {
                    std::condition_variable* pTicket = new std::condition_variable();
                    m_pendingTickets.push(pTicket);
                    do
                    {
                        pTicket->wait(guard);
                    } while (m_notifiedTickets.find(pTicket) == m_notifiedTickets.end());
                    
                    m_notifiedTickets.erase(pTicket);
                    delete pTicket;
                }
                --m_count;
            }
            /// @brief Try to lock, if not already locked
            /// @returns Success (true) or not
            inline bool tryWait() noexcept
            {
                std::unique_lock<mutex> guard(m_lock);
                if (m_count > m_notifiedTickets.size())
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
                if (!m_pendingTickets.empty())
                {
                    std::condition_variable* pTicket = m_pendingTickets.front();
                    m_pendingTickets.pop();
                    
                    if (pTicket != nullptr)
                    {
                        m_notifiedTickets.insert(pTicket);
                        pTicket->notify_one();
                    }
                }
            }
            /// @brief Awake all waiting threads
            inline void notifyAll() noexcept
            {
                std::unique_lock<mutex> guard(m_lock);
                while (!m_pendingTickets.empty())
                {
                    std::condition_variable* pTicket = m_pendingTickets.front();
                    m_pendingTickets.pop();
                    
                    if (pTicket != nullptr)
                    {
                        ++m_count;
                        m_notifiedTickets.insert(pTicket);
                        pTicket->notify_one();
                        guard.unlock();
                        
                        guard.lock();
                    }
                }
            }
            

        private:
            uint32_t m_count;  ///< Semaphore count (if 0, wait() will wait until notify() is called)
            std::mutex m_lock; ///< Internal lock system
            std::queue<std::condition_variable*> m_pendingTickets; ///< Queue of pending tickets (condition variables)
            std::unordered_set<std::condition_variable*> m_notifiedTickets; ///< Tickets that have been notified (condition variables)
        };
    }
}
