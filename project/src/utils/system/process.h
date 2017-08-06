/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : process creation and management
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

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
            /// @brief Move an instance
            /// @param[in] other  Other instance
            Process(Process&& other);
            // no copy allowed
            Process(const Process& other) = delete;
            
            /// @brief Directly terminate the child process (unless it's been detached or interrupted) and remove it from process table
            /// @warning It is recommended to aither call interrupt() (to kill it softly in a manageable way) or detach() before the destructor is called
            ~Process()
            {
                if (m_isDetached == false && isChildProcess(*this))
                    destroy();
            }
            
            // no copy assignment allowed
            Process& operator=(const Process& other) = delete;
            // no move assignment allowed
            Process& operator=(Process&& other) = delete;
            
            
            // -- Child process info --
            
            /// @brief Get child process identifier (system specific)
            /// @returns Process ID or handle
            uint32_t getProcessId();
            /// @brief Check if the child process is still running
            /// @returns Running or exited
            bool isRunning();
            
            /// @brief Get child process priority
            /// @returns Process priority value
            uint32_t getPriority();
            /// @brief Change child process priority
            /// @param[in] priority  Process priority value
            void setPriority(const uint32_t& priority);
            
            
            // -- Main/parent process info --
            
            /// @brief Check if a process has been created by the current main process or not (if forked, it may not have been)
            /// @param[in] process  Process instance
            /// @return Child process (true) or not
            static bool isChildProcess(const Process& process);
            
            /// @brief Get main process priority
            /// @returns Process priority value
            static uint32_t getMainProcessPriority();
            /// @brief Change main process priority
            /// @param[in] priority  Process priority value
            static void setMainProcessPriority(const uint32_t& priority);
            
            
            // -- Child process operations --
            
            /// @brief Detach the child process : make it able to live without the parent process
            /// @warning The process won't be terminated by destructor + no need (and no possibility) to wait for it to exit anymore
            void detach();
            
            /// @brief Send interruption signal to child process (will softly terminate it, can be handled by the child process if necessary)
            /// @warning This call does not wait, and thus, the process won't be removed from the process table when it finally decides to exit (call waitForExit for that)
            void interrupt();
            /// @brief Send termination signal to child process (will immediately terminate it, can't be handled) and remove it from process table (no need for waitForExit)
            /// @warning Unlike interrupt(), the process is removed from the process table, because there's no need to wait (the process is directly terminated)
            void destroy();
            
            
            // -- Main/parent process operations --
            
            /// @brief Create a child process by launching a specific executable
            /// @param[in] filePath  Path to the executable file
            /// @returns Management object for child process
            /// @throws std::invalid_argument  The file path is invalid or is not an executable file
            /// @throws std::runtime_error     The process could not be created
            static inline std::unique_ptr<Process> create(const std::string& filePath)
            {
                return utils::system::Process::create(filePath, "");
            }
            /// @brief Create a child process by launching a specific executable with command-line arguments
            /// @param[in] filePath   Path to the executable file
            /// @param[in] arguments  Arguments string
            /// @returns Management object for child process
            /// @throws std::invalid_argument  The file path is invalid or is not an executable file
            /// @throws std::runtime_error     The process could not be created
            static std::unique_ptr<Process> create(const std::string& filePath, const std::string& arguments);
            /// @brief Clone main process and its context - a child process will run from the same position as the parent
            /// @returns If parent, management object for child process (if child, returns nullptr)
            /// @throws std::runtime_error  The process could not be created
            static std::unique_ptr<Process> fork();
            
            
            // -- Synchronize/release process --
            
            /// @brief Wait for a specific child process to exit and/or remove zombie-process from process table
            /// @warning If the process has already been removed from the process table, this function will do nothing
            static void waitForExit(const Process& process);
            /// @brief Wait for a specific child process to exit and/or remove zombie-process from process table
            /// @param[in] timeout  Maximum wait (milliseconds)
            /// @returns True if a child process has exited, or false if timeout
            /// @warning If the process has already been removed from the process table, this function will do nothing
            static void waitForExit(const Process& process, const uint32_t timeout);
            
            /// @brief Wait until any child process exits and becomes a zombie-process, then remove it from process table
            /// @warning Will wait unconditionally, even if no child process exists (and will be stuck forever in that case).
            static void waitForAnyChildExit();
            /// @brief Wait until any child process exits and becomes a zombie-process, then remove it from process table
            /// @param[in] timeout  Maximum wait (milliseconds)
            /// @returns True if a child process has exited, or false if timeout
            /// @warning Will wait unconditionally until the timeout occurs
            static bool waitForAnyChildExit(const uint32_t timeout);
            
            
        protected:
            /// @brief Create empty instance with no execution (used by main process operations)
            /// @warning Member m_isDetached should be set to false if a child process is created
            Process() : m_isDetached(true), m_isWaitingDone(false) {}
            
            
        private:
            bool m_isDetached;
            bool m_isWaitingDone;
            //...processID
            //...parentID
        };
    }
}
