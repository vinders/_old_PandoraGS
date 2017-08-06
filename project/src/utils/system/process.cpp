/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : process creation and management
*******************************************************************************/
#include <cstddef>
#include <cstdint>
#include "signal.h"
#include "process.h"

using namespace utils::system;

/// @brief Create a child process by launching a specific executable
/// @param[in] filePath  Path to the executable file
Process::Process(const std::string& filePath)
{
    
}

/// @brief Move an instance
/// @param[in] other  Other instance
Process::Process(Process&& other)
{
    
}



// -- Child process info --

/// @brief Get child process identifier (system specific)
/// @returns Process ID or handle
uint32_t Process::getProcessId()
{
    
}

/// @brief Check if the child process is still running
/// @returns Running or exited
bool Process::isRunning()
{
    
}

/// @brief Get child process priority
/// @returns Process priority value
uint32_t Process::getPriority()
{
    
}

/// @brief Change child process priority
/// @param[in] priority  Process priority value
void Process::setPriority(const uint32_t& priority)
{
    
}



// -- Main/parent process info --

/// @brief Get main process priority
/// @returns Process priority value
uint32_t Process::getMainProcessPriority()
{
    
}

/// @brief Change main process priority
/// @param[in] priority  Process priority value
void Process::setMainProcessPriority(const uint32_t& priority)
{
    
}



// -- Child process operations --

/// @brief Detach the child process : make it able to live without the parent process
/// @warning The process won't be terminated by destructor + no need (and no possibility) to wait for it to exit anymore
void Process::detach()
{
    
}


/// @brief Send interruption signal to child process (will softly terminate it, can be handled by the child process if necessary)
/// @warning This call does not wait, and thus, the process won't be removed from the process table when it finally decides to exit (call waitForExit for that)
void Process::interrupt()
{
    
}

/// @brief Send termination signal to child process (will immediately terminate it, can't be handled) and remove it from process table (no need for waitForExit)
/// @warning Unlike interrupt(), the process is removed from the process table, because there's no need to wait (the process is directly terminated)
void Process::destroy()
{
    
}



// -- Main/parent process operations --

/// @brief Clone main process and its context - a child process will run from the same position as the parent
/// @returns Management object for child process
Process Process::fork()
{
    
}


/// @brief Wait for a specific child process to exit and/or remove zombie-process from process table
/// @warning If the process has already been removed from the process table, this function will do nothing
void Process::waitForExit(const Process& process)
{
    
}

/// @brief Wait for a specific child process to exit and/or remove zombie-process from process table
/// @param[in] timeout  Maximum wait (milliseconds)
/// @returns True if a child process has exited, or false if timeout
/// @warning If the process has already been removed from the process table, this function will do nothing
void Process::waitForExit(const Process& process, const uint32_t timeout)
{
    
}


/// @brief Wait until any child process exits and becomes a zombie-process, then remove it from process table
/// @warning Will wait unconditionally, even if no child process exists (and will be stuck forever in that case).
void Process::waitForAnyChildExit()
{
    
}

/// @brief Wait until any child process exits and becomes a zombie-process, then remove it from process table
/// @param[in] timeout  Maximum wait (milliseconds)
/// @returns True if a child process has exited, or false if timeout
/// @warning Will wait unconditionally until the timeout occurs
bool Process::waitForAnyChildExit(const uint32_t timeout)
{
    
}
