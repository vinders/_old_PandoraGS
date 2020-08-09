/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
*******************************************************************************/
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <string>
#include <vector>
#include "io/_private/_filesystem_api.h"
#include "io/_private/_system_location_finder.h"
#include "io/_private/_user_home_path.h"

#if !defined(_WINDOWS)
# if defined(__APPLE__)
#   include <TargetConditionals.h>
# elif defined(__linux__) || defined(__linux) || defined(__unix__) || defined(__unix)
#   include <cstdlib>
#   include "io/_private/_key_value_file_reader.h"
# endif
#endif

#include "io/standard_location.h"

using namespace pandora::io;

// -- current location --

pandora::io::Directory::Path LocationFinder::currentLocation() noexcept {
# ifndef _USE_NATIVE_FILESYSTEM
    std::error_code errorCode;
#   ifdef _WINDOWS
      pandora::io::Directory::Path outPath = std::filesystem::current_path(errorCode).wstring();
#   else
      pandora::io::Directory::Path outPath = std::filesystem::current_path(errorCode).u8string();
#   endif
    if (!errorCode)
      return outPath;
# endif
# ifdef _WINDOWS
    wchar_t buffer[MAX_PATH+1];
    if (GetCurrentDirectoryW(sizeof(buffer) / sizeof(wchar_t), buffer) > 0)
      return pandora::io::Directory::Path(buffer);
# else
    char* buffer = getenv("PWD");
    if (buffer != nullptr)
      return pandora::io::Directory::Path(buffer);
    char curdir[_MAX_PATH_SIZE];
    if (getcwd(curdir, _MAX_PATH_SIZE) == 0)
      return pandora::io::Directory::Path(curdir);
# endif
  return pandora::io::Directory::Path{};
}

// -- home directory location --

pandora::io::Directory::Path LocationFinder::homeLocation() noexcept {
# if !defined(__ANDROID__) || defined(_WINDOWS) || defined(__APPLE__)
    return _getUserHomePath();
# else
    return pandora::io::Directory::Path("files");
# endif
}

// -- standard locations --

#if defined(__linux__) || defined(__linux) || defined(__unix__) || defined(__unix)
  // read customizable user directory path
  static inline pandora::io::Directory::Path _getUserDataDirectoryPath(const pandora::io::Directory::PathChar* prop, const pandora::io::Directory::PathChar* defaultSuffix) noexcept {
    assert(prop != nullptr && defaultSuffix != nullptr);
    char buffer[_MAX_PATH_SIZE];

    // local directory settings
    std::string homeConfig = _getUserLocalConfigPath() + "/user-dirs.dirs";
    std::string localPropertyId = std::string("XDG_") + prop + "_DIR";
    size_t bufferLength = KeyValueFileReader::searchFileForValue(homeConfig.c_str(), localPropertyId.c_str(), buffer, _MAX_PATH_SIZE);
    if (bufferLength != 0u) {
      if (bufferLength >= 5 && memcmp(buffer, "$HOME", 5) == 0) // buffer has $HOME/path
        return _getUserHomePath() + &buffer[5];
      return Directory::Path(buffer); // buffer has absolute path
    }
    // global directory settings
    if (KeyValueFileReader::searchFileForValue("/etc/xdg/user-dirs.defaults", prop, buffer, _MAX_PATH_SIZE) != 0u)
      return _getUserHomePath() + '/' + buffer; // buffer has relative path
    // default directory settings
    return _getUserHomePath() + defaultSuffix;
  }
#endif

