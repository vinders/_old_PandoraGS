/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdarg>
#include <cstdint>
#include <cassert>
#include <memory>
#include <string>
#include <iostream>
#include "./preprocessor_tools.h"

#ifndef _P_MAX_LOG_MESSAGE_SIZE
# define _P_MAX_LOG_MESSAGE_SIZE 255
#endif

namespace pandora {
  namespace system {
    /// @brief Log level type
    enum class LogLevel: uint32_t {
      none = 0u,
      trace = 1u,
      debug = 2u,
      standard = 3u,
      critical = 4u
    };
    _P_SERIALIZABLE_ENUM(LogLevel, none, critical, standard, debug, trace);

    /// @brief Log content category
    enum class LogCategory: uint32_t {
      none = 0u,
      event = 1u,
      info = 2u,
      warning = 3u,
      error = 4u,
    };
    _P_SERIALIZABLE_ENUM(LogCategory, none, event, info, warning, error);

    /// @class Logger
    /// @brief Log writing utility
    template <typename _LogFormatter> // _LogFormatter must have these static methods:
                                      // 'basic_string<> openContext(bool addHeaders)'
                                      // 'basic_string<> closeContext()'
                                      // 'basic_string<> format(Level, Category, const char* origin, uint32_t line, const char* message)'
    class Logger final {
    public:
      /// @brief Create logger utility (inject output stream, set output level, add optional headers (for example, for empty files))
      Logger(std::unique_ptr<std::ostream>&& logStream, LogLevel minWriteLevel = LogLevel::standard, bool addOutputHeaders = false)
        : _logStream(std::move(logStream)), _level(minWriteLevel) {
        auto opening = _LogFormatter::openContext(addOutputHeaders);
        if (!opening.empty())
          *_logStream << opening << std::endl;
      }
      Logger(const Logger&) = delete;
      Logger(Logger&&) = default;
      Logger& operator=(const Logger&) = delete;
      Logger& operator=(Logger&&) = default;
      ~Logger() {
        if (_logStream != nullptr) {
          auto closure = _LogFormatter::closeContext();
          if (!closure.empty())
            *_logStream << closure << std::endl;
          flushBuffer();
        }
      }

      // -- minimum active log level --

      inline LogLevel getMinLevel() const noexcept { return this->_level; }      ///< get minimum output log level
      inline void setMinLevel(LogLevel level) noexcept { this->_level = level; } ///< set minimum output log level

      // -- status --

      inline void clearErrorStatus() noexcept { this->_logStream->clear(); } ///< reset error flags
      inline bool hasLogicalFailure() const noexcept { return (this->_logStream->rdstate() & std::ostream::failbit); } ///< read logical error flag
      inline bool hasBadBitError() const noexcept { return (this->_logStream->rdstate() & std::ostream::badbit); }     ///< read write error flag

      // -- operations --
     
      /// @brief Log new entry (only written if 'level' <= minimum active level)
      /// @param[in] level  Log level for current message (if less important than configured output level, message ignored)
      /// @param[in] flag   Type of message
      /// @param[in] origin Source location of the message (example: __FILE__ or method name)
      /// @param[in] line   Line number in source file (example: __LINE__)
      /// @param[in] format Log message with printf-like format, followed by optional arguments
#     ifdef __GNUC__
        __attribute__((format(printf, 6, 7)))
#     endif
      void log(LogLevel level, LogCategory flag, const char* origin, uint32_t line, const char* format, ...) 
      {
        assert(origin != nullptr && format != nullptr && this->_logStream != nullptr);
        if (level >= this->_level) {
          char messageBuffer[_P_MAX_LOG_MESSAGE_SIZE]{ 0 };
          va_list args;
          va_start(args, format);
          vsnprintf(&messageBuffer[0], _P_MAX_LOG_MESSAGE_SIZE, format, args);
          va_end(args);

          *_logStream << _LogFormatter::format(level, flag, origin, line, messageBuffer) << std::endl;
        }
      }

      /// @brief Flush log stream buffer
      inline void flushBuffer() noexcept { this->_logStream->flush(); }


    private:
      std::unique_ptr<std::ostream> _logStream = nullptr;
      LogLevel _level = LogLevel::standard;
    };

  }
}
