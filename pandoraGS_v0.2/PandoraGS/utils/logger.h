/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   logger.h
Description : advanced log utility (csv)
*******************************************************************************/
#ifndef _LOGGER_H
#define _LOGGER_H

#include <string>
#include <mutex>

// Advanced log utility (CSV)
class Logger
{
private:
    static Logger* s_pInstance; // singleton logger instance
    std::mutex m_hMtxInstance; // unique access mutex
    std::string m_filePath;    // logfile path
    
private:
    ///<summary>Create logger instance</summary>
    Logger();
public:

    // -- SINGLETON -- ---------------------------------------------------------

    ///<summary>Create or get existing singleton instance</summary>
    ///<returns>Log utility singleton instance</returns>
    static Logger* getInstance();
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
