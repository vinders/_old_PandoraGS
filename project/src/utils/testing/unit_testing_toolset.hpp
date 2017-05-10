/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : unit testing toolset - implementation (see 'unit_testing.h' for usage)
*******************************************************************************/
#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <memory>
#include <algorithm>
#include <functional>
#include <thread>
#include <chrono>
#include "object_verifier.hpp"
#include "string_verifier.hpp"
#include "collection_verifier.hpp"
#include "../display/ansi_color_codes.h"
#include "../assert.h"

#define _UTT_CONCURRENCY_THREAD_NB 4

// -- private macros --

// create unit test case
#define _UTT_BEGIN_UNIT_TEST(unitTestName,unitTestNameString,callBefore,callAfter) \
        namespace unit_testing  \
        { \
            void unitTestName(bool isConcurrency) \
            { \
                utils::testing::UnitTesting _ut(unitTestNameString, isConcurrency, callBefore, callAfter);
#define _UTT_END_UNIT_TEST() \
            } \
        }

// set functions to call before/after each test
#define _UTT_SET_PROCEDURE_CALLS(callBeforeEach, callAfterEach) \
        _ut.setCallBeforeEach(callBeforeEach); _ut.setCallAfterEach(callAfterEach)

// add procedure to unit test
#define _UTT_CREATE_UNIT_TEST_PROCEDURE(testNameString,callBefore,callAfter,procedure) \
        _ut.doTest(testNameString, callBefore, callAfter, [](std::string& msg)->bool procedure)

// report failed test
#define _UTT_FAILED_UNIT_TEST(failureMsg)  ASSERT(false); \
                                           msg = std::string("Error line ")+std::to_string(__LINE__)+std::string(": ")+failureMsg; \
                                           return false

// exception-safe test check
#define _UTT_CHECK_TEST(test,failureMsg)  try { \
                                              if ((test) == false) { \
                                                  _UTT_FAILED_UNIT_TEST(failureMsg); \
                                              } \
                                          } catch (...) { _UTT_FAILED_UNIT_TEST(failureMsg); }


