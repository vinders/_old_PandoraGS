/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : unit testing tool
*******************************************************************************/
#pragma once

#include "unit_test_data.hpp"
#include "unit_test_register.h"
#include "unit_test_notifier.hpp"
#include "unit_test_runner.hpp"
#include "../object_verifier.hpp"
#include "../string_verifier.hpp"
#include "../collection_verifier.hpp"


// ----------------
// -- how to use --
// ----------------
/*
// Define test case (group) with TEST_CASE in a source file (.cpp) :
//      TEST_CASE(testCaseName,[initUniqueHook,closeUniqueHook,preTestHook,postTestHook]);
//      - testCaseName :               test case name (without quotes, must start with a letter).
//      - initUniqueHook (optional) :  special hook to execute once, before the whole test case.
//      - closeUniqueHook (optional) : special hook to execute once, after the whole test case.
//      - preTestHook (optional) :     special hook to execute before each unit test function.
//      - postTestHook (optional) :    special hook to execute after each unit test function.

// Define unit test (function) with TEST_CASE in a source file (.cpp) :
//      TEST_FCT(testCaseName,testName,[allowConcurrency,preTestHookOverride,postTestHookOverride]) { ... }
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

TEST_CASE(myTestCase1);              // required, before declaring any test

TEST_FCT(myTestCase1, MyTest1)  // example of test
{
    int i = 2;
    ASSERT_EQ(i, 2);
}

int main()    // program entry point
{
    return RUN_ALL_TESTS(false);
}


// - advanced example -
// --------------------

TEST_CASE(myTestCase2,
                 [](void) { printf("start"); },          // called once before whole test case
                 [](void) { printf("end");   },          // called once after whole test case
                 [](void) { printf("new test"); },       // called before each test
                 [](void) { printf("end of test"); });   // called after each test

TEST_FCT(myTestCase2, MySimpleTest)
{
    int i = 2;
    ASSERT_EQ(i, 2);
}

TEST_FCT(myTestCase2, MyThreadUnsafeTest, false)  // force concurrency to be disabled for this test
{
    static bool isTrue = true;
    ASSERT_TRUE(isTrue);
}

TEST_FCT(myTestCase2, MyTestWithCustomHooks, true,
                     [](void) { printf("overriden start"); },   // called only before this specific test
                     [](void) { printf("overriden end !"); })   // called only after this specific test
{
    bool boolArray[] = { true, false };
    ASSERT_TRUE(boolArray[0]);
    ASSERT_COLL_FIND(false);
}

int main()
{
    return RUN_ALL_TESTS(true);   // enable concurrency -> multi-threading
}
*/


// -- macros to declare tests --

// run unit tests
#define RUN_ALL_TESTS(useConcurrency)    UnitTestRegister::tryRunTests(useConcurrency)

// create test case
#define TEST_CASE(testCaseName)                                                          _UT_CREATE_TEST_CASE(testCaseName,[](void){},[](void){},[](void){},[](void){})
#define TEST_CASE(testCaseName,initUniqueHook)                                           _UT_CREATE_TEST_CASE(testCaseName,initUniqueHook,[](void){},[](void){},[](void){})
#define TEST_CASE(testCaseName,initUniqueHook,closeUniqueHook)                           _UT_CREATE_TEST_CASE(testCaseName,initUniqueHook,closeUniqueHook,[](void){},[](void){})
#define TEST_CASE(testCaseName,initUniqueHook,closeUniqueHook,preTestHook)               _UT_CREATE_TEST_CASE(testCaseName,initUniqueHook,closeUniqueHook,preTestHook,[](void){})
#define TEST_CASE(testCaseName,initUniqueHook,closeUniqueHook,preTestHook,postTestHook)  _UT_CREATE_TEST_CASE(testCaseName,initUniqueHook,closeUniqueHook,preTestHook,postTestHook)

// create test function
#define TEST_FCT(testCaseName,testName)                                                            _UT_CREATE_TEST_FUNCTION(testCaseName,testName,true,[](void){},[](void){})
#define TEST_FCT(testCaseName,testName,allowConcurrency)                                           _UT_CREATE_TEST_FUNCTION(testCaseName,testName,allowConcurrency,[](void){},[](void){})
#define TEST_FCT(testCaseName,testName,allowConcurrency,preTestHookOverride)                       _UT_CREATE_TEST_FUNCTION(testCaseName,testName,allowConcurrency,preTestHookOverride,[](void){})
#define TEST_FCT(testCaseName,testName,allowConcurrency,preTestHookOverride,postTestHookOverride)  _UT_CREATE_TEST_FUNCTION(testCaseName,testName,allowConcurrency,preTestHookOverride,postTestHookOverride)


// -- assertions to use in tests --

// exceptions (throwing / not throwing)
#define ASSERT_THROW(test)                   try { (test); _UT_REPORT_FAILED_TEST(); } catch (...) {}
#define ASSERT_NOTHROW(test)                 try { (test); } catch (...) { _UT_REPORT_FAILED_TEST(); }

// booleans (true / false)
#define ASSERT_TRUE(test)                    _UT_CHECK_TEST(test,    std::string("expected: true ; value: false"))
#define ASSERT_FALSE(test)                   _UT_CHECK_TEST(!(test), std::string("expected: false ; value: true"))

