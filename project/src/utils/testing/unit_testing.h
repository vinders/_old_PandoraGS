/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : unit testing tool
*******************************************************************************/
#pragma once

#include "unit_test_data.hpp"
#include "unit_test_register.hpp"
#include "unit_test_notifier.hpp"
#include "unit_test_runner.hpp"
#include "../object_verifier.hpp"
#include "../string_verifier.hpp"
#include "../collection_verifier.hpp"


// ----------------
// -- how to use --
// ----------------
/*
// Define test case (group) with CREATE_TEST_CASE in a source file (.cpp) :
//      CREATE_TEST_CASE(testCaseName,[initUniqueHook,closeUniqueHook,preTestHook,postTestHook]);
//      - testCaseName :               test case name (without quotes, must start with a letter).
//      - initUniqueHook (optional) :  special hook to execute once, before the whole test case.
//      - closeUniqueHook (optional) : special hook to execute once, after the whole test case.
//      - preTestHook (optional) :     special hook to execute before each unit test function.
//      - postTestHook (optional) :    special hook to execute after each unit test function.

// Define unit test (function) with CREATE_TEST_CASE in a source file (.cpp) :
//      CREATE_TEST_FUNCTION(testCaseName,testName,[allowConcurrency,preTestHookOverride,postTestHookOverride]) { ... }
//      - testCaseName :               test case name (without quotes, must start with a letter).
//      - allowConcurrency (optional) : allow or disable concurrency (allowed by default) 
                                        -> if global concurrency check is enabled, disabling it here will prevent it for this function.
//      - preTestHook (optional) :     special hook to execute before the unit test function (if defined, will override the one defined in the test case).
//      - postTestHook (optional) :    special hook to execute after the unit test function (if defined, will override the one defined in the test case).

// Use lambdas ([](void){ ... }) or std::function<void()> to declare the hooks.

// In the program entry point, you can execute all test cases at once, with RUN_ALL_TESTS(useConcurrency) :
        RUN_ALL_TESTS(false);  // simple execution, one test after another.
        RUN_ALL_TESTS(true);   // concurrency check : tests executed multiple times by multiple threads.
        
// Inside the test functions, use asserts to check the validity of your unit. See list of asserts below, at the end of this file.


// - simple example -
// -----------------

CREATE_TEST_CASE(myTestCase1);              // required, before declaring any test

CREATE_TEST_FUNCTION(myTestCase1, MyTest1)  // example of test
{
    int i = 2;
    ASSERT_EQ(i, 2);
}

void main()    // program entry point
{
    RUN_ALL_TESTS(false);
}


// - advanced example -
// --------------------

CREATE_TEST_CASE(myTestCase2,
                 [](void) { printf("start"); },          // called once before whole test case
                 [](void) { printf("end");   },          // called once after whole test case
                 [](void) { printf("new test"); },       // called before each test
                 [](void) { printf("end of test"); });   // called after each test

CREATE_TEST_FUNCTION(myTestCase2, MySimpleTest)
{
    int i = 2;
    ASSERT_EQ(i, 2);
}

CREATE_TEST_FUNCTION(myTestCase2, MyTestWithCustomHooks,
                     [](void) { printf("overriden start"); },   // called only before this specific test
                     [](void) { printf("overriden end !"); })   // called only after this specific test
{
    bool boolArray[] = { true, false };
    ASSERT_TRUE(boolArray[0]);
    ASSERT_ARRAY_CONTAINS(false);
}

void main()
{
    RUN_ALL_TESTS(true);   // enable concurrency -> multi-threading
}
*/


// -- macros to declare tests --

// run unit tests
#define RUN_ALL_TESTS(useConcurrency)    UnitTestRegister::runTests(useConcurrency)

// create test case
#define CREATE_TEST_CASE(testCaseName)                                                          _UT_CREATE_TEST_CASE(testCaseName,[](void){},[](void){},[](void){},[](void){})
#define CREATE_TEST_CASE(testCaseName,initUniqueHook)                                           _UT_CREATE_TEST_CASE(testCaseName,initUniqueHook,[](void){},[](void){},[](void){})
#define CREATE_TEST_CASE(testCaseName,initUniqueHook,closeUniqueHook)                           _UT_CREATE_TEST_CASE(testCaseName,initUniqueHook,closeUniqueHook,[](void){},[](void){})
#define CREATE_TEST_CASE(testCaseName,initUniqueHook,closeUniqueHook,preTestHook)               _UT_CREATE_TEST_CASE(testCaseName,initUniqueHook,closeUniqueHook,preTestHook,[](void){})
#define CREATE_TEST_CASE(testCaseName,initUniqueHook,closeUniqueHook,preTestHook,postTestHook)  _UT_CREATE_TEST_CASE(testCaseName,initUniqueHook,closeUniqueHook,preTestHook,postTestHook)

// create test function
#define CREATE_TEST_FUNCTION(testCaseName,testName)                                                            _UT_CREATE_TEST_FUNCTION(testCaseName,testName,true,[](void){},[](void){})
#define CREATE_TEST_FUNCTION(testCaseName,testName,allowConcurrency)                                           _UT_CREATE_TEST_FUNCTION(testCaseName,testName,allowConcurrency,[](void){},[](void){})
#define CREATE_TEST_FUNCTION(testCaseName,testName,allowConcurrency,preTestHookOverride)                       _UT_CREATE_TEST_FUNCTION(testCaseName,testName,allowConcurrency,preTestHookOverride,[](void){})
#define CREATE_TEST_FUNCTION(testCaseName,testName,allowConcurrency,preTestHookOverride,postTestHookOverride)  _UT_CREATE_TEST_FUNCTION(testCaseName,testName,allowConcurrency,preTestHookOverride,postTestHookOverride)


