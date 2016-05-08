/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   display_thread.cpp
Description : display thread manager
*******************************************************************************/
#include <string>
#include <errno.h>
#include <time.h>
using namespace std;
#include "log_utility.h"
#include "framerate_manager.h"
#include "input_manager.h"
#include "psx_core_memory.h"
#include "display_thread.h"

#define SEMAPHORE_RETRY_NB 5
extern PsxCoreMemory* g_pMemory; // main console emulated memory

// private prototypes
#ifdef _WINDOWS
DWORD WINAPI displayLoop(LPVOID arg); // thread main display loop
#else
void* DisplayThread::DisplayLoop(void* arg)
#endif

DisplayThread** DisplayThread::s_pInstances = NULL; // instances global array


// -- INSTANCE & THREAD MANAGEMENT -- ------------------------------------------

/// <summary>Create thread manager instance</summary>
DisplayThread::DisplayThread()
{
    // initialize values
    m_threadStatus = THREAD_STATUS_STOPPED;
    #ifdef _WINDOWS
    m_hDisplayThread = NULL;
    #if _THREADSYNC == THREADSYNC_SEMAPHORE
    m_semUpdate = NULL;
    m_semReport = NULL;
    #endif
    #endif

    m_isChangePending = false;
    #if _THREADSYNC != THREADSYNC_SEMAPHORE
    m_isQueryWaiting = false;
    m_isQueryReply = false;
    #endif
}

/// <summary>Close display thread</summary>
DisplayThread::~DisplayThread()
{
    if (isThreadStarted())
    {
        // request thread end
        m_threadStatus = THREAD_STATUS_STOPPED;
        #if _THREADSYNC == THREADSYNC_SEMAPHORE
        for (int sig = 0; signalUpdate() == false && sig < SEMAPHORE_RETRY_NB; sig++);
        #else
        m_isQueryWaiting = true;
        #endif

        // wait for ending
        #ifdef _WINDOWS
        WaitForMultipleObjects(1, &m_hDisplayThread, TRUE, 400);
        CloseHandle(m_hDisplayThread);
        m_hDisplayThread = NULL;
        #else
        pthread_join(m_hDisplayThread, NULL);
        #endif
    }
    // remove from pool
    if (m_poolIndex < THREADS_POOL_SIZE && s_pInstances[m_poolIndex] == this)
        s_pInstances[m_poolIndex] = NULL;
    m_pConfig = NULL;

    // destroy semaphores
    #if _THREADSYNC == THREADSYNC_SEMAPHORE
    #ifdef _WINDOWS
    if (m_semUpdate != NULL)
        CloseHandle(m_semUpdate);
    if (m_semReport != NULL)
        CloseHandle(m_semReport);
    #else
    sem_destroy(&m_semUpdate);
    sem_destroy(&m_semReport);
    #endif
    #endif
}

/// <summary>Initialize display thread (called from main thread only)</summary>
/// <param name="pConfig">Thread configuration reference</param>
/// <exception cref="std::exception">Thread creation failure</exception>
void DisplayThread::initThread(Config* pConfig)
{
    // add to thread pool
    insertInstance();

    // create sync and thread
    m_pConfig = pConfig;
    initSync();
    startThread();
    
    // wait for thread answer (after API init)
    #if _THREADSYNC == THREADSYNC_SEMAPHORE
    waitReport();
    #else
    while (m_isQueryReply == false);
    m_isQueryReply = false;
    #endif

    if (isThreadStarted() == false) // init failure
    {
        #ifdef _WINDOWS
        WaitForMultipleObjects(1, &m_hDisplayThread, TRUE, 1600);
        CloseHandle(m_hDisplayThread);
        m_hDisplayThread = NULL;
        #else
        pthread_join(m_hDisplayThread, NULL);
        #endif
        throw new std::exception("DisplayThread::InitThread(): Rendering API start failure");
    }
}

