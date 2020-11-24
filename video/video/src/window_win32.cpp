/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
--------------------------------------------------------------------------------
Description : Window management: Win32 implementation (Windows)
*******************************************************************************/
#ifdef _WINDOWS
# include <cstdint>
# include <cstddef>
# include <cstring>
# include <string>
# include <system/api/windows_api.h>
# include <windowsx.h>
# include "video/_private/_utils_win32.h"
# include "video/window_handle.h"
# include "video/window_style.h"
# include "video/window.h"

# if !defined(_CPP_REVISION) || _CPP_REVISION != 14
#   define __if_constexpr if constexpr
# else
#   define __if_constexpr if
# endif

  using namespace pandora::video;
  
  struct WindowAdjustedPosition final {
    uint32_t x;
    uint32_t y;
    uint32_t totalWidth;
    uint32_t totalHeight;
  };
  
  
  
  
//TODO!!!!
//-> EnumDisplaySettings affiche le statut courant -> pour obtenir la résolution de l'écran, ça ne marche qu'au début, AVANT de faire ChangeDisplaySettings
//-> à stocker quelque part!

//TODO: même pour les bordures, lecture avec DPI
  
  
  

// -- utilities -- -------------------------------------------------------------

# define _WIN10_ANNIVERSARY_MINOR 0
# define _WIN10_ANNIVERSARY_BUILD 14393

  static bool _isWindowsVersionGreaterEqual(DWORD major, DWORD minor, DWORD build) {
    OSVERSIONINFOEXW versionInfo = { sizeof(OSVERSIONINFOEXW), major, minor, build };
    ULONGLONG versionCheck = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
    versionCheck = VerSetConditionMask(versionCheck, VER_MINORVERSION, VER_GREATER_EQUAL);
    versionCheck = VerSetConditionMask(versionCheck, VER_BUILDNUMBER, VER_GREATER_EQUAL);
    return (RtlVerifyVersionInfo(&versionInfo, (VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER), versionCheck) == STATUS_SUCCESS);
  }
  
  std::string Window::formatLastError(const char prefix[]) noexcept {
    char error[256];
    error[255] = 0;
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 255, nullptr);
    return std::string(prefix) + error;
  }
  
  static inline HMONITOR _getPrimaryMonitorHandle() {
    return MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
  }


