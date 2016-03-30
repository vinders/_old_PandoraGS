/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   log_utility.h
Description : messages log utility (CSV)
*******************************************************************************/
#ifndef LOG_UTILITY_H
#define LOG_UTILITY_H
#include "globals.h"

#include <string>

// Messages log utility (CSV file format)
class LogUtility
{
private:
    static LogUtility* s_pInstance; // singleton log instance
protected:
    pthread_mutex_t m_hMtxInstance; // unique access mutex
    std::string m_filePath;         // logfile path
    bool m_isReady;                 // init success indicator


private:
    ///<summary>Create logger instance</summary>
    ///<exception cref="std::exception">Mutex init failure</exception>
    LogUtility();
public:
    ///<summary>Destroy logger instance</summary>
    ~LogUtility();


    // -- SINGLETON -- ---------------------------------------------------------

    ///<summary>Create or get existing singleton instance</summary>
    ///<returns>Log utility singleton instance</returns>
    ///<exception cref="std::exception">Instance creation failure</exception>
    static LogUtility* getInstance();
    ///<summary>Close log utility singleton instance</summary>
    static void closeInstance();


    // -- GETTERS / SETTERS -- -------------------------------------------------

    ///<summary>Set the logfile path</summary>
    ///<param name="filePath">Logfile path</param>
    void setFilePath(const std::string filePath);
    ///<summary>Get the logfile path</summary>
    ///<returns>Logfile path</returns>
    std::string getFilePath() const;


    // -- LOG WRITING -- -------------------------------------------------------

    ///<summary>Write general entry in logfile</summary>
    ///<param name="origin">Message origin (ClassName.MethodName)</param>
    ///<param name="type">Message type description</param>
    ///<param name="message">Message content</param>
    void writeEntry(const std::string origin, const std::string type, const std::string message);
    ///<summary>Write error or warning entry in logfile</summary>
    ///<param name="origin">Message origin (ClassName.MethodName)</param>
    ///<param name="message">Message content (concatenated with errno message if available)</param>
    void writeErrorEntry(const std::string origin, std::string message);
};

#endif
