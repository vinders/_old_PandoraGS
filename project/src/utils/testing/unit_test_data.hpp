/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : unit testing toolset - implementation (see 'unit_testing.h' for usage)
*******************************************************************************/
#pragma once

#include <cstdlib>
#include <cstddef>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>


/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.testing
    /// Testing utilities
    namespace testing
    {
        /// @struct UnitTestData
        /// @brief Test execution data container
        struct UnitTestData
        {
            const std::string name;
            const bool isConcurrencyAllowed;
            const std::function<void()> preTestHook;
            const std::function<void()> postTestHook;
            const void (*testFunction)(std::string&);
            
            /// @brief Create test container
            UnitTestData(const std::string _name, const bool allowConcurrency, const void (*_testFunction)(void), 
                         const std::function<void()> _preTestHook = {}, const std::function<void()> _postTestHook = {}) :
                name(_name),
                isConcurrencyAllowed(allowConcurrency),
                testFunction(_testFunction),
                preTestHook(_preTestHook),
                postTestHook(_postTestHook) {}
        };
        
        /// @struct TestCaseData
        /// @brief Test case group container
        struct TestCaseData
        {
            const std::string name;
            const std::function<void()> initHook;
            const std::function<void()> closeHook;
            const std::function<void()> preTestHook;
            const std::function<void()> postTestHook;
            const std::vector<UnitTestData> unitTests;
            
            /// @brief Create test case container
            TestCaseData(std::string _name, std::function<void()> _initHook, std::function<void()> _closeHook, 
                         std::function<void()> _preTestHook = {}, std::function<void()> _postTestHook = {}) :
                name(_name),
                initHook(_initHook),
                closeHook(_closeHook),
                preTestHook(_preTestHook),
                postTestHook(_postTestHook) {}
        };
    }
}