// -- assertions to use in tests --

// exceptions
#define ASSERT_THROWS(test)                  try { (test); _UT_REPORT_FAILED_TEST(); } catch (...) {}
#define ASSERT_NO_THROW(test)                try { (test); } catch (...) { _UT_REPORT_FAILED_TEST(); }

// booleans
#define ASSERT_TRUE(test)                    _UT_CHECK_TEST(test,    std::string("expected: true ; value: false"))
#define ASSERT_FALSE(test)                   _UT_CHECK_TEST(!(test), std::string("expected: false ; value: true"))

// comparisons
#define ASSERT_EQ(test,val)                  _UT_CHECK_TEST((test == val), (std::string("expected == ")+val)+std::string("; value: ")+test)
#define ASSERT_NOT_EQ(test,val)              _UT_CHECK_TEST((test != val), (std::string("expected != ")+val)+std::string("; value: ")+test)
#define ASSERT_LOWER(test,val)               _UT_CHECK_TEST((test < val),  (std::string("expected < ")+val)+std::string("; value: ")+test)
#define ASSERT_LOWER_EQ(test,val)            _UT_CHECK_TEST((test <= val), (std::string("expected <= ")+val)+std::string("; value: ")+test)
#define ASSERT_GREATER(test,val)             _UT_CHECK_TEST((test > val),  (std::string("expected > ")+val)+std::string("; value: ")+test)
#define ASSERT_GREATER_EQ(test,val)          _UT_CHECK_TEST((test >= val), (std::string("expected >= ")+val)+std::string("; value: ")+test)

// objects state
#define ASSERT_NULL(obj)                     _UT_CHECK_TEST(utils::ObjectVerifier::isNull(obj),                std::string("expected: nullptr ; value: not null"))
#define ASSERT_NOT_NULL(obj)                 _UT_CHECK_TEST(!utils::ObjectVerifier::isNull(obj),               std::string("expected: not null; value: nullptr"))
#define ASSERT_INSTANCE(class,obj)           _UT_CHECK_TEST(utils::ObjectVerifier::isInstanceOf<class>(obj),   std::string("not an instance of specified class"))
#define ASSERT_NOT_INSTANCE(class,obj)       _UT_CHECK_TEST(!utils::ObjectVerifier::isInstanceOf<class>(obj),  std::string("is instance of specified class"))

// strings
#define ASSERT_STR_EMPTY(str)                _UT_CHECK_TEST(utils::StringVerifier::isEmpty(str),         std::string("expected: \"\"; value: \")+str+std::string("\""))
#define ASSERT_STR_NOT_EMPTY(str)            _UT_CHECK_TEST(!utils::StringVerifier::isEmpty(str),        std::string("expected: not empty; value: \"\""))
#define ASSERT_STR_EQ(str,val)               _UT_CHECK_TEST(utils::StringVerifier::equals(str,val),      std::string("expected: \"")+val+std::string("\"; value: \")+str+std::string("\""))
#define ASSERT_STR_NOT_EQ(str,val)           _UT_CHECK_TEST(!utils::StringVerifier::equals(str,val),     std::string("expected: != \"")+val+std::string("\"; value: \")+str+std::string("\""))
#define ASSERT_STR_START(str,val)            _UT_CHECK_TEST(utils::StringVerifier::startsWith(str,val),  std::string("expected: \"")+val+std::string("\"[...]; value: \")+str+std::string("\""))
#define ASSERT_STR_NOT_START(str,val)        _UT_CHECK_TEST(!utils::StringVerifier::startsWith(str,val), std::string("expected: \"")+val+std::string("\"[...]; value: \")+str+std::string("\""))
#define ASSERT_STR_END(str,val)              _UT_CHECK_TEST(utils::StringVerifier::endsWith(str,val),    std::string("expected: [...]\"")+val+std::string("\"; value: \")+str+std::string("\""))
#define ASSERT_STR_NOT_END(str,val)          _UT_CHECK_TEST(!utils::StringVerifier::endsWith(str,val),   std::string("expected: [...]\"")+val+std::string("\"; value: \")+str+std::string("\""))
#define ASSERT_STR_CONTAINS(str,val)         _UT_CHECK_TEST(utils::StringVerifier::contains(str,val),    std::string("expected: [...]\"")+val+std::string("\"[...]; value: \")+str+std::string("\""))
#define ASSERT_STR_NOT_CONTAINS(str,val)     _UT_CHECK_TEST(!utils::StringVerifier::contains(str,val),   std::string("expected: [...]\"")+val+std::string("\"[...]; value: \")+str+std::string("\""))

// arrays / hashes
#define ASSERT_ARRAY_EMPTY(array)            _UT_CHECK_TEST(utils::CollectionVerifier::isEmpty(array),           std::string("expected: {}; value: not empty"))
#define ASSERT_ARRAY_NOT_EMPTY(array)        _UT_CHECK_TEST(!utils::CollectionVerifier::isEmpty(array),          std::string("expected: not empty; value: {}"))
#define ASSERT_ARRAY_CONTAINS(array,val)     _UT_CHECK_TEST(utils::CollectionVerifier::contains(array,val),      std::string("value not found in array"))
#define ASSERT_ARRAY_CONTAINS(array,key,val) _UT_CHECK_TEST(utils::CollectionVerifier::contains(array,key,val),  std::string("key or value not found in hash"))
#define ASSERT_ARRAY_LACKS(array,val)        _UT_CHECK_TEST(!utils::CollectionVerifier::contains(array,val),     std::string("excluded value is in array"))
#define ASSERT_ARRAY_LACKS(array,key,val)    _UT_CHECK_TEST(!utils::CollectionVerifier::contains(array,key,val), std::string("excluded key and value is in hash"))
