/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : Process signal sending and handling
*******************************************************************************/
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include "process.h"
#include "delegate.h"
#include "signal.h"

using namespace utils::system;

std::unordered_map<utils::system::signal_t,utils::system::Delegate<void,utils::system::signal_t>> Signal::m_handlers; ///< Registered signal handlers for current process
utils::thread::SpinLock Signal::m_handlersMutex; ///< Mutex for map of handlers


/// @brief Send a signal to a child process
/// @param[in] target  Target process
/// @param[in] sig     Signal to send
/// @returns Success
bool Signal::send(const Process& target, const utils::system::signal_t sig)
{
    /*...*/
    /*...*/
    /*...*/
}



// -- Signal handling --

/// @brief Register a signal handler
/// @param[in] sig      Signal to handle
/// @param[in] handler  Handler(s)
void Signal::registerHandler(const utils::system::signal_t sig, const utils::system::delegate<void,utils::system::signal_t>& handler)
{
    m_handlersMutex.lock();
    if (m_handlers.find(sig) == m_handlers.end())
    {
        registerSignal(sig);
        m_handlers[sig] = handler;
    }
    else
        m_handlers[sig] += handler;
    m_handlersMutex.unlock();
}

/// @brief Unregister a signal handler
/// @param[in] sig      Signal to stop handling
/// @param[in] handler  Handler(s) to remove
void Signal::unregisterHandler(const utils::system::signal_t sig, const utils::system::delegate<void,utils::system::signal_t>& handler)
{
    m_handlersMutex.lock();
    if (m_handlers.find(sig) != m_handlers.end())
    {
        m_handlers[sig] -= handler;
        if (m_handlers[sig].isEmpty())
        {
            m_handlers.erase(sig);
            unregisterSignal(sig);
        }
    }
    m_handlersMutex.unlock();
}

/// @brief Unregister all handlers for a signal
/// @param[in] sig      Signal to stop handling
void Signal::unregisterAllHandlers(const utils::system::signal_t sig)
{
    m_handlersMutex.lock();
    if (m_handlers.find(sig) != m_handlers.end())
    {
        m_handlers.erase(sig);
        unregisterSignal(sig);
    }
    m_handlersMutex.unlock();
}



// -- Signal handling (internal) --

/// @brief Handle received signal
/// @param[in] sig  Signal code
template <utils::system::signal_t Sig>
void onSignalReceived(int sig)
{
    if (m_handlers.find(Sig) != m_handlers.end())
        m_handlers[Sig](Sig);
}

/// @brief Internal signal registering
/// @param[in] sig  Signal type
void Signal::registerSignal(const utils::system::signal_t sig)
{
    void(*baseHandler)(int) = nullptr;
    switch (sig)
    {
        case signal_t::sigint:  baseHandler = onSignalReceived<signal_t::sigint>; break;
        case signal_t::sigkill: baseHandler = onSignalReceived<signal_t::sigkill>; break;
    }
    if (baseHandler != nullptr)
    {
        /*...*/
        /*...*/
        /*...*/
    }
}

/// @brief Internal signal unregistering
/// @param[in] sig  Signal type
void Signal::unregisterSignal(const utils::system::signal_t sig)
{
    switch (sig)
    {
        case signal_t::sigint:  /*...*/ break;
        case signal_t::sigkill: /*...*/ break;
    }
}
