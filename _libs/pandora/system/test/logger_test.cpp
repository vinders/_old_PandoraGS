#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <gtest/gtest.h>
#include <system/logger.h>

using namespace pandora::system;

class LoggerTest : public testing::Test {
public:
protected:
  //static void SetUpTestCase() {}
  //static void TearDownTestCase() {}
  void SetUp() override {}
  void TearDown() override {}
};

// -- formatter mock --

class HtmlLogFormatter {
public:
  static std::string openContext(bool addHeaders) { return (addHeaders) ? "<table>\n<tr><th>Level</th><th>Category</th><th>Source</th><th>Message</th></tr>" : "<table>"; }
  static std::string closeContext() { return "</table>"; }

  template <typename... _Args>
  static std::string format(LogLevel level, LogCategory category, const char* origin, uint32_t line, const char* message) {
    char buffer[256] = { 0 };
    std::string levelStr = toString(level);
    std::string catStr = toString(category);
    snprintf(buffer, 255, "<tr><td>%s</td><td>%s</td><td>%s(%d)</td><td>", levelStr.c_str(), catStr.c_str(), origin, line);
    return std::string(buffer) + message + "</td></tr>";
  }
};

// -- open/close logger --

TEST_F(LoggerTest, loggerDefault) {
  auto streamMoved = std::make_unique<std::ostringstream>();
  std::ostringstream* stream = streamMoved.get();
  Logger<HtmlLogFormatter> logger(std::move(streamMoved));

  EXPECT_EQ(LogLevel::standard, logger.getMinLevel());
  EXPECT_FALSE(logger.hasLogicalFailure());
  EXPECT_FALSE(logger.hasBadBitError());
  logger.flushBuffer();
  EXPECT_EQ(std::string("<table>\n"), stream->str());
}

TEST_F(LoggerTest, loggerWithoutHeaders) {
  auto streamMoved = std::make_unique<std::ostringstream>();
  std::ostringstream* stream = streamMoved.get();
  Logger<HtmlLogFormatter> logger(std::move(streamMoved), LogLevel::debug, false);

  EXPECT_EQ(LogLevel::debug, logger.getMinLevel());
  EXPECT_FALSE(logger.hasLogicalFailure());
  EXPECT_FALSE(logger.hasBadBitError());
  logger.flushBuffer();
  EXPECT_EQ(std::string("<table>\n"), stream->str());
}

TEST_F(LoggerTest, loggerWithHeaders) {
  auto streamMoved = std::make_unique<std::ostringstream>();
  std::ostringstream* stream = streamMoved.get();
  Logger<HtmlLogFormatter> logger(std::move(streamMoved), LogLevel::critical, true);

  EXPECT_EQ(LogLevel::critical, logger.getMinLevel());
  EXPECT_FALSE(logger.hasLogicalFailure());
  EXPECT_FALSE(logger.hasBadBitError());
  logger.flushBuffer();
  EXPECT_EQ(std::string("<table>\n<tr><th>Level</th><th>Category</th><th>Source</th><th>Message</th></tr>\n"), stream->str());
}

TEST_F(LoggerTest, logMessageWithoutHeaders) {
  auto streamMoved = std::make_unique<std::ostringstream>();
  std::ostringstream* stream = streamMoved.get();
  Logger<HtmlLogFormatter> logger(std::move(streamMoved), LogLevel::none, false);

  logger.log(LogLevel::none, LogCategory::none, "nowhere", 12, "hello world !");
  logger.log(LogLevel::trace, LogCategory::event, "sherlock", 44, "this is not an %s", "event");
  logger.log(LogLevel::debug, LogCategory::info, "mib", 21, "code %d - %d %s to remove", 257, 1, "alien");
  logger.log(LogLevel::standard, LogCategory::warning, "irobot", 0, "AI will %s the world", "save");
  logger.log(LogLevel::critical, LogCategory::error, "mars-attacks", 8, " ");

  logger.flushBuffer();
  EXPECT_EQ(std::string("<table>\n"
            "<tr><td>none</td><td>none</td><td>nowhere(12)</td><td>hello world !</td></tr>\n"
            "<tr><td>trace</td><td>event</td><td>sherlock(44)</td><td>this is not an event</td></tr>\n"
            "<tr><td>debug</td><td>info</td><td>mib(21)</td><td>code 257 - 1 alien to remove</td></tr>\n"
            "<tr><td>standard</td><td>warning</td><td>irobot(0)</td><td>AI will save the world</td></tr>\n"
            "<tr><td>critical</td><td>error</td><td>mars-attacks(8)</td><td> </td></tr>\n"), stream->str());
}

