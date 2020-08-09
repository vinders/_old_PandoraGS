#include <gtest/gtest.h>
#include <io/directory.h>

using namespace pandora::io;

class DirectoryTest : public testing::Test {
public:
protected:
  static void SetUpTestCase() {
    if (Directory::createDirectory(testCaseDir) == 0) {
      FileHandle fp = Directory::openFile(testCaseFile.c_str(), fileCreateMode.c_str());
      if (fp.isOpen()) {
        fputs("directory_test", fp.handle());
        isCaseSetupSuccess = true;
      }
    }
  }
  static void TearDownTestCase() {
    Directory::removeFile(testCaseFile);
    Directory::removeDirectory(testCaseDir);
  }

  void SetUp() override {
    ASSERT_TRUE(isCaseSetupSuccess);
  }
  void TearDown() override {}

protected:
  static Directory::Path testCaseFile;
  static Directory::Path testCaseFileName;
  static Directory::Path testCaseDir;
  static Directory::Path fileCreateMode;
private:
  static bool isCaseSetupSuccess;
};
# ifdef _WINDOWS
  Directory::Path DirectoryTest::testCaseFile = L"./testDir/testFile.txt";
  Directory::Path DirectoryTest::testCaseFileName = L"testFile.txt";
  Directory::Path DirectoryTest::testCaseDir = L"testDir";
  Directory::Path DirectoryTest::fileCreateMode = L"w";
# else
  Directory::Path DirectoryTest::testCaseFile = "./testDir/testFile.txt";
  Directory::Path DirectoryTest::testCaseFileName = "testFile.txt";
  Directory::Path DirectoryTest::testCaseDir = "testDir";
  Directory::Path DirectoryTest::fileCreateMode = "w";
# endif
bool DirectoryTest::isCaseSetupSuccess = false;


// -- constructors/accessors --

TEST_F(DirectoryTest, defaultCtorAccessors) {
  Directory testDir;
  EXPECT_TRUE(testDir.getPath().empty());
  EXPECT_EQ(Directory::PathType::relative, testDir.getPathType());
  EXPECT_EQ(Directory::PathTarget::unknown, testDir.getPathTargetType());
  EXPECT_EQ(Directory::PathTarget::unknown, Directory::getPathTargetType(testDir.getPath().c_str()));
  EXPECT_TRUE(testDir.getSegments().empty());
  EXPECT_TRUE(Directory::getPathSegments(testDir.getPath()).empty());
}

TEST_F(DirectoryTest, initCtorAccessors) {
# ifdef _WINDOWS
    Directory::PathChar testDirInit[2]; testDirInit[0] = L'.'; testDirInit[1] = L'\0';
    Directory testDir(testDirInit);
    EXPECT_EQ(std::wstring(L"."), testDir.getPath());
# else
    Directory::PathChar testDirInit[2]; testDirInit[0] = '.'; testDirInit[1] = '\0';
    Directory testDir(testDirInit);
    EXPECT_EQ(std::string("."), testDir.getPath());
# endif
  EXPECT_EQ(Directory::PathType::relative, testDir.getPathType());
  EXPECT_EQ(Directory::PathType::relative, Directory::getPathType(testDir.getPath().c_str()));
  EXPECT_EQ(Directory::PathTarget::directory, testDir.getPathTargetType());
  EXPECT_EQ(Directory::PathTarget::directory, Directory::getPathTargetType(testDir.getPath().c_str()));
  EXPECT_FALSE(testDir.isHidden());
  EXPECT_FALSE(Directory::isHidden(testDir.getPath().c_str()));
  EXPECT_EQ(size_t{ 1u }, Directory::getPathSegments(testDir.getPath()).size());
  auto segments = testDir.getSegments();
  EXPECT_EQ(size_t{ 1u }, segments.size());
  if (!segments.empty()) {
# ifdef _WINDOWS
    EXPECT_EQ(std::wstring(L"."), segments[0]);
# else
    EXPECT_EQ(std::string("."), segments[0]);
# endif
  }

# ifdef _WINDOWS
    Directory::Path testDirAbsInit = L"C:\\Windows\\";
    Directory testDirAbs(testDirAbsInit);
    EXPECT_EQ(std::wstring(L"C:\\Windows\\"), testDirAbs.getPath());
# else
    Directory::Path testDirAbsInit = "/usr/bin/";
    Directory testDirAbs(testDirAbsInit);
    EXPECT_EQ(std::string("/usr/bin/"), testDirAbs.getPath());
# endif
  EXPECT_EQ(Directory::PathType::absolute, testDirAbs.getPathType());
  EXPECT_EQ(Directory::PathType::absolute, Directory::getPathType(testDirAbs.getPath().c_str()));
# if !defined(__APPLE__) && !defined(__ANDROID__)
    EXPECT_EQ(Directory::PathTarget::directory, testDir.getPathTargetType());
    EXPECT_EQ(Directory::PathTarget::directory, Directory::getPathTargetType(testDir.getPath().c_str()));
    EXPECT_FALSE(testDirAbs.isHidden());
    EXPECT_FALSE(Directory::isHidden(testDirAbs.getPath().c_str()));
# endif
  EXPECT_EQ(size_t{ 2u }, Directory::getPathSegments(testDirAbs.getPath()).size());
  segments = testDirAbs.getSegments();
  EXPECT_EQ(size_t{ 2u }, segments.size());
  if (!segments.empty()) {
# ifdef _WINDOWS
    EXPECT_EQ(std::wstring(L"C:"), segments[0]);
    EXPECT_EQ(std::wstring(L"Windows"), segments[1]);
# else
    EXPECT_EQ(std::string("usr"), segments[0]);
    EXPECT_EQ(std::string("bin"), segments[1]);
# endif
  }
}

