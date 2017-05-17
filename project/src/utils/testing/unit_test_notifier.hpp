/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : unit testing toolset - message notifier
*******************************************************************************/
#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <vector>
#include "../window/drawing/ansi_color_codes.h"


/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.testing
    /// Testing utilities
    namespace testing
    {
        /// @class UnitTestNotifier
        /// @brief Unit test notification class
        class UnitTestNotifier
        {
        public:
            /// @brief Initialize notifier color system
            static void init()
            {
                INIT_ANSI_COLOR_SUPPORT();
            }
        
        
            /// @brief Print temporary unit test title
            /// @param[in] testCase     Name of test case
            /// @param[in] name         Name of test function
            static void printTestTitle(const std::string& testCase, const std::string& name)
            {
                printf(" %s.%s...", testCase.c_str(), name.c_str());
                fflush(stdin);
            }
            /// @brief Move output cursor back to the beginning of the line (to overwrite title)
            /// @param[in] testCase     Name of test case
            /// @param[in] name         Name of test function
            static void resetTitlePosition(const std::string& testCase, const std::string& name)
            {
                size_t length = testCase.size() + name.size() + 5;
                char eraser = new char[length + 1];
                memset(eraser, '\b', length);
                eraser[length] = '\0';
                printf(eraser);
                fflush(stdin);
                delete [] eraser;
            }
            
            
            /// @brief Print unit test result - success
            /// @param[in] testCase     Name of test case
            /// @param[in] name         Name of test function
            /// @param[in] timeElapsed  Time elapsed during test
            static void printSuccess(const std::string& testCase, const std::string& name, uint32_t timeElapsed)
            {
                SET_ANSI_COLOR(ANSI_COLOR_GREEN);
                printf("[      OK ]");
                SET_ANSI_COLOR(ANSI_COLOR_RESET);
                printf(" %s.%s (%u ms)\n", testCase.c_str(), name.c_str(), timeElapsed);
            }
            
            /// @brief Print unit test result - failure
            /// @param[in] testCase     Name of test case
            /// @param[in] name          Name of test function
            /// @param[in] errorMessage  Detailed error message
            /// @param[in] timeElapsed   Time elapsed during test
            static void printFailure(const std::string& testCase, const std::string& name, const std::string& errorMessage, uint32_t timeElapsed)
            {
                SET_ANSI_COLOR(ANSI_COLOR_RED);
                printf("[  FAILED ]");
                SET_ANSI_COLOR(ANSI_COLOR_RESET);
                printf(" %s.%s (%u ms)\n %s\n", testCase.c_str(), name.c_str(), timeElapsed, msg.c_str());
            }
            
            
            /// @brief Print test case headline
            /// @param[in] testCase        Name of test case
            /// @param[in] totalTests      Total number of unit tests
            /// @param[in] useConcurrency  Concurrency testing state
            static void printTestCaseName(const std::string& testCase, uint32_t totalTests, bool useConcurrency)
            {
                printf("[---------] %u tests from %s", totalTests, testCase.c_str());
                if (useConcurrency)
                    printf(" (concurrency ON)");
                printf("\n");
            }
            
            /// @brief Print test case statistics (number of successes and failures)
            /// @param[in] testCase     Name of test case
            /// @param[in] totalTests   Total number of unit tests
            /// @param[in] failedTests  Total number of failed tests
            /// @param[in] timeElapsed  Total time elapsed during test case
            static void printTestCaseResult(const std::string& testCase, uint32_t totalTests, uint32_t failedTests, uint32_t timeElapsed)
            {
                printf("[---------] %u tests from %s (%u ms total)\n", totalTests, testCase.c_str(), timeElapsed);
                
                uint32_t succeededTests = totalTests - failedTests;
                SET_ANSI_COLOR(ANSI_COLOR_GREEN);
                printf("[ SUCCESS ]");
                SET_ANSI_COLOR(ANSI_COLOR_RESET);
                printf(" %u of %u tests succeeded.\n", succeededTests, totalTests);
                    
                if (failedTests > 0u)
                {
                    SET_ANSI_COLOR(ANSI_COLOR_RED);
                    printf("[ FAILURE ]");
                    SET_ANSI_COLOR(ANSI_COLOR_RESET);
                    printf(" %u tests failed.\n", failedTests);
                }
                printf("\n");
            }
            
            
            /// @brief Print global statistics (number of successes and failures)
            /// @param[in] testCases    Total number of test cases
            /// @param[in] totalTests   Total number of unit tests
            /// @param[in] failedTests  Total number of failed tests
            /// @param[in] timeElapsed  Total time elapsed during unit testing
            static void printGlobalResult(const uint32_t testCases, uint32_t totalTests, uint32_t failedTests, uint32_t timeElapsed)
            {
                printf("[---------] Global results: %u tests from %u test cases (%u ms)\n", totalTests, testCases, timeElapsed);
                
                uint32_t succeededTests = totalTests - failedTests;
                SET_ANSI_COLOR(ANSI_COLOR_GREEN);
                printf("[ SUCCESS ]");
                SET_ANSI_COLOR(ANSI_COLOR_RESET);
                printf(" %u of %u tests succeeded.\n", succeededTests, totalTests);
                    
                if (failedTests > 0u)
                {
                    SET_ANSI_COLOR(ANSI_COLOR_RED);
                    printf("[ FAILURE ]");
                    SET_ANSI_COLOR(ANSI_COLOR_RESET);
                    printf(" %u tests failed.\n", failedTests);
                }
            }
            
            /// @brief Print list of failed tests
            /// @param[in] failedTests  List of failed tests
            static void printFailedList(const std::vector<std::string> failedTests)
            {
                for (auto it = failedTests.begin(); it != failedTests.end(); ++it)
                {
                    SET_ANSI_COLOR(ANSI_COLOR_RED);
                    printf("[  FAILED ]");
                    SET_ANSI_COLOR(ANSI_COLOR_RESET);
                    printf(" %s\n", *it);
                }
            }
            
            
        private:
            static DECLARE_ANSI_COLOR_SUPPORT();
        };
    }
}
