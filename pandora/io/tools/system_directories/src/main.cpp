/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
--------------------------------------------------------------------------------
List common system-specific directories
*******************************************************************************/
#include <cstdio>
#include <io/standard_location.h>

using namespace pandora::io;

// Clear all traces in console
void clearScreen() {
# ifdef _WINDOWS
    system("cls");
# elif defined(__linux__) || defined(__linux) || defined(__unix__) || defined(__unix)
    system("clear");
# endif
}

// Display list of directories
void displayDirectories(const std::string& title, const std::vector<Directory::Path>& directories) {
  Directory::PathChar offset[32];
  for (int i = 0; i < 32; ++i) {
# ifdef _WINDOWS
    offset[i] = L' ';
# else 
    offset[i] = ' ';
# endif
  }
  offset[title.size()] = 0;
  offset[title.size() + 1] = 0;
  
  printf(" %s  ", title.c_str());
  if (!directories.empty()) {
#     ifdef _WINDOWS
        wprintf(L"%s\n", directories.begin()->c_str());
#     else
        printf("%s\n", directories.begin()->c_str());
#     endif
    
    for (auto it = directories.begin() + 1; it != directories.end(); ++it) {
#     ifdef _WINDOWS
        wprintf(L" %s  %s\n", offset, it->c_str());
#     else
        printf("%s%s\n", offset, it->c_str());
#     endif
    }
  }
  else
    printf("-\n");
}

// CPU analysis
int main() {
# ifdef _WINDOWS
    std::wstring appName = L"system_directories";
# else
    std::string appName = "system_directories";
# endif
  printf("\n COMMON SYSTEM-SPECIFIC DIRECTORIES\n"
         "____________________________________________________________\n\n");

  printf("-- USER DATA -- --------------------------------------------\n\n");
  displayDirectories("User home     ", LocationFinder::standardLocations(StandardLocation::home, appName));
  displayDirectories("Desktop       ", LocationFinder::standardLocations(StandardLocation::desktop, appName));
  displayDirectories("Documents     ", LocationFinder::standardLocations(StandardLocation::documents, appName));
  displayDirectories("Pictures      ", LocationFinder::standardLocations(StandardLocation::pictures, appName));
  displayDirectories("Music         ", LocationFinder::standardLocations(StandardLocation::music, appName));
  displayDirectories("Videos        ", LocationFinder::standardLocations(StandardLocation::movies, appName));
  displayDirectories("Downloads     ", LocationFinder::standardLocations(StandardLocation::downloads, appName));
  
  printf("\n-- APPLICATIVE -- ------------------------------------------\n\n");
  displayDirectories("Current dir   ", std::vector<pandora::io::Directory::Path>{ LocationFinder::currentLocation() });
  displayDirectories("Applications  ", LocationFinder::standardLocations(StandardLocation::applications, appName));
  displayDirectories("Shared apps   ", LocationFinder::standardLocations(StandardLocation::sharedApplications, appName));
  displayDirectories("App menus     ", LocationFinder::standardLocations(StandardLocation::appMenu, appName));
  displayDirectories("System fonts  ", LocationFinder::standardLocations(StandardLocation::fonts, appName));

  printf("\n-- APPLICATION DATA -- -------------------------------------\n\n");
  displayDirectories("Roaming data  ", LocationFinder::standardLocations(StandardLocation::appData, appName));
  displayDirectories("Local data    ", LocationFinder::standardLocations(StandardLocation::localAppData, appName));
  displayDirectories("App config    ", LocationFinder::standardLocations(StandardLocation::appConfig, appName));
  displayDirectories("Shared config ", LocationFinder::standardLocations(StandardLocation::commonAppConfig, appName));
  displayDirectories("User cache    ", LocationFinder::standardLocations(StandardLocation::cache, appName));
  displayDirectories("Local cache   ", LocationFinder::standardLocations(StandardLocation::localCache, appName));
  displayDirectories("Shared cache  ", LocationFinder::standardLocations(StandardLocation::sharedCache, appName));
  displayDirectories("Temporary     ", LocationFinder::standardLocations(StandardLocation::temp, appName));
  
  printf("\n-- SYSTEM & BINARIES -- ------------------------------------\n\n");
  displayDirectories("System        ", LocationFinder::standardLocations(StandardLocation::system, appName));
  displayDirectories("Local binaries", LocationFinder::standardLocations(StandardLocation::localBinaries, appName));
  displayDirectories("Shared bin    ", LocationFinder::standardLocations(StandardLocation::sharedBinaries, appName));
  displayDirectories("Common bin    ", LocationFinder::standardLocations(StandardLocation::commonBinaryFiles, appName));
  displayDirectories("Shared libs   ", LocationFinder::standardLocations(StandardLocation::sharedLibraries, appName));
  displayDirectories("Printers links", LocationFinder::standardLocations(StandardLocation::printers, appName));
  displayDirectories("Trash         ", LocationFinder::standardLocations(StandardLocation::trash, appName));
  
  printf("\n____________________________________________________________\n\n"
         " Press enter to exit...");
  getchar();
  return 0;
}
