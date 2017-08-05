/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : Process signal sending and handling
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include "../thread/spin_lock.h"
#include "process.h"
#include "delegate.h"

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.system
    /// System management utilities
    namespace system
    {
        /// @enum signal_t
        /// @brief Process signal types
        enum class signal_t : uint32_t
        {
            sigint = 2,
            sigkill = 9
        };
        
        /// @namespace Signal
        /// @brief Process signal sending and handling
        class Signal
        {
        public:
            /// @brief Send a signal to a child process
            /// @param[in] target  Target process
            /// @param[in] sig     Signal to send
            /// @returns Success
            static bool send(const Process& target, const utils::system::signal_t sig);
            
            // -- Signal handling --
            
            /// @brief Register a signal handler
            /// @param[in] sig      Signal to handle
            /// @param[in] handler  Handler(s)
            static void registerHandler(const utils::system::signal_t sig, const utils::system::delegate& handler);
            /// @brief Unregister a signal handler
            /// @param[in] sig      Signal to stop handling
            /// @param[in] handler  Handler(s) to remove
            static void unregisterHandler(const utils::system::signal_t sig, const utils::system::delegate& handler);
            /// @brief Unregister all handlers for a signal
            /// @param[in] sig      Signal to stop handling
            static void unregisterAllHandlers(const utils::system::signal_t sig);
            
        private:
            /// @brief Internal signal registering
            /// @param[in] sig  Signal type
            static void registerSignal(const utils::system::signal_t sig)
            /// @brief Internal signal unregistering
            /// @param[in] sig  Signal type
            static void unregisterSignal(const utils::system::signal_t sig)
            
            
        private:
            static std::unordered_map<utils::system::signal_t,utils::system::Delegate<void,utils::system::signal_t>> m_handlers; ///< Registered signal handlers for current process
            static utils::thread::SpinLock m_handlersMutex; ///< Mutex for map of handlers
        };
    }
}
