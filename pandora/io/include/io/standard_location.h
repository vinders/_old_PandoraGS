/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
*******************************************************************************/
#pragma once

#include <cstdio>
#include <cassert>
#include <string>
#include <vector>
#include "./directory.h"

#ifdef _WINDOWS
# define __P_DEFAULT_LOCATION_APP_DIR L"common"
#else
# define __P_DEFAULT_LOCATION_APP_DIR "common"
#endif

namespace pandora { 
  namespace io {
    /// @brief Standard known directories
    enum class StandardLocation: uint32_t {
      // user data directories
      home = 0u, ///< User home directory (may be application-specific on embedded/mobile systems)
      desktop,   ///< User desktop directory (or home directory, if system doesn't have a desktop)
      documents, ///< User document files directory
      pictures,  ///< User image files directory (or user documents, if system doesn't have a picture directory)
      music,     ///< User audio/music files directory (or user documents, if system doesn't have a music directory)
      movies,    ///< User video/movie files directory (or user documents, if system doesn't have a movie directory)
      downloads, ///< User downloaded files directory (may be application-specific on embedded/mobile systems)
      
      // application directories
      applications,       ///< User installed application links directory (right access is platform-specific)
      sharedApplications, ///< System-wide application links directory (may be the same as 'applications' on some systems)
      appMenu,            ///< Application menu directories (start menu, ...)
      fonts,              ///< System directories containing available fonts (read-only)

      // app data directories
      appData,        ///< Directory where persistent application-specific data can be written (profiles, settings, ...)
      localAppData,   ///< Directory where local application data can be written
      appConfig,      ///< Directory where application-specific configuration files for current user can be stored
      commonAppConfig,///< Directory where configuration files for current user can be stored
      cache,          ///< Directory where non-essential application-specific data for current user can be written
      localCache,     ///< Directory where non-essential application data for current user can be written
      sharedCache,    ///< Directory where shared non-essential application data can be written
      temp,           ///< Directory where short-term temporary files can be written

      // binary/system directories
      system,           ///< System binary files directories (may not exist on embedded/mobile systems)
      localBinaries,    ///< User-specific application binary files directories (may not exist on embedded/mobile systems)
      sharedBinaries,   ///< Installed application binary files directories (may not exist on embedded/mobile systems)
      sharedLibraries,  ///< Installed library/framework files directories (may be application-specific on embedded/mobile systems)
      commonBinaryFiles,///< Directory to store common binary/library files shared by multiple applications
      printers,         ///< Directory containing symbolic links to available printers (may not exist on embedded/mobile systems)
      trash             ///< System directory containing deleted files (may not exist on embedded/mobile systems)
    };
  
    // -- standard location accessors --

    class LocationFinder final {
    public:
      LocationFinder() = delete;

      /// @brief Get absolute path of current directory
      /// @warning: in special environments (sandboxes,...), this may return the relative location of the app (empty string)
      static pandora::io::Directory::Path currentLocation() noexcept;

      /// @brief Get absolute path of user home directory
      /// @warning: in special environments (sandboxes,...), this may return the relative location of the app (empty string)
      static pandora::io::Directory::Path homeLocation() noexcept;

      /// @brief Get standard user/app/system absolute path(s)
      /// @warning: - may contain multiple results
      ///           - may be empty if not found on current system
      ///           - returns potential known locations paths, but some of them may currently be non-existing folders.
      static std::vector<pandora::io::Directory::Path> standardLocations(StandardLocation location, const pandora::io::Directory::Path& appDirLabel = __P_DEFAULT_LOCATION_APP_DIR, uint32_t systemFlags = 0u) noexcept;
    };

  }
}
