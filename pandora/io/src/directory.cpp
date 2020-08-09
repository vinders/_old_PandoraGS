/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
*******************************************************************************/
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <cassert>
#include <string>
#include <vector>
#include <errno.h>
#include "io/_private/_filesystem_api.h"
#include "io/_private/_file_time.h"

#include "io/directory.h"

using namespace pandora::io;

// -- Path metadata - absolute/relative --

// get absolute/relative path type
Directory::PathType Directory::getPathType(const Directory::PathChar* path) noexcept {
# ifdef _WINDOWS
    if (path != nullptr) {
      if (path[0] >= L'A' && path[0] <= L'Z' && path[1] == L':')
        return Directory::PathType::absolute;
      if (path[0] == L'/' || path[0] == L'\\')
        return Directory::PathType::absolute;
    }
    return Directory::PathType::relative;
# else
    return (path != nullptr  && (*path == '/' || *path == '\\')) ? Directory::PathType::absolute : Directory::PathType::relative;
# endif
}


// -- Path metadata - target type --

#ifdef _WINDOWS
  // get target type from path attributes - windows native
  static inline Directory::PathTarget _getPathTargetTypeFromAttributes(DWORD attributes) noexcept {
    if (attributes & FILE_ATTRIBUTE_DIRECTORY)
      return Directory::PathTarget::directory;
    if (attributes & FILE_ATTRIBUTE_REPARSE_POINT)
      return Directory::PathTarget::symlink;
    return Directory::PathTarget::file;
  }
#else
  // get target type from path attributes - linux/unix native
  static inline Directory::PathTarget _getPathTargetTypeFromAttributes(const struct stat& attributes) noexcept {
    if (S_ISDIR(attributes.st_mode) != 0)
      return Directory::PathTarget::directory;
    if (S_ISREG(attributes.st_mode) != 0)
      return Directory::PathTarget::file;
    if (S_ISLNK(attributes.st_mode) != 0)
      return Directory::PathTarget::symlink;
    if (S_ISSOCK(attributes.st_mode) != 0)
      return Directory::PathTarget::socket;
    return Directory::PathTarget::unknown;
  }
#endif
#ifndef _USE_NATIVE_FILESYSTEM
  // get target type from path attributes
  static inline Directory::PathTarget _getPathTargetTypeFromAttributes(const std::filesystem::directory_entry& dirEntry, std::error_code& errorCode) noexcept {
    if (dirEntry.is_directory(errorCode))
      return Directory::PathTarget::directory;
    if (dirEntry.is_regular_file(errorCode))
      return Directory::PathTarget::file;
    if (dirEntry.is_symlink(errorCode))
      return Directory::PathTarget::symlink;
    if (dirEntry.is_socket(errorCode))
      return Directory::PathTarget::socket;

    if (errorCode) {
#     ifdef _WINDOWS
        auto entryPath = dirEntry.path().wstring();
        return _getPathTargetTypeFromAttributes(GetFileAttributesW(entryPath.c_str()));
#     else
        auto entryPath = dirEntry.path().string();
        struct stat pathInfo;
        stat(entryPath.c_str(), &pathInfo);
        return _getPathTargetTypeFromAttributes(pathInfo);
#     endif
    }
    return Directory::PathTarget::unknown;
  }
#endif

// get target type from path
Directory::PathTarget Directory::getPathTargetType(const Directory::PathChar* path) noexcept {
  if (path == nullptr || *path == 0)
    return Directory::PathTarget::unknown;

# ifndef _USE_NATIVE_FILESYSTEM
    std::error_code errorCode; // to disable exceptions
    std::filesystem::directory_entry target(std::filesystem::path(path), errorCode);
    Directory::PathTarget type = _getPathTargetTypeFromAttributes(target, errorCode);
    return type;
# else
#   ifdef _WINDOWS
      if (PathFileExistsW(path) == TRUE)
        return _getPathTargetTypeFromAttributes(GetFileAttributesW(path));
      return Directory::PathTarget::unknown;
#   else
      struct stat pathInfo;
      stat(path, &pathInfo);
      return _getPathTargetTypeFromAttributes(pathInfo);
#   endif
# endif
}