TEST_F(DirectoryTest, copyMoveCtorAccessors) {
# ifdef _WINDOWS
    Directory testDir(L"./abc/d&f");
    EXPECT_EQ(std::wstring(L"./abc/d&f"), testDir.getPath());
    auto type = testDir.getPathTargetType();

    Directory testDirCopy(testDir);
    EXPECT_EQ(std::wstring(L"./abc/d&f"), testDirCopy.getPath());
    EXPECT_EQ(type, testDirCopy.getPathTargetType());
    Directory testDirMove(std::move(testDir));
    EXPECT_EQ(std::wstring(L"./abc/d&f"), testDirMove.getPath());
    EXPECT_EQ(type, testDirMove.getPathTargetType());

    testDir = Directory(L".");
    EXPECT_EQ(std::wstring(L"."), testDir.getPath());
    testDir = testDirCopy;
    EXPECT_EQ(std::wstring(L"./abc/d&f"), testDir.getPath());
    EXPECT_EQ(type, testDir.getPathTargetType());

    testDir = Directory(L".");
    testDir = std::move(testDirMove);
    EXPECT_EQ(std::wstring(L"./abc/d&f"), testDir.getPath());
    EXPECT_EQ(type, testDir.getPathTargetType());
# else
    Directory testDir("./abc/d&f");
    EXPECT_EQ(std::string("./abc/d&f"), testDir.getPath());
    auto type = testDir.getPathTargetType();

    Directory testDirCopy(testDir);
    EXPECT_EQ(std::string("./abc/d&f"), testDirCopy.getPath());
    EXPECT_EQ(type, testDirCopy.getPathTargetType());
    Directory testDirMove(std::move(testDir));
    EXPECT_EQ(std::string("./abc/d&f"), testDirMove.getPath());
    EXPECT_EQ(type, testDirMove.getPathTargetType());

    testDir = Directory(".");
    EXPECT_EQ(std::string("."), testDir.getPath());
    testDir = testDirCopy;
    EXPECT_EQ(std::string("./abc/d&f"), testDir.getPath());
    EXPECT_EQ(type, testDir.getPathTargetType());

    testDir = Directory(".");
    testDir = std::move(testDirMove);
    EXPECT_EQ(std::string("./abc/d&f"), testDir.getPath());
    EXPECT_EQ(type, testDir.getPathTargetType());
# endif
}

TEST_F(DirectoryTest, swapObjects) {
# ifdef _WINDOWS
    Directory testDir(L"./abc/d&f");
    Directory testDir2(L"C:\\Windows");
    EXPECT_EQ(std::wstring(L"./abc/d&f"), testDir.getPath());
    auto type = testDir.getPathTargetType();
    EXPECT_EQ(std::wstring(L"C:\\Windows"), testDir2.getPath());
    auto type2 = testDir2.getPathTargetType();

    testDir.swap(testDir2);
    EXPECT_EQ(std::wstring(L"C:\\Windows"), testDir.getPath());
    EXPECT_EQ(type2, testDir.getPathTargetType());
    EXPECT_EQ(std::wstring(L"./abc/d&f"), testDir2.getPath());
    EXPECT_EQ(type, testDir2.getPathTargetType());
# else
    Directory testDir("./abc/d&f");
    Directory testDir2("/usr/bin");
    EXPECT_EQ(std::string("./abc/d&f"), testDir.getPath());
    auto type = testDir.getPathTargetType();
    EXPECT_EQ(std::string("/usr/bin"), testDir2.getPath());
    auto type2 = testDir2.getPathTargetType();

    testDir.swap(testDir2);
    EXPECT_EQ(std::string("/usr/bin"), testDir.getPath());
    EXPECT_EQ(type2, testDir.getPathTargetType());
    EXPECT_EQ(std::string("./abc/d&f"), testDir2.getPath());
    EXPECT_EQ(type, testDir2.getPathTargetType());
# endif
}