// get special system/user directory
std::vector<pandora::io::Directory::Path> LocationFinder::standardLocations(StandardLocation location, const pandora::io::Directory::Path& appDirLabel, uint32_t systemFlags) noexcept {
  std::vector<pandora::io::Directory::Path> paths;
    
# if defined(_WINDOWS)
    switch (location) {
      case StandardLocation::home:      paths = { _findLocation(FOLDERID_Profile, systemFlags, L"%USERPROFILE%") }; break;
      case StandardLocation::desktop:   paths = { _findLocation(FOLDERID_Desktop, systemFlags, L"%USERPROFILE%\\Desktop") }; break;
      case StandardLocation::documents: paths = { _findLocation(FOLDERID_Documents, systemFlags, L"%USERPROFILE%\\Documents") }; break;
      case StandardLocation::pictures:  paths = { _findLocation(FOLDERID_Pictures, systemFlags, L"%USERPROFILE%\\Pictures") }; break;
      case StandardLocation::music:     paths = { _findLocation(FOLDERID_Music, systemFlags, L"%USERPROFILE%\\Music") }; break;
      case StandardLocation::movies:    paths = { _findLocation(FOLDERID_Videos, systemFlags, L"%USERPROFILE%\\Videos") }; break;
      case StandardLocation::downloads: paths = { _findLocation(FOLDERID_Downloads, systemFlags, L"%USERPROFILE%\\Downloads") }; break;

      case StandardLocation::applications:       paths = { _findLocation(FOLDERID_StartMenu, systemFlags, L"%APPDATA%\\Microsoft\\Windows\\Start Menu"),
                                                           _findLocation(FOLDERID_CommonStartMenu, systemFlags, L"%ALLUSERSPROFILE%\\Microsoft\\Windows\\Start Menu") }; break;
      case StandardLocation::sharedApplications: paths = { _findLocation(FOLDERID_CommonStartMenu, systemFlags, L"%ALLUSERSPROFILE%\\Microsoft\\Windows\\Start Menu") }; break;
      case StandardLocation::appMenu:            paths = { _findLocation(FOLDERID_StartMenu, systemFlags, L"%APPDATA%\\Microsoft\\Windows\\Start Menu") }; break;
      case StandardLocation::fonts:              paths = { _findLocation(FOLDERID_Fonts, systemFlags, L"%windir%\\Fonts") }; break;
        
      case StandardLocation::appData:        paths = { _findLocation(FOLDERID_RoamingAppData, systemFlags, L"%APPDATA%") + L"\\" + appDirLabel,
                                                        _findLocation(FOLDERID_ProgramData, systemFlags, L"%ALLUSERSPROFILE%") + L"\\" + appDirLabel,
                                                        currentLocation(),
                                                        currentLocation() + L"\\data" }; break;
      case StandardLocation::localAppData:   paths = { _findLocation(FOLDERID_LocalAppData, systemFlags, L"%LOCALAPPDATA%") + L"\\" + appDirLabel,
                                                        _findLocation(FOLDERID_ProgramData, systemFlags, L"%ALLUSERSPROFILE%") + L"\\" + appDirLabel,
                                                        currentLocation(),
                                                        currentLocation() + L"\\data" }; break;
      case StandardLocation::appConfig:      paths = { _findLocation(FOLDERID_LocalAppData, systemFlags, L"%LOCALAPPDATA%") + L"\\" + appDirLabel,
                                                        _findLocation(FOLDERID_ProgramData, systemFlags, L"%ALLUSERSPROFILE%") + L"\\" + appDirLabel }; break;
      case StandardLocation::commonAppConfig:paths = { _findLocation(FOLDERID_LocalAppData, systemFlags, L"%LOCALAPPDATA%"),
                                                        _findLocation(FOLDERID_ProgramData, systemFlags, L"%ALLUSERSPROFILE%") }; break;
      case StandardLocation::cache:          paths = { _findLocation(FOLDERID_LocalAppData, systemFlags, L"%LOCALAPPDATA%") + L"\\" + appDirLabel + L"\\cache" }; break;
      case StandardLocation::localCache:     paths = { _findLocation(FOLDERID_LocalAppData, systemFlags, L"%LOCALAPPDATA%") + L"\\cache" }; break;
      case StandardLocation::sharedCache:    paths = { _findLocation(FOLDERID_ProgramData, systemFlags, L"%ALLUSERSPROFILE%") + L"\\cache" }; break;
      case StandardLocation::temp:           paths = { _findLocation(FOLDERID_LocalAppData, systemFlags, L"%LOCALAPPDATA%") + L"\\Temp" }; break;

#     if defined(_WIN64) || defined(WIN64) || defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64) || defined(_AMD64_) || defined(_M_X64) || defined(_M_I64) || defined(_M_IX64) || defined(_M_AMD64) || defined(__IA64__) || defined(_IA64_)
        case StandardLocation::system:            paths = { _findLocation(FOLDERID_SystemX86, systemFlags, L"%windir%\\system32"),
                                                            _findLocation(FOLDERID_System, systemFlags, L"%windir%\\syswow64"),
                                                            _findLocation(FOLDERID_Windows, systemFlags, L"%windir%") }; break;
        case StandardLocation::sharedBinaries:    paths = { _findLocation(FOLDERID_ProgramFiles, systemFlags, L"%ProgramFiles%"),
                                                            _findLocation(FOLDERID_ProgramFilesX86, systemFlags, L"%ProgramFiles% (x86)") }; break;
        case StandardLocation::sharedLibraries:   paths = { _findLocation(FOLDERID_System, systemFlags, L"%windir%\\syswow64"),
                                                            _findLocation(FOLDERID_SystemX86, systemFlags, L"%windir%\\system32"),
                                                            _findLocation(FOLDERID_ProgramFilesCommon, systemFlags, L"%ProgramFiles%\\Common Files"),
                                                            _findLocation(FOLDERID_ProgramFilesCommonX86, systemFlags, L"%ProgramFiles% (x86)\\Common Files") }; break;
        case StandardLocation::commonBinaryFiles: paths = { _findLocation(FOLDERID_ProgramFilesCommon, systemFlags, L"%ProgramFiles%\\Common Files"),
                                                            _findLocation(FOLDERID_ProgramFilesCommonX86, systemFlags, L"%ProgramFiles% (x86)\\Common Files") }; break;
#     else
        case StandardLocation::system:            paths = { _findLocation(FOLDERID_System, systemFlags, L"%windir%\\system32"),
                                                            _findLocation(FOLDERID_Windows, systemFlags, L"%windir%") }; break;
        case StandardLocation::sharedBinaries:    paths = { _findLocation(FOLDERID_ProgramFiles, systemFlags, L"%ProgramFiles%") }; break;
        case StandardLocation::sharedLibraries:   paths = { _findLocation(FOLDERID_System, systemFlags, L"%windir%\\system32"),
                                                            _findLocation(FOLDERID_ProgramFilesCommon, systemFlags, L"%ProgramFiles%\\Common Files") }; break;
        case StandardLocation::commonBinaryFiles: paths = { _findLocation(FOLDERID_ProgramFilesCommon, systemFlags, L"%ProgramFiles%\\Common Files") }; break;
#     endif
      case StandardLocation::localBinaries:       paths = { _findLocation(FOLDERID_UserProgramFiles, systemFlags, L"%LOCALAPPDATA%\\Programs") }; break;
        
      case StandardLocation::printers: { 
        pandora::io::Directory::Path path;
        if (_findLocation(FOLDERID_PrintersFolder, path, systemFlags)) 
          paths = { path }; 
        break;
      }
      case StandardLocation::trash: { 
        pandora::io::Directory::Path path;
        if (_findLocation(FOLDERID_RecycleBinFolder, path, systemFlags)) 
          paths = { path }; 
        break;
      }
      default: assert(false); break;
    }
    
# elif defined(__APPLE__)
#   if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
      switch (location) {
        case StandardLocation::home:      paths = { _getUserHomePath() }; break;
        case StandardLocation::desktop:   paths = { "Documents/Desktop" }; break;
        case StandardLocation::documents: paths = { "Documents" }; break;
        case StandardLocation::pictures:  paths = { "Documents/Pictures" }; break;
        case StandardLocation::music:     paths = { "Documents/Music" }; break;
        case StandardLocation::movies:    paths = { "Documents/Movies" }; break;
        case StandardLocation::downloads: paths = { "Documents/Downloads" }; break;

        case StandardLocation::applications:      break;
        case StandardLocation::sharedApplications:break;
        case StandardLocation::appMenu:           paths = { "/System/Library/CoreServices" }; break;
        case StandardLocation::fonts:             paths = { "Library/Fonts", "/System/Library/Fonts" }; break;
                
        case StandardLocation::appData:
        case StandardLocation::localAppData:   paths = { "Library/Application Support" }; break;
        case StandardLocation::appConfig:      paths = { std::string("Library/Preferences/") + appDirLabel }; break;
        case StandardLocation::commonAppConfig:paths = { "Library/Preferences" }; break;
        case StandardLocation::cache:
        case StandardLocation::localCache:     paths = { "Library/Caches" }; break;
        case StandardLocation::sharedCache:    paths = { "Documents/Inbox" }; break;
        case StandardLocation::temp:           paths = { "tmp" }; break;

        case StandardLocation::system:            break;
        case StandardLocation::localBinaries:     break;
        case StandardLocation::sharedBinaries:    break;
        case StandardLocation::sharedLibraries:   paths = { "lib", "Library/Frameworks", "/System/Library/Frameworks" }; break;
        case StandardLocation::commonBinaryFiles: paths = { "Library" }; break;

        case StandardLocation::printers: paths = { "Library/Printers/PPDs" }; break;
        case StandardLocation::trash:    break;
        default: assert(false); break;
      }
#   else
      switch (location) {
        case StandardLocation::home:      paths = { _getUserHomePath() }; break;
        case StandardLocation::desktop:   paths = _findLocation(SYSDIR_DIRECTORY_DESKTOP, SYSDIR_DOMAIN_MASK_USER, _getUserHomePath() + "/Desktop"); break;
        case StandardLocation::documents: paths = _findLocation(SYSDIR_DIRECTORY_DOCUMENT, SYSDIR_DOMAIN_MASK_USER, _getUserHomePath() + "/Documents"); break;
        case StandardLocation::pictures:  paths = _findLocation(SYSDIR_DIRECTORY_PICTURES, SYSDIR_DOMAIN_MASK_USER, _getUserHomePath() + "/Pictures"); break;
        case StandardLocation::music:     paths = _findLocation(SYSDIR_DIRECTORY_MUSIC, SYSDIR_DOMAIN_MASK_USER, _getUserHomePath() + "/Music"); break;
        case StandardLocation::movies:    paths = _findLocation(SYSDIR_DIRECTORY_MOVIES, SYSDIR_DOMAIN_MASK_USER, _getUserHomePath() + "/Movies"); break;
        case StandardLocation::downloads: paths = _findLocation(SYSDIR_DIRECTORY_DOWNLOADS, SYSDIR_DOMAIN_MASK_USER, _getUserHomePath() + "/Downloads"); break;

        case StandardLocation::applications:      paths = _findLocation(SYSDIR_DIRECTORY_ALL_APPLICATIONS, SYSDIR_DOMAIN_MASK__SYSTEM_USER_LOCAL, _getUserHomePath() + "/Applications"); break;
        case StandardLocation::sharedApplications:paths = _findLocation(SYSDIR_DIRECTORY_ALL_APPLICATIONS, SYSDIR_DOMAIN_MASK_ALL, _getUserHomePath() + "/Applications"); break;
        case StandardLocation::appMenu:           paths = _findLocation(SYSDIR_DIRECTORY_CORESERVICE, SYSDIR_DOMAIN_MASK_ALL, "/System/Library/CoreServices"); break;
        case StandardLocation::fonts:             paths = _findLocation(SYSDIR_DIRECTORY_ALL_LIBRARIES, SYSDIR_DOMAIN_MASK_ALL, "/System/Library", "/Fonts"); break;
        
        case StandardLocation::appData:        paths = _findLocation(SYSDIR_DIRECTORY_ALL_LIBRARIES, SYSDIR_DOMAIN_MASK_ALL, _getUserHomePath() + "/Library/Application Support", std::string("/") + appDirLabel); paths.emplace_back("../Resources"); break;
        case StandardLocation::localAppData:   paths = _findLocation(SYSDIR_DIRECTORY_ALL_LIBRARIES, SYSDIR_DOMAIN_MASK_ALL, _getUserHomePath() + "/Library/Application Support"); paths.emplace_back("../Resources"); break;
        case StandardLocation::appConfig:      paths = _findLocation(SYSDIR_DIRECTORY_LIBRARY, SYSDIR_DOMAIN_MASK_USER, _getUserHomePath() + "/Library", std::string("/Preferences/") + appDirLabel); break;
        case StandardLocation::commonAppConfig:paths = _findLocation(SYSDIR_DIRECTORY_LIBRARY, SYSDIR_DOMAIN_MASK_USER, _getUserHomePath() + "/Library", "/Preferences"); break;
        case StandardLocation::cache:          paths = _findLocation(SYSDIR_DIRECTORY_CACHES, SYSDIR_DOMAIN_MASK__USER_LOCAL, _getUserHomePath() + "/Library/Caches", std::string("/") + appDirLabel); break;
        case StandardLocation::localCache:     paths = _findLocation(SYSDIR_DIRECTORY_CACHES, SYSDIR_DOMAIN_MASK__USER_LOCAL, _getUserHomePath() + "/Library/Caches"); break;
        case StandardLocation::sharedCache:    paths = _findLocation(SYSDIR_DIRECTORY_CACHES, SYSDIR_DOMAIN_MASK_LOCAL, "/Library/Caches", std::string("/") + appDirLabel); break;
        case StandardLocation::temp: {
          char* envBuffer = getenv("TMPDIR");
          paths.emplace_back((envBuffer != nullptr) ? envBuffer : "$TMPDIR");
          break;
        }

        case StandardLocation::system:           paths = _findLocation(SYSDIR_DIRECTORY_ADMIN_APPLICATION, SYSDIR_DOMAIN_MASK__SYSTEM_LOCAL); paths.emplace_back("/System"); break;
        case StandardLocation::localBinaries:    paths = _findLocation(SYSDIR_DIRECTORY_ALL_APPLICATIONS, SYSDIR_DOMAIN_MASK_USER, "/System/Library/CoreServices"); break;
        case StandardLocation::sharedBinaries:   paths = _findLocation(SYSDIR_DIRECTORY_ALL_APPLICATIONS, SYSDIR_DOMAIN_MASK__SYSTEM_NET_LOCAL, "/System/Library/CoreServices"); break;
        case StandardLocation::sharedLibraries:  paths = { _getUserHomePath() + "/lib", "/usr/local/lib", "/usr/lib" }; break;
        case StandardLocation::commonBinaryFiles:paths = _findLocation(SYSDIR_DIRECTORY_ALL_LIBRARIES, SYSDIR_DOMAIN_MASK__SYSTEM_LOCAL, "/Library"); break;

        case StandardLocation::printers: paths = _findLocation(SYSDIR_DIRECTORY_PRINTER_DESCRIPTION, SYSDIR_DOMAIN_MASK_ALL); break;
        case StandardLocation::trash:    paths = _findLocation(SYSDIR_DIRECTORY_DESKTOP, SYSDIR_DOMAIN_MASK_USER, _getUserHomePath() + "/Desktop", "/.Trash"); break;
        default: assert(false); break;
      }
#   endif
    
# elif defined(__ANDROID__)
    switch (location) {
      case StandardLocation::home:      paths = { "files" }; break;
      case StandardLocation::desktop:   paths = { "files" }; break;
      case StandardLocation::documents: paths = { "files/Documents", "/storage/sdcard0/Documents", std::string("/storage/sdcard0/") + appDirLabel + "/Documents" }; break;
      case StandardLocation::pictures:  paths = { "files/Pictures", "/storage/sdcard0/Pictures" }; break;
      case StandardLocation::music:     paths = { "files/Music", "/storage/sdcard0/Music" }; break;
      case StandardLocation::movies:    paths = { "files/Movies", "/storage/sdcard0/Movies" }; break;
      case StandardLocation::downloads: paths = { "files/Downloads", "/storage/sdcard0/Downloads", std::string("/storage/sdcard0/") + appDirLabel + "/Downloads" }; break;

      case StandardLocation::applications:      break;
      case StandardLocation::sharedApplications:break;
      case StandardLocation::appMenu:           break;
      case StandardLocation::fonts:             paths = { "/system/fonts" }; break;
                
      case StandardLocation::appData:
      case StandardLocation::localAppData:   paths = { "files" }; break;
      case StandardLocation::appConfig:      paths = { "files/settings/" }; break;
      case StandardLocation::commonAppConfig:paths = { "/storage/sdcard0/settings" }; break;
      case StandardLocation::cache:          paths = { "cache" }; break;
      case StandardLocation::localCache:     paths = { "cache", std::string("/storage/sdcard0/") + appDirLabel + "/cache" }; break;
      case StandardLocation::sharedCache:    paths = { "/storage/sdcard0/cache" }; break;
      case StandardLocation::temp:           paths = { "cache" }; break;

      case StandardLocation::system:            break;
      case StandardLocation::localBinaries:     break;
      case StandardLocation::sharedBinaries:    break;
      case StandardLocation::sharedLibraries:   paths = { "lib" }; break;
      case StandardLocation::commonBinaryFiles: break;

      case StandardLocation::printers: break;
      case StandardLocation::trash:    break;
      default: assert(false); break;
    }
    
# elif defined(__linux__) || defined(__linux) || defined(__unix__) || defined(__unix)
    switch (location) {
      case StandardLocation::home:      paths = { _getUserHomePath() }; break;
      case StandardLocation::desktop:   paths = { _getUserDataDirectoryPath("DESKTOP", "/Desktop") }; break;
      case StandardLocation::documents: paths = { _getUserDataDirectoryPath("DOCUMENTS", "/Documents") }; break;
      case StandardLocation::pictures:  paths = { _getUserDataDirectoryPath("PICTURES", "/Pictures") }; break;
      case StandardLocation::music:     paths = { _getUserDataDirectoryPath("MUSIC", "/Music") }; break;
      case StandardLocation::movies:    paths = { _getUserDataDirectoryPath("VIDEOS", "/Videos") }; break;
      case StandardLocation::downloads: paths = { _getUserDataDirectoryPath("DOWNLOAD", "/Downloads") }; break;

      case StandardLocation::applications:      paths = { _getUserHomePath() + "/.local/share/applications", "/usr/local/share/applications", "/usr/share/applications" }; break;
      case StandardLocation::sharedApplications:paths = { "/usr/local/share/applications", "/usr/share/applications" }; break;
      case StandardLocation::appMenu:           paths = { _getUserHomePath() + "/.local/share/applications" }; break;
      case StandardLocation::fonts:             paths = { "/usr/share/fonts", "/usr/local/share/fonts", _getUserHomePath() + "/.fonts", _getUserHomePath() + "/.local/share/fonts" }; break;

      case StandardLocation::appData:        paths = { _getUserHomePath() + "/.local/share/" + appDirLabel, std::string("/usr/local/share/") + appDirLabel, std::string("/usr/share/") + appDirLabel }; break;
      case StandardLocation::localAppData:   paths = { _getUserHomePath() + "/.local/share", "/usr/local/share", "/usr/share" }; break;
      case StandardLocation::appConfig:      paths = { _getUserHomePath() + "/.config/" + appDirLabel, std::string("/etc/xdg/") + appDirLabel }; break;
      case StandardLocation::commonAppConfig:paths = { _getUserHomePath() + "/.config", "/etc/xdg" }; break;
      case StandardLocation::cache:          paths = { _getUserHomePath() + "/.cache/" + appDirLabel }; break;
      case StandardLocation::localCache:     paths = { _getUserHomePath() + "/.cache" }; break;
      case StandardLocation::sharedCache:    paths = { "/root" }; break;
      case StandardLocation::temp:           paths = { "/tmp", "/var/tmp" }; break;

      case StandardLocation::system:            paths = { "/bin", "/lib", "/sys" }; break;
      case StandardLocation::localBinaries:     paths = { "/usr/local/bin" }; break;
      case StandardLocation::sharedBinaries:    paths = { "/bin", "/usr/bin", "/usr/share/bin" }; break;
      case StandardLocation::sharedLibraries:   paths = { "/lib", "/usr/lib", "/usr/local/lib", "/usr/share/lib" }; break;
      case StandardLocation::commonBinaryFiles: paths = { "/usr/lib", "/usr/share/lib" }; break;

      case StandardLocation::printers: paths = { "/dev" }; break;
      case StandardLocation::trash:    paths = { _getUserHomePath() + "/.local/share/Trash" }; break;
      default: assert(false); break;
    }
# endif
  return paths;
}
