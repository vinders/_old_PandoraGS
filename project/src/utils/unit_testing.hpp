/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : unit testing toolset
*******************************************************************************/
#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <memory>
#include <algorithm>
#include <functional>
#include "verifier.hpp"
#include "assert.h"

// macro helpers
#define _FAILED_TEST(failureMsg)  ASSERT(false); \
                                  msg = std::string("Error line ")+std::to_string(__LINE__)+std::string(": ")+failureMsg; \
                                  return false
#define _CHECK_TEST(test,failureMsg)  try { if (test == false) { _FAILED_TEST(failureMsg); } } catch (...) { _FAILED_TEST(failureMsg); }


// -- how to use --
/*
CREATE_TEST_PROC(myTest1)
{
    ASSERT_EQ(2, 3);
    return true;
}
int main()
{
    // new scope, to allow UnitTesting object destruction
    { 
        std::string unitName("myUnit");
        UnitTesting ut(unitName);
        ut.executeTest("myTest1", myTest1);
        // end of the scope -> ut destroyed -> result is displayed
    }
}
// note that you can pass callbacks to execute 
//      - before/after the whole unit testing :   UnitTesting ut(unitName, myBefore, myAfter);
//      - before/after each test :   ut.executeTest("myTest1", myTest1, myBefore, myAfter);
// use void() signature for callback methods
*/

// -- macros to use --
#define CREATE_TEST_PROC(nameWithoutQuotes)   bool nameWithoutQuotes(std::string& msg)

#define ASSERT_THROWS(test)    try { (test); _FAILED_TEST(); } catch (...) {}
#define ASSERT_NO_THROW(test)  try { (test); } catch (...) { _FAILED_TEST(); }
   
#define ASSERT_TRUE(test)   _CHECK_TEST(Verifier::Boolean::isTrue(test),  std::string("expected: true ; value: false"))
#define ASSERT_FALSE(test)  _CHECK_TEST(Verifier::Boolean::isFalse(test), std::string("expected: false ; value: true"))

#define ASSERT_NULL(test)                _CHECK_TEST(Verifier::Object::isNull(test),    std::string("expected: nullptr ; value: not null"))
#define ASSERT_NOT_NULL(test)            _CHECK_TEST(Verifier::Object::isNotNull(test), std::string("expected: not null; value: nullptr"))
#define ASSERT_INSTANCE(class,test)      _CHECK_TEST(Verifier::Object::isInstanceOf<class>(test),    std::string("not an instance of specified class"))
#define ASSERT_NOT_INSTANCE(class,test)  _CHECK_TEST(Verifier::Object::isNotInstanceOf<class>(test), std::string("is instance of specified class"))

#define ASSERT_EQ(test,val)         _CHECK_TEST(Verifier::Compare::isEqual(test,val),    (std::string("expected == ")+val)+std::string("; value: ")+test)
#define ASSERT_NOT_EQ(test,val)     _CHECK_TEST(Verifier::Compare::isNotEqual(test,val), (std::string("expected != ")+val)+std::string("; value: ")+test)
#define ASSERT_LOWER(test,val)      _CHECK_TEST(Verifier::Compare::isLowerThan(test,val),    (std::string("expected < ")+val)+std::string("; value: ")+test)
#define ASSERT_LOWER_EQ(test,val)   _CHECK_TEST(Verifier::Compare::isLowerEqual(test,val), (std::string("expected <= ")+val)+std::string("; value: ")+test)
#define ASSERT_HIGHER(test,val)     _CHECK_TEST(Verifier::Compare::isHigherThan(test,val),    (std::string("expected > ")+val)+std::string("; value: ")+test)
#define ASSERT_HIGHER_EQ(test,val)  _CHECK_TEST(Verifier::Compare::isHigherEqual(test,val), (std::string("expected >= ")+val)+std::string("; value: ")+test)

#define ASSERT_STR_EMPTY(test)      _CHECK_TEST(Verifier::String::isEmpty(test), std::string("expected: \"\"; value: \")+test+std::string("\""))
#define ASSERT_STR_NOT_EMPTY(test)  _CHECK_TEST(Verifier::String::isNotEmpty(test), std::string("expected: not empty; value: \"\""))
//...
// -- macros to use -- end


/// @namespace utils
/// General utilities
namespace utils
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
            : m_unitName(unitName), m_callAfter(callAfter), failedTests(0u)
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
        /// @param[in] testName    Name of test function
        /// @param[in] testProcedure  Test function : - must return true at the end
        ///                                           - must use ASSERT_... macros for tests (defined on top of this file)
        ///                                           - must have (std::string& msg) argument
        void executeTest(const std::string testName, const std::function<bool(std::string&)> testProcedure, const std::function<void()> callBefore = {}, const std::function<void()> callAfter = {})
        {
            printTestName(testName);
            callBefore();
            
            std::string msg;
            if (testProcedure(msg))
            {
                printSuccess();
            }
            else
            {
                printFailure(msg);
                ++failedTests;
            }
            callAfter();
        }
        
    private:
        void printUnitName()
        {
            //...
        }
        static void printTestName(std::string name)
        {
            //...
        }
        static void printSuccess()
        {
            //...
        }
        static void printFailure(std::string msg)
        {
            //...
        }
        
        
    private:
        uint32_t failedTests;   ///< Number of failed tests
        std::string m_unitName; ///< Name of tested unit
        const std::function<void()> m_callAfter; ///< Callback procedure, called on object destruction
    };
}