/// <summary>Add current instance into global thread pool</summary>
void DisplayThread::insertInstance()
{
    // create instances array
    if (s_pInstances == NULL)
    {
        s_pInstances = new DisplayThread*[THREADS_POOL_SIZE]();
        for (int i = 0; i < THREADS_POOL_SIZE; i++)
            s_pInstances[i] = NULL;

        // first entry
        s_pInstances[0] = this;
        m_poolIndex = 0;
    }
    // add instance to array
    else
    {
        // first available entry
        m_poolIndex = 0;
        while (m_poolIndex < THREADS_POOL_SIZE && s_pInstances[m_poolIndex] != NULL)
            m_poolIndex++;
        // full -> replace first
        if (m_poolIndex >= THREADS_POOL_SIZE) 
        {
            delete s_pInstances[0];
            m_poolIndex = 0;
        }

        // store index
        s_pInstances[m_poolIndex] = this;
    }
}

/// <summary>Set thread sync/communication mechanics</summary>
/// <exception cref="std::exception">Sync init failure</exception>
void DisplayThread::initSync()
{
    #if _THREADSYNC == THREADSYNC_SEMAPHORE
    // create sync semaphores
    unsigned int semaInitVal = 0;
    #ifdef _WINDOWS
    if ((m_semUpdate = CreateSemaphore(NULL, semaInitVal, 65535, NULL)) == NULL
        || (m_semReport = CreateSemaphore(NULL, semaInitVal, 65535, NULL)) == NULL)
    #else
    if (sem_init(&m_semUpdate, 0, semaInitVal) == -1
        || sem_init(&m_semReport, 0, semaInitVal) == -1)
    #endif
    {
        throw new std::exception("DisplayThread::InitThread(): Semaphores init failure");
    }

    #else
    m_isQueryWaiting = false;
    m_isQueryReply = false;
    #endif
}

/// <summary>Start and initialize new rendering thread</summary>
/// <exception cref="std::exception">Thread creation failure</exception>
void DisplayThread::startThread()
{
    // thread values
    int* pData = new int(m_poolIndex);
    m_threadStatus = THREAD_STATUS_RUNNING;

    // start new thread
    #ifdef _WINDOWS
    if ((m_hDisplayThread = CreateThread(NULL, 0, displayLoop, pData, 0, NULL)) == NULL)
    #else
    if (pthread_create(&m_hDisplayThread, NULL, displayLoop, (void*)pData) == -1)
    #endif
    {
        m_threadStatus = THREAD_STATUS_STOPPED;
        delete pData;
        throw new std::exception("DisplayThread::InitThread(): Thread start failure");
    }
}


// -- GETTERS - SETTERS -- -----------------------------------------------------

/// <summary>Get thread manager existing instance (no check!)</summary>
/// <param name="index">Instance index in the thread pool</param>
/// <returns>Thread manager instance</returns>
DisplayThread* DisplayThread::getLoadedInstance(int index)
{
    if (s_pInstances != NULL)
        return s_pInstances[index];
    return NULL;
}

/// <summary>Destroy all threads managers instances (called from main thread only)</summary>
void DisplayThread::releaseInstances()
{
    if (s_pInstances != NULL)
    {
        for (int i = 0; i < THREADS_POOL_SIZE; i++)
        {
            delete s_pInstances[i];
            s_pInstances[i] = NULL;
        }
        delete [] s_pInstances;
        s_pInstances = NULL;
    }
}

/// <summary>Request thread cancellation (quit or error)</summary>
void DisplayThread::setThreadStop()
{
    m_threadStatus = THREAD_STATUS_STOPPED;
}

/// <summary>Send change signal and wait for reply</summary>
void DisplayThread::changeQuery()
{
    // send change signal
    m_isChangePending = true;
    #if _THREADSYNC == THREADSYNC_SEMAPHORE
    m_isPaused = false;
    int sig = 0;
    for (sig = 0; sig < SEMAPHORE_RETRY_NB && signalUpdate() == false; sig++)
    {
        try { LogUtility::getInstance()->writeEntry("updateQuery", "signal", "Update sync signal failure"); }
        catch (...) {}
    }
    #else
    m_isQueryWaiting = true;
    m_isPaused = false;
    #endif

    // wait for reply
    #if _THREADSYNC == THREADSYNC_SEMAPHORE
    if (sig < SEMAPHORE_RETRY_NB) // prevent deadlock
        waitReport();
    #else
    while (m_isQueryReply == false);
    m_isQueryReply = false;
    #endif
}