/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.testing
    /// Testing utilities
    namespace testing
    {
        /// @class UnitTesting
        /// @brief Unit testing toolset
        class UnitTesting
        {
        public:
            /// @brief Create test instance for one unit
            /// @param[in] unitName       Name of tested unit
            /// @param[in] isConcurrency  Check concurrency or not
            /// @param[in] callBefore     Procedure to call before executing the first test
            /// @param[in] callAfter      Procedure to call after the last test (on destruction)
            UnitTesting(const std::string unitName, const bool isConcurrency, const std::function<void()> callBefore = {}, const std::function<void()> callAfter = {}) 
                : m_unitName(unitName), m_callAfterUnit(callAfter), m_totalTests(0u), m_failedTests(0u), m_isConcurrency(isConcurrency)
            {
                printUnitName();
                if (callBefore)
                    callBefore();
                m_startTime = std::chrono::system_clock::now();
            }
            /// @brief Destroy unit testing object + execute "after" callback
            ~UnitTesting()
            {
                auto totalTimeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_startTime);
                printGeneralResult(totalTimeElapsed.count());
                if (m_callAfterUnit)
                    m_callAfterUnit();
            }
            
            /// @brief Set procedure to call before each test
            /// @param[in] call  Procedure to call before each test
            void setCallBeforeEach(const std::function<void()> call)
            {
                m_callBeforeEach = call;
            }
            /// @brief Set procedure to call after each test
            /// @param[in] call  Procedure to call after each test
            void setCallAfterEach(const std::function<void()> call)
            {
                m_callAfterEach = call;
            }
            
            /// @brief Execute test method + before/after calls + result management
            /// @param[in] testName       Name of test function
            /// @param[in] callBefore     Procedure to call before the test
            /// @param[in] callAfter      Procedure to call after the test
            /// @param[in] testProcedure  Test function : - must return true at the end
            ///                                           - must use ASSERT_... macros for tests (defined on top of this file)
            ///                                           - must have (std::string& msg) argument
            void doTest(const std::string name, const std::function<void()> callBefore, const std::function<void()> callAfter, const std::function<bool(std::string&)> testCall)
            {
                if (m_callBeforeEach) // call before each test
                    m_callBeforeEach();
                if (callBefore) // pre-test
                    callBefore();
                
                ++m_totalTests;
                std::string msg;
                bool isSuccess = true;
                auto startTime = std::chrono::system_clock::now();
                
                // multi-thread test execution
                if (m_isConcurrency)
                {
                    // prepare test loop
                    bool isTestReady = false;
                    std::function<void()> testLoop = [&isTestReady, &isSuccess, &msg]()
                    {
                        while (isTestReady == false)
                            std::this_thread::yield();
                        
                        std::string privateMsg;
                        for (uint32_t i = 0; i < 4 && isSuccess; ++i)
                        {
                            if (testProcedure(privateMsg) == false)
                            {
                                isSuccess = false;
                                msg = privateMsg;
                                break;
                            }
                        }
                    };
                    
                    // execute threads
                    std::vector<std::thread> threads;
                    threads.reserve(_UTT_CONCURRENCY_THREAD_NB);
                    for (uint32_t i = 0; i < _UTT_CONCURRENCY_THREAD_NB; ++i)
                    {
                        threads.push_back(std::thread(testLoop));
                    }
                    isTestReady = true;
                    std::this_thread::yield();
                    
                    // wait until complete
                    for (uint32_t i = 0; i < _UTT_CONCURRENCY_THREAD_NB; ++i)
                    {
                        threads.at(i).join();
                    }
                }
                else // single thread test execution
                {
                    isSuccess = testProcedure(msg);
                }
                
                // result
                auto endTime = std::chrono::system_clock::now();
                auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                if (isSuccess) 
                {
                    UnitTesting::printSuccess(name, timeElapsed.count());
                }
                else
                {
                    UnitTesting::printFailure(name, msg, timeElapsed.count());
                    ++m_failedTests;
                }
                
                if (callAfter) // post-test
                    callAfter();
                if (m_callAfterEach) // call after each test
                    m_callAfterEach();
            }
            
        private:
            /// @brief Print unit test headline on screen
            void printUnitName()
            {
                printf("[---------] starting tests from %s", m_unitName.c_str());
                if (m_isConcurrency)
                    printf(" (with concurrency tests)");
                printf("\n");
            }
            /// @brief Print current test result on screen - success
            /// @param[in] testName     Name of test function
            /// @param[in] timeElapsed  Time elapsed during test
            static void printSuccess(const std::string name, const uint32_t timeElapsed)
            {
                INIT_ANSI_COLOR_SUPPORT();
                SET_ANSI_COLOR(ANSI_COLOR_GREEN);
                printf("[      OK ]");
                SET_ANSI_COLOR(ANSI_COLOR_RESET);
                printf(" %s.%s (%u ms)\n", m_unitName.c_str(), name.c_str(), timeElapsed);
            }
            /// @brief Print current test result on screen - failure
            /// @param[in] testName     Name of test function
            /// @param[in] msg          Error message
            /// @param[in] timeElapsed  Time elapsed during test
            static void printFailure(const std::string name, const std::string msg, const uint32_t timeElapsed)
            {
                INIT_ANSI_COLOR_SUPPORT();
                SET_ANSI_COLOR(ANSI_COLOR_RED);
                printf("[  FAILED ]");
                SET_ANSI_COLOR(ANSI_COLOR_RESET);
                printf(" %s.%s (%u ms)\n %s\n", m_unitName.c_str(), name.c_str(), timeElapsed, msg.c_str());
            }
            /// @brief Print statistics (number of successes and failures) on screen
            /// @param[in] totalTimeElapsed  Total time elapsed during unit testing
            void printGeneralResult(const uint32_t totalTimeElapsed)
            {
                INIT_ANSI_COLOR_SUPPORT();
                printf("[---------] %u tests from %s (%u ms total)\n", m_totalTests, m_unitName.c_str(), totalTimeElapsed);
                
                uint32_t succeededTests = m_totalTests - m_failedTests;
                SET_ANSI_COLOR(ANSI_COLOR_GREEN);
                printf("[ SUCCESS ]");
                SET_ANSI_COLOR(ANSI_COLOR_RESET);
                printf(" %u of %u tests succeeded.\n", succeededTests, m_totalTests);
                    
                if (m_failedTests > 0u)
                {
                    SET_ANSI_COLOR(ANSI_COLOR_RED);
                    printf("[ FAILURE ]");
                    SET_ANSI_COLOR(ANSI_COLOR_RESET);
                    printf(" %u tests failed.\n\n", m_failedTests);
                }
            }
            
            
        private:
            uint32_t m_totalTests;    ///< Number of failed tests
            uint32_t m_failedTests;   ///< Number of failed tests
            std::chrono::time_point<std::chrono::system_clock> m_startTime; ///< Time measured during object creation (for duration of tests)
            
            std::string m_unitName; ///< Name of tested unit
            bool m_isConcurrency;   ///< Enable concurrency testing (multiple threads)
            
            const std::function<void()> m_callBeforeEach; ///< Callback procedure, called before each test
            const std::function<void()> m_callAfterEach;  ///< Callback procedure, called after each test
            const std::function<void()> m_callAfterUnit;  ///< Callback procedure, called on object destruction
        };
    }
}
