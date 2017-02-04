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
            Logger() noexcept;
        public:

            // -- singleton -- ---------------------------------------------------------

            /// @brief Create or get existing singleton instance
            /// @returns Log utility singleton instance
            static Logger* getInstance() noexcept;

            /// @brief Close log utility singleton instance
            static void closeInstance() noexcept;


            // -- getters / setters -- -------------------------------------------------

            /// @brief Set the log file path
            /// @param[in] filePath  Log file path
            void setFilePath(const std::string filePath) noexcept;

            /// @brief Get the log file path
            /// @returns Log file path
            std::string getFilePath() const noexcept;

            /// @brief Get formatted date/time string
            /// @returns Date/time
            void getDateTime(std::string& outDate, std::string& outTime) const noexcept;


            // -- log writing -- -------------------------------------------------------

            /// @brief Write general entry in log file
            /// @param[in] origin   Message origin (ClassName.MethodName)
            /// @param[in] type     Message type description
            /// @param[in] message  Message content
            void writeEntry(const std::string origin, const std::string type, const std::string message) noexcept;

            /// @brief Write error or warning entry in log file
            /// @param[in] origin   Message origin (ClassName.MethodName)
            /// @param[in] message  Message content (concatenated with errno message if available)
            void writeErrorEntry(const std::string origin, std::string message) noexcept;

            /// @brief Destroy log file
            /// @throws runtime_error  Removal failure
            void removeLogFile();
        };
    }
}
