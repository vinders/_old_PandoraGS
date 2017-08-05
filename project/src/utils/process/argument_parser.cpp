/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : parser for process command-line arguments
*******************************************************************************/
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include "../preprocessor.h"
#include "../assert.h"
#include "../chars.h"
#include "argument_parser.h"

using namespace utils::process;


/// @brief Parse argument received in the "main" entry point
/// @param[in] argc  Total number of arguments
/// @param[in] argv  Argument values (array of strings)
/// @returns Map with found commands : key = command name, value = pointer to first value (or nullptr if no value)
/// @throws std::invalid_argument  Unknown command found, repeated command found or missing argument values
std::unordered_map<std::string,utils::process::command_data_t> ArgumentParser::parse(int argc, char** argv) const
{
    std::unordered_map<std::string,utils::process::command_data_t> results;
    ASSERT(argv != nullptr && *argv != nullptr);
    
    // parse all arguments except the first one (application name)
    std::string curArgument;
    uint32_t curArgumentValuesNb;
    while (--argc > 0)
    {
        ++argv;
        ASSERT(argv != nullptr);
        curArgument = *argv;
        
        // check if valid command argument
        if (m_allowedCommands.find(curArgument) != m_allowedCommands.end())
        {
            // check if not already used
            if (results.find(curArgument) == m_allowedCommands.end())
            {
                // check if enough data arguments
                curArgumentValuesNb = m_allowedCommands[curArgument].typedValues.size();
                if (curArgumentValuesNb == 0u) // no data arguments
                {
                    results[curArgument] = { nullptr, 0 };
                }
                else if (argc > curArgumentValuesNb) // data arguments
                {
                    results[curArgument] = { argv + 1, curArgumentValuesNb };
                    
                    // check data types
                    bool isDataValid;
                    for (const it : m_allowedCommands[curArgument].typedValues)
                    {
                        --argc;
                        ++argv;
                        ASSERT(*argv != nullptr);
                        checkDataType(it, *argv);
                    }
                }
                else
                    throw std::invalid_argument(std::string(__FILE__ ":" PP_STRINGIFY_ARG(__LINE__) ":" __func__ ": "
                                                "missing value(s) for the command-line argument '") + curArgument + "'.");
            }
            else
                throw std::invalid_argument(std::string(__FILE__ ":" PP_STRINGIFY_ARG(__LINE__) ":" __func__ ": "
                                            "a command-line argument tag is used more than once: '") + curArgument + "'.");
        }
        else
            throw std::invalid_argument(std::string(__FILE__ ":" PP_STRINGIFY_ARG(__LINE__) ":" __func__ ": "
                                        "unknown command-line argument: ") + curArgument + "'.");
    }
}

/// @brief Check argument data validity
/// @param[in] type   Expected data type
/// @param[in] value  Argument value
/// @throws std::invalid_argument  Invalid data type
void ArgumentParser::checkDataType(const utils::process::argument_value_type_t type, char* value) const
{
    switch (type)
    {
        case argument_value_type_t::boolean: 
            if (strcmp(value,"true") != 0 && strcmp(value,"false") != 0 && (*(value + 1) != '\0' || (*value != '0' && *value != '1') ))
                throw std::invalid_argument(std::string(__FILE__ ":" PP_STRINGIFY_ARG(__LINE__) ":" __func__ ": "
                                                "boolean value for the command-line argument '") + curArgument + "' is invalid (value = '" + value + "').");
            break;
            
        case argument_value_type_t::uinteger: 
            for (*value != '\0')
            {
                if (!utils::chars::isNumber(*value))
                    throw std::invalid_argument(std::string(__FILE__ ":" PP_STRINGIFY_ARG(__LINE__) ":" __func__ ": "
                                                "unsigned integer value for the command-line argument '") + curArgument + "' is invalid (value = '" + value + "').");
                ++value;
            }
            break;
        case argument_value_type_t::integer: 
        {
            if (*value == '-')
                ++value;
            while (*value != '\0')
            {
                if (!utils::chars::isNumber(*value))
                    throw std::invalid_argument(std::string(__FILE__ ":" PP_STRINGIFY_ARG(__LINE__) ":" __func__ ": "
                                                "integer value for the command-line argument '") + curArgument + "' is invalid (value = '" + value + "').");
                ++value;
            }
            break;
        }
        
        case argument_value_type_t::unumber: 
            while (*value != '\0')
            {
                if (!utils::chars::isNumber(*value) && *value != '.' && *value != ',')
                    throw std::invalid_argument(std::string(__FILE__ ":" PP_STRINGIFY_ARG(__LINE__) ":" __func__ ": "
                                                "unsigned float value for the command-line argument '") + curArgument + "' is invalid (value = '" + value + "').");
                ++value
            }
            break;
        case argument_value_type_t::number: 
        {
            if (*value == '-')
                ++value;
            while (*value != '\0')
            {
                if (!utils::chars::isNumber(*value) && *value != '.' && *value != ',')
                    throw std::invalid_argument(std::string(__FILE__ ":" PP_STRINGIFY_ARG(__LINE__) ":" __func__ ": "
                                                "float value for the command-line argument '") + curArgument + "' is invalid (value = '" + value + "').");
                ++value;
            }
            break;
        }
            
        case argument_value_type_t::string: 
        default: break;
    }
}