// -- display/size/style/visibility native enums -- ----------------------------

  // convert window display mode enum to native flags
  template <bool _ResetPrevious>
  static inline void _setWindowDisplayFlags(WindowDisplayMode displayMode, DWORD& outStyle, DWORD& outExtStyle) noexcept {
    __if_constexpr (_ResetPrevious) {
      outStyle    &= ~((DWORD)(WS_OVERLAPPED | WS_POPUP | WS_DLGFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX));
      outExtStyle &= ~((DWORD)WS_EX_TOOLWINDOW);
    }
    outStyle |= (WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    
    switch (params.displayMode) {
      case WindowDisplayMode::fullscreen: outStyle |= WS_POPUP; 
                                          break;
      case WindowDisplayMode::borderless: outStyle |= WS_POPUP; 
                                          break;
      case WindowDisplayMode::dialog:     outStyle |= (WS_POPUP | WS_DLGFRAME | WS_CAPTION | WS_SYSMENU); 
                                          break;
      case WindowDisplayMode::window:     outStyle |= (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX); 
                                          break;
      case WindowDisplayMode::floatingWindow: outStyle |= (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU);
                                              outExtStyle |= WS_EX_TOOLWINDOW;
                                              break;
      default: outStyle |= (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX); break;
    }
  }

  // convert window size/widget enums to native flags
  template <bool _ResetPrevious>
  static inline void _setWindowWidgetFlags(WindowDisplayMode displayMode, WindowSizeMode sizeMode, WindowWidgetMode widgets, DWORD& outStyle, DWORD& outExtStyle) noexcept {
    __if_constexpr (_ResetPrevious) {
      outStyle    &= ~((DWORD)(WS_SIZEBOX | WS_MAXIMIZEBOX | WS_HSCROLL | WS_VSCROLL));
      outExtStyle &= ~((DWORD)(WS_EX_TOPMOST | WS_EX_APPWINDOW | WS_EX_ACCEPTFILES));
    }
    if (sizeMode & (WindowSizeMode::resizableX | WindowSizeMode::resizableY)) { // no size style in fullscreen mode
      outStyle |= WS_SIZEBOX;
      if ((sizeMode & (WindowSizeMode::resizableX | WindowSizeMode::resizableY)) == (WindowSizeMode::resizableX | WindowSizeMode::resizableY))
        outStyle |= WS_MAXIMIZEBOX;
    }
    
    if (widgets & WindowWidgetMode::scrollbarX)
      outStyle |= WS_HSCROLL;
    if (widgets & WindowWidgetMode::scrollbarY)
      outStyle |= WS_VSCROLL;
    if (widgets & WindowWidgetMode::topMost || displayMode == WindowDisplayMode::fullscreen || displayMode == WindowDisplayMode::borderless)
      outExtStyle |= WS_EX_TOPMOST;
    if (widgets & WindowWidgetMode::aboveTaskbar)
      outExtStyle |= WS_EX_APPWINDOW;
  }
  
  // convert window visibility enum to native flag
  template <bool _IsFirstCall>
  static inline int _toShowWindowFlag(WindowDisplayMode displayMode, WindowVisibility status) noexcept {
    switch(status) {
      case WindowVisibility::hide:
        __if_constexpr (_IsFirstCall) return (displayMode != WindowDisplayMode::fullscreen) ? SW_SHOW : SW_SHOWMAXIMIZED;
        else                          return SW_HIDE;
      case WindowVisibility::minimize:
        __if_constexpr (_IsFirstCall) return (displayMode != WindowDisplayMode::fullscreen) ? SW_SHOW : SW_SHOWMAXIMIZED;
        else                          return SW_MINIMIZE;
      case WindowVisibility::restore:
        __if_constexpr (_IsFirstCall) return (displayMode != WindowDisplayMode::fullscreen) ? SW_SHOWNORMAL : SW_SHOWMAXIMIZED;
        else                          return SW_RESTORE;
      case WindowVisibility::maximize:     return SW_SHOWMAXIMIZED;
      case WindowVisibility::show:         return (displayMode != WindowDisplayMode::fullscreen) ? SW_SHOW        : SW_SHOWMAXIMIZED;
      case WindowVisibility::showInactive: return (displayMode != WindowDisplayMode::fullscreen) ? SW_SHOWNA      : SW_SHOWMAXIMIZED;
      case WindowVisibility::showDefault:  return (displayMode != WindowDisplayMode::fullscreen) ? SW_SHOWDEFAULT : SW_SHOWMAXIMIZED;
      default:                             return (displayMode != WindowDisplayMode::fullscreen) ? SW_SHOWDEFAULT : SW_SHOWMAXIMIZED;
    }
  }


// -- window metrics -- --------------------------------------------------------

  static inline void _getWindowBorderSizes(WindowDisplayMode displayMode, bool isResizable, uint32_t& outBorderX, uint32_t& outBorderY, uint32_t& outCaptionY) noexcept {
    if (displayMode == WindowDisplayMode::fullscreen || displayMode == WindowDisplayMode::borderless) {
      outBorderX = 0;
      outBorderY = 0;
      outCaptionY = 0;
    }
    else {
      if (isResizable) {
        outBorderX = GetSystemMetrics(SM_CXSIZEFRAME);
        outBorderY = GetSystemMetrics(SM_CYSIZEFRAME);
      }
      else {
        outBorderX = GetSystemMetrics(SM_CXFIXEDFRAME);
        outBorderY = GetSystemMetrics(SM_CYFIXEDFRAME);
      }
      outCaptionY = GetSystemMetrics(SM_CYCAPTION);
    }
  }
  
  bool Window::readMonitorDisplayResolution(uint32_t& outX, uint32_t& outY) noexcept {
    DEVMODE deviceInfo;
    EnumDisplaySettingsExW(nullptr, ENUM_CURRENT_SETTINGS, &deviceInfo, 0);
    outX = deviceInfo.dmPelsWidth;
    outY = deviceInfo.dmPelsHeight;
    if (outX >= 480u && outY >= 480u && outX != (uint32_t)-1 && outY != (uint32_t)-1)
      return true;
    
    HDC screenDC = GetDC(nullptr);
    int x = GetDeviceCaps(screenDC, HORZRES);
    int y = GetDeviceCaps(screenDC, VERTRES);
    ReleaseDC(nullptr, screenDC);
    screenDC = nullptr;
    
    if (x < 480 || y < 480) {
      if (_isWindowsVersionGreaterEqual(10, _WIN10_ANNIVERSARY_MINOR, _WIN10_ANNIVERSARY_BUILD)) {
        UINT dpi = GetDpiForWindow(windowHandle);
        if (dpi == 0)
          dpi = USER_DEFAULT_SCREEN_DPI;
        x = GetSystemMetricsForDpi(SM_CXSCREEN, dpi);
        y = GetSystemMetricsForDpi(SM_CYSCREEN, dpi);
      }
      else {
        x = GetSystemMetrics(SM_CXSCREEN);
        y = GetSystemMetrics(SM_CYSCREEN);
      }
    }
    if (x > 0 && y > 0) {
      outX = static_cast<uint32_t>(x);
      outY = static_cast<uint32_t>(y);
      return true;
    }
    return false;
  }
  
  static inline bool _readClientArea(HWND windowHandle, RECT& out) noexcept {
    return (GetClientRect(windowHandle, &out) != FALSE && out.right > out.left && out.bottom > out.top);
  }
  static inline void _calculateClientArea(const WindowPosition& position, WindowDisplayMode displayMode, WindowSizeMode sizeMode, RECT& out) noexcept {
    uint32_t borderX, borderY, captionY, screenX = 800u, screenY = 600u;
    _getWindowBorderSizes(displayMode, (sizeMode & (WindowSizeMode::resizableX | WindowSizeMode::resizableY)), borderX, borderY, captionY);
    
    if (position.x != defaultWindowPosition()) {
      out.left = static_cast<LONG>(position.x + borderX);
      out.right = (position.clientAreaWidth != defaultWindowSize()) ? out.left + static_cast<LONG>(position.clientAreaWidth) : out.left;
    }
    else if (displayMode == WindowDisplayMode::fullscreen || displayMode == WindowDisplayMode::borderless) {
      readMonitorDisplayResolution(screenX, screenY);
      out.left = 0;
      out.right = static_cast<LONG>(screenX);
    }
    else { // store "invalid" area to refresh it later
      out.left = 0;
      out.right = 0;
    }
    if (position.y != defaultWindowPosition()) {
      out.top = static_cast<LONG>(position.y + captionY);
      out.bottom = (position.clientAreaHeight != defaultWindowSize()) ? out.top + static_cast<LONG>(position.clientAreaHeight) : out.top;
    }
    else if (displayMode == WindowDisplayMode::fullscreen || displayMode == WindowDisplayMode::borderless) {
      if (position.x != defaultWindowPosition())
        readMonitorDisplayResolution(screenX, screenY);
      out.top = 0;
      out.bottom = static_cast<LONG>(screenY);
    }
    else { // store "invalid" area to refresh it later
      out.top = 0;
      out.bottom = 0;
    }
  }
  
  // calculate total window size in windowed mode
  static inline void _getAdjustedWindowSize(const WindowPosition& position, HWND windowHandle, HMONITOR monitor, WindowDisplayMode displayMode, bool hasMenu, DWORD style, DWORD styleExt, uint32_t& outWidth, uint32_t& outHeight) noexcept {
    RECT area;
    area.left = 100;
    area.top = 100;
    area.right = (position.clientAreaWidth != defaultWindowSize()) ? area.left + position.clientAreaWidth : area.left + 200;
    area.bottom = (position.clientAreaHeight != defaultWindowSize()) ? area.left + position.clientAreaHeight : area.top + 200;

    bool isSuccess = false;
    if (_isWindowsVersionGreaterEqual(10, _WIN10_ANNIVERSARY_MINOR, _WIN10_ANNIVERSARY_BUILD)) {
      UINT dpi = 0;
      if (windowHandle != nullptr) {
        dpi = GetDpiForWindow(windowHandle);
        if (dpi == 0)
          dpi = USER_DEFAULT_SCREEN_DPI;
      }
      else if (monitor != nullptr) {
        UINT dpiY;
        if (GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, dpi, dpiY) != S_OK)
          dpi = USER_DEFAULT_SCREEN_DPI;
      }
      else
        dpi = USER_DEFAULT_SCREEN_DPI;
      isSuccess = (AdjustWindowRectExForDpi(&area, style, hasMenu ? TRUE : FALSE, styleExt, dpi) != FALSE);
    }
    else
      isSuccess = (AdjustWindowRectEx(&area, style, hasMenu ? TRUE : FALSE, styleExt) != FALSE);

    if (isSuccess && (area.right - area.left) >= position.clientAreaWidth)) {
      outWidth = (position.clientAreaWidth != defaultWindowSize()) ? (area.right - area.left) : CW_USEDEFAULT;
      outHeight = (position.clientAreaHeight != defaultWindowSize()) ? (area.bottom - area.top) : CW_USEDEFAULT;
    }
    else { // failed -> calculate manually
      uint32_t borderX, borderY, captionY;
      _getWindowBorderSizes(displayMode, (style & WS_SIZEBOX), borderX, borderY, captionY);
      outWidth = (position.clientAreaWidth != defaultWindowSize()) ? position.clientAreaWidth + (borderX<<1) : CW_USEDEFAULT;
      outHeight = (position.clientAreaHeight != defaultWindowSize()) ? position.clientAreaHeight + captionY + borderY : CW_USEDEFAULT;
    }
  }
  
  // calculate total window area (including window decorations)
  static void _getWindowCreationArea(const WindowPosition& position, HWND windowHandle, HMONITOR monitor, WindowDisplayMode displayMode, bool hasMenu, DWORD style, DWORD styleExt, WindowAdjustedPosition& out) noexcept {
    uint32_t screenX = 800u;
    uint32_t screenY = 600u;
    if (position.clientAreaWidth == centeredWindowPosition() || position.clientAreaHeight == centeredWindowPosition() 
    || ((displayMode == WindowDisplayMode::fullscreen || displayMode == WindowDisplayMode::borderless) 
       && (position.clientAreaWidth == defaultWindowSize() || position.clientAreaHeight == defaultWindowSize())) )
      Window::readMonitorDisplayResolution(screenX, screenY);
    
    if (displayMode == WindowDisplayMode::fullscreen || displayMode == WindowDisplayMode::borderless) {
      out.totalWidth = (position.clientAreaWidth != defaultWindowSize()) ? position.clientAreaWidth : screenX;
      out.totalHeight = (position.clientAreaHeight != defaultWindowSize()) ? position.clientAreaHeight : screenY;
    }
    else if (position.clientAreaWidth == defaultWindowSize() && position.clientAreaHeight == defaultWindowSize()) {
      out.totalWidth = CW_USEDEFAULT;
      out.totalHeight = CW_USEDEFAULT;
    }
    else {
      _getAdjustedWindowSize(position, windowHandle, monitor, displayMode, hasMenu, style, styleExt, out.totalWidth, out.totalHeight);
    }
    
    switch (position.x) {
      case defaultWindowPosition():  out.x = CW_USEDEFAULT; break;
      case centeredWindowPosition(): 
        out.x = (out.totalWidth != CW_USEDEFAULT && out.totalWidth <= screenX) ? (screenX - out.totalWidth)>>1 : 0; 
        break;
      default: out.x = position.x; break;
    }
    switch (position.y) {
      case defaultWindowPosition():  out.y = CW_USEDEFAULT; break;
      case centeredWindowPosition(): 
        out.y = (out.totalHeight != CW_USEDEFAULT && out.totalHeight <= screenY) ? (screenY - out.totalHeight)>>1 : 0; 
        break;
      default: out.y = position.y; break;
    }
  }
  

