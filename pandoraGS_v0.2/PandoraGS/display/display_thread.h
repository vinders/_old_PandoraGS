/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   display_thread.h
Description : display thread manager
*******************************************************************************/
#ifndef _DISPLAY_THREAD_H
#define _DISPLAY_THREAD_H
#include "globals.h"

// thread config
#define THREADS_POOL_SIZE       1
// thread status
#define THREAD_STATUS_RUNNING   1
#define THREAD_STATUS_STOPPED   2

#ifdef _WINDOWS
// threads
#define pthread_t               HANDLE
#define pthread_exit(code)      ExitThread(code)
#define THREAD_RETURN           0
#else
// threads
#include <pthread.h>
#define THREAD_RETURN           NULL
#endif

#include "render.h"
#include "config.h"


// Display thread manager
class DisplayThread
{
private:
    static DisplayThread** s_pInstances; // instances global array
    // no mutex required because only main thread affects it
    int m_poolIndex; // current instance index
protected:
    pthread_t m_hDisplayThread; // thread handle
    int m_threadStatus; // thread status
public:
    bool m_isQueryWaiting; // update signal (for polling)
    bool m_isQueryReply;   // reply signal (for polling)

    Config* m_pConfig;  // thread config reference
    bool m_isChangePending; // profile change request
    bool m_isPausePending; // thread pause request (close window and wait)
    bool m_isPaused; // thread pause state (blocked until false)


public:
    /// <summary>Create thread manager instance</summary>
    DisplayThread();
    /// <summary>Close display thread</summary>
    ~DisplayThread();
    /// <summary>Initialize display thread (called from main thread only)</summary>
    /// <param name="pConfig">Thread configuration reference</param>
    /// <exception cref="std::exception">Thread creation failure</exception>
    void initThread(Config* config);

protected:
    /// <summary>Add current instance into global thread pool</summary>
    void insertInstance();
    /// <summary>Set thread sync/communication mechanics</summary>
    /// <exception cref="std::exception">Sync init failure</exception>
    void initSync();
    /// <summary>Start and initialize new rendering thread</summary>
    /// <exception cref="std::exception">Thread creation failure</exception>
    void startThread();


public:
    // -- GETTERS - SETTERS -- -------------------------------------------------

    /// <summary>Get thread manager existing instance (no check!)</summary>
    /// <param name="index">Instance index in the thread pool</param>
    /// <returns>Thread manager instance</returns>
    static DisplayThread* getLoadedInstance(int index);
    /// <summary>Destroy all threads managers instances (called from main thread only)</summary>
    static void releaseInstances();

    /// <summary>Check if thread is started</summary>
    /// <returns>Thread start indicator</returns>
    bool isThreadStarted()
    {
        return (m_threadStatus != THREAD_STATUS_STOPPED);
    }
    /// <summary>Request thread cancellation (quit or error)</summary>
    void setThreadStop();


    // -- THREAD SYNC -- -------------------------------------------------------

    /// <summary>Send change signal and wait for reply</summary>
    void changeQuery();
    /// <summary>Send draw update signal</summary>
    void drawQuery();
    /// <summary>Request thread pause (UI closed) and wait for reply</summary>
    void pauseQuery();
};

#endif
