/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : standard exception
*******************************************************************************/
#pragma once

#include <string>

/// @namespace event
/// Event management
namespace event
{
    /// @namespace event.utils
    /// Event management utilities
    namespace utils
    {
        /// @class StdException
        /// @brief Standard exception
        class StdException : public std::exception
        {
        protected:
            std::string m_message; ///< exception message
            
        public:
            /// @brief Default constructor - no message
            StdException() : m_message("") {}
            /// @brief Constructor with message
            /// @param message Exception message
            StdException(std::string const& message) : m_message(message) {}
            /// @brief Deep copy constructor
            /// @param copy Exception object reference
            StdException(const StdException& copy) : m_message(copy.m_message) {}
            
            /// @brief Get exception message
            /// @return Exception message
            virtual char const* what() const noexcept
            {
                return m_message.c_str();
            }
        };
    }
}