// -- window event processing -- -----------------------------------------------

  bool Window::pollEvents() noexcept {
    static bool _isAlive = true;
    MSG msg;
    if (_isAlive) {
      while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
          _isAlive = false;
          break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
      }
    }
    return _isAlive;
  }

  LRESULT CALLBACK Window::windowEventHandler(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam) {
    static bool _isInSizeMove = false; //1 redimensionnée à la fois, donc ok
    static bool _isMinimized = false;//à placer dans window
    static bool _isInSuspend = false;//dans window
    
    //TODO: tenter d'avoir l'intégralité des changements faits ici
    
    switch (message) {
      case WM_ACTIVATEAPP: {
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
        if (wparam == FALSE) {
          if (window == nullptr || window->_displayMode == WindowDisplayMode::fullscreen)
            _disableFullscreenDisplay();
        }
        else {
          if (window != nullptr) {
            RECT area;
            if (_readClientArea(area))
              window->_setClientArea(area);
            
            if (!window->_isClientAreaInvalid() && window->_displayMode == WindowDisplayMode::fullscreen) {
              area = window->_getClientArea();
              _setFullscreenDisplay(area.right - area.left, area.bottom - area.top);
            }
          }
        }
        break;
      }
      case WM_PAINT: {
        PAINTSTRUCT ps;
        (void)BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
        if (_isInSizeMove && window != nullptr && window->resizeHandler) {
          RECT area;
          if (window->_readClientArea()) {
            _setClientArea(area);
            window->resizeHandler(area.right - area.left, area.bottom - area.top);
          }
        }
        break;
      }
      case WM_MOVE: {
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
        if (!_isInSizeMove && window != nullptr) {
          RECT area = _getClientArea();
          area.right -= area.left;
          area.bottom -= area.top;
          area.left = LOWORD(lParam);
          area.top = HIWORD(lParam);
          area.right += area.left;
          area.bottom += area.top;
          window->_setClientArea(area);
          if (window->moveHandler != nullptr)
            window->moveHandler(area.left, area.top);
        }
        break;
      }
      case WM_SIZE: {
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
        if (wparam == SIZE_MINIMIZED) {
          if (!_isMinimized) {
            _isMinimized = true;
            if (!_isInSuspend && window != nullptr && window->suspendHandler)
              window->suspendHandler(true);
            _isInSuspend = true;
          }
        }
        else if (_isMinimized) {
          _isMinimized = false;
          if (_isInSuspend && window != nullptr && window->suspendHandler)
            window->suspendHandler(false);
          _isInSuspend = false;
        }
        else if (!_isInSizeMove && window != nullptr) {
          RECT area = _getClientArea();
          area.right = area.left + LOWORD(lParam);
          area.bottom = area.top + HIWORD(lParam);
          window->_setClientArea(area);
          if (resizeHandler != nullptr)
            window->resizeHandler(LOWORD(lParam), HIWORD(lParam));
        }
        break;
      }
      case WM_ENTERSIZEMOVE: {
        _isInSizeMove = true;
        break;
      }
      case WM_EXITSIZEMOVE: {
        _isInSizeMove = false;
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
        if (window != nullptr) {
          RECT area;
          if (window->_readClientArea(area) && area != window->_getClientArea()) {
            window->_setClientArea(area);
            window->resizeHandler(LOWORD(lParam), HIWORD(lParam));
          }
        }
        break;
      }
      case WM_SIZING: {
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
        if (window != nullptr && lparam && (window->_sizeMode & WindowSizeMode::homothety)) {
          LONG windowBordersX = 0, windowBordersY = 0;
          //TODO: stocker dans window quand on crée ou change display mode ou size mode:  adjusted - clientarea
          
          auto rect = reinterpret_cast<RECT*>(lparam);
          switch (wparam) {
            case WMSZ_TOPLEFT:
            case WMSZ_TOPRIGHT: {
              int32_t sizeX = static_cast<int32_t>(rect.right - rect.left - windowBordersX);
              if (sizeX > 8 && sizeX != 1366) {
                sizeX = (sizeX>>3)<<3; // move by blocks of 8px
                if (sizeX < window->_minSizeX)
                  sizeX = window->_minSizeX;
              }
              rect.top = rect.bottom - static_cast<LONG>(_calculateClientSizeY(sizeX)) - windowBordersY;
              break;
            }
            case WMSZ_LEFT:
            case WMSZ_BOTTOMLEFT:
            case WMSZ_RIGHT:
            case WMSZ_BOTTOMRIGHT: {
              int32_t sizeX = static_cast<int32_t>(rect.right - rect.left - windowBordersX);
              if (sizeX > 8 && sizeX != 1366) {
                sizeX = (sizeX>>3)<<3; // move by blocks of 8px
                if (sizeX < window->_minSizeX)
                  sizeX = window->_minSizeX;
              }
              rect.bottom = rect.top + static_cast<LONG>(_calculateClientSizeY(sizeX)) + windowBordersY;
              break;
            }
            case WMSZ_TOP:
            case WMSZ_BOTTOM: {
              int32_t sizeY = static_cast<int32_t>(rect.bottom - rect.top - windowBordersY);
              rect.right = rect.left + static_cast<LONG>(_calculateClientSizeX(sizeY)) + windowBordersX;
              break;
            }
          }
        }
        break;
      }
      case WM_GETMINMAXINFO: {
        if (lparam) {
          auto info = reinterpret_cast<MINMAXINFO*>(lParam);
          Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
          if (window != nullptr) {
            info->ptMinTrackSize.x = static_cast<LONG>(window->_minSizeX);
            info->ptMinTrackSize.y = static_cast<LONG>(window->_minSizeY);
            if (window->_maxSizeX > 0u) {
              info->ptMaxTrackSize.x = static_cast<LONG>(window->_maxSizeX);
              info->ptMaxTrackSize.y = static_cast<LONG>(window->_maxSizeY);
            }
          }
          else {
            info->ptMinTrackSize.x = 320;
            info->ptMinTrackSize.y = 200;
          }
        }
        break;
      }
      case WM_ACTIVATEAPP: {
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
        if (window != nullptr) {
          if (wparam == FALSE) {
            if (window->suspendHandler != nullptr)
              window->suspendHandler(true);
            _isInSuspend = true;
          }
          else {
            if (window->suspendHandler != nullptr)
              window->suspendHandler(false);
            _isInSuspend = false;
          }
        }
        break;
      }
      case WM_POWERBROADCAST: {
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
        if (window != nullptr) {
          if (wparam == PBT_APMQUERYSUSPEND) {
            if (window->suspendHandler != nullptr)
              window->suspendHandler(true);
            _isInSuspend = true;
          }
          else if (wparam == PBT_APMRESUMESUSPEND) {
            if (window->suspendHandler != nullptr)
              window->suspendHandler(false);
            _isInSuspend = false;
          }
        }
        break;
      }
      case WM_SHOWWINDOW: {
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
        if (window != nullptr) {
          if (wparam == FALSE) {
            if (window->suspendHandler != nullptr)
              window->suspendHandler(true);
            _isInSuspend = true;
          }
          else {
            RECT area;
            if (window->_readClientArea()) {
              _setClientArea(area);
              if (window->resizeHandler != nullptr)
                window->resizeHandler(area.right - area.left, area.bottom - area.top);
              if (window->suspendHandler != nullptr)
                window->suspendHandler(false);
              _isInSuspend = false;
            }
          }
        }
        break;
      }
      case WM_CLOSE: {
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
        if (window == nullptr) {
          DestroyWindow(windowHandle);
        }
        else {
          if (window->closeHandler == nullptr || window->closeHandler() == true)
            window->_destroyWindow();
          else
            return 0;
        }
        break;
      }
      case WM_DESTROY: {
        if (Window::_windowCounter <= 0)
          PostQuitMessage(0)
        break;
      }
      case WM_SYSKEYDOWN: { // alt pressed + other key / F10 key (menu activated) -> check lparam
        if (lparam & 0x2000000) { // ALT key pressed + other key
          Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
          if (window != nullptr && window->keyDownHandler != nullptr) {
            if (window->keyDownHandler(wparam, true/*isAlt*/, ((lparam&0x4000000) != 0)/*prevStateAlreadyDown*/, (lparam&0xFFFF)/*repeats/duration(not cumulated with other messages)*/))
              return 0;
          }
        }
        break;
      }
      case WM_KEYDOWN: {
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
        if (window != nullptr && window->keyDownHandler != nullptr) {
          if (window->keyDownHandler(wparam, false/*isAlt*/, ((lparam&0x4000000) != 0)/*prevStateAlreadyDown*/, (lparam&0xFFFF)/*repeats/duration(not cumulated with other messages)*/))
            return 0;
        }
        break;
      }
      case WM_SYSKEYUP: { // alt released / F10 key (menu deactivated) -> check lparam
        if (lparam & 0x2000000) { // ALT key
          Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
          if (window != nullptr && window->keyUpHandler != nullptr) {
            if (window->keyUpHandler(wparam, true/*isAlt*/))
              return 0;
          }
        }
        break;
      }
      case WM_KEYUP: {
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
        if (window != nullptr && window->keyUpHandler != nullptr) {
          if (window->keyUpHandler(wparam, false/*isAlt*/))
            return 0;
        }
        break;
      }
      case WM_MENUCHAR: {
        // no menu handler -> invalid mnemonic or accelerator key -> avoid beep
        return MAKELRESULT(0, MNC_CLOSE);
      }
      case WM_MOUSEMOVE:
      case WM_LBUTTONDBLCLK:
      case WM_LBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_MBUTTONDBLCLK:
      case WM_MBUTTONDOWN:
      case WM_MBUTTONUP:
      case WM_RBUTTONDBLCLK:
      case WM_RBUTTONDOWN:
      case WM_RBUTTONUP:
      case WM_MOUSEHOVER:
      case WM_MOUSEWHEEL:
      case WM_XBUTTONDBLCLK:
      case WM_XBUTTONDOWN:
      case WM_XBUTTONUP: {
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
        if (window != nullptr) {
          auto handler = window->mouseHandlers.find(message);
          if (handler != window->mouseHandlers.end())
            if (*handler(wparam, lparam))
              return 0;
        }
        break;
      }
    }
    return DefWindowProc(windowHandle, message, wparam, lparam);
  }


