/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : process creation and management
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.system
    /// System management utilities
    namespace system
    {
        /// @class Process
        /// @brief Process creation and management
        class Process
        {
        public:
            Process(const std::string& path);
            Process(const Process& other) = delete;
            Process(Process&& other);
            ~Process();
            
            Process& operator=(const Process& other) = delete;
            Process& operator=(Process&& other);
            
            
            uint32_t getProcessId();
            bool isRunning();
            
            uint32_t getPriority();
            void setPriority(const uint32_t& priority);
            
            
            static Process fork();
            
            void detach();
            static void waitForExit(const Process& process);
            static void waitForAnyChildExit();
            
            void interrupt();
            void destroy();
            
        protected:
            Process();
            
        private:
            bool m_isDetached;
            //...
        };
    }
}