// -- Path metadata - hidden files --

#ifdef _WINDOWS
  // verify if a target is hidden - windows native
  static inline bool _isHiddenFile(DWORD attributes) noexcept {
    return ((attributes & FILE_ATTRIBUTE_HIDDEN) != 0);
  }
#else
  // verify if a target is hidden - linux/unix native
  static inline bool _isHiddenFile(const Directory::PathChar* fileName) noexcept {
    assert(fileName != nullptr);
    return (fileName[0] == '.' && (fileName[1] != 0 && fileName[1] != '/') && (fileName[1] != '.' && fileName[2] != 0 && fileName[2] != '/'));
  }
#endif

// verify if a target is hidden
bool Directory::isHidden(const Directory::PathChar* path) noexcept {
  assert(path != nullptr);
# ifdef _WINDOWS
    return _isHiddenFile(GetFileAttributesW(path));
# else
    size_t last = std::string(path).find_last_of('/');
    return (last == std::string::npos) ? _isHiddenFile(path) : _isHiddenFile(&path[last + 1u]);
# endif
}


// -- Path metadata - segments --

// split a path into segments
std::vector<Directory::Path> Directory::getPathSegments(const Directory::Path& path) noexcept {
  std::vector<Directory::Path> segments;
  size_t current = 0u;
  size_t last = 0u;
  
  if (!path.empty()) {
    current = Directory::_findDelimiter(path, 0u);
    while (current != Directory::Path::npos) {
      if (current != last)
        segments.emplace_back(path.substr(last, current - last));
      last = current + 1;
      current = Directory::_findDelimiter(path, last);
    }
    if (last < path.size())
      segments.emplace_back(path.substr(last));
  }
  return segments;
}


// -- Check existence and access mode --

// verify if an access mode is available for a path
bool Directory::verifyPathAccess(const Directory::PathChar* path, Directory::AccessMode mode) noexcept {
  assert(path != nullptr);
  return (access(path, static_cast<int32_t>(mode)) == 0);
}

// get available access mode for a path (doesn't guarantee the file exists!)
static inline Directory::AccessMode _getAccessMode(const Directory::PathChar* path) noexcept {
  assert(path != nullptr);
  Directory::AccessMode accessMode = (access(path, static_cast<int32_t>(Directory::AccessMode::read)) == 0)
                                   ? Directory::AccessMode::read : Directory::AccessMode::existence;
  if (access(path, static_cast<int32_t>(Directory::AccessMode::write)) == 0)
    accessMode = static_cast<Directory::AccessMode>(static_cast<int32_t>(accessMode) | static_cast<int32_t>(Directory::AccessMode::write));
  return accessMode;
}


// -- Directory / file content --

int Directory::removeFile(const Path& path) noexcept {
# ifdef _WINDOWS
    if (_wremove(path.c_str()) == 0)
      return 0;
# else
    if (remove(path.c_str()) == 0)
      return 0;
# endif
  int errorCode = (errno != 0) ? errno : -1;
  errno = 0;
  return errorCode;
}

int Directory::createDirectory(const Path& path) noexcept {
# ifdef _WINDOWS
    if (_wmkdir(path.c_str()) == 0)
      return 0;
# else
    if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0)
      return 0;
# endif
  int errorCode = (errno != 0) ? errno : -1;
  errno = 0;
  return errorCode;
}

int Directory::removeDirectory(const Path& path) noexcept {
# ifdef _WINDOWS
    if (_wrmdir(path.c_str()) == 0)
      return 0;
# else
    if (rmdir(path.c_str()) == 0)
      return 0;
# endif
  int errorCode = (errno != 0) ? errno : -1;
  errno = 0;
  return errorCode;
}


// -- Directory / file metadata --

