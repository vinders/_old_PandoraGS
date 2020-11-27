/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
*******************************************************************************/
#ifdef _WINDOWS
# include <stdexcept>
# include "hardware/_private/_win32_libraries.h"

# if !defined(NTDDI_VERSION) || (NTDDI_VERSION < NTDDI_WIN10_RS2)
    pandora::hardware::Win32Libraries pandora::hardware::Win32Libraries::_libs{};

    // ntdll.dll functions
    typedef LONG (WINAPI * __win32_RtlVerifyVersionInfo)(OSVERSIONINFOEXW*,ULONG,ULONGLONG);

    using namespace pandora::hardware;


    // -- utilities --

    // verify Windows version on running system
    static inline bool _isWindowsVersionGreaterOrEqual(__win32_RtlVerifyVersionInfo verifyCommand, DWORD major, DWORD minor, DWORD build) noexcept {
      if (verifyCommand != nullptr) {
        OSVERSIONINFOEXW versionInfo = { sizeof(versionInfo), major, minor, build };
        DWORD mask = VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER;
        ULONGLONG check = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
        check = VerSetConditionMask(check, VER_MINORVERSION, VER_GREATER_EQUAL);
        check = VerSetConditionMask(check, VER_BUILDNUMBER, VER_GREATER_EQUAL);
        return (verifyCommand(&versionInfo, mask, check) == 0);
      }
      else {
  #     if defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
          return (major < 10u || (major == 10u && minor == 0u && (build == 0 || build <= __P_WIN_10_RS2_BUILD)) );
  #     elif defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_WIN10_RS1)
          return (major < 10u || (major == 10u && minor == 0u && (build == 0 || build <= __P_WIN_10_RS1_BUILD)) );
  #     elif defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_WINBLUE)
          return (major < HIBYTE(_WIN32_WINNT_WINBLUE) 
             || (major == HIBYTE(_WIN32_WINNT_WINBLUE) && minor <= LOBYTE(_WIN32_WINNT_WINBLUE) && (build == 0 || build <= __P_WIN_8_1_BLUE_BUILD)) );
  #     elif defined(WINVER) && (WINVER >= _WIN32_WINNT_WIN7)
          return (major < HIBYTE(_WIN32_WINNT_WIN7)
             || (major == HIBYTE(_WIN32_WINNT_WIN7) && minor <= LOBYTE(_WIN32_WINNT_WIN7) && (build == 0 || build <= __P_WIN_7_BUILD)) );
  #     else
          return (major < HIBYTE(_WIN32_WINNT_VISTA)
             || (major == HIBYTE(_WIN32_WINNT_VISTA) && minor == LOBYTE(_WIN32_WINNT_VISTA) && (build == 0 || build <= __P_WIN_VISTA_SP1_BUILD)) );
  #     endif
      }
    }

    // -- init --

    void Win32Libraries::_init() noexcept {
      // load system libraries
      Win32Libraries::_libs.user32.instance = LoadLibraryA("user32.dll");
      if (Win32Libraries::_libs.user32.instance != nullptr) {
        Win32Libraries::_libs.user32.SetProcessDpiAwarenessContext_ = (__win32_SetProcessDpiAwarenessContext)GetProcAddress(Win32Libraries::_libs.user32.instance, "SetProcessDpiAwarenessContext");
        Win32Libraries::_libs.user32.EnableNonClientDpiScaling_ = (__win32_EnableNonClientDpiScaling)GetProcAddress(Win32Libraries::_libs.user32.instance, "EnableNonClientDpiScaling");
        Win32Libraries::_libs.user32.GetDpiForWindow_ = (__win32_GetDpiForWindow)GetProcAddress(Win32Libraries::_libs.user32.instance, "GetDpiForWindow");
        Win32Libraries::_libs.user32.GetSystemMetricsForDpi_ = (__win32_GetSystemMetricsForDpi)GetProcAddress(Win32Libraries::_libs.user32.instance, "GetSystemMetricsForDpi");
        Win32Libraries::_libs.user32.AdjustWindowRectExForDpi_ = (__win32_AdjustWindowRectExForDpi)GetProcAddress(Win32Libraries::_libs.user32.instance, "AdjustWindowRectExForDpi");
      }

      Win32Libraries::_libs.shcore.instance = LoadLibraryA("shcore.dll");
      if (Win32Libraries::_libs.shcore.instance != nullptr) {
        Win32Libraries::_libs.shcore.SetProcessDpiAwareness_ = (__win32_SetProcessDpiAwareness)GetProcAddress(Win32Libraries::_libs.shcore.instance, "SetProcessDpiAwareness");
        Win32Libraries::_libs.shcore.GetDpiForMonitor_ = (__win32_GetDpiForMonitor)GetProcAddress(Win32Libraries::_libs.shcore.instance, "GetDpiForMonitor");
      }

      // identify windows compatibility
      HINSTANCE ntdll_instance = LoadLibraryA("ntdll.dll");
      __win32_RtlVerifyVersionInfo verifyVersionInfo_ = nullptr;
      if (ntdll_instance != nullptr)
        verifyVersionInfo_ = (__win32_RtlVerifyVersionInfo)GetProcAddress(ntdll_instance, "RtlVerifyVersionInfo");

      if (_isWindowsVersionGreaterOrEqual(verifyVersionInfo_, 10u, 0u, __P_WIN_10_RS2_BUILD))
        Win32Libraries::_libs._windowsReferenceBuild = __P_WIN_10_RS2_BUILD;
      else if (_isWindowsVersionGreaterOrEqual(verifyVersionInfo_, 10u, 0u, __P_WIN_10_RS1_BUILD))
        Win32Libraries::_libs._windowsReferenceBuild = __P_WIN_10_RS1_BUILD;
      else if (_isWindowsVersionGreaterOrEqual(verifyVersionInfo_, HIBYTE(_WIN32_WINNT_WINBLUE), LOBYTE(_WIN32_WINNT_WINBLUE), __P_WIN_8_1_BLUE_BUILD))
        Win32Libraries::_libs._windowsReferenceBuild = __P_WIN_8_1_BLUE_BUILD;
      else if (_isWindowsVersionGreaterOrEqual(verifyVersionInfo_, HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), __P_WIN_7_BUILD))
        Win32Libraries::_libs._windowsReferenceBuild = __P_WIN_7_BUILD;
      else
        Win32Libraries::_libs._windowsReferenceBuild = __P_WIN_VISTA_SP1_BUILD;

      if (ntdll_instance != nullptr)
        FreeLibrary(ntdll_instance);

      Win32Libraries::_libs._isInit = true;
    }

    // -- shutdown --

    Win32Libraries::~Win32Libraries() {
      if (user32.instance != nullptr)
        FreeLibrary(user32.instance);
      if (shcore.instance != nullptr)
        FreeLibrary(shcore.instance);

      user32.instance = nullptr;
      shcore.instance = nullptr;
    }
# endif
#endif
