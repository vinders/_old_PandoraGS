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
// semaphores
#define sem_t                   HANDLE
#define SEMAPHORE_WAIT_OK       WAIT_OBJECT_0
#define SEMAPHORE_TIMEOUT       WAIT_TIMEOUT
#define SEMAPHORE_SIGNAL_ERR    0
#define semaphore_wait(sema)    WaitForSingleObject(sema, 0L) //INFINITE
#define semaphore_wait_timeout(sema,time) WaitForSingleObject(sema, time)
#define semaphore_signal(sema)  ReleaseSemaphore(sema, 1, NULL)
#else
// threads
#include <pthread.h>
#define THREAD_RETURN           NULL
// semaphores
#include <semaphore.h>
#define semaphore_wait(sema)    sem_wait(&sema)
#define semaphore_signal(sema)  sem_post(&sema)
#define semaphore_signal_timeout(sema,time) sem_post(&sema)
#define SEMAPHORE_OK            0
#define SEMAPHORE_TIMEOUT      -1
#define SEMAPHORE_SIGNAL_ERR   -1
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
    #if _THREADSYNC == THREADSYNC_SEMAPHORE
        sem_t m_semReport;  // report sync semaphore (thread to manager)
        sem_t m_semUpdate;  // update sync semaphore (manager to thread)
    #else
    public:
        bool m_isQueryWaiting; // update signal (for polling)
        bool m_isQueryReply;   // reply signal (for polling)
    #endif
public:
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


    // -- SEMAPHORES -- --------------------------------------------------------

    #if _THREADSYNC == THREADSYNC_SEMAPHORE
    /// <summary>Wait for report sync signal</summary>
    /// <param name="timeout">Max wait time (ms)</param>
    /// <returns>Wait success (true)</returns>
    bool waitReport(unsigned long timeout = 0uL)
    {
        return (semaphore_wait_timeout(m_semReport,timeout) == SEMAPHORE_WAIT_OK);
    }
    /// <summary>Send report sync signal</summary>
    /// <returns>Signal success (true)</returns>
    bool signalReport()
    {
        return (semaphore_signal(m_semReport) != SEMAPHORE_SIGNAL_ERR);
    }
    /// <summary>Wait for update sync signal</summary>
    /// <returns>Wait success (true)</returns>
    bool waitUpdate()
    {
        return (semaphore_wait(m_semUpdate) == SEMAPHORE_WAIT_OK);
    }
    /// <summary>Send update sync signal</summary>
    /// <returns>Signal success (true)</returns>
    bool signalUpdate()
    {
        return (semaphore_signal(m_semUpdate) != SEMAPHORE_SIGNAL_ERR);
    }
    #endif

    /// <summary>Send change signal and wait for reply</summary>
    void changeQuery();
    /// <summary>Send draw update signal</summary>
    void drawQuery();
    /// <summary>Request thread pause (UI closed) and wait for reply</summary>
    void pauseQuery();
};

#endif