bool Directory::readFileMetadata(const Directory::Path& path, Directory::TargetMetadata& outFile) noexcept {
  if (path.empty())
    return false;

# ifndef _USE_NATIVE_FILESYSTEM
    std::error_code errorCode; // to disable exceptions
    std::filesystem::directory_entry entry(path, errorCode);
    if (errorCode || !entry.exists())
      return false;

    outFile = Directory::TargetMetadata{
#     ifdef _WINDOWS
        Directory::Path(entry.path().filename().wstring()),
#     else
        Directory::Path(entry.path().filename().string()),
#     endif
      _toWriteTime(entry),
      entry.file_size(errorCode),
      _getPathTargetTypeFromAttributes(entry, errorCode),
      Directory::AccessMode::existence,
      false
    };

    outFile.accessMode = _getAccessMode(path.c_str());
#   ifdef _WINDOWS
      outFile.isHidden = isHidden(path.c_str());
#   else
      outFile.isHidden = _isHiddenFile(outFile.name.c_str());
#   endif
    return true;

# else
#   ifdef _WINDOWS
      WIN32_FIND_DATAW fileInfo;
      HANDLE fileHandle = FindFirstFileW(path.c_str(), &fileInfo);
      SetLastError(0);
      if (fileHandle == INVALID_HANDLE_VALUE)
        return false;

      outFile = Directory::TargetMetadata{
        Directory::Path(fileInfo.cFileName),
        _toWriteTime(fileInfo.ftLastWriteTime),
        (static_cast<uintmax_t>(fileInfo.nFileSizeHigh) << sizeof(fileInfo.nFileSizeLow) * 8) | static_cast<uintmax_t>(fileInfo.nFileSizeLow),
        _getPathTargetTypeFromAttributes(fileInfo.dwFileAttributes),
        _getAccessMode(path.c_str()),
        _isHiddenFile(fileInfo.dwFileAttributes)
      };
      DWORD error = GetLastError();
      FindClose(fileHandle);
      SetLastError(0);
      return (error == 0 || error == ERROR_NO_MORE_FILES);

#   else
      if (!verifyPathAccess(path.c_str(), Directory::AccessMode::existence))
        return false;

      size_t pathLastChar = path.size() - 1u;
      while (pathLastChar > 0u && (path[pathLastChar] == '/' || path[pathLastChar] == '\\'))
        --pathLastChar;
      size_t lastSeparator = path.find_last_of("/\\", pathLastChar);

      struct stat fileMetadata;
      stat(path.c_str(), &fileMetadata);
      outFile = Directory::TargetMetadata{
        Directory::Path((lastSeparator != std::string::npos) ? path.substr(lastSeparator + 1u) : path),
        fileMetadata.st_mtime,
        static_cast<uintmax_t>(fileMetadata.st_size),
        _getPathTargetTypeFromAttributes(fileMetadata),
        _getAccessMode(path.c_str()),
        false
      };
      outFile.isHidden = _isHiddenFile(outFile.name.c_str());
      return true;
#   endif
# endif
}

// get list of files present in path directory - target name + type
bool Directory::listFilesInDirectory(const Directory::Path& path, std::vector<Directory>& outFiles) noexcept {
# ifndef _USE_NATIVE_FILESYSTEM
    std::error_code errorCode; // to disable exceptions
    for (auto& entry : std::filesystem::directory_iterator(path, errorCode)) {
      Directory::PathTarget type = _getPathTargetTypeFromAttributes(entry, errorCode);
#     ifdef _WINDOWS
        outFiles.push_back(Directory(entry.path().filename().wstring(), type));
#     else
        outFiles.push_back(Directory(entry.path().filename().string(), type));
#     endif
    }
    return (!errorCode);

# else
#   ifdef _WINDOWS
      Directory::PathChar separator = (getPathType(path.c_str()) == Directory::PathType::absolute) ? L'\\' : L'/';
      Directory::Path query = (path.back() != separator) ? path + separator + L'*' : path + L'*';
      WIN32_FIND_DATAW fileInfo;
      HANDLE fileHandle = FindFirstFileW(query.c_str(), &fileInfo);
      SetLastError(0);
      if (fileHandle == INVALID_HANDLE_VALUE)
        return false;

      do {
        outFiles.push_back( Directory(Directory::Path(fileInfo.cFileName), _getPathTargetTypeFromAttributes(fileInfo.dwFileAttributes)) );
      }
      while (FindNextFileW(fileHandle, &fileInfo) != 0);

      DWORD error = GetLastError();
      FindClose(fileHandle);
      SetLastError(0);
      return (error == 0 || error == ERROR_NO_MORE_FILES);

#   else
      DIR* access = opendir(path.c_str());
      if (access == nullptr)
        return false;

      struct dirent* fileInfo = nullptr;
      while ((fileInfo = readdir(access)) != nullptr)
        outFiles.emplace_back(Directory::Path(fileInfo->d_name));
      closedir(access);
      return true;
#   endif
# endif
}

