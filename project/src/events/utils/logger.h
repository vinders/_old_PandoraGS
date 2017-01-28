/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : event log utility
*******************************************************************************/
#pragma once

#include <string>
#include <mutex>

/// @namespace events
/// Event management
namespace events
{
    /// @namespace events.utils
    /// Event management utilities
    namespace utils
    {
        /// @class Logger
        /// @brief Event log utility
        class Logger
        {
        private:
            static Logger* s_pInstance; ///< Singleton logger instance
            std::mutex m_mutexInstance; ///< Unique access mutex
            std::string m_filePath;     ///< Log file path

        private:
            /// @brief Create logger instance
            Logger();
        public:

            // -- singleton -- ---------------------------------------------------------

            /// @brief Create or get existing singleton instance
            /// @return Log utility singleton instance
            static Logger* getInstance();

            /// @brief Close log utility singleton instance
            static void closeInstance();


            // -- getters / setters -- -------------------------------------------------

            /// @brief Set the log file path
            /// @param filePath Log file path
            void setFilePath(const std::string filePath);

            /// @brief Get the log file path
            /// @return Log file path
            std::string getFilePath() const;

            /// @brief Get formatted date/time string
            /// @return Date/time
            void getDateTime(std::string& outDate, std::string& outTime) const;


            // -- log writing -- -------------------------------------------------------

            /// @brief Write general entry in log file
            /// @param origin Message origin (ClassName.MethodName)
            /// @param type Message type description
            /// @param message Message content
            void writeEntry(const std::string origin, const std::string type, const std::string message);

            /// @brief Write error or warning entry in log file
            /// @param origin Message origin (ClassName.MethodName)
            /// @param message Message content (concatenated with errno message if available)
            void writeErrorEntry(const std::string origin, std::string message);

            /// @brief Destroy log file
            /// @throw Removal failure
            void removeLogFile();
        };
    }
}
