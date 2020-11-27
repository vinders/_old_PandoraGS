/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
*******************************************************************************/
#include <cstdint>
#include <string>
#include <stdexcept>
#include <vector>
#include "hardware/display_monitor.h"

#ifdef _WINDOWS
# include <system/api/windows_api.h>
# include "hardware/_private/_win32_libraries.h"
# if defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_WINBLUE)
#   include <shellscalingapi.h>
# endif
#elif defined(__APPLE__)
# include <TargetConditionals.h>
#elif defined(__linux__) || defined(__linux) || defined(__unix__) || defined(__unix)
# include <cstdlib>
# include "io/_private/_key_value_file_reader.h"
#endif

using namespace pandora::hardware;


// -- monitor description & display mode - implementations -- ------------------

#ifdef _WINDOWS // Win32 implementation
  // --- read attributes ---

  // read brand/description string of monitor and associated adapter
  static inline void __readDeviceDescription(const DisplayMonitor::DeviceId& id, std::wstring& outDescription, std::wstring& outAdapter) noexcept {
    DISPLAY_DEVICEW device;
    ZeroMemory(&device, sizeof(device));
    device.cb = sizeof(device);

    if (!id.empty() && EnumDisplayDevicesW(id.c_str(), 0, &device, 0) != FALSE)
      outDescription = device.DeviceString;
    else
      outDescription = L"Generic PnP Monitor";

    if (!id.empty()) {
      BOOL result = TRUE;
      for (uint32_t index = 0; result; ++index) {
        if ((result = EnumDisplayDevicesW(nullptr, index, &device, 0) != FALSE) && id == device.DeviceName)
          outAdapter = device.DeviceString;
      }
      if (outAdapter.empty() && EnumDisplayDevicesW(nullptr, 0, &device, 0) != FALSE)
        outAdapter = device.DeviceString;
    }
    else if (EnumDisplayDevicesW(nullptr, 0, &device, 0) != FALSE)
      outAdapter = device.DeviceString;
  }

  // convert and fill attributes of a monitor (id, position/size, primary)
  static inline void __fillMonitorAttributes(MONITORINFOEXW& info, DisplayMonitor::Attributes& outAttr) noexcept {
    outAttr.id = info.szDevice;
    outAttr.screenArea.x = info.rcMonitor.left;
    outAttr.screenArea.y = info.rcMonitor.top;
    outAttr.screenArea.width  = static_cast<uint32_t>(info.rcMonitor.right - info.rcMonitor.left);
    outAttr.screenArea.height = static_cast<uint32_t>(info.rcMonitor.bottom - info.rcMonitor.top);
    outAttr.workArea.x = info.rcWork.left;
    outAttr.workArea.y = info.rcWork.top;
    outAttr.workArea.width  = static_cast<uint32_t>(info.rcWork.right - info.rcWork.left);
    outAttr.workArea.height = static_cast<uint32_t>(info.rcWork.bottom - info.rcWork.top);
    outAttr.isPrimary = ((info.dwFlags & MONITORINFOF_PRIMARY) != 0);
  }
  // read attributes of a monitor handle
  static bool _readDisplayMonitorAttributes(DisplayMonitor::Handle monitorHandle, DisplayMonitor::Attributes& outAttr) noexcept {
    MONITORINFOEXW info;
    ZeroMemory(&info, sizeof(info));
    info.cbSize = sizeof(info);

    if (GetMonitorInfoW((HMONITOR)monitorHandle, (MONITORINFO*)&info) != FALSE && info.rcMonitor.right > info.rcMonitor.left) {
      __fillMonitorAttributes(info, outAttr);
      __readDeviceDescription(outAttr.id, outAttr.description, outAttr.adapter);
      return true;
    }
    return false;
  }
  // read primary monitor size/position (fallback if no handle or if _readDisplayMonitorAttributes failed)
  static bool __readPrimaryDisplayMonitorScreenArea_fallback(DisplayArea& out) {
    DEVMODEW deviceInfo;
    ZeroMemory(&deviceInfo, sizeof(deviceInfo));
    deviceInfo.dmSize = sizeof(deviceInfo);

    out.x = 0;
    out.y = 0;
    if (EnumDisplaySettingsExW(nullptr, ENUM_CURRENT_SETTINGS, &deviceInfo, 0) != FALSE && deviceInfo.dmPelsWidth > 0u && deviceInfo.dmPelsHeight > 0u) {
      out.width  = static_cast<uint32_t>(deviceInfo.dmPelsWidth);
      out.height = static_cast<uint32_t>(deviceInfo.dmPelsHeight);
    }
    else {
      HDC screenDC = GetDC(nullptr);
      int x = GetDeviceCaps(screenDC, HORZRES);
      int y = GetDeviceCaps(screenDC, VERTRES);
      ReleaseDC(nullptr, screenDC);
      if (x > 0 && y > 0) {
        out.width = static_cast<uint32_t>(x);
        out.height = static_cast<uint32_t>(y);
      }
      else {
        x = GetSystemMetrics(SM_CXSCREEN);
        y = GetSystemMetrics(SM_CYSCREEN);
        if (x > 0 && y > 0) {
          out.width = static_cast<uint32_t>(x);
          out.height = static_cast<uint32_t>(y);
        }
        else
          return false;
      }
    }
    return true;
  }
  // read primary monitor work area size/position (fallback if no handle or if _readDisplayMonitorAttributes failed)
  static bool __readPrimaryDisplayMonitorWorkArea_fallback(DisplayArea& out) {
    RECT workArea;
    if (SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0) != FALSE && workArea.right > workArea.left) {
      out.x = workArea.left;
      out.y = workArea.top;
      out.width  = static_cast<uint32_t>(workArea.right - workArea.left);
      out.height = static_cast<uint32_t>(workArea.bottom - workArea.top);
      return true;
    }
    return false;
  }

  // --- get monitor handles/attributes ---

  // verify handle validity
  static inline bool _isHandleValid(DisplayMonitor::Handle monitorHandle) noexcept { return (monitorHandle != nullptr); }

  // get handle/attributes of primary monitor
  static inline void _readPrimaryDisplayMonitorInfo(DisplayMonitor::Handle& outHandle, DisplayMonitor::Attributes& outAttr) {
    outHandle = (DisplayMonitor::Handle)MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
    if (outHandle == nullptr || !_readDisplayMonitorAttributes(outHandle, outAttr)) {
      outAttr.isPrimary = true;
      outAttr.id.clear();
      __readDeviceDescription(outAttr.id, outAttr.description, outAttr.adapter);
      if (!__readPrimaryDisplayMonitorScreenArea_fallback(outAttr.screenArea)) {
        outAttr.screenArea.width = 0u; // no display device connected
        outAttr.screenArea.height = 0u;
      }
      if (!__readPrimaryDisplayMonitorWorkArea_fallback(outAttr.workArea))
        outAttr.workArea = outAttr.screenArea;
    }
  }

  // get handle/attributes of any monitor by ID
  struct __DisplayMonitorHandleSearch final {
    const DisplayMonitor::DeviceId* id;
    DisplayMonitor::Handle handle;
    DisplayMonitor::Attributes* attributes;
  };
  static BOOL CALLBACK __getDisplayMonitorByIdCallback(HMONITOR handle, HDC, RECT*, LPARAM data) {
    MONITORINFOEXW info;
    ZeroMemory(&info, sizeof(info));
    info.cbSize = sizeof(info);

    if (handle != nullptr && GetMonitorInfoW(handle, (MONITORINFO*)&info) != FALSE) {
      __DisplayMonitorHandleSearch* query = (__DisplayMonitorHandleSearch*)data;
      if (query != nullptr && query->id != nullptr && wcscmp(info.szDevice, query->id->c_str()) == 0) {
        query->handle = (DisplayMonitor::Handle)handle;
        if (query->attributes != nullptr)
          __fillMonitorAttributes(info, *(query->attributes));
      }
    }
    return TRUE;
  }
  static inline DisplayMonitor::Handle _getDisplayMonitorById(const DisplayMonitor::DeviceId& id, DisplayMonitor::Attributes* outAttr) noexcept {
    __DisplayMonitorHandleSearch query{ &id, nullptr, outAttr };
    if (EnumDisplayMonitors(nullptr, nullptr, __getDisplayMonitorByIdCallback, (LPARAM)&query) != FALSE)
      return query.handle;
    return nullptr;
  }

  // list handles of all active monitors
  static BOOL CALLBACK __listDisplayMonitorsCallback(HMONITOR handle, HDC, RECT*, LPARAM data) {
    std::vector<DisplayMonitor::Handle>* handleList = (std::vector<DisplayMonitor::Handle>*)data;
    if (handle != nullptr && handleList != nullptr)
      handleList->emplace_back((DisplayMonitor::Handle)handle);
    return TRUE;
  }
  static inline bool _listDisplayMonitors(std::vector<DisplayMonitor::Handle>& out) {
    return (EnumDisplayMonitors(nullptr, nullptr, __listDisplayMonitorsCallback, (LPARAM)&out) != FALSE);
  }

  // --- display modes ---

  // read display resolution/depth/rate of a monitor
  static inline bool _getMonitorDisplayMode(const DisplayMonitor::DeviceId& id, DisplayMode& out) noexcept {
    DEVMODEW screenMode;
    ZeroMemory(&screenMode, sizeof(screenMode));
    screenMode.dmSize = sizeof(screenMode);
    if (EnumDisplaySettingsExW(id.c_str(), ENUM_CURRENT_SETTINGS, &screenMode, 0) != FALSE && screenMode.dmPelsWidth != 0) {
      out.width = screenMode.dmPelsWidth;
      out.height = screenMode.dmPelsHeight;    
      out.bitDepth = screenMode.dmBitsPerPel;
      out.refreshRate = screenMode.dmDisplayFrequency;
      return true;
    }
    return false;
  }
  // set display resolution/depth/rate of a monitor
  static inline bool _setMonitorDisplayMode(const DisplayMonitor::DeviceId& id, const DisplayMode& mode) noexcept {
    DEVMODEW screenMode;
    ZeroMemory(&screenMode, sizeof(screenMode));
    screenMode.dmSize = sizeof(screenMode);   

    screenMode.dmPelsWidth = mode.width;
    screenMode.dmPelsHeight = mode.height;    
    screenMode.dmBitsPerPel = mode.bitDepth;
    screenMode.dmDisplayFrequency = mode.refreshRate;
    screenMode.dmFields = (DM_PELSWIDTH | DM_PELSHEIGHT);
    if (mode.bitDepth != 0)
      screenMode.dmFields |= DM_BITSPERPEL;
    if (mode.refreshRate != undefinedRefreshRate())
      screenMode.dmFields |= DM_DISPLAYFREQUENCY;

    if (!id.empty())
      return (ChangeDisplaySettingsExW(id.c_str(), &screenMode, nullptr, CDS_FULLSCREEN, nullptr) == DISP_CHANGE_SUCCESSFUL);
    else // primary monitor
      return (ChangeDisplaySettingsW(&screenMode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL);
  }
  // reset display resolution/depth/rate of a monitor to default values
  static inline bool _setDefaultMonitorDisplayMode(const DisplayMonitor::DeviceId& id) noexcept {
    if (!id.empty())
      return (ChangeDisplaySettingsExW(id.c_str(), nullptr, nullptr, 0, nullptr) == DISP_CHANGE_SUCCESSFUL);
    else // primary monitor
      return (ChangeDisplaySettingsW(nullptr, 0) == DISP_CHANGE_SUCCESSFUL);
  }

  // list all display modes for an output device
  static inline std::vector<DisplayMode> _listDisplayModes(const DisplayMonitor::DeviceId& id) noexcept {
    std::vector<DisplayMode> modes;
    BOOL result = TRUE;
    for (DWORD index = 0; result != FALSE; ++index) {
      DEVMODEW info;
      ZeroMemory(&info, sizeof(info));
      info.dmSize = sizeof(info);

      result = EnumDisplaySettingsExW(id.c_str(), index, &info, 0);
      if (result != FALSE && info.dmPelsWidth != 0u && info.dmPelsHeight != 0u && (info.dmBitsPerPel >= 15u || info.dmBitsPerPel == 0))
        modes.push_back(DisplayMode{ info.dmPelsWidth, info.dmPelsHeight, info.dmBitsPerPel, info.dmDisplayFrequency });
    }
    return modes;
  }

#elif defined(__APPLE__) && defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE // iOS implementation
  static bool _readDisplayMonitorAttributes(DisplayMonitor::Handle monitorHandle, DisplayMonitor::Attributes& outAttr) noexcept { return false; }
  static inline bool _isHandleValid(DisplayMonitor::Handle monitorHandle) noexcept { return (monitorHandle != 0); }
  static inline void _readPrimaryDisplayMonitorInfo(DisplayMonitor::Handle& outHandle, DisplayMonitor::Attributes& outAttr) {}
  static inline DisplayMonitor::Handle _getDisplayMonitorById(DisplayMonitor::DeviceId id, DisplayMonitor::Attributes* outAttr) noexcept { return 0; }
  static inline bool _listDisplayMonitors(std::vector<DisplayMonitor::Handle>& out) { return false; }
  static inline bool _getMonitorDisplayMode(const DisplayMonitor::DeviceId& id, DisplayMode& out) noexcept { return false; }
  static inline bool _setMonitorDisplayMode(const DisplayMonitor::DeviceId& id, const DisplayMode& mode) noexcept { return false; }
  static inline bool _setDefaultMonitorDisplayMode(const DisplayMonitor::DeviceId& id) noexcept { return false; }
  static inline std::vector<DisplayMode> _listDisplayModes(const DisplayMonitor::DeviceId& id) noexcept { return {}; }

#elif defined(__APPLE__) // Mac OS implementation
  static bool _readDisplayMonitorAttributes(DisplayMonitor::Handle monitorHandle, DisplayMonitor::Attributes& outAttr) noexcept { return false; }
  static inline bool _isHandleValid(DisplayMonitor::Handle monitorHandle) noexcept { return (monitorHandle != 0); }
  static inline void _readPrimaryDisplayMonitorInfo(DisplayMonitor::Handle& outHandle, DisplayMonitor::Attributes& outAttr) {}
  static inline DisplayMonitor::Handle _getDisplayMonitorById(DisplayMonitor::DeviceId id, DisplayMonitor::Attributes* outAttr) noexcept { return 0; }
  static inline bool _listDisplayMonitors(std::vector<DisplayMonitor::Handle>& out) { return false; }
  static inline bool _getMonitorDisplayMode(const DisplayMonitor::DeviceId& id, DisplayMode& out) noexcept { return false; }
  static inline bool _setMonitorDisplayMode(const DisplayMonitor::DeviceId& id, const DisplayMode& mode) noexcept { return false; }
  static inline bool _setDefaultMonitorDisplayMode(const DisplayMonitor::DeviceId& id) noexcept { return false; }
  static inline std::vector<DisplayMode> _listDisplayModes(const DisplayMonitor::DeviceId& id) noexcept { return {}; }

#elif defined(__ANDROID__) // Android implementation
  static bool _readDisplayMonitorAttributes(DisplayMonitor::Handle monitorHandle, DisplayMonitor::Attributes& outAttr) noexcept { return false; }
  static inline bool _isHandleValid(DisplayMonitor::Handle monitorHandle) noexcept { return (monitorHandle != 0); }
  static inline void _readPrimaryDisplayMonitorInfo(DisplayMonitor::Handle& outHandle, DisplayMonitor::Attributes& outAttr) {}
  static inline DisplayMonitor::Handle _getDisplayMonitorById(DisplayMonitor::DeviceId id, DisplayMonitor::Attributes* outAttr) noexcept { return 0; }
  static inline bool _listDisplayMonitors(std::vector<DisplayMonitor::Handle>& out) { return false; }
  static inline bool _getMonitorDisplayMode(const DisplayMonitor::DeviceId& id, DisplayMode& out) noexcept { return false; }
  static inline bool _setMonitorDisplayMode(const DisplayMonitor::DeviceId& id, const DisplayMode& mode) noexcept { return false; }
  static inline bool _setDefaultMonitorDisplayMode(const DisplayMonitor::DeviceId& id) noexcept { return false; }
  static inline std::vector<DisplayMode> _listDisplayModes(const DisplayMonitor::DeviceId& id) noexcept { return {}; }

#elif defined(__linux__) || defined(__linux) || defined(__unix__) || defined(__unix) // -- monitor description - X11 implementation --
  static bool _readDisplayMonitorAttributes(DisplayMonitor::Handle monitorHandle, DisplayMonitor::Attributes& outAttr) noexcept { return false; }
  static inline bool _isHandleValid(DisplayMonitor::Handle monitorHandle) noexcept { return (monitorHandle != 0); }
  static inline void _readPrimaryDisplayMonitorInfo(DisplayMonitor::Handle& outHandle, DisplayMonitor::Attributes& outAttr) {}
  static inline DisplayMonitor::Handle _getDisplayMonitorById(DisplayMonitor::DeviceId id, DisplayMonitor::Attributes* outAttr) noexcept { return 0; }
  static inline bool _listDisplayMonitors(std::vector<DisplayMonitor::Handle>& out) { return false; }
  static inline bool _getMonitorDisplayMode(const DisplayMonitor::DeviceId& id, DisplayMode& out) noexcept { return false; }
  static inline bool _setMonitorDisplayMode(const DisplayMonitor::DeviceId& id, const DisplayMode& mode) noexcept { return false; }
  static inline bool _setDefaultMonitorDisplayMode(const DisplayMonitor::DeviceId& id) noexcept { return false; }
  static inline std::vector<DisplayMode> _listDisplayModes(const DisplayMonitor::DeviceId& id) noexcept { return {}; }
#endif


// -- monitor description - common -- ------------------------------------------

DisplayMonitor::DisplayMonitor() noexcept {
  _readPrimaryDisplayMonitorInfo(this->_handle, this->_attributes);
}
DisplayMonitor::DisplayMonitor(Handle monitorHandle, bool usePrimaryAsDefault)
  : _handle(monitorHandle) {
  if (!_isHandleValid(this->_handle) || !_readDisplayMonitorAttributes(this->_handle, this->_attributes)) {
    if (usePrimaryAsDefault)
      _readPrimaryDisplayMonitorInfo(this->_handle, this->_attributes);
    else
      throw std::invalid_argument("DisplayMonitor: monitor handle is invalid or can't be used.");
  }
}
DisplayMonitor::DisplayMonitor(const DisplayMonitor::DeviceId& id, bool usePrimaryAsDefault) {
  this->_handle = _getDisplayMonitorById(id, &(this->_attributes));
  if (!_isHandleValid(this->_handle)) {
    if (usePrimaryAsDefault)
      _readPrimaryDisplayMonitorInfo(this->_handle, this->_attributes);
    else
      throw std::invalid_argument("DisplayMonitor: monitor ID was not found on system.");
  }
}
DisplayMonitor::DisplayMonitor(uint32_t index, bool usePrimaryAsDefault) {
  std::vector<DisplayMonitor::Handle> handles;
  if (_listDisplayMonitors(handles) && index < handles.size())
    this->_handle = handles[index];
  if (!_isHandleValid(this->_handle) || !_readDisplayMonitorAttributes(this->_handle, this->_attributes)) {
    if (usePrimaryAsDefault)
      _readPrimaryDisplayMonitorInfo(this->_handle, this->_attributes);
    else
      throw std::invalid_argument("DisplayMonitor: monitor index was not found on system.");
  }
}

std::vector<DisplayMonitor> DisplayMonitor::listAvailableMonitors() noexcept {
  std::vector<DisplayMonitor> monitors;

  std::vector<DisplayMonitor::Handle> handles;
  if (_listDisplayMonitors(handles)) {
    for (auto it : handles) {
      try {
        monitors.emplace_back(it, false);
      }
      catch (...) {}
    }
  }

  if (monitors.empty()) { // primary monitor as default
    monitors.emplace_back();
    if (monitors[0].attributes().screenArea.width == 0) // no display monitor
      monitors.clear();
  }
  return monitors;
}


// -- display mode - common -- -------------------------------------------------

DisplayMode DisplayMonitor::getDisplayMode() noexcept {
  DisplayMode mode;
  if (!_getMonitorDisplayMode(this->_attributes.id, mode)) {
    mode.width = this->_attributes.screenArea.width;
    mode.height = this->_attributes.screenArea.height;
    mode.bitDepth = 32;
    mode.refreshRate = undefinedRefreshRate();
  }
  return mode;
}
bool DisplayMonitor::setDisplayMode(const DisplayMode& mode, bool refreshAttributes) noexcept {
  if (_setMonitorDisplayMode(this->_attributes.id, mode)) {
    if (refreshAttributes) {
      if (this->_attributes.isPrimary) {
        _readPrimaryDisplayMonitorInfo(this->_handle, this->_attributes);
      }
      else if (!_readDisplayMonitorAttributes(this->_handle, this->_attributes)) {
        this->_attributes.screenArea.width = mode.width;
        this->_attributes.screenArea.height = mode.height;
      }
    }
    return true;
  }
  return false;
}
bool DisplayMonitor::setDefaultDisplayMode(bool refreshAttributes) noexcept {
  if (_setDefaultMonitorDisplayMode(this->_attributes.id)) {
    if (refreshAttributes) {
      if (this->_attributes.isPrimary) {
        _readPrimaryDisplayMonitorInfo(this->_handle, this->_attributes);
      }
      else
        _readDisplayMonitorAttributes(this->_handle, this->_attributes);
    }
    return true;
  }
  return false;
}

std::vector<DisplayMode> DisplayMonitor::listAvailableDisplayModes() noexcept {
  std::vector<DisplayMode> modes = _listDisplayModes(this->_attributes.id);
  if (modes.empty())
    modes.emplace_back(getDisplayMode());
  return modes;
}


// -- DPI awareness - implementations -- ---------------------------------------

bool DisplayMonitor::setDpiAwareness(bool isEnabled) noexcept {
# ifdef _WINDOWS
#   if defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
      return (SetProcessDpiAwarenessContext(isEnabled ? DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 : DPI_AWARENESS_CONTEXT_UNAWARE) != FALSE
           || SetProcessDpiAwareness(isEnabled ? PROCESS_PER_MONITOR_DPI_AWARE : PROCESS_DPI_UNAWARE) == S_OK);
#   else
      Win32Libraries& libs = Win32Libraries::instance();
      if (libs.isAtLeastWindows10_RS2() && libs.user32.SetProcessDpiAwarenessContext_
      && libs.user32.SetProcessDpiAwarenessContext_(isEnabled ? DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 : DPI_AWARENESS_CONTEXT_UNAWARE) != FALSE)
        return true;

#     if defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_WINBLUE)
        return (SetProcessDpiAwareness(isEnabled ? PROCESS_PER_MONITOR_DPI_AWARE : PROCESS_DPI_UNAWARE) == S_OK);
#     else
        if (libs.isAtLeastWindows8_1_Blue() && libs.shcore.SetProcessDpiAwareness_
        && libs.shcore.SetProcessDpiAwareness_(isEnabled ? PROCESS_PER_MONITOR_DPI_AWARE : PROCESS_DPI_UNAWARE) == S_OK)
          return true;
        return (isEnabled && SetProcessDPIAware() != FALSE);
#     endif
#   endif
# else
    return true;
# endif
}

#ifdef _WINDOWS
  // read per-monitor DPI (if Win10+ build) or system DPI (if DPI aware)
  static inline uint32_t _readDisplayMonitorDpi(DisplayMonitor::Handle monitorHandle, DisplayMonitor::WindowHandle windowHandle) noexcept {
#   if !defined(NTDDI_VERSION) || (NTDDI_VERSION < NTDDI_WIN10_RS1)
      Win32Libraries& libs = Win32Libraries::instance();
#   endif

    // per monitor DPI (best, if Win10 RS1+ and window handle)
    if (windowHandle != nullptr) {
#     if defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_WIN10_RS1)
        UINT dpi = GetDpiForWindow((HWND)windowHandle);
        if (dpi > 0)
          return static_cast<uint32_t>(dpi);
#     else
        if (libs.isAtLeastWindows10_RS1() && libs.user32.GetDpiForWindow_) {
          UINT dpi = libs.user32.GetDpiForWindow_((HWND)windowHandle);
          if (dpi > 0)
            return static_cast<uint32_t>(dpi);
        }
#     endif
    }
    // per system DPI (fallback, if Win8.1+)
    UINT dpiX, dpiY;
#   if defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_WINBLUE)
      if (GetDpiForMonitor((HMONITOR)monitorHandle, MDT_EFFECTIVE_DPI, &dpiX, &dpiY) == S_OK) 
        return static_cast<uint32_t>(dpiY);
#   else
      if (libs.isAtLeastWindows8_1_Blue() && libs.shcore.GetDpiForMonitor_) {
        if (libs.shcore.GetDpiForMonitor_((HMONITOR)monitorHandle, MDT_EFFECTIVE_DPI, &dpiX, &dpiY) == S_OK) 
          return static_cast<uint32_t>(dpiY);
      }
#   endif

    // per system DPI (legacy)
    HDC hdc = GetDC(nullptr);
    int pixelsPerLogicInch = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(nullptr, hdc);
    return (pixelsPerLogicInch > 0) ? pixelsPerLogicInch : USER_DEFAULT_SCREEN_DPI;
  }

  // read system DPI-scale factors (if DPI aware)
  static inline void _readDisplayMonitorContentScale(DisplayMonitor::Handle monitorHandle, float& factorX, float& factorY) {
    UINT dpiX, dpiY;
#   if defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_WINBLUE)
      if (GetDpiForMonitor((HMONITOR)monitorHandle, MDT_EFFECTIVE_DPI, &dpiX, &dpiY) != S_OK || dpiX == 0 || dpiY == 0)
#   else
      Win32Libraries& libs = Win32Libraries::instance();
      if (libs.isAtLeastWindows8_1_Blue() == false || libs.shcore.GetDpiForMonitor_ == nullptr
      || libs.shcore.GetDpiForMonitor_((HMONITOR)monitorHandle, MDT_EFFECTIVE_DPI, &dpiX, &dpiY) != S_OK 
      || dpiX == 0 || dpiY == 0)
#   endif
    {
      const HDC hdc = GetDC(nullptr);
      int pxPerInchX = GetDeviceCaps(hdc, LOGPIXELSX);
      int pxPerInchY = GetDeviceCaps(hdc, LOGPIXELSY);
      ReleaseDC(nullptr, hdc);
      dpiX = (pxPerInchX > 0) ? pxPerInchX : USER_DEFAULT_SCREEN_DPI;
      dpiY = (pxPerInchY > 0) ? pxPerInchY : USER_DEFAULT_SCREEN_DPI;
    }

    factorX = static_cast<float>(dpiX) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
    factorY = static_cast<float>(dpiY) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
  }
#endif

uint32_t DisplayMonitor::getMonitorDpi(DisplayMonitor::WindowHandle windowHandle) const noexcept {
  return _readDisplayMonitorDpi(this->_handle, windowHandle);
}
void DisplayMonitor::getMonitorContentScale(float& factorX, float& factorY) const noexcept {
  return _readDisplayMonitorContentScale(this->_handle, factorX, factorY);
}