// get list of files present in path directory - full target metadata
bool Directory::listFilesInDirectory(const Directory::Path& path, std::vector<Directory::TargetMetadata>& outFiles) noexcept {
# ifdef _WINDOWS
    Directory::PathChar separator = (getPathType(path.c_str()) == Directory::PathType::absolute) ? L'\\' : L'/';
# else
    Directory::PathChar separator = '/';
# endif
  Directory::Path pathWithSeparator = (path.back() != separator) ? path + separator : path;

# ifndef _USE_NATIVE_FILESYSTEM
    std::error_code errorCode; // to disable exceptions
    for (auto& entry : std::filesystem::directory_iterator(path, errorCode)) {
      outFiles.emplace_back(Directory::TargetMetadata{
#       ifdef _WINDOWS
          Directory::Path(entry.path().filename().wstring()),
#       else
          Directory::Path(entry.path().filename().string()),
#       endif
        _toWriteTime(entry),
        entry.file_size(errorCode),
        _getPathTargetTypeFromAttributes(entry, errorCode),
        Directory::AccessMode::existence,
        false
      });

      auto fullPath = pathWithSeparator + outFiles.back().name;
      outFiles.back().accessMode = _getAccessMode(fullPath.c_str());
#     ifdef _WINDOWS
        outFiles.back().isHidden = isHidden(fullPath.c_str());
#     else
        outFiles.back().isHidden = _isHiddenFile(outFiles.back().name.c_str());
#     endif
    }
    return (!errorCode);

# else
#   ifdef _WINDOWS
      WIN32_FIND_DATAW fileInfo;
      pathWithSeparator.push_back(L'*');
      HANDLE fileHandle = FindFirstFileW(pathWithSeparator.c_str(), &fileInfo);
      SetLastError(0);
      if (fileHandle == INVALID_HANDLE_VALUE)
        return false;
      pathWithSeparator.pop_back();

      do {
        auto fullPath = pathWithSeparator + fileInfo.cFileName;

        outFiles.emplace_back(Directory::TargetMetadata{
          Directory::Path(fileInfo.cFileName),
          _toWriteTime(fileInfo.ftLastWriteTime),
          (static_cast<uintmax_t>(fileInfo.nFileSizeHigh) << sizeof(fileInfo.nFileSizeLow)*8) | static_cast<uintmax_t>(fileInfo.nFileSizeLow),
          _getPathTargetTypeFromAttributes(fileInfo.dwFileAttributes),
          _getAccessMode(fullPath.c_str()),
          _isHiddenFile(fileInfo.dwFileAttributes)
        });
      }
      while (FindNextFileW(fileHandle, &fileInfo) != 0);

      DWORD error = GetLastError();
      FindClose(fileHandle);
      SetLastError(0);
      return (error == 0 || error == ERROR_NO_MORE_FILES);

#   else
      DIR* access = opendir(path.c_str());
      if (access == nullptr)
        return false;

      struct stat fileMetadata;
      struct dirent* fileInfo = nullptr;
      while ((fileInfo = readdir(access)) != nullptr) {
        auto fullPath = pathWithSeparator + fileInfo->d_name;
        stat(fullPath.c_str(), &fileMetadata);

        outFiles.emplace_back(Directory::TargetMetadata{
            Directory::Path(fileInfo->d_name),
            fileMetadata.st_mtime,
            static_cast<uintmax_t>(fileMetadata.st_size),
            _getPathTargetTypeFromAttributes(fileMetadata),
            _getAccessMode(fullPath.c_str()),
            _isHiddenFile(fileInfo->d_name)
        });
      }
      closedir(access);
      return true;
#   endif
# endif
}
