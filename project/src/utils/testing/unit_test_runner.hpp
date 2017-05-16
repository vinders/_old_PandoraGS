/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : unit testing toolset - test runner
*******************************************************************************/
#pragma once

#include <cstdlib>
#include <cstddef>
#include <string>
#include <vector>
#include <thread>
#include "unit_test_data.hpp"
#include "unit_test_notifier.hpp"
#include "../assert.h"

#define _UT_CONCURRENCY_THREAD_NB 4


/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.testing
    /// Testing utilities
    namespace testing
    {
        /// @class UnitTestRunner
        /// @brief Unit test execution
        class UnitTestRunner
        {
        public:
            static bool doTest(const TestCaseData& testCase, const UnitTestData& testData, const bool useConcurrency)
            {
                bool isSuccess = true;
                std::string outputError;
                
                // pre-test hook
                if (testData.preTestHook)
                {
                    testData.preTestHook();
                }
                else if (testCase.preTestHook)
                {
                    testCase.preTestHook();
                }
                auto startTime = std::chrono::system_clock::now();
                
                
                // multi-thread test execution
                if (useConcurrency && testData.isConcurrencyAllowed)
                {
                    // prepare test loop
                    bool isTestReady = false;
                    std::function<void()> testLoop = [&isTestReady, &isSuccess, &output]()
                    {
                        while (isTestReady == false)
                            std::this_thread::yield();
                        
                        try
                        {
                            for (uint32_t i = 0; i < 4 && isSuccess; ++i)
                                testData.testFunction();
                        } 
                        catch (const std::exception& exc)
                        {
                            isSuccess = false;
                            outputError = exc.what();
                        }
                        catch (...)
                        {
                            isSuccess = false;
                            outputError = "Unhandled thrown object...";
                        }
                    };
                    
                    // execute threads
                    std::vector<std::thread> threads;
                    threads.reserve(_UT_CONCURRENCY_THREAD_NB);
                    for (uint32_t i = 0; i < _UT_CONCURRENCY_THREAD_NB; ++i)
                        threads.push_back(std::thread(testLoop));
                    isTestReady = true;
                    std::this_thread::yield();
                    
                    // wait until complete
                    for (uint32_t i = 0; i < _UT_CONCURRENCY_THREAD_NB; ++i)
                        threads.at(i).join();
                }
                else // single thread test execution
                {
                    try
                    {
                        testData.testFunction(output);
                    } 
                    catch (const std::exception& exc)
                    {
                        isSuccess = false;
                        outputError = exc.what();
                    }
                    catch (...)
                    {
                        isSuccess = false;
                        outputError = "Unhandled thrown object...";
                    }
                }
                        
                        
                // test results
                auto endTime = std::chrono::system_clock::now();
                auto testTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
                if (isSuccess)
                {
                    UnitTestNotifier::printSuccess(testCase.name, testData.name, testTime);
                }
                else
                {
                    UnitTestNotifier::printFailure(testCase.name, testData.name, outputError, testTime);
                    s_failedTests.push_back(testCase.name + std::string(".") + testData.name);
                }
                
                // post-test hook
                if (testData.postTestHook)
                {
                    testData.postTestHook();
                }
                else if (testCase.postTestHook)
                {
                    testCase.postTestHook();
                }
                return isSuccess;
            }
            
            /// @brief Get list of failed tests
            /// @returns List of failed tests
            static std::vector<std::string> getFailedTests()
            {
                return s_failedTests;
            }
            
            
        private:
            static std::vector<std::string> s_failedTests;
        };
    }
}