/// <summary>Send draw update signal</summary>
void DisplayThread::drawQuery()
{
    // send update signal
    #if _THREADSYNC == THREADSYNC_SEMAPHORE
    static int sig;
    for (sig = 0; signalUpdate() == false && sig < SEMAPHORE_RETRY_NB; sig++);
    #else
    m_isQueryWaiting = true;
    #endif
}

/// <summary>Request thread pause (UI closed) and wait for reply</summary>
void DisplayThread::pauseQuery()
{
    // send pause signal
    m_isPausePending = true;
    #if _THREADSYNC == THREADSYNC_SEMAPHORE
    int sig = 0;
    for (sig = 0; sig < SEMAPHORE_RETRY_NB && signalUpdate() == false; sig++)
    {
        try { LogUtility::getInstance()->writeEntry("pauseQuery", "signal", "Update sync signal failure"); }
        catch (...) {}
    }
    #else
    m_isQueryWaiting = true;
    #endif

    // wait for reply (frame complete and window closed)
    #if _THREADSYNC == THREADSYNC_SEMAPHORE
    if (sig < SEMAPHORE_RETRY_NB) // prevent deadlock
        waitReport(8000uL); //8s timeout
    #else
    while (m_isQueryReply == false);
    m_isQueryReply = false;
    #endif
}



// -- DISPLAY LOOP -- ----------------------------------------------------------
//______________________________________________________________________________

