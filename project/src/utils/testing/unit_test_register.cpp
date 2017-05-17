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
#include <chrono>
#include <stdexcept>
#include "unit_test_data.hpp"
#include "unit_test_notifier.hpp"
#include "unit_test_runner.hpp"
using namespace utils::testing;

std::vector<std::string> UnitTestRegister::s_registeredTestCaseNames;                 ///< List of registered test cases, by order of declaration
std::unordered_map<std::string,TestCaseData> UnitTestRegister::s_registeredTestCases; ///< Map of registered test cases and their tests

/// @brief Run test case
/// @param[in] currentTestCase    Test case to run
/// @param[in] isConcurrency      Enable concurrency testing (multiple executions with multiple threads)
/// @param[out] totalTests        Total number of tests (updated)
/// @param[out] totalFailedTests  Total number of failed tests (updated)
/// @returns Time elapsed
uint32_t UnitTestRegister::runCase(TestCaseData& currentTestCase, const bool useConcurrency, uint32_t& totalTests, uint32_t& totalFailedTests)
{
    uint32_t failedTests = 0u;
    UnitTestNotifier::printTestCaseName(currentTestCase.name, currentTestCase.unitTests.size(), useConcurrency);
    
    // init test case
    if (currentTestCase.initHook)
        currentTestCase.initHook();
    
    // run all tests from test case
    auto caseStartTime = std::chrono::system_clock::now();
    for (auto it = currentTestCase.unitTests.begin(); it != currentTestCase.unitTests.end(); ++it)
    {
        bool isSuccess = UnitTestRunner::doTest(currentTestCase, *it, useConcurrency);
        if (!isSuccess)
            ++failedTests;
    }
    auto caseEndTime = std::chrono::system_clock::now();
    
    // test case results
    auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(caseEndTime - caseStartTime).count();
    UnitTestNotifier::printTestCaseResult(currentTestCase.name, currentTestCase.unitTests.size(), failedTests, timeElapsed);
    totalTests += currentTestCase.unitTests.size();
    totalFailedTests += failedTests;
    
    // stop test case
    if (currentTestCase.closeHook)
        currentTestCase.closeHook();
    return timeElapsed;
}


/// @brief Run all tests
/// @param[in] isConcurrency  Enable concurrency testing (multiple executions with multiple threads)
/// @returns Success code (0 = success)
int UnitTestRegister::runTests(const bool useConcurrency)
{
    UnitTestNotifier::init();
    
    // run test cases, by order of declaration
    uint32_t totalTests = 0u;
    uint32_t totalFailedTests = 0u;
    uint32_t totalTime = 0u;
    for (auto it = s_registeredTestCaseNames.begin(); it != s_registeredTestCaseNames.end(); ++it)
    {
        totalTime += runCase(s_registeredTestCases[*it], useConcurrency, totalTests, totalFailedTests);
    }
    
    // global results
    UnitTestNotifier::printGlobalResult(s_registeredTestCaseNames.size(), totalTests, totalFailedTests, totalTime);
    if (totalFailedTests > 0u)
    {
        UnitTestNotifier::printFailedList(UnitTestRunner::getFailedTests());
        return -1;
    }
    return 0;
}


/// @brief Run all tests from one test case
/// @param[in] testCaseName   Test case to run
/// @param[in] isConcurrency  Enable concurrency testing (multiple executions with multiple threads)
/// @returns Success code (0 = success)
int UnitTestRegister::runTestCase(const std::string testCaseName, const bool useConcurrency)
{
    UnitTestNotifier::init();
    
    // run test case, if available
    uint32_t totalTests = 0u;
    uint32_t totalFailedTests = 0u;
    uint32_t totalTime = 0u;
    if (s_registeredTestCases.find(testCaseName) != s_registeredTestCases.end())
    {
        totalTime += runCase(s_registeredTestCases[testCaseName], useConcurrency, totalTests, totalFailedTests);
    }
    
    // global results
    UnitTestNotifier::printGlobalResult(1u, totalTests, totalFailedTests, totalTime);
    if (totalFailedTests > 0u)
    {
        UnitTestNotifier::printFailedList(UnitTestRunner::getFailedTests());
        return -1;
    }
    return 0;
}
