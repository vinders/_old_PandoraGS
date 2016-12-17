/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   logger.cpp
Description : advanced log utility (csv)
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <io.h>
#include <fstream>
#include <errno.h>
#include <cstring>
#include <ctime>
using namespace std;
#include "logger.h"

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

Logger* Logger::s_pInstance = NULL; // singleton log instance


///<summary>Create logger instance</summary>
Logger::Logger()
{
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
        }
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
}


// -- SINGLETON -- -------------------------------------------------------------

///<summary>Create or get existing singleton instance</summary>
///<returns>Log utility singleton instance</returns>
Logger* Logger::getInstance()
{
    if (s_pInstance == NULL)
        s_pInstance = new Logger();
    return s_pInstance;
}


// -- GETTERS / SETTERS -- -------------------------------------------------

///<summary>Set the logfile path</summary>
///<param name="filePath">Logfile path</param>
void Logger::setFilePath(const std::string filePath)
{
    m_filePath = filePath;
}

///<summary>Get the logfile path</summary>
///<returns>Logfile path</returns>
std::string Logger::getFilePath() const
{
    return m_filePath;
}


// -- LOG WRITING -- -------------------------------------------------------

///<summary>Write general entry in logfile</summary>
///<param name="origin">Message origin (ClassName.MethodName)</param>
///<param name="type">Message type description</param>
///<param name="message">Message content</param>
void Logger::writeEntry(const std::string origin, const std::string type, const std::string message)
{
    m_hMtxInstance.lock();
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
    m_hMtxInstance.unlock();
}

///<summary>Write error or warning entry in logfile</summary>
///<param name="origin">Message origin (ClassName.MethodName)</param>
///<param name="message">Message content (concatenated with errno message if available)</param>
void Logger::writeErrorEntry(const std::string origin, std::string message)
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