// -- window creation/destruction -- -------------------------------------------

  static bool _setFullscreenDisplay(uint32_t sizeX, uint32_t sizeY) noexcept {
    DEVMODE screenMode;
    memset(&screenMode, 0, sizeof(DEVMODE));
    screenMode.dmSize = sizeof(DEVMODE);   
    screenMode.dmPelsWidth = sizeX;
    screenMode.dmPelsHeight = sizeY;    
    screenMode.dmBitsPerPel = 32;
    screenMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
    return (ChangeDisplaySettings(&screenMode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL);
  }
  static void _disableFullscreenDisplay() noexcept {
    ChangeDisplaySettings(nullptr, 0);
  }

  // create window
  Window::Window(const WindowPosition& position, std::shared_ptr<WindowStyle> windowStyleDef, WindowWidgetMode widgets, WindowSizeMode sizeMode,
                 const wchar_t title[], bool isCursorVisible, WindowVisibility initialStatus, std::shared_ptr<MenuResource> menu, WindowHandle owner)
    : _windowStyleDef(std::move(windowStyleDef)),
      _menu(std::move(menu)),
      _widgets(widgets),
      _sizeMode(sizeMode),
      _isCursorVisible(isCursorVisible) {
    this->_displayMode = this->_windowStyleDef->displayMode;
    assert(title != nullptr);
    if (_windowCounter == 0 && owner != nullptr) // if an external window exists (created by another module), do not trigger WM_QUIT when destroying this one
      ++_windowCounter;
    ++_windowCounter;

    // set window style
    DWORD styleFlags = 0;
    DWORD styleExtFlags = 0;
    if (this->_displayMode == WindowDisplayMode::fullscreen || this->_displayMode == WindowDisplayMode::borderless)
      sizeMode &= ~((uint32_t)(WindowSizeMode::resizableX | WindowSizeMode::resizableY));
    _setWindowDisplayFlags<false>(this->_displayMode, styleFlags, styleExtFlags);
    _setWindowWidgetFlags<false>(this->_displayMode, sizeMode, widgets, styleFlags, styleExtFlags);
    
    // set window position + create
    WindowAdjustedPosition creationPosition;
    _getWindowCreationArea(position, nullptr, _getPrimaryMonitorHandle(), this->_displayMode, 
                           (this->_menu != nullptr && this->_menu->isValid()), styleFlags, styleExtFlags, creationPosition);
    this->_windowHandle = CreateWindowExW(styleExtFlags, this->_windowStyleDef->_styleName.c_str(), caption, styleFlags, 
                                          (int)creationPosition.x, (int)creationPosition.y, (int)creationPosition.totalWidth, (int)creationPosition.totalHeight, 
                                          (HWND)owner, (this->_menu != nullptr) ? (HMENU)(this->_menu->handle()) : nullptr, instance, nullptr);
    if (this->_windowHandle == nullptr)
      throw std::invalid_argument(Window::formatLastError("Window: creation failure: "));

    // display window
    if (ShowWindow(this->_windowHandle, _toShowWindowFlag<true>(displayMode, initialStatus)) == FALSE) // no hiding/minimizing is done during first call (will be done after reading metrics)
      throw std::invalid_argument(Window::formatLastError("Window: display failure: "));
    if (this->_displayMode == WindowDisplayMode::fullscreen && initialStatus != WindowVisibility::minimize) {
      if (!_setFullscreenDisplay(creationPosition.totalWidth, creationPosition.totalHeight))
        throw std::invalid_argument("Window: display failure: requested fullscreen resolution not supported on current system.");
    }
    if (!isCursorVisible)
      ShowCursor(false);
    
    // store client area
    if (!_readClientArea(this->_windowHandle, this->_clientArea))
      _calculateClientArea(position, this->_displayMode, sizeMode, this->_clientArea);
      
    // client area ratio: used for homothety
    if ((sizeMode & WindowSizeMode::homothety) || this->_clientArea.bottom == this->_clientArea.top) {
      uint32_t originalWidth = (position.clientAreaWidth != defaultWindowSize()) ? position.clientAreaWidth : (this->_clientArea.right - this->_clientArea.left);
      uint32_t originalHeight = (position.clientAreaHeight != defaultWindowSize()) ? position.clientAreaHeight : (this->_clientArea.bottom - this->_clientArea.top);
      this->_sizeRatio = (originalHeight != 0) ? static_cast<double>(originalWidth) / static_cast<double>(originalHeight) : 4.0/3.0;
    }
    else
      this->_sizeRatio = static_cast<double>(this->_clientArea.right - this->_clientArea.left) / static_cast<double>(this->_clientArea.bottom - this->_clientArea.top);
        
    // provide event handler with address of current object as "user data"
    SetLastError(0);
    if (SetWindowLongPtr(this->_windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)) == 0 && GetLastError() != 0)
      throw std::runtime_error(Window::formatLastError("Window: set user data (message handling) failure: "));
    // enable window thread + user input
    if (initialStatus != WindowVisibility::hide && initialStatus != WindowVisibility::minimize) {
      SetForegroundWindow(this->_windowHandle);
      if (initialStatus != WindowVisibility::showInactive)
        SetFocus(this->_windowHandle);
    }
    else // can be hidden/minimized after initializing metrics
      ShowWindow(this->_windowHandle, _toShowWindowFlag<false>(displayMode, initialStatus));
  }
  
  // destroy window
  void Window::_destroyWindow() noexcept {
    if (this->_windowHandle != nullptr) {
      --_windowCounter;
      if (this->_isCursorVisible == false)
        ShowCursor(true);
      if (this->_displayMode == WindowDisplayMode::fullscreen)
        _disableFullscreenDisplay();
      
      DestroyWindow(this->_windowHandle);
      this->_windowHandle = nullptr;
      this->_windowStyleDef = nullptr;
      this->_menu = nullptr;
    }
  }
  
  // -- accessors --
  
  WindowPixel Window::clientAreaSize() const noexcept {
    return { this->_clientArea.right - this->_clientArea.left, this->_clientArea.bottom - this->_clientArea.top };
  }
  WindowPixel Window::cursorScreenPosition() noexcept {
    POINT pos;
    if (GetCursorPos(&pos) != FALSE)
      return { pos.x, pos.y };
    return { -1, -1 };
  }
  WindowPixel Window::cursorWindowPosition() noexcept {
    POINT pos;
    if (GetCursorPos(&pos) != FALSE) {
      RECT clientArea = _getClientArea();
      if (pos.x >= clientArea.left && pos.x < clientArea.right && pos.y >= clientArea.top && pos.y < clientArea.bottom)
        return { pos.x - clientArea.left, pos.y - clientArea.top };
    }
    return { -1, -1 };
  }