#ifdef _WINDOWS
/// <summary>Main synchronized display loop</summary>
/// <param name="arg">Initialization argument</param>
/// <returns>Thread ending code</returns>
DWORD WINAPI displayLoop(LPVOID arg)
#else
void* DisplayThread::DisplayLoop(void* arg)
#endif
{
    // get thread info
    int threadIndex = *((int*)arg); // instance index
    delete (int*)arg;
    DisplayThread* threadManager = DisplayThread::getLoadedInstance(threadIndex); // manager
    if (threadManager == NULL)
    {
        pthread_exit(1); return THREAD_RETURN;
    }

    Render* render = NULL;
    try
    {
        // start rendering pipeline
        render = Render::createInstance(threadManager->m_pConfig);
        render->initApi();

        // reply to thread manager
        #if _THREADSYNC == THREADSYNC_SEMAPHORE
        for (int sig = 0; threadManager->signalReport() == false && sig < SEMAPHORE_RETRY_NB; sig++)
        {
            try { LogUtility::getInstance()->writeEntry("displayLoop", "signal", "Report sync signal failure"); }
            catch (...) {}
        }
        #else
        threadManager->m_isQueryReply = true;
        #endif
    }
    catch (std::exception exc) // failure -> stop
    {
        threadManager->setThreadStop();
        if (render != NULL)
            delete render;

        #if _THREADSYNC == THREADSYNC_SEMAPHORE
        for (int sig = 0; threadManager->signalReport() == false && sig < SEMAPHORE_RETRY_NB; sig++);
        #else
        threadManager->m_isQueryReply = true;
        #endif

        try { LogUtility::getInstance()->writeEntry("displayLoop", "GL API", exc.what()); }
        catch (...) {}
        pthread_exit(1); return THREAD_RETURN;
    }


    // -----------------------
    // -- main display loop --
    // -----------------------
    bool hasWindow = false;
    int loadedProfile = -1;
    do
    {
        // wait for sync signal
        #if _THREADSYNC == THREADSYNC_SEMAPHORE
        do {
            threadManager->waitUpdate();
        } while (threadManager->m_isPaused && threadManager->isThreadStarted()); // pause loop
        #else
        while ((threadManager->m_isQueryWaiting == false || threadManager->m_isPaused) 
            && threadManager->isThreadStarted());
        threadManager->m_isQueryWaiting = false;
        #endif

        // thread exit signal
        if (threadManager->isThreadStarted() == false) 
            break;


        // - pause query (close window and stop drawing) -
        // ---------------------------------------------------------
        if (threadManager->m_isPausePending)
        {
            // close window and stop rendering
            //... fermer fenetre
            hasWindow = false;
            //... effacer shaders
            loadedProfile = -1;

            // reply to thread manager (changes done)
            threadManager->m_isPaused = true;
            threadManager->m_isChangePending = false;
            #if _THREADSYNC == THREADSYNC_SEMAPHORE
            for (int sig = 0; threadManager->signalReport() == false && sig < SEMAPHORE_RETRY_NB; sig++)
            {
                try { LogUtility::getInstance()->writeEntry("displayLoop", "signal", "Report sync signal failure"); }
                catch (...) {}
            }
            #else
            threadManager->m_isQueryReply = true;
            #endif
        }

        // - change query (profile change, window change, stretching change) -
        // ---------------------------------------------------------------------
        else if (threadManager->m_isChangePending)
        {
            // window mode or stretching mode change
            if (InputManager::m_isWindowModeChangePending && hasWindow)
            {
                if (InputManager::m_isStretchingChangePending) // stretching mode
                {
                    if (threadManager->m_pConfig->dsp_isFullscreen == false // window resized
                    && InputManager::m_isSizeChangePending)
                    {
                        #ifdef _WINDOWS
                        //... récupérer taille courante de la fenêtre (- marge de sécurité 40px, ou bien inner size)
                        //!!!vérif min/max -> doit être entre 4:3 et 16:9 -> changer taille en Y si plus/moins
                        //!!!si fenetre plus petite que taille native -> prendre taille native
                            //SetWindowPos(g_pMemory->gen_hDisplayWindow, HWND_NOTOPMOST, x, y, cx, cy, SWP_NOSENDCHANGING);
                        #endif
                    }
                    //... changer étirement/ratio
                    //->si natif, centrer
                    //->sinon
                        //->si garder aspect ou pixel, faire même multiple en y qu'en x (centrer par rapport à fenetre)
                        //->sinon étirer/couper selon le mode
                    InputManager::m_isSizeChangePending = false;
                }
                else // window mode
                {
                    InputManager::setDisplayListener(false);
                    //... fermer fenetre
                    hasWindow = false;
                }
                InputManager::m_isWindowModeChangePending = false;
                InputManager::m_isStretchingChangePending = false;
            }
            // renderer has no window -> create one
            if (hasWindow == false)
            {
                //... créer fenetre

                if (threadManager->m_pConfig->dsp_isFullscreen == false)
                    InputManager::setDisplayListener(true);
            }
            
            // profile change -> recreate rendering pipeline (shaders)
            if (loadedProfile != (int)(threadManager->m_pConfig->getCurrentProfileId()))
            {
                //... recréer shaders selon profil actif
                loadedProfile = (int)(threadManager->m_pConfig->getCurrentProfileId());
            }

            // reply to thread manager (changes done)
            threadManager->m_isChangePending = false;
            #if _THREADSYNC == THREADSYNC_SEMAPHORE
            for (int sig = 0; threadManager->signalReport() == false && sig < SEMAPHORE_RETRY_NB; sig++)
            {
                try { LogUtility::getInstance()->writeEntry("displayLoop", "signal", "Report sync signal failure"); }
                catch (...) {}
            }
            #else
            threadManager->m_isQueryReply = true;
            #endif
        }

        // - update display -
        // ------------------
        else
        {
            // copie données (arrays GL)
            //...
            FramerateManager::signalFrameDrawing(); // signal drawing

            //...
            //upscaling
            //...
            //insertion dans shaders, lancement pipeline, affichage résultat dès que vsync

            // si vsync actif, attendre vsync
            //(ne pas attendre de signal autre que vsync. 
            // render buffer swap
        }
    } 
    while (1);


    // stop renderer
    if (render != NULL)
        delete render;

    // fail-safe exit
    #if _THREADSYNC == THREADSYNC_SEMAPHORE
    threadManager->signalReport();
    #else
    threadManager->m_isQueryReply = true;
    #endif
    pthread_exit(1);
    return THREAD_RETURN;
}
