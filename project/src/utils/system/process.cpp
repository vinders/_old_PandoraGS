/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : process creation and management
*******************************************************************************/
#include <cstddef>
#include <cstdint>
#include <memory>
#include "process.h"
#include "signal.h"

using namespace utils::system;


/// @brief Move an instance
/// @param[in] other  Other instance
Process::Process(Process&& other)
{
    //...copy data into new instance
    
    other.m_isDetached = true;
    other.m_isWaitingDone = true;
    //...other.processID = 0
    //...other.parentID = 0
}



// -- Child process info --

/// @brief Get child process identifier (system specific)
/// @returns Process ID or handle
uint32_t Process::getProcessId()
{
    //...return processID
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

/// @brief Check if a process has been created by the current main process or not (if forked, it may not have been)
/// @param[in] process  Process instance
/// @return Child process (true) or not
bool Process::isChildProcess(const Process& process)
{
    //...return (process.parentID == current_processID);
}

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
    if (isChildProcess(*this))
    {
        //...detach(processID)...
    }
    m_isDetached = true;
}


/// @brief Send interruption signal to child process (will softly terminate it, can be handled by the child process if necessary)
/// @warning This call does not wait, and thus, the process won't be removed from the process table when it finally decides to exit (call waitForExit for that)
void Process::interrupt()
{
    if (isRunning())
        utils::system::Signal::send(*this, utils::system::signal_t::sigint);
}

/// @brief Send termination signal to child process (will immediately terminate it, can't be handled) and remove it from process table (no need for waitForExit)
/// @warning Unlike interrupt(), the process is removed from the process table, because there's no need to wait (the process is directly terminated)
void Process::destroy()
{
    if (isRunning())
        utils::system::Signal::send(*this, utils::system::signal_t::sigkill);
    waitForExit(*this); // might be zombie-process -> make sure it's removed from process table, even if not running
}



// -- Main/parent process operations --

/// @brief Create a child process by launching a specific executable with command-line arguments
/// @param[in] filePath   Path to the executable file
/// @param[in] arguments  Arguments string
/// @returns Management object for child process
/// @throws std::invalid_argument  The file path is invalid or is not an executable file
/// @throws std::runtime_error     The process could not be created
std::unique_ptr<Process> Process::create(const std::string& filePath, const std::string& arguments)
{
    // check executable file validity
    //...vérifier si filePath existe et est exécutable (sinon exception)
    // if invalid
        // throw std::invalid_argument
    
    // create process
    std::unique_ptr<Process> pChild = utils::system::Process::fork();
    if (pChild == nullptr) // child process -> launch executable
    {
        if (arguments.empty())
        {
            //...exec d'application
            // if exec failed
                //exit(-1)
        }
        else
        {
            //...formater arguments + exec d'application avec arguments
            // if exec failed
                //exit(-1)
        }
    }
    return pChild; // only reached by parent -> never nullptr
}

/// @brief Clone main process and its context - a child process will run from the same position as the parent
/// @returns If parent, management object for child process (if child, returns nullptr)
/// @throws std::runtime_error  The process could not be created
std::unique_ptr<Process> Process::fork()
{
    // create process
    if (1/*...(data = fork()) != 0*/) // parent process -> store process info
    {
        //if (data == -1)
            //throw std::runtime_error
        std::unique_ptr<Process> pChild = std::unique_ptr<Process>(new Process());
        pChild->m_isDetached = false;
        //...initialize "pChild" with forked process info
        //...
        return pChild;
    }
    return nullptr;
}


// -- Synchronize/release process --

/// @brief Wait for a specific child process to exit and/or remove zombie-process from process table
/// @warning If the process has already been removed from the process table, this function will do nothing
void Process::waitForExit(const Process& process)
{
    if (m_isWaitingDone == false)
    {
        //...wait_for_one...
    }
}

/// @brief Wait for a specific child process to exit and/or remove zombie-process from process table
/// @param[in] timeout  Maximum wait (milliseconds)
/// @returns True if a child process has exited, or false if timeout
/// @warning If the process has already been removed from the process table, this function will do nothing
void Process::waitForExit(const Process& process, const uint32_t timeout)
{
    //...set alarm
    //...
    waitForExit(process);
    //...check if alarm occurred
    //...disable alarm
    //...
}


/// @brief Wait until any child process exits and becomes a zombie-process, then remove it from process table
/// @warning Will wait unconditionally, even if no child process exists (and will be stuck forever in that case).
void Process::waitForAnyChildExit()
{
    if (m_isWaitingDone == false)
    {
        //...wait...
    }
}

/// @brief Wait until any child process exits and becomes a zombie-process, then remove it from process table
/// @param[in] timeout  Maximum wait (milliseconds)
/// @returns True if a child process has exited, or false if timeout
/// @warning Will wait unconditionally until the timeout occurs
bool Process::waitForAnyChildExit(const uint32_t timeout)
{
    //...set alarm
    //...
    waitForAnyChildExit();
    //...check if alarm occurred
    //...disable alarm
    //...
}
