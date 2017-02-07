/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : event log utility
*******************************************************************************/
#include "../../globals.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <string>
#include <ctime>
#include <stdexcept>
#ifdef _WINDOWS
//#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif
using namespace std::literals::string_literals;
#include "file_io.h"
#include "logger.h"
using namespace events::utils;

Logger* Logger::s_pInstance = NULL; ///< Singleton logger instance


/// @biref Create logger instance
Logger::Logger() noexcept
{
    m_filePath = FileIO::getWritableFilePath() + "pandoraGS_log.csv"s;
}


// -- singleton -- ---------------------------------------------------------

/// @brief Create or get existing singleton instance
/// @returns Log utility singleton instance
Logger* Logger::getInstance() noexcept
{
    if (s_pInstance == nullptr)
        s_pInstance = new Logger();
    return s_pInstance;
}

/// @brief Close log utility singleton instance
void Logger::closeInstance() noexcept
{
    if (s_pInstance != nullptr)
    {
        delete s_pInstance;
        s_pInstance = nullptr;
    }
}


// -- getters / setters -- -------------------------------------------------

/// @brief Set the log file path
/// @param[in] filePath  Log file path
void Logger::setFilePath(const std::string filePath) noexcept
{
    m_filePath = filePath;
}

/// @brief Get the log file path
/// @returns Log file path
std::string Logger::getFilePath() const noexcept
{
    return m_filePath;
}

/// @brief Get formatted date/time string
/// @returns Date/time
void Logger::getDateTime(std::string& outDate, std::string& outTime) const noexcept
{
    time_t tm = time(0);
    #ifdef _WINDOWS
    struct tm now;
    if (localtime_s(&now, &tm) == 0)
    {
        outDate = std::to_string(now.tm_mday) + "/"s + std::to_string(now.tm_mon + 1) + "/"s + std::to_string(now.tm_year + 1900);
        outTime = std::to_string(now.tm_hour) + ":"s + std::to_string(now.tm_min) + ":"s + std::to_string(now.tm_sec);
    }
    else
    {
        outDate = "<undefined>"s;
        outTime = "00:00:00"s;
    }
    #else
    struct tm* now = localtime(&tm);
    outDate = std::to_string(now->tm_mday) + "/"s + std::to_string(now->tm_mon + 1) + "/"s + std::to_string(now->tm_year + 1900);
    outTime = std::to_string(now->tm_hour) + ":"s + std::to_string(now->tm_min) + ":"s + std::to_string(now->tm_sec);
    #endif
}


// -- log writing -- -------------------------------------------------------

/// @brief Write general entry in log file
/// @param[in] origin   Message origin (ClassName.MethodName)
/// @param[in] type     Message type description
/// @param[in] message  Message content
void Logger::writeEntry(const std::string origin, const std::string type, const std::string message) noexcept
{
    // create log message
    std::string curDate;
    std::string curTime;
    getDateTime(curDate, curTime);
    std::string logMsg = curDate + ";"s + curTime + ";"s + type + ";"s + origin + ";"s + message;

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
        printf("%s", (std::string("LOG not written (") + type + "): "s + origin + ": "s + message).c_str());
    }
    m_mutexInstance.unlock();
}

/// @brief Write error or warning entry in log file
/// @param[in] origin   Message origin (ClassName.MethodName)
/// @param[in] message  Message content (concatenated with errno message if available)
void Logger::writeErrorEntry(const std::string origin, std::string message) noexcept
{
    const std::string type = std::to_string(errno); // error code
    if (errno != 0)
    {
        #ifdef _WINDOWS
        char buffer[160];
        strerror_s(buffer, sizeof(buffer), errno);
        message = message + ": "s + buffer; // error message
        #else
        message = message + ": "s + strerror(errno); // error message
        #endif
    }
    errno = 0;

    writeEntry(origin, type, message);
}

/// @brief Destroy log file
/// @throws runtime_error  Removal failure
void Logger::removeLogFile()
{
    m_mutexInstance.lock();
    int err = std::remove(getFilePath().c_str());
    m_mutexInstance.unlock();
    if (err)
        throw std::runtime_error("Unable to remove log file");
}
