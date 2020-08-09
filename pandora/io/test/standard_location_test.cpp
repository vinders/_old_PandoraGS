#define GTEST_HAS_TR1_TUPLE 0
#include <gtest/gtest.h>
#include <cstdio>
#include <io/standard_location.h>
#if defined(__APPLE__)
# include <TargetConditionals.h>
#endif

using namespace pandora::io;

class StandardLocationTest : public testing::Test {
public:
protected:
  //static void SetUpTestCase() {}
  //static void TearDownTestCase() {}
  void SetUp() override {}
  void TearDown() override {}
  
  // verifications to repeat for each standard location
  static inline void _verifyStandardLocation(StandardLocation location) {
#   if defined(_WINDOWS)
      std::vector<pandora::io::Directory::Path> paths = LocationFinder::standardLocations(location, L"StandardLocationTest");
#   else
      std::vector<pandora::io::Directory::Path> paths = LocationFinder::standardLocations(location, "StandardLocationTest");
#   endif
    EXPECT_FALSE(paths.empty());
    for (auto& it : paths) {
      EXPECT_FALSE(it.empty());
      if (!it.empty()) {
#       if defined(_WINDOWS)
          EXPECT_TRUE((it[0] >= 'A' && it[0] <= 'Z') || it[0] == '\\' || it[0] == '/');
#       elif !defined(__ANDROID__) && (!defined(TARGET_OS_IPHONE) || !TARGET_OS_IPHONE)
#         if defined(__APPLE__)
            EXPECT_TRUE(it[0] == '/' || it[0] == '~' || it[0] == '.');
#         else
            EXPECT_TRUE(it[0] == '/');
#         endif
#       endif
      }
    }
  }
};

// -- current location --

TEST_F(StandardLocationTest, currentLocationPath) {
  auto path = LocationFinder::currentLocation();
# ifdef _WINDOWS
    wprintf(L"%s\n", path.c_str());
    ASSERT_LE(size_t{ 4u }, path.size());
    EXPECT_TRUE(path[0] == L'%' || path[0] == L'\\' || path[0] == L'/' || (path[0] >= L'A' && path[0] <= L'Z'));
# elif defined(__ANDROID__) || (defined(__APPLE__) && defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)
    EXPECT_TRUE(path.empty() || path[0] == L'$' || path[0] == L'\\' || path[0] == L'/' || path[0] == 'h' || path[0] == '.' || path[0] == 's' || path[0] == '~');
# else
    printf("%s\n", path.c_str());
    ASSERT_LE(size_t{ 4u }, path.size());
    EXPECT_TRUE(path[0] == L'$' || path[0] == L'\\' || path[0] == L'/' || path[0] == 'h' || path[0] == '.' || path[0] == 's' || path[0] == '~');
# endif
}

// -- home location --

TEST_F(StandardLocationTest, homeLocationPath) {
  auto path = LocationFinder::homeLocation();
# ifdef _WINDOWS
    wprintf(L"%s\n", path.c_str());
    ASSERT_LE(size_t{ 4u }, path.size());
    EXPECT_TRUE(path[0] == L'%' || path[0] == L'\\' || path[0] == L'/' || (path[0] >= L'A' && path[0] <= L'Z'));
# elif defined(__ANDROID__)
    printf("%s\n", path.c_str());
    EXPECT_TRUE(path.empty() || path == std::string("files"));
# elif defined(__APPLE__) && defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    EXPECT_TRUE(path.empty() || path[0] == L'$' || path[0] == L'\\' || path[0] == L'/' || path[0] == 'h' || path[0] == '.' || path[0] == 's' || path[0] == '~');
# else
    printf("%s\n", path.c_str());
    ASSERT_LE(size_t{ 4u }, path.size());
    EXPECT_TRUE(path[0] == L'$' || path[0] == L'\\' || path[0] == L'/' || path[0] == 'h' || path[0] == '.' || path[0] == 's' || path[0] == '~');
# endif
}

// -- standard system locations --

TEST_F(StandardLocationTest, standardLocations) {
  _verifyStandardLocation(StandardLocation::home);
  _verifyStandardLocation(StandardLocation::desktop);
  _verifyStandardLocation(StandardLocation::documents);
  _verifyStandardLocation(StandardLocation::pictures);
  _verifyStandardLocation(StandardLocation::music);
  _verifyStandardLocation(StandardLocation::movies);
  _verifyStandardLocation(StandardLocation::downloads);

  _verifyStandardLocation(StandardLocation::appData);
  _verifyStandardLocation(StandardLocation::localAppData);
  _verifyStandardLocation(StandardLocation::appConfig);
  _verifyStandardLocation(StandardLocation::commonAppConfig);
  _verifyStandardLocation(StandardLocation::cache);
  _verifyStandardLocation(StandardLocation::localCache);
  _verifyStandardLocation(StandardLocation::sharedCache);
  _verifyStandardLocation(StandardLocation::temp);
  _verifyStandardLocation(StandardLocation::fonts);

# if defined(_WINDOWS) || (!defined(__ANDROID__) && (!defined(TARGET_OS_IPHONE) || !TARGET_OS_IPHONE))
    _verifyStandardLocation(StandardLocation::applications);
    _verifyStandardLocation(StandardLocation::sharedApplications);
    _verifyStandardLocation(StandardLocation::appMenu);

    _verifyStandardLocation(StandardLocation::system);
    _verifyStandardLocation(StandardLocation::localBinaries);
    _verifyStandardLocation(StandardLocation::sharedBinaries);
    _verifyStandardLocation(StandardLocation::sharedLibraries);
    _verifyStandardLocation(StandardLocation::commonBinaryFiles);
# endif
    EXPECT_NO_THROW(LocationFinder::standardLocations(StandardLocation::printers));
    EXPECT_NO_THROW(LocationFinder::standardLocations(StandardLocation::trash));
}