TEST_F(LoggerTest, logMessageWithHeaders) {
  auto streamMoved = std::make_unique<std::ostringstream>();
  std::ostringstream* stream = streamMoved.get();
  Logger<HtmlLogFormatter> logger(std::move(streamMoved), LogLevel::none, true);

  logger.log(LogLevel::none, LogCategory::none, "nowhere", 12, "hello world !");
  logger.log(LogLevel::trace, LogCategory::event, "sherlock", 44, "this is not an %s", "event");
  logger.log(LogLevel::debug, LogCategory::info, "mib", 21, "code %d - %d %s to remove", 257, 1, "alien");
  logger.log(LogLevel::standard, LogCategory::warning, "irobot", 0, "AI will %s the world", "save");
  logger.log(LogLevel::critical, LogCategory::error, "mars-attacks", 8, " ");

  logger.flushBuffer();
  EXPECT_EQ(std::string("<table>\n<tr><th>Level</th><th>Category</th><th>Source</th><th>Message</th></tr>\n"
            "<tr><td>none</td><td>none</td><td>nowhere(12)</td><td>hello world !</td></tr>\n"
            "<tr><td>trace</td><td>event</td><td>sherlock(44)</td><td>this is not an event</td></tr>\n"
            "<tr><td>debug</td><td>info</td><td>mib(21)</td><td>code 257 - 1 alien to remove</td></tr>\n"
            "<tr><td>standard</td><td>warning</td><td>irobot(0)</td><td>AI will save the world</td></tr>\n"
            "<tr><td>critical</td><td>error</td><td>mars-attacks(8)</td><td> </td></tr>\n"), stream->str());
}

TEST_F(LoggerTest, logIgnoredMessage) {
  auto streamMoved = std::make_unique<std::ostringstream>();
  std::ostringstream* stream = streamMoved.get();
  Logger<HtmlLogFormatter> logger(std::move(streamMoved), LogLevel::critical, false);

  logger.log(LogLevel::none, LogCategory::info, "abc", 42, "blabla");
  logger.log(LogLevel::trace, LogCategory::info, "abc", 42, "blabla");
  logger.log(LogLevel::debug, LogCategory::info, "abc", 42, "blabla");
  logger.log(LogLevel::standard, LogCategory::info, "abc", 42, "blabla");
  logger.log(LogLevel::critical, LogCategory::info, "abc", 42, "blabla");

  logger.setMinLevel(LogLevel::standard);
  EXPECT_EQ(LogLevel::standard, logger.getMinLevel());
  logger.log(LogLevel::none, LogCategory::event, "def", 24, "trololololo");
  logger.log(LogLevel::trace, LogCategory::event, "def", 24, "trololololo");
  logger.log(LogLevel::debug, LogCategory::event, "def", 24, "trololololo");
  logger.log(LogLevel::standard, LogCategory::event, "def", 24, "trololololo");
  logger.log(LogLevel::critical, LogCategory::event, "def", 24, "trololololo");

  logger.flushBuffer();
  EXPECT_EQ(std::string("<table>\n"
            "<tr><td>critical</td><td>info</td><td>abc(42)</td><td>blabla</td></tr>\n"
            "<tr><td>standard</td><td>event</td><td>def(24)</td><td>trololololo</td></tr>\n"
            "<tr><td>critical</td><td>event</td><td>def(24)</td><td>trololololo</td></tr>\n"), stream->str());
}

TEST_F(LoggerTest, movedLogger) {
  auto streamMoved = std::make_unique<std::ostringstream>();
  std::ostringstream* stream = streamMoved.get();
  Logger<HtmlLogFormatter> movedLogger(std::move(streamMoved), LogLevel::critical, false);
  Logger<HtmlLogFormatter> logger(std::move(movedLogger));

  logger.log(LogLevel::critical, LogCategory::event, "def", 24, "trololololo");

  logger.flushBuffer();
  EXPECT_EQ(std::string("<table>\n<tr><td>critical</td><td>event</td><td>def(24)</td><td>trololololo</td></tr>\n"), stream->str());
}
