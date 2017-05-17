/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : unit testing toolset - unit test register
*******************************************************************************/
#pragma once

#include <cstdlib>
#include <cstddef>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "unit_test_data.hpp"
#include "unit_test_notifier.hpp"
#include "unit_test_runner.hpp"

#define _UT_STRINGIFY(name) #name


// -- private macros --

// create test case
#define _UT_CREATE_TEST_CASE(testCaseName,initHook,closeHook,preTestHook,postTestHook) \
        class TestCase##testCaseName \
        { \
        public: \
            TestCase##testCaseName() \
            { \
                UnitTestRegister::registerTestCase(std::string(_UT_STRINGIFY(testCaseName)),initHook,closeHook,preTestHook,postTestHook); \
            } \
        }; \
        static TestCase##testCaseName _testCase_##testCaseName

// create test function
#define _UT_CREATE_TEST_FUNCTION(testCaseName,testName,allowConcurrency,preTestHook,postTestHook) \
        void _fct_##testCaseName##testName(); \
        class UT##testCaseName##testName \
        { \
        public: \
            UT##testCaseName##testName() \
            { \
                UnitTestRegister::registerUnitTest(std::string(_UT_STRINGIFY(testCaseName)),std::string(_UT_STRINGIFY(testName)),allowConcurrency,_fct_##testCaseName##testName,preTestHook,postTestHook); \
            } \
        }; \
        static UT##testCaseName##testName _ut_##testCaseName##testName; \
        void _fct_##testCaseName##testName() \

// report failed test
#define _UT_REPORT_FAILED_TEST(msg)  ASSERT(false); throw std::runtime_error(msg); return;

// test check
#define _UT_CHECK_TEST(test,failureMsg)  if ((test) == false) { \
                                              _UT_REPORT_FAILED_TEST(failureMsg); \
                                          } \

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.testing
    /// Testing utilities
    namespace testing
    {
        /// @class UnitTestRegister
        /// @brief Unit test registration class
        class UnitTestRegister
        {
        public:
            /// @brief Register test case to execute
            /// @param[in] name          Name of the test case
            /// @param[in] initHook      Hook to run once, before the first test
            /// @param[in] closeHook     Hook to run once, after the last test
            /// @param[in] preTestHook   Default hook to run before each test
            /// @param[in] postTestHook  Default hook to run after each test
            static void registerTestCase(const std::string name, const std::function<void()> initHook, const std::function<void()> closeHook, 
                                         const std::function<void()> preTestHook = {}, const std::function<void()> postTestHook = {})
            {
                if (s_registeredTestCases.find(name) == s_registeredTestCases.end())
                {
                    s_registeredTestCaseNames.push_back(name);
                    s_registeredTestCases[name] = TestCaseData(name, initHook, closeHook, preTestHook, postTestHook);
                }
            }
            
            /// @brief Register unit test for a test case
            /// @param[in] testCase          Name of the test case
            /// @param[in] name              Name of the unit test
            /// @param[in] allowConcurrency  Allow concurrency if requested by test case
            /// @param[in] testFunction      Test function with assertions
            /// @param[in] preTestHook       Hook to run before the test, instead of the default one of the test case
            /// @param[in] postTestHook      Hook to run after the test, instead of the default one of the test case
            static void registerUnitTest(const std::string testCase, const std::string name, const bool allowConcurrency, const void (*testFunction)(void), 
                                         const std::function<void()> preTestHook = {}, const std::function<void()> postTestHook = {})
            {
                if (s_registeredTestCases.find(testCase) != s_registeredTestCases.end())
                {
                    s_registeredTestCases[testCase].unitTests.push_back(UnitTestData(name, allowConcurrency, testFunction, preTestHook, postTestHook));
                }
            }
            
            /// @brief Run all tests
            /// @param[in] isConcurrency  Enable concurrency testing (multiple executions with multiple threads)
            /// @returns Success code (0 = success)
            static int runTests(const bool useConcurrency);
            /// @brief Run all tests (with exception catched)
            /// @param[in] isConcurrency  Enable concurrency testing (multiple executions with multiple threads)
            /// @returns Success code (0 = success)
            static int tryRunTests(const bool useConcurrency)
            {
                int resultCode = -1;
                try
                {
                    resultCode = runTests(useConcurrency);
                }
                catch (const std::exception& exc)
                {
                    printf("Exception - %s", exc.what());
                }
                catch (...)
                {
                    printf("Error - Unhandled object thrown");
                }
                return resultCode;
            }
            
            /// @brief Run all tests from one test case
            /// @param[in] testCaseName   Test case to run
            /// @param[in] isConcurrency  Enable concurrency testing (multiple executions with multiple threads)
            /// @returns Success code (0 = success)
            static int runTestCase(const std::string testCaseName, const bool useConcurrency);
            /// @brief Run all tests from one test case
            /// @param[in] testCaseName   Test case to run
            /// @param[in] isConcurrency  Enable concurrency testing (multiple executions with multiple threads)
            /// @returns Success code (0 = success)
            static int tryRunTestCase(const std::string testCaseName, const bool useConcurrency)
            {
                int resultCode = -1;
                try
                {
                    resultCode = runTestCase(testCaseName, useConcurrency);
                }
                catch (const std::exception& exc)
                {
                    printf("Exception - %s", exc.what());
                }
                catch (...)
                {
                    printf("Error - Unhandled object thrown");
                }
                return resultCode;
            }
            
            
        private:
            /// @brief Run test case
            /// @param[in] currentTestCase    Test case to run
            /// @param[in] isConcurrency      Enable concurrency testing (multiple executions with multiple threads)
            /// @param[out] totalTests        Total number of tests (updated)
            /// @param[out] totalFailedTests  Total number of failed tests (updated)
            /// @returns Time elapsed
            uint32_t UnitTestRegister::runCase(TestCaseData& currentTestCase, const bool useConcurrency, uint32_t totalTests, uint32_t totalFailedTests);
            
            static std::vector<std::string> s_registeredTestCaseNames;                 ///< List of registered test cases, by order of declaration
            static std::unordered_map<std::string,TestCaseData> s_registeredTestCases; ///< Map of registered test cases and their tests
        };
    }
}
