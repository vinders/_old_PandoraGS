/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   log_utility.cpp
Description : messages log utility (CSV)
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <io.h>
#include <fstream>
#include <errno.h>
#include <string.h>
#include <ctime>
using namespace std;
#include "log_utility.h"

#ifdef _WINDOWS
#include <shlobj.h>
#include "pandoraGS.h"
#define _CRT_SECURE_NO_WARNINGS
#define access(dir, rights) _access(dir, rights)
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif


LogUtility* LogUtility::s_pInstance = NULL; // singleton log instance

///<summary>Create logger instance</summary>
///<exception cref="std::exception">Mutex init failure</exception>
LogUtility::LogUtility()
{
    m_isReady = false;

    // default file path (same as plugin)
    if (access("./", 06) == 0)
    {
        m_filePath = "pandoraGS_log.csv";
    }
    else // default access denied
    {
        // get home folder path
        std::string homePath;
        #ifdef _WINDOWS
        char outPath[MAX_PATH];
        if (SHGetSpecialFolderPathA(NULL, outPath, CSIDL_APPDATA, 0)) 
        {
            homePath = outPath; // %AppData%/Roaming
            if (homePath.length() > 0)
                homePath += std::string("\\");
            else
                homePath = std::string("C:\\ProgramData\\");
        }
        else
            homePath = std::string("C:\\ProgramData\\");

        #else
        char* buffer = getenv("HOME");
        if (buffer != NULL) 
            homePath = buffer;
        else
        {
            char* homedir = getpwuid(getuid())->pw_dir;
            homePath = buffer;
        }
        homePath += std::string("/");
        #endif

        // set home file path
        m_filePath = homePath + std::string("pandoraGS_log.csv");
        errno = 0;
    }
    
    // init mutex handle
    #ifdef _WINDOWS
    if ((m_hMtxInstance = CreateMutex(NULL, FALSE, NULL)) == NULL)
        throw std::exception("LogUtility.LogUtility: mutex init failure");
    #else
    if (pthread_mutex_init(&m_hMtxInstance, NULL) == -1)
        throw std::exception((std::string("LogUtility.LogUtility: mutex init failure: ") + std::to_string(strerror(errno))).c_str());
    #endif
    m_isReady = true;
}

///<summary>Destroy logger instance</summary>
LogUtility::~LogUtility()
{
    if (m_isReady)
    {
        // destroy mutex
        #ifdef _WINDOWS
        CloseHandle(m_hMtxInstance);
        #else
        pthread_mutex_destroy(&m_hMtxInstance);
        #endif
    }
}


// -- SINGLETON -- -------------------------------------------------------------

///<summary>Create or get existing singleton instance</summary>
///<returns>Log utility singleton instance</returns>
///<exception cref="std::exception">Instance creation failure</exception>
LogUtility* LogUtility::getInstance()
{
    if (s_pInstance == NULL)
        s_pInstance = new LogUtility();
    return s_pInstance;
}

///<summary>Close log utility singleton instance</summary>
void LogUtility::closeInstance()
{
    if (s_pInstance != NULL)
        delete s_pInstance;
    s_pInstance = NULL;
}


// -- GETTERS / SETTERS -- -------------------------------------------------

///<summary>Set the logfile path</summary>
///<param name="filePath">Logfile path</param>
void LogUtility::setFilePath(const std::string filePath)
{
    m_filePath = filePath;
}

///<summary>Get the logfile path</summary>
///<returns>Logfile path</returns>
std::string LogUtility::getFilePath() const
{
    return m_filePath;
}


// -- LOG WRITING -- -------------------------------------------------------

///<summary>Write general entry in logfile</summary>
///<param name="origin">Message origin (ClassName.MethodName)</param>
///<param name="type">Message type description</param>
///<param name="message">Message content</param>
void LogUtility::writeEntry(const std::string origin, const std::string type, const std::string message)
{
    // lock
    #ifdef _WINDOWS
    if (WaitForSingleObject(m_hMtxInstance, 2000) != WAIT_OBJECT_0)
        return;
    #else
    pthread_mutex_lock(&m_hMtxInstance);
    #endif

    try
    {
        // open logfile
        std::ofstream out;
        out.open(getFilePath(), std::ios_base::app); // append
        if (!out.is_open())
        {
            out.open(getFilePath().c_str(), ios_base::trunc); // create file
            out << "DATE;TIME;TYPE;ORIGIN;MESSAGE" << endl; // header
        }

        // get date/time
        time_t tm = time(0); 
        std::string curDate;
        std::string curTime;
        #ifdef _WINDOWS
        struct tm now;
        if (localtime_s(&now, &tm) == 0)
        {
            curDate = std::to_string(now.tm_mday) + std::string("/") + std::to_string(now.tm_mon + 1) 
                    + std::string("/") + std::to_string(now.tm_year + 1900);
            curTime = std::to_string(now.tm_hour) + std::string(":") + std::to_string(now.tm_min) 
                    + std::string(":") + std::to_string(now.tm_sec);
        }
        else
        {
            curDate = "<undefined>";
            curTime = "00:00:00";
        }
        #else
        struct tm* now = localtime(&tm);
        curDate = std::to_string(now->tm_mday) + std::string("/") + std::to_string(now->tm_mon + 1) 
                + std::string("/") + std::to_string(now->tm_year + 1900);
        curTime = std::to_string(now->tm_hour) + std::string(":") + std::to_string(now->tm_min) 
                + std::string(":") + std::to_string(now->tm_sec);
        #endif

        // write new entry
        out << curDate << ";" << curTime << ";" << type << ";" << origin << ";" << message << endl;

        out.close();
    }
    catch (exception exc)
    {
        // log error
    }

    // unlock
    #ifdef _WINDOWS
    ReleaseMutex(m_hMtxInstance);
    #else
    pthread_mutex_unlock(&m_hMtxInstance); 
    #endif
}

///<summary>Write error or warning entry in logfile</summary>
///<param name="origin">Message origin (ClassName.MethodName)</param>
///<param name="message">Message content (concatenated with errno message if available)</param>
void LogUtility::writeErrorEntry(const std::string origin, std::string message)
{
    const std::string type = std::to_string(errno); // error code
    if (errno != 0)
    {
        #ifdef _WINDOWS
        char buffer[160];
        strerror_s(buffer, (size_t)160, errno);
        std::string errStr(buffer);
        message = message + std::string(": ") + errStr; // error message
        #else
        message = message + std::string(": ") + std::string(strerror(errno)); // error message
        #endif
    }
    errno = 0;

    writeEntry(origin, type, message);
}