// -- access mode --

TEST_F(DirectoryTest, accessMode) {
# ifdef _WINDOWS
    Directory::Path invalidPath = L"dummy\\//NonExisting$%:@";
# else
    Directory::Path invalidPath = "dummy\\//NonExisting$%:@";
# endif

  Directory testInvalidDir(invalidPath);
  EXPECT_FALSE(testInvalidDir.hasAccess(Directory::AccessMode::existence));
  EXPECT_FALSE(testInvalidDir.hasAccess(Directory::AccessMode::read));
  EXPECT_FALSE(testInvalidDir.hasAccess(Directory::AccessMode::write));
  EXPECT_FALSE(testInvalidDir.hasAccess(Directory::AccessMode::readWrite));
  EXPECT_FALSE(Directory::verifyPathAccess(invalidPath.c_str(), Directory::AccessMode::existence));
  EXPECT_FALSE(Directory::verifyPathAccess(invalidPath.c_str(), Directory::AccessMode::read));
  EXPECT_FALSE(Directory::verifyPathAccess(invalidPath.c_str(), Directory::AccessMode::write));
  EXPECT_FALSE(Directory::verifyPathAccess(invalidPath.c_str(), Directory::AccessMode::readWrite));

  Directory testValidDir(testCaseFile);
  EXPECT_TRUE(testValidDir.hasAccess(Directory::AccessMode::existence));
  EXPECT_TRUE(testValidDir.hasAccess(Directory::AccessMode::read));
  EXPECT_TRUE(testValidDir.hasAccess(Directory::AccessMode::write));
  EXPECT_TRUE(testValidDir.hasAccess(Directory::AccessMode::readWrite));
  EXPECT_TRUE(Directory::verifyPathAccess(testCaseFile.c_str(), Directory::AccessMode::existence));
  EXPECT_TRUE(Directory::verifyPathAccess(testCaseFile.c_str(), Directory::AccessMode::read));
  EXPECT_TRUE(Directory::verifyPathAccess(testCaseFile.c_str(), Directory::AccessMode::write));
  EXPECT_TRUE(Directory::verifyPathAccess(testCaseFile.c_str(), Directory::AccessMode::readWrite));
}


// -- file operations --

TEST_F(DirectoryTest, knownFileAttributes) {
  EXPECT_EQ(Directory::PathTarget::file, Directory::getPathTargetType(testCaseFile.c_str()));
  EXPECT_FALSE(Directory::isHidden(testCaseFile.c_str()));
  EXPECT_EQ(Directory::PathTarget::directory, Directory::getPathTargetType(testCaseDir.c_str()));
  EXPECT_FALSE(Directory::isHidden(testCaseDir.c_str()));
}

TEST_F(DirectoryTest, knownFileMetadata) {
# ifdef _WINDOWS
  Directory::Path invalidPath = L"dummyNonExisting";
# else
  Directory::Path invalidPath = "dummyNonExisting";
# endif
  Directory::TargetMetadata metadata;

  EXPECT_FALSE(Directory::readFileMetadata(invalidPath, metadata));

  EXPECT_TRUE(Directory::readFileMetadata(testCaseFile, metadata));
  EXPECT_EQ(testCaseFileName, metadata.name);
  EXPECT_EQ(Directory::PathTarget::file, metadata.type);
  EXPECT_EQ(Directory::AccessMode::readWrite, metadata.accessMode);
  EXPECT_TRUE(metadata.size > 0);
  EXPECT_FALSE(metadata.isHidden);

  EXPECT_TRUE(Directory::readFileMetadata(testCaseDir, metadata));
  EXPECT_EQ(testCaseDir, metadata.name);
  EXPECT_EQ(Directory::PathTarget::directory, metadata.type);
  EXPECT_EQ(Directory::AccessMode::readWrite, metadata.accessMode);
  EXPECT_FALSE(metadata.isHidden);

# ifdef _WINDOWS
    EXPECT_FALSE(Directory::readFileMetadata(Directory::Path{}, metadata));
    EXPECT_TRUE(Directory::readFileMetadata(testCaseDir + L"/", metadata));
# else
    EXPECT_FALSE(Directory::readFileMetadata(Directory::Path{}, metadata));
    EXPECT_TRUE(Directory::readFileMetadata(testCaseDir + "/", metadata));
# endif
}