// comparisons (equal / not equal / lower / lower or equal / greater / greater or equal)
#define ASSERT_EQ(test,val)                  _UT_CHECK_TEST((test == val), (std::string("expected == ")+val)+std::string("; value: ")+test)
#define ASSERT_NEQ(test,val)                 _UT_CHECK_TEST((test != val), (std::string("expected != ")+val)+std::string("; value: ")+test)
#define ASSERT_LOW(test,val)                 _UT_CHECK_TEST((test < val),  (std::string("expected < ")+val)+std::string("; value: ")+test)
#define ASSERT_LOWEQ(test,val)               _UT_CHECK_TEST((test <= val), (std::string("expected <= ")+val)+std::string("; value: ")+test)
#define ASSERT_GREAT(test,val)               _UT_CHECK_TEST((test > val),  (std::string("expected > ")+val)+std::string("; value: ")+test)
#define ASSERT_GREATEQ(test,val)             _UT_CHECK_TEST((test >= val), (std::string("expected >= ")+val)+std::string("; value: ")+test)

// objects (null / not null / instance of / not an instance of)
#define ASSERT_NULL(obj)                     _UT_CHECK_TEST(utils::ObjectVerifier::isNull(obj),                std::string("expected: nullptr ; value: not null"))
#define ASSERT_NNULL(obj)                    _UT_CHECK_TEST(!utils::ObjectVerifier::isNull(obj),               std::string("expected: not null; value: nullptr"))
#define ASSERT_INST(class,obj)               _UT_CHECK_TEST(utils::ObjectVerifier::isInstanceOf<class>(obj),   std::string("not an instance of specified class"))
#define ASSERT_NINST(class,obj)              _UT_CHECK_TEST(!utils::ObjectVerifier::isInstanceOf<class>(obj),  std::string("is instance of specified class"))

// strings (empty / not empty / equal / not equal / beginning with / not beginning with / ending with / not ending with / containing / not containing)
#define ASSERT_STR_EMPTY(str)                _UT_CHECK_TEST(utils::StringVerifier::isEmpty(str),         std::string("expected: \"\"; value: \")+str+std::string("\""))
#define ASSERT_STR_NEMPTY(str)               _UT_CHECK_TEST(!utils::StringVerifier::isEmpty(str),        std::string("expected: not empty; value: \"\""))
#define ASSERT_STR_EQ(str,val)               _UT_CHECK_TEST(utils::StringVerifier::equals(str,val),      std::string("expected: \"")+val+std::string("\"; value: \")+str+std::string("\""))
#define ASSERT_STR_NEQ(str,val)              _UT_CHECK_TEST(!utils::StringVerifier::equals(str,val),     std::string("expected: != \"")+val+std::string("\"; value: \")+str+std::string("\""))
#define ASSERT_STR_BEG(str,val)              _UT_CHECK_TEST(utils::StringVerifier::startsWith(str,val),  std::string("expected: \"")+val+std::string("\"[...]; value: \")+str+std::string("\""))
#define ASSERT_STR_NBEG(str,val)             _UT_CHECK_TEST(!utils::StringVerifier::startsWith(str,val), std::string("expected: \"")+val+std::string("\"[...]; value: \")+str+std::string("\""))
#define ASSERT_STR_END(str,val)              _UT_CHECK_TEST(utils::StringVerifier::endsWith(str,val),    std::string("expected: [...]\"")+val+std::string("\"; value: \")+str+std::string("\""))
#define ASSERT_STR_NEND(str,val)             _UT_CHECK_TEST(!utils::StringVerifier::endsWith(str,val),   std::string("expected: [...]\"")+val+std::string("\"; value: \")+str+std::string("\""))
#define ASSERT_STR_FIND(str,val)             _UT_CHECK_TEST(utils::StringVerifier::contains(str,val),    std::string("expected: [...]\"")+val+std::string("\"[...]; value: \")+str+std::string("\""))
#define ASSERT_STR_NFIND(str,val)            _UT_CHECK_TEST(!utils::StringVerifier::contains(str,val),   std::string("expected: [...]\"")+val+std::string("\"[...]; value: \")+str+std::string("\""))

// collections (empty / not empty / containing value / containing key with value / not containing value / not containing key with value)
#define ASSERT_COLL_EMPTY(array)             _UT_CHECK_TEST(utils::CollectionVerifier::isEmpty(array),           std::string("expected: {}; value: not empty"))
#define ASSERT_COLL_NEMPTY(array)            _UT_CHECK_TEST(!utils::CollectionVerifier::isEmpty(array),          std::string("expected: not empty; value: {}"))
#define ASSERT_COLL_FIND(array,val)          _UT_CHECK_TEST(utils::CollectionVerifier::contains(array,val),      std::string("value not found in array"))
#define ASSERT_HASH_FIND(array,key,val)      _UT_CHECK_TEST(utils::CollectionVerifier::contains(array,key,val),  std::string("key or value not found in hash"))
#define ASSERT_COLL_NFIND(array,val)         _UT_CHECK_TEST(!utils::CollectionVerifier::contains(array,val),     std::string("excluded value is in array"))
#define ASSERT_HASH_NFIND(array,key,val)     _UT_CHECK_TEST(!utils::CollectionVerifier::contains(array,key,val), std::string("excluded key and value is in hash"))
