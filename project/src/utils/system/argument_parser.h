/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : parser for process command-line arguments
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.system
    /// System management utilities
    namespace system
    {
        /// @enum process_arg_data_type_t
        /// @brief Type of argument values
        enum class process_arg_data_type_t
        {
            boolean,  ///< Boolean typed value : "true", "false", "1", "0"
            uinteger, ///< Unsigned integer type value
            integer,  ///< Integer type value
            number,   ///< Floating-point value
            unumber,  ///< Unsigned floating-point value
            string    ///< Any string value
        };
        
        /// @struct process_arg_spec_t
        /// @brief Command argument specification
        struct process_arg_spec_t
        {
            std::string commandName; ///< Complete command name (e.g.: "-h", "--help", "async", "on", ...)
            std::vector<utils::system::process_arg_data_type_t> typedValues; ///< Expected values after command
        };
        /// @struct process_arg_spec_t
        /// @brief Command argument specification
        struct process_arg_data_t
        {
            char** values;       ///< Array of string values
            size_t valuesNumber; ///< Number of values
        };
        
        
        /// @class ArgumentParser
        /// @brief Parser for process command-line arguments
        class ArgumentParser
        {
        public:
            /// @brief Create process argument parser
            /// @param[in] commands  Allowed commands specification
            ArgumentParser(const std::vector<utils::system::process_arg_spec_t>& commands)
            {
                for (const auto& it : commands)
                {
                    ASSERT(!it.commandName.empty());
                    addCommand(it);
                }
            }
            /// @brief Copy other instance
            /// @param[in] other  Other instance
            ArgumentParser(const ArgumentParser& other) : m_allowedCommands(other.m_allowedCommands) {}
            /// @brief Move other instance
            /// @param[in] other  Other instance
            ArgumentParser(ArgumentParser&& other) : m_allowedCommands(std::move(other.m_allowedCommands)) {}
            
            /// @brief Assign copy of other instance
            /// @param[in] other  Other instance
            ArgumentParser& operator=(const ArgumentParser& other)
            {
                m_allowedCommands = other.m_allowedCommands;
                return *this;
            }
            /// @brief Assign moved instance
            /// @param[in] other  Other instance
            ArgumentParser& operator=(ArgumentParser&& other)
            {
                m_allowedCommands = std::move(other.m_allowedCommands);
                return *this;
            }
            
            
            // -- Setters --
            
            /// @brief Add command argument specification
            /// @param[in] command  Allowed command to add
            inline void addCommand(const utils::system::process_arg_spec_t& command)
            {
                ASSERT(!command.commandName.empty());
                m_allowedCommands[command.commandName] = command.typedValues;
            }
            /// @brief Remove command argument specification
            /// @param[in] commandName  Name of command to remove
            inline void removeCommand(const std::string& commandName)
            {
                ASSERT(!commandName.empty());
                if (m_allowedCommands.find(commandName) != m_allowedCommands.end())
                    m_allowedCommands.erase(commandName);
            }
            
            // -- Parser --

            /// @brief Parse argument received in the "main" entry point
            /// @param[in] argc  Total number of arguments
            /// @param[in] argv  Argument values (array of strings)
            /// @returns Map with found commands : key = command name, value = pointer to first value (or nullptr if no value)
            /// @throws std::invalid_argument  Unknown command found, repeated command found or missing/invalid argument values
            std::unordered_map<std::string,utils::system::process_arg_data_t> parse(int argc, char** argv) const;
            
        protected:
            /// @brief Check argument data validity
            /// @param[in] type   Expected data type
            /// @param[in] value  Argument value
            /// @throws std::invalid_argument  Invalid data type
            void checkDataType(const utils::system::process_arg_data_type_t type, char* value) const;

            
        private:
            std::unordered_map<std::string,std::vector<utils::system::process_arg_data_type_t>> m_allowedCommands
        };
    }
}
