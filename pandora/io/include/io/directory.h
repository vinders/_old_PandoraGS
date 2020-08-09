/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
*******************************************************************************/
#pragma once

#include <cstdio>
#include <cassert>
#include <cerrno>
#include <string>
#include <vector>
#include <fstream>
#if !defined(_MSC_VER) && defined(_WINDOWS)
# include <locale>
# include <codecvt>
#endif
#include "./file_handle.h"

namespace pandora { 
  namespace io {
    /// @class Directory
    /// @brief Directory or file path information utility
    class Directory final {
    public:
#     ifdef _WINDOWS
        using Path = std::wstring; ///< Path string
        using PathChar = wchar_t;  ///< Path character
#     else
        using Path = std::string; ///< Path string
        using PathChar = char;    ///< Path character
#     endif
      
      /// @brief Create relative path to current directory
      Directory() = default;
      /// @brief Initialize directory/file path
      Directory(const PathChar* path) noexcept
        : _directoryPath(path), _targetType(Directory::getPathTargetType(path)) { assert(path != nullptr); }
      /// @brief Initialize directory/file path
      explicit Directory(const Path& path) noexcept
          : _directoryPath(path), _targetType(Directory::getPathTargetType(path.c_str())) {}
        
      Directory(const Directory&) = default;
      Directory(Directory&&) = default;
      Directory& operator=(const Directory&) = default;
      Directory& operator=(Directory&&) = default;

      void swap(Directory& rhs) noexcept {
        std::swap(this->_directoryPath, rhs._directoryPath);
        auto bufferType = this->_targetType;
        this->_targetType = rhs._targetType;
        rhs._targetType = bufferType;
      }


      // -- Path metadata --
      
      inline const Path& getPath() const noexcept { return this->_directoryPath; } ///< Get stored path value
      
      /// @brief Type of path
      enum class PathType: uint32_t {
        absolute = 0,
        relative = 1
      };
      /// @brief Get type of stored path
      inline PathType getPathType() const noexcept { return Directory::getPathType(this->_directoryPath.c_str()); }
      /// @brief Get type of any path
      static PathType getPathType(const PathChar* path) noexcept;

      /// @brief Type of item targeted by path
      enum class PathTarget: uint32_t {
        unknown = 0,
        directory = 1,
        file = 2,
        symlink = 3,
        socket = 4
      };
      /// @brief Get type of stored path target
      inline PathTarget getPathTargetType() const noexcept { return this->_targetType; }
      /// @brief Get type of any path target
      static PathTarget getPathTargetType(const PathChar* path) noexcept;

      /// @brief Verify if stored target directory / file is hidden
      inline bool isHidden() noexcept { return Directory::isHidden(this->_directoryPath.c_str()); }
      /// @brief Verify if any directory / file is hidden
      static bool isHidden(const PathChar* path) noexcept;

      /// @brief Split stored path (on '/' or '\') into a vector of segments
      inline std::vector<Path> getSegments() const noexcept { return Directory::getPathSegments(this->_directoryPath.c_str()); }
      /// @brief Split any path (on '/' or '\') into a vector of segments
      static std::vector<Path> getPathSegments(const Path& path) noexcept;


      // -- Check existence and access mode --
      
      /// @brief Type of access to verify
      enum class AccessMode: int32_t {
        existence = 0,
        read      = 2,
        write     = 4,
        readWrite = 6
      };
      /// @brief Verify if stored path is existing / readable / writable / all
      inline bool hasAccess(AccessMode mode) const noexcept { return Directory::verifyPathAccess(this->_directoryPath.c_str(), mode); }
      /// @brief Verify if any path is existing / readable / writable / all
      static bool verifyPathAccess(const PathChar* path, AccessMode mode) noexcept;

      
      // -- file content --
      
      /// @brief Open buffered legacy file access (r/w/a/r+/w+/a+)
      /// @warning Check result.isOpen() to verify success
      static inline FileHandle openFile(const PathChar* path, const PathChar* mode) noexcept {
        assert(path != nullptr);
#       ifdef _WINDOWS
          FILE* fileAccess = nullptr;
          if (_wfopen_s(&fileAccess, path, mode) == 0)
            return FileHandle(fileAccess);
          errno = 0;
          return FileHandle{};
#       else
          return FileHandle(fopen(path, mode));
#       endif
      }
      /// @brief Open file stream (in/out, binary, ate/app/trunc)
      template <typename _StreamType>
      static inline bool openFileStream(const Path& path, std::ios_base::openmode mode, _StreamType& outStream) {
#       if !defined(_WINDOWS) || defined(_MSC_VER)
          outStream.open(path.c_str(), mode);
#       else
          std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
          std::string utf8String = converter.to_bytes(path);
          outStream.open(utf8String.c_str(), mode);
#       endif
        return outStream.is_open();
      }

      /// @brief Delete an existing file
      /// @returns 0 on success, or the value of errno on failure
      static int removeFile(const Path& path) noexcept;
      /// @brief Create a new directory
      /// @returns 0 on success, or the value of errno on failure
      static int createDirectory(const Path& path) noexcept;
      /// @brief Delete an existing directory
      /// @returns 0 on success, or the value of errno on failure
      static int removeDirectory(const Path& path) noexcept;
      
      
      // -- file metadata & directory content --

      /// @brief Directory / file metadata
      struct TargetMetadata {
        Path name;
        time_t lastUpdateTime;
        uintmax_t size = 0;
        PathTarget type = PathTarget::unknown;
        AccessMode accessMode = AccessMode::existence;
        bool isHidden = false;
      };
      
      /// @brief Read metadata of a file or directory
      static bool readFileMetadata(const Path& path, TargetMetadata& outFile) noexcept;
      /// @brief List files in directory
      static bool listFilesInDirectory(const Path& path, std::vector<Directory>& outFiles) noexcept;
      /// @brief List files in directory with their metadata
      static bool listFilesInDirectory(const Path& path, std::vector<TargetMetadata>& outFiles) noexcept;
      
      
    private:
      Directory(const PathChar* path, PathTarget targetType) noexcept
        : _directoryPath(path), _targetType(targetType) { assert(path != nullptr); }
      Directory(const Path& path, PathTarget targetType) noexcept
        : _directoryPath(path), _targetType(targetType) {}
      Directory(Path&& path, PathTarget targetType) noexcept
        : _directoryPath(std::move(path)), _targetType(targetType) {}

      static inline size_t _findDelimiter(const Path& path, size_t offset) noexcept {
#       ifdef _WINDOWS
          return path.find_first_of(L"\\/", offset);
#       else
          return path.find_first_of("\\/", offset);
#       endif
      }
    
    private:
      Path _directoryPath;
      PathTarget _targetType{ PathTarget::unknown };
    };

  }
}