// -- window style change -- ---------------------------------------------------
  
  // -- change display settings - display/size mode --

  bool Window::displayMode(WindowDisplayMode displayMode, const WindowPosition& position) {
    if (!this->_isCursorVisible)
      ShowCursor(true);
    if (this->_displayMode == WindowDisplayMode::fullscreen)
      _disableFullscreenDisplay();
    
    //lock
    //recalculer style et styleext
    //adjust size/default/center/...
    //SetWindowLongPtr(hWnd, GWL_STYLE, /*nouveaux flags WS_...*/); + GWL_EXSTYLE
    //MoveWindow(hWnd, 0, 0, width, height, TRUE);

    //...
    if (success) {
      this->_displayMode = displayMode;
      // unlock
      
      if (this->_displayMode == WindowDisplayMode::fullscreen) {
        if (!_setFullscreenDisplay(???/*TODO*/, ???/*TODO*/))
          throw std::invalid_argument("Window: display failure: requested fullscreen resolution not supported on current system.");
      }
      if (!this->_isCursorVisible)
        ShowCursor(false);
      
      RECT area;//TODO à remplacer par même chose que constructeur -> gérer positions par défaut et échec de read client area)
      if (_readClientArea(area))
        _setClientAreaForceRatio(area);
    }
    return success;
  } 
  bool Window::sizeMode(WindowSizeMode sizeMode) noexcept {
    if (this->_displayMode == WindowDisplayMode::fullscreen || this->_displayMode == WindowDisplayMode::borderless) {
      this->_sizeMode = sizeMode;
      return true;
    }
    
    RECT area;
    if (_isClientAreaInvalid()) {
      if (_readClientArea(area))
        _setClientArea(area);
    }
    
    DWORD styleFlags = 0;
    DWORD styleExtFlags = 0; // only present to reconstruct all styles, but won't change -> no "ext update"
    _setWindowDisplayFlags<false>(this->_displayMode, styleFlags, styleExtFlags);
    _setWindowWidgetFlags<false>(this->_displayMode, sizeMode, widgets, styleFlags, styleExtFlags);
    
    if (SetWindowLongW(this->_windowHandle, GWL_STYLE, styleFlags) != FALSE) {
      SetWindowPos(this->_windowHandle, nullptr, 0, 0, 0, 0, (SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER));
      return true;
    }
    return false;
  }

  bool Window::visibility(WindowVisibility status) noexcept {
    bool wasHiddenOrInvalid = _isClientAreaInvalid();
    if (ShowWindow(this->_windowHandle, _toShowWindowFlag<false>(displayMode, status)) != FALSE) {
      if (status == WindowVisibility::hide || status == WindowVisibility::minimize) {
        if (this->_displayMode == WindowDisplayMode::fullscreen)
          _disableFullscreenDisplay();
      }
      else {
        RECT area;
        if (wasHiddenOrInvalid) {
          if (_readClientArea(area))
            _setClientArea(area);
        }
        if (!_isClientAreaInvalid() && this->_displayMode == WindowDisplayMode::fullscreen) {
          area = _getClientArea();
          _setFullscreenDisplay(area.right - area.left, area.bottom - area.top);
        }
      }
      return true;
    }
    return false;
  }
  
  // -- change display settings - cursor --
  
  bool Window::cursorScreenPosition(uint32_t absoluteX, uint32_t absoluteY) noexcept {
    //SetCursorPos
  }
  bool Window::cursorWindowPosition(uint32_t clientAreaX, uint32_t clientAreaY) noexcept {
    //SetCursorPos
  }
  void Window::cursorVisibility(bool isVisible) noexcept {
    if (this->_isCursorVisible == isVisible)
      return true;
    this->_isCursorVisible = isVisible;
    ShowCursor(isVisible);
  }
  
  // -- change display settings - title/menu --

  bool Window::title(const wchar_t title[]) noexcept {
    assert(title != nullptr);
    return (SetWindowTextW(this->_windowHandle, title) != FALSE);
  }

  bool Window::menu(std::shared_ptr<MenuResource> menu) noexcept {
    if (SetMenu(this->_windowHandle, (menu != nullptr && menu->isValid()) ? (HMENU)(menu->handle()) : nullptr) != FALSE) {
      this->_menu = menu; // release old value AFTER it's been replaced in window
      _refreshClientSizeY();
      return true;
    }
    return false;
  }
  
  // -- draw rectangle --

  void Window::clearClientArea() noexcept {
    if (this->_windowStyleDef != nullptr) {
      RECT area = _getClientArea();
      FillRect(context, &area, (HBRUSH)(this->_windowStyleDef->_resources.backgroundColor->handle()));
    }
  }
  void Window::clearClientArea(const WindowPosition& position) noexcept {
    if (this->_windowStyleDef != nullptr && position.clientAreaWidth > 0u && position.clientAreaHeight > 0u) {
      RECT area = _getClientArea();
      if (position.x != defaultWindowPosition())
        area.left += static_cast<LONG>(position.x);
      if (position.y != defaultWindowPosition())
        area.top += static_cast<LONG>(position.y);
      area.right = area.left + static_cast<LONG>(position.clientAreaWidth);
      area.bottom = area.top + static_cast<LONG>(position.clientAreaHeight);
      FillRect(context, &area, (HBRUSH)(this->_windowStyleDef->_resources.backgroundColor->handle()));
    }
  }
  
  void Window::fillClientArea(std::shared_ptr<ColorResource> color) noexcept {
    if (color != nullptr) {
      RECT area = _getClientArea();
      FillRect(context, &area, (HBRUSH)(color->handle()));
    }
  }
  void Window::fillClientArea(const WindowPosition& position, std::shared_ptr<ColorResource> color) noexcept{
    if (color != nullptr && position.clientAreaWidth > 0u && position.clientAreaHeight > 0u) {
        RECT area = _getClientArea();
        if (position.x != defaultWindowPosition())
          area.left += static_cast<LONG>(position.x);
        if (position.y != defaultWindowPosition())
          area.top += static_cast<LONG>(position.y);
        area.right = area.left + static_cast<LONG>(position.clientAreaWidth);
        area.bottom = area.top + static_cast<LONG>(position.clientAreaHeight);
        FillRect(context, &area, (HBRUSH)(color->handle()));
      }
    }
  }

  // -- change size / position --

  bool Window::resize(uint32_t width, uint32_t height) noexcept {
    RECT area = _getClientArea();
    //TODO defaultWindowSize() -> si fullscreen, screenX/Y (sinon ne fait rien)
    //TODO + taille bordures
    area.right = area.left + width;
    //TODO defaultWindowSize()-> si fullscreen, screenX/Y (sinon ne fait rien)
    //TODO + taille bordures
    area.bottom = area.top + height;
    return (SetWindowPos(this->_windowHandle, HWND_TOP, 0, 0, width, height, SWP_NOMOVE) != FALSE);
  }
  
  bool Window::move(uint32_t x, uint32_t y) noexcept {
    RECT area = _getClientArea();
    area.right -= area.left;
    area.bottom -= area.top;
    //TODO - centered / defaultWindowPosition
    area.left = position.x;
    //TODO - centered / defaultWindowPosition
    area.top = position.y;
    //TODO + taille bordures
    area.right += area.left;
    //TODO + taille bordures
    area.bottom += area.top;

    if (SetWindowPos(this->_windowHandle, HWND_TOP, area.left, area.top, area.right - area.left, area.bottom - area.top, SWP_NOSIZE) != FALSE) {
      _setClientAreaForceRatio(area);
      return true;
    }
    return false;
  }

  bool Window::moveResize(const WindowPosition& position) noexcept {
    RECT area;
    //TODO - centered / defaultWindowPosition
    area.left = position.x;
    //TODO - centered / defaultWindowPosition
    area.top = position.y;
    //TODO - defaultWindowSize() -> si fullscreen, screenX/Y (sinon ne fait rien)
    //TODO + taille bordures
    area.right = area.left + width;
    //TODO - defaultWindowSize() -> si fullscreen, screenX/Y (sinon ne fait rien)
    //TODO + taille bordures
    area.bottom = area.top + height;
    return (SetWindowPos(this->_windowHandle, HWND_TOP, area.left, area.top, area.right - area.left, area.bottom - area.top, 0) != FALSE);
  }

  bool Window::_refreshClientSizeY() noexcept {
    RECT area;
    if (_readClientArea(area)) {
      _setClientArea(area);
      
      if ((this->_sizeMode & WindowSizeMode::homothety)
      && this->_displayMode != WindowDisplayMode::fullscreen && this->_displayMode != WindowDisplayMode::borderless) {
        LONG newSize = static_cast<LONG>(static_cast<double>(area.right - area.left)/_getClientAreaRatio() + 0.5000000001);
        newSize &= ~((LONG)0x1); // no odd resolution value
        if (newSize != area.bottom - area.top)
          return (SetWindowPos(this->_windowHandle, HWND_TOP, 0, 0, area.right - area.left, newSize, SWP_NOMOVE) != FALSE);
      }
    }
    return false;
  }
  int32_t Window::_calculateClientSizeX(int32_t sizeY) noexcept {
    int32_t newSize = static_cast<int32_t>(static_cast<double>(sizeY)*_getClientAreaRatio() + 0.5000000001);
    if (newSize > 1)
      newSize &= ~((int32_t)0x1); // no odd resolution value
    else
      newSize = 1; // no "0" size
    return newSize;
  }
  int32_t Window::_calculateClientSizeY(uint32_t sizeX) noexcept {
    double ratio = _getClientAreaRatio();
    if (ratio == 0.0);
      ratio = 4.0/3.0;
    int32_t newSize = static_cast<int32_t>(static_cast<double>(sizeX)/ratio + 0.5000000001);
    if (newSize > 1)
      newSize &= ~((int32_t)0x1); // no odd resolution value
    else
      newSize = 1; // no "0" size
    return newSize;
  }
#endif
