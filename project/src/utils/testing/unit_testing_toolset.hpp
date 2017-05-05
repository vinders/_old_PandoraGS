/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : unit testing toolset - implementation (include 'unit_testing.h' for your tests)
*******************************************************************************/
#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <memory>
#include <algorithm>
#include <functional>
#include "object_verifier.hpp"
#include "string_verifier.hpp"
#include "collection_verifier.hpp"
#include "../device/ansi_color_codes.h"
#include "../assert.h"

// -- private macros --

// create unit test
#define _UTT_BEGIN_UNIT_TEST(unitTestName,unitTestNameString,callBefore,callAfter) \
        namespace unit_testing  \
        { \
            void unitTestName() \
            { \
                utils::testing::UnitTesting _ut(unitTestNameString, callBefore, callAfter);
#define _UTT_END_UNIT_TEST() \
                _ut.printResult(); \
            } \
        }

// add procedure to unit test
#define _UTT_CREATE_UNIT_TEST_PROCEDURE(testNameString,callBefore,callAfter,procedure) \
        _ut.doTest(testNameString, callBefore, callAfter, [](std::string& msg)->bool procedure)

// report failed test
#define _UTT_FAILED_UNIT_TEST(failureMsg)  ASSERT(false); \
                                  msg = std::string("Error line ")+std::to_string(__LINE__)+std::string(": ")+failureMsg; \
                                  return false

// exception-safe test check
#define _UTT_CHECK_TEST(test,failureMsg)  try { if ((test) == false) { _UTT_FAILED_UNIT_TEST(failureMsg); } } catch (...) { _UTT_FAILED_UNIT_TEST(failureMsg); }


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
            /// @param[in] unitName    Name of tested unit
            /// @param[in] callBefore  Procedure to call before executing the first test
            /// @param[in] callAfter   Procedure to call after the last test (on destruction)
            UnitTesting(const std::string unitName, const std::function<void()> callBefore = {}, const std::function<void()> callAfter = {}) 
                : m_unitName(unitName), m_callAfter(callAfter), totalTests(0u), failedTests(0u)
            {
                printUnitName();
                // beginning indicator
                if (callBefore)
                    callBefore();
            }
            /// @brief Destroy unit testing object + execute "after" callback
            ~UnitTesting()
            {
                if (m_callAfter)
                    m_callAfter();
                // end indicator
                printUnitName();
                if (failedTests == 0u)
                    printSuccess();
                else
                    printFailure(std::to_string(failedTests) + std::string(" failed test(s)"));
            }
            
            /// @brief Execute overriden doTest method + before/after calls + result management
            /// @param[in] testName       Name of test function
            /// @param[in] callBefore     Procedure to call before the test
            /// @param[in] callAfter      Procedure to call after the test
            /// @param[in] testProcedure  Test function : - must return true at the end
            ///                                           - must use ASSERT_... macros for tests (defined on top of this file)
            ///                                           - must have (std::string& msg) argument
            void doTest(const std::string name, const std::function<void()> callBefore, const std::function<void()> callAfter, const std::function<bool(std::string&)> testCall)
            {
                printTestName(name);
                if (callBefore) // pre-test
                    callBefore();
                
                ++totalTests;
                std::string msg;
                if (testProcedure(msg)) // test
                {
                    printSuccess();
                }
                else
                {
                    printFailure(msg);
                    ++failedTests;
                }
                
                if (callAfter) // post-test
                    callAfter();
            }
            
            /// @brief Print current statistics (number of successes and failures) on screen
            void printResult()
            {
                INIT_ANSI_COLOR_SUPPORT();
                uint32_t succeededTests = totalTests - failedTests;
                if (failedTests > 0u)
                {
                    printf(" %s complete - ", m_unitName.c_str());
                    SET_ANSI_COLOR(ANSI_COLOR_RED);
                    printf("[ FAILURE ]");
                    SET_ANSI_COLOR(ANSI_COLOR_RESET);
                    printf("\n %u of %u tests succeeded.", succeededTests, totalTests);
                    printf("\n %u tests failed.\n\n", failedTests);
                }
                else
                {
                    printf(" %s complete - ", m_unitName.c_str());
                    SET_ANSI_COLOR(ANSI_COLOR_GREEN);
                    printf("[ SUCCESS ]");
                    SET_ANSI_COLOR(ANSI_COLOR_RESET);
                    printf("\n %u of %u tests succeeded.", succeededTests, totalTests);
                    printf("\n %u tests failed.\n\n", failedTests);
                    printf("%u of %u tests succeeded.\n\n", succeededTests, totalTests);
                }
                printf("---\n\n");
            }
            
        private:
            /// @brief Print unit test headline on screen
            void printUnitName()
            {
                printf("--------------------\n %s - start of tests\n--------------------\n\n", m_unitName.c_str());
            }
            /// @brief Print current test name on screen
            static void printTestName(std::string name)
            {
                printf("   %s - ", name.c_str());
            }
            /// @brief Print current test result on screen - success
            static void printSuccess()
            {
                INIT_ANSI_COLOR_SUPPORT();
                SET_ANSI_COLOR(ANSI_COLOR_GREEN);
                printf("[ SUCCESS ]\n");
                SET_ANSI_COLOR(ANSI_COLOR_RESET);
            }
            /// @brief Print current test result on screen - failure
            /// @param[in] msg  Error message
            static void printFailure(std::string msg)
            {
                INIT_ANSI_COLOR_SUPPORT();
                SET_ANSI_COLOR(ANSI_COLOR_RED);
                printf("[ FAILURE ] - %s\n", msg.c_str());
                SET_ANSI_COLOR(ANSI_COLOR_RESET);
            }
            
            
        private:
            uint32_t totalTests;    ///< Number of failed tests
            uint32_t failedTests;   ///< Number of failed tests
            std::string m_unitName; ///< Name of tested unit
            const std::function<void()> m_callAfter; ///< Callback procedure, called on object destruction
        };
    }
}