TEST_F(DirectoryTest, readFile) {
# ifdef _WINDOWS
    Directory::Path invalidPath = L"dummyNonExisting";
    Directory::Path readMode = L"r";
# else
    Directory::Path invalidPath = "dummyNonExisting";
    Directory::Path readMode = "r";
# endif

  FileHandle fpInvalid = Directory::openFile(invalidPath.c_str(), readMode.c_str());
  EXPECT_FALSE(fpInvalid.isOpen());

  FileHandle fp = Directory::openFile(testCaseFile.c_str(), readMode.c_str());
  ASSERT_TRUE(fp.isOpen());
  char buffer[64];
  fgets(buffer, 64, fp.handle());
  EXPECT_EQ(std::string("directory_test"), std::string(buffer));
}

TEST_F(DirectoryTest, readFileStream) {
# ifdef _WINDOWS
    Directory::Path invalidPath = L"dummyNonExisting";
# else
    Directory::Path invalidPath = "dummyNonExisting";
# endif

  std::ifstream fpInvalid;
  EXPECT_FALSE(Directory::openFileStream(invalidPath, std::fstream::in, fpInvalid));
  EXPECT_FALSE(fpInvalid.is_open());

  std::ifstream fp;
  EXPECT_TRUE(Directory::openFileStream(testCaseFile, std::fstream::in, fp));
  ASSERT_TRUE(fp.is_open());
  std::string buffer;
  std::getline(fp, buffer);
  EXPECT_EQ(std::string("directory_test"), buffer);
}

TEST_F(DirectoryTest, createRemoveFile) {
# ifdef _WINDOWS
    Directory::Path invalidPath = L"$.\\//./$:|\\.";
    Directory::Path validPath = L"abc.txt";
# else
    Directory::Path invalidPath = "$.\\//./$:|\\.";
    Directory::Path validPath = "abc.txt";
# endif

  FileHandle fpInvalid = Directory::openFile(invalidPath.c_str(), fileCreateMode.c_str());
  EXPECT_FALSE(fpInvalid.isOpen());

  EXPECT_FALSE(Directory::removeFile(invalidPath.c_str()) == 0);
  EXPECT_FALSE(Directory::removeFile(validPath.c_str()) == 0); // non existing yet

  {// scope
    FileHandle fp = Directory::openFile(validPath.c_str(), fileCreateMode.c_str());
    ASSERT_TRUE(fp.isOpen());
  }// fp out of scope
  EXPECT_TRUE(Directory::removeFile(validPath.c_str()) == 0);
}


// -- list directory content --

TEST_F(DirectoryTest, listSubFiles) {
# ifdef _WINDOWS
    Directory::Path invalidPath = L"dummyNonExisting";
# else
    Directory::Path invalidPath = "dummyNonExisting";
# endif

  std::vector<Directory> subFiles;
  std::vector<Directory::TargetMetadata> subFilesMetadata;

  EXPECT_FALSE(Directory::listFilesInDirectory(invalidPath, subFiles));

  ASSERT_TRUE(Directory::listFilesInDirectory(testCaseDir, subFiles));
  EXPECT_TRUE(subFiles.size() >= 1);
  bool testFileFound = false;
  for (auto& file : subFiles) {
    auto segments = file.getSegments();
    if (segments.back() == testCaseFileName) {
      testFileFound = true;
      break;
    }
  }
  EXPECT_TRUE(testFileFound);

  ASSERT_TRUE(Directory::listFilesInDirectory(testCaseDir, subFilesMetadata));
  EXPECT_TRUE(subFilesMetadata.size() >= 1);
  testFileFound = false;
  for (auto& file : subFilesMetadata) {
    if (file.name == testCaseFileName) {
      EXPECT_EQ(testCaseFileName, file.name);
      EXPECT_EQ(Directory::PathTarget::file, file.type);
      EXPECT_EQ(Directory::AccessMode::readWrite, file.accessMode);
      EXPECT_TRUE(file.size > 0);
      EXPECT_FALSE(file.isHidden);
      testFileFound = true;
      break;
    }
  }
  EXPECT_TRUE(testFileFound);
}

TEST_F(DirectoryTest, createRemoveDirectory) {
# ifdef _WINDOWS
    Directory::Path invalidPath = L"$.\\//./$:|\\.";
    Directory::Path validPath = L"abc";
# else
    Directory::Path invalidPath = "$.\\//./$:|\\.";
    Directory::Path validPath = "abc";
# endif

  EXPECT_FALSE(Directory::createDirectory(invalidPath) == 0);

  EXPECT_FALSE(Directory::removeDirectory(invalidPath) == 0);
  EXPECT_FALSE(Directory::removeDirectory(validPath) == 0); // non existing yet

  ASSERT_TRUE(Directory::createDirectory(validPath) == 0);

  EXPECT_TRUE(Directory::removeDirectory(validPath) == 0);
}
