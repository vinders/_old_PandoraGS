/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : unit testing tool
*******************************************************************************/
#pragma once

#include "unit_testing_toolset.hpp"


// -- how to use --
/*
BEGIN_UNIT_TEST(myUnitTest1, "myUnitTest1", myBefore, myAfter); // required, before first procedure

// example of procedure
CREATE_UNIT_TEST_PROCEDURE("myTest", {
    int i = 2;
    ASSERT_EQ(i, 2);
    return true;
});

END_UNIT_TEST(); // required, after last procedure


// program entry point
void main()
{
    unit_testing::myUnitTest1();  // call unit test (execute all procedures)
}


// Between BEGIN_UNIT_TEST and END_UNIT_TEST, only procedures can be declared.
// If the program entry point is in a different file, use DECLARE_UNIT_TEST_HEADER(unitTestName).
// Note that you can pass callbacks to execute 
//      - before/after the whole unit testing :
//        CREATE_UNIT_TEST(myUnitTest1, "myUnitTest1", myBefore, myAfter)
//      - before/after each test :
//        CREATE_UNIT_TEST_PROCEDURE("myTest", myBefore, myAfter, {});
//      - use lambda or std::function for myBefore and myAfter, with signature void(void).
*/


// -- macros to declare test procedures --

// declare unit test (h/hpp file, if called from a different file)
#define DECLARE_UNIT_TEST_HEADER(unitTestName)                                     namespace unit_testing { void unitTestName(); }

// create unit test (cpp file)
#define BEGIN_UNIT_TEST(unitTestName,unitTestNameString)                           _UTT_BEGIN_UNIT_TEST(unitTestName,unitTestNameString,[](void){},[](void){})
#define BEGIN_UNIT_TEST(unitTestName,unitTestNameString,callBefore,callAfter)      _UTT_BEGIN_UNIT_TEST(unitTestName,unitTestNameString,callBefore,callAfter)
#define END_UNIT_TEST()                                                            _UTT_END_UNIT_TEST()

// add procedure to unit test
#define CREATE_UNIT_TEST_PROCEDURE(testNameString,procedure)                       _UTT_CREATE_UNIT_TEST_PROCEDURE(testNameString,[](void){},[](void){},procedure)
#define CREATE_UNIT_TEST_PROCEDURE(testNameString,callBefore,callAfter,procedure)  _UTT_CREATE_UNIT_TEST_PROCEDURE(testNameString,callBefore,callAfter,procedure)


// -- macros to use in tests --

// exceptions
#define ASSERT_THROWS(test)                  try { (test); _UTT_FAILED_UNIT_TEST(); } catch (...) {}
#define ASSERT_NO_THROW(test)                try { (test); } catch (...) { _UTT_FAILED_UNIT_TEST(); }

// booleans
#define ASSERT_TRUE(test)                    _UTT_CHECK_TEST(test,    std::string("expected: true ; value: false"))
#define ASSERT_FALSE(test)                   _UTT_CHECK_TEST(!(test), std::string("expected: false ; value: true"))

// comparisons
#define ASSERT_EQ(test,val)                  _UTT_CHECK_TEST((test == val), (std::string("expected == ")+val)+std::string("; value: ")+test)
#define ASSERT_NOT_EQ(test,val)              _UTT_CHECK_TEST((test != val), (std::string("expected != ")+val)+std::string("; value: ")+test)
#define ASSERT_LOWER(test,val)               _UTT_CHECK_TEST((test < val),  (std::string("expected < ")+val)+std::string("; value: ")+test)
#define ASSERT_LOWER_EQ(test,val)            _UTT_CHECK_TEST((test <= val), (std::string("expected <= ")+val)+std::string("; value: ")+test)
#define ASSERT_HIGHER(test,val)              _UTT_CHECK_TEST((test > val),  (std::string("expected > ")+val)+std::string("; value: ")+test)
#define ASSERT_HIGHER_EQ(test,val)           _UTT_CHECK_TEST((test >= val), (std::string("expected >= ")+val)+std::string("; value: ")+test)

// objects state
#define ASSERT_NULL(test)                    _UTT_CHECK_TEST(utils::testing::ObjectVerifier::isNull(test),                 std::string("expected: nullptr ; value: not null"))
#define ASSERT_NOT_NULL(test)                _UTT_CHECK_TEST(utils::testing::ObjectVerifier::isNotNull(test),              std::string("expected: not null; value: nullptr"))
#define ASSERT_INSTANCE(class,test)          _UTT_CHECK_TEST(utils::testing::ObjectVerifier::isInstanceOf<class>(test),    std::string("not an instance of specified class"))
#define ASSERT_NOT_INSTANCE(class,test)      _UTT_CHECK_TEST(utils::testing::ObjectVerifier::isNotInstanceOf<class>(test), std::string("is instance of specified class"))

// strings
#define ASSERT_STR_EMPTY(test)               _UTT_CHECK_TEST(utils::testing::StringVerifier::isEmpty(test),        std::string("expected: \"\"; value: \")+test+std::string("\""))
#define ASSERT_STR_NOT_EMPTY(test)           _UTT_CHECK_TEST(utils::testing::StringVerifier::isNotEmpty(test),     std::string("expected: not empty; value: \"\""))
#define ASSERT_STR_EQ(test,val)              _UTT_CHECK_TEST(utils::testing::StringVerifier::isEqual(test,val),    std::string("expected: \"")+val+std::string("\"; value: \")+test+std::string("\""))
#define ASSERT_STR_NOT_EQ(test,val)          _UTT_CHECK_TEST(utils::testing::StringVerifier::isNotEqual(test,val), std::string("expected: != \"")+val+std::string("\"; value: \")+test+std::string("\""))
#define ASSERT_STR_START(test,val)           _UTT_CHECK_TEST(utils::testing::StringVerifier::startsWith(test,val), std::string("expected: \"")+val+std::string("\"[...]; value: \")+test+std::string("\""))
#define ASSERT_STR_END(test,val)             _UTT_CHECK_TEST(utils::testing::StringVerifier::endsWith(test,val),   std::string("expected: [...]\"")+val+std::string("\"; value: \")+test+std::string("\""))
#define ASSERT_STR_CONTAINS(test,val)        _UTT_CHECK_TEST(utils::testing::StringVerifier::contains(test,val),   std::string("expected: [...]\"")+val+std::string("\"[...]; value: \")+test+std::string("\""))

// arrays / hashes
#define ASSERT_ARRAY_EMPTY(test)             _UTT_CHECK_TEST(utils::testing::CollectionVerifier::isEmpty(test),          std::string("expected: {}; value: not empty"))
#define ASSERT_ARRAY_NOT_EMPTY(test)         _UTT_CHECK_TEST(utils::testing::CollectionVerifier::isNotEmpty(test),       std::string("expected: not empty; value: {}"))
#define ASSERT_ARRAY_CONTAINS(test,val)      _UTT_CHECK_TEST(utils::testing::CollectionVerifier::contains(test,val),     std::string("value not found in array"))
#define ASSERT_ARRAY_CONTAINS(test,key,val)  _UTT_CHECK_TEST(utils::testing::CollectionVerifier::contains(test,key,val), std::string("key or value not found in hash"))
#define ASSERT_ARRAY_LACKS(test,val)         _UTT_CHECK_TEST(utils::testing::CollectionVerifier::lacks(test,val),        std::string("excluded value is in array"))
#define ASSERT_ARRAY_LACKS(test,key,val)     _UTT_CHECK_TEST(utils::testing::CollectionVerifier::lacks(test,key,val),    std::string("excluded key and value is in hash"))
