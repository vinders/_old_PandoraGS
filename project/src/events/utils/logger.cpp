/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : event log utility
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <string>
#include <ctime>
#include <stdexcept>
#ifdef _WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif
#include "../../globals.h"
#include "file_io.h"
#include "logger.h"
using namespace events::utils;

Logger* Logger::s_pInstance = NULL; ///< Singleton logger instance


///<summary>Create logger instance</summary>
Logger::Logger()
{
    m_filePath = FileIO::getWritableFilePath() + std::string("pandoraGS_log.csv");
}


// -- singleton -- ---------------------------------------------------------

/// @brief Create or get existing singleton instance
/// @return Log utility singleton instance
Logger* Logger::getInstance()
{
    if (s_pInstance == NULL)
        s_pInstance = new Logger();
    return s_pInstance;
}

/// @brief Close log utility singleton instance
void Logger::closeInstance()
{
    if (s_pInstance != NULL)
    {
        delete s_pInstance;
        s_pInstance = NULL;
    }
}


// -- getters / setters -- -------------------------------------------------

/// @brief Set the log file path
/// @param filePath Log file path
void Logger::setFilePath(const std::string filePath)
{
    m_filePath = filePath;
}

/// @brief Get the log file path
/// @return Log file path
std::string Logger::getFilePath() const
{
    return m_filePath;
}

/// @brief Get formatted date/time string
/// @return Date/time
void Logger::getDateTime(std::string& outDate, std::string& outTime) const
{
    time_t tm = time(0);
    #ifdef _WINDOWS
    struct tm now;
    if (localtime_s(&now, &tm) == 0)
    {
        outDate = std::to_string(now.tm_mday) + std::string("/") + std::to_string(now.tm_mon + 1)
            + std::string("/") + std::to_string(now.tm_year + 1900);
        outTime = std::to_string(now.tm_hour) + std::string(":") + std::to_string(now.tm_min)
            + std::string(":") + std::to_string(now.tm_sec);
    }
    else
    {
        outDate = "<undefined>";
        outTime = "00:00:00";
    }
    #else
    struct tm* now = localtime(&tm);
    outDate = std::to_string(now->tm_mday) + std::string("/") + std::to_string(now->tm_mon + 1)
        + std::string("/") + std::to_string(now->tm_year + 1900);
    outTime = std::to_string(now->tm_hour) + std::string(":") + std::to_string(now->tm_min)
        + std::string(":") + std::to_string(now->tm_sec);
    #endif
}


// -- log writing -- -------------------------------------------------------

/// @brief Write general entry in log file
/// @param origin Message origin (ClassName.MethodName)
/// @param type Message type description
/// @param message Message content
void Logger::writeEntry(const std::string origin, const std::string type, const std::string message)
{
    // create log message
    std::string curDate;
    std::string curTime;
    getDateTime(curDate, curTime);
    std::string logMsg = curDate + std::string(";") + curTime + std::string(";") + type + std::string(";") + origin + std::string(";") + message;

    m_mutexInstance.lock();
    try
    {
        // open logfile
        std::ofstream out;
        out.open(getFilePath(), std::ios_base::app); // append
        if (!out.is_open()) // create file if non-existing
        {
            out.open(getFilePath().c_str(), std::ios_base::trunc);
            out << "DATE;TIME;TYPE;ORIGIN;MESSAGE" << std::endl; // header
        }
        // write new entry
        out << logMsg << std::endl;
        out.close();
    }
    catch (...)
    {
        // output in terminal
        printf("%s", (std::string("LOG not written (") + type + std::string("): ") + origin + std::string(": ") + message).c_str());
    }
    m_mutexInstance.unlock();
}

/// @brief Write error or warning entry in log file
/// @param origin Message origin (ClassName.MethodName)
/// @param message Message content (concatenated with errno message if available)
void Logger::writeErrorEntry(const std::string origin, std::string message)
{
    const std::string type = std::to_string(errno); // error code
    if (errno != 0)
    {
        #ifdef _WINDOWS
        char buffer[160];
        strerror_s(buffer, (size_t)159, errno);
        message = message + std::string(": ") + buffer; // error message
        #else
        message = message + std::string(": ") + strerror(errno); // error message
        #endif
    }
    errno = 0;

    writeEntry(origin, type, message);
}

/// @brief Destroy log file
/// @throw Removal failure
void Logger::removeLogFile()
{
    m_mutexInstance.lock();
    int err = std::remove(getFilePath().c_str());
    m_mutexInstance.unlock();
    if (err)
        throw std::runtime_error("Unable to remove log file");
}
