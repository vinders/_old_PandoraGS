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
#endif

using namespace pandora::hardware;

#ifdef _WINDOWS
  // --------------------------------------------------------
  // -- Read handles and attributes - Win32 implementation --
  // --------------------------------------------------------

  // initialize DPI awareness of the process (must be called before using any window or DPI-dependant resource)
  static inline bool _enableDpiAwareness() noexcept {
#   if defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
      return (SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2) != FALSE
           || SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE) == S_OK);
#   else
      Win32Libraries& libs = Win32Libraries::instance();
      if (libs.isAtLeastWindows10_RS2() && libs.user32.SetProcessDpiAwarenessContext_)
        return (libs.user32.SetProcessDpiAwarenessContext_(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2) != FALSE);
#     if defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_WINBLUE)
        return (SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE) == S_OK);
#     else
        if (libs.isAtLeastWindows8_1_Blue() && libs.shcore.SetProcessDpiAwareness_)
          return (libs.shcore.SetProcessDpiAwareness_(PROCESS_PER_MONITOR_DPI_AWARE) == S_OK);
        return (SetProcessDPIAware() != FALSE);
#     endif
#   endif
  }

  // -- monitor handles --

  // Get handle of primary monitor
  static inline DisplayMonitor::Handle _getPrimaryDisplayMonitorHandle() noexcept {
    return (DisplayMonitor::Handle)MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
  }

  // Get handle of any monitor - callback
  struct __DisplayMonitorHandleSearch final {
    DisplayMonitor::Id id;
    DisplayMonitor::Handle handle;
  };
  static BOOL CALLBACK _getDisplayMonitorHandleCallback(HMONITOR handle, HDC, RECT*, LPARAM data) {
    MONITORINFOEXW info;
    ZeroMemory(&info, sizeof(info));
    info.cbSize = sizeof(info);

    if (handle != nullptr && GetMonitorInfoW(handle, (MONITORINFO*)&info) != FALSE) {
      __DisplayMonitorHandleSearch* query = (__DisplayMonitorHandleSearch*)data;
      if (query != nullptr && wcscmp(info.szDevice, query->id.c_str()) == 0) {
        query->handle = (DisplayMonitor::Handle)handle;
      }
    }
    return TRUE;
  }
  // Get handle of any monitor
  inline DisplayMonitor::Handle _getDisplayMonitorHandle(DisplayMonitor::Id id) noexcept {
    __DisplayMonitorHandleSearch query{ id, nullptr };
    if (EnumDisplayMonitors(nullptr, nullptr, _getDisplayMonitorHandleCallback, (LPARAM)&query) != FALSE)
      return query.handle;
    return nullptr;
  }

  // List handles of all monitors in use - callback
  static BOOL CALLBACK _listDisplayMonitorsCallback(HMONITOR handle, HDC, RECT*, LPARAM data) {
    std::vector<DisplayMonitor::Handle>* handleList = (std::vector<DisplayMonitor::Handle>*)data;
    if (handle != nullptr && handleList != nullptr)
      handleList->emplace_back((DisplayMonitor::Handle)handle);
    return TRUE;
  }
  // List handles of all monitors in use
  static inline bool _listDisplayMonitors(std::vector<DisplayMonitor::Handle>& out) {
    return (EnumDisplayMonitors(nullptr, nullptr, _listDisplayMonitorsCallback, (LPARAM)&out) != FALSE);
  }


  // -- monitor attributes --

  // read attributes of a monitor handle (id, position/size, primary)
  static inline bool _readDisplayMonitorAttributes(DisplayMonitor::Handle monitorHandle, DisplayMonitor::Attributes& outAttr) noexcept {
    MONITORINFOEXW info;
    ZeroMemory(&info, sizeof(info));
    info.cbSize = sizeof(info);

    if (GetMonitorInfoW((HMONITOR)monitorHandle, (MONITORINFO*)&info) != FALSE) {
      outAttr.id = info.szDevice;
      outAttr.screenArea.x = info.rcMonitor.left;
      outAttr.screenArea.y = info.rcMonitor.top;
      outAttr.screenArea.width  = info.rcMonitor.right - info.rcMonitor.left;
      outAttr.screenArea.height = info.rcMonitor.bottom - info.rcMonitor.top;
      outAttr.workArea.x = info.rcWork.left;
      outAttr.workArea.y = info.rcWork.top;
      outAttr.workArea.width  = info.rcWork.right - info.rcWork.left;
      outAttr.workArea.height = info.rcWork.bottom - info.rcWork.top;
      outAttr.isPrimary = ((info.dwFlags & MONITORINFOF_PRIMARY) != 0);
      return true;
    }
    return false;
  }

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


// ---------------------------
// -- Display monitor class --
// ---------------------------

DisplayMonitor::DisplayMonitor(DisplayMonitor::Handle monitorHandle, bool& isValid)  noexcept
  : _handle(monitorHandle) {
  isValid = (this->_handle != nullptr && _readDisplayMonitorAttributes(this->_handle, this->_attributes));
}

// -- factory --

bool DisplayMonitor::createPrimaryMonitor(DisplayMonitor& out) noexcept {
  bool isValid = true;
  out = DisplayMonitor(_getPrimaryDisplayMonitorHandle(), isValid);
  return isValid;
}
bool DisplayMonitor::createMonitorById(DisplayMonitor::Id id, DisplayMonitor& out) noexcept {
  bool isValid = true;
  out = DisplayMonitor(_getDisplayMonitorHandle(id), isValid);
  return isValid;
}
bool DisplayMonitor::createMonitorByIndex(uint32_t index, DisplayMonitor& out) noexcept {
  std::vector<DisplayMonitor::Handle> handles;
  if (!_listDisplayMonitors(handles) || handles.empty())
    handles.emplace_back(_getPrimaryDisplayMonitorHandle());

  if (index < handles.size()) {
    bool isValid = true;
    out = DisplayMonitor(handles[index], isValid);
    return isValid;
  }
  else
    return false;
}

std::vector<DisplayMonitor> DisplayMonitor::listAvailableMonitors() noexcept {
  std::vector<DisplayMonitor::Handle> handles;
  if (!_listDisplayMonitors(handles) || handles.empty())
    handles.emplace_back(_getPrimaryDisplayMonitorHandle());

  bool isValid = true;
  std::vector<DisplayMonitor> monitors;
  for (auto it : handles) {
    monitors.emplace_back(it, isValid);
    if (!isValid)
      monitors.pop_back();
  }
  return monitors;
}

// -- accessors --

uint32_t DisplayMonitor::getMonitorDpi(DisplayMonitor::WindowHandle windowHandle) const noexcept {
  return _readDisplayMonitorDpi(this->_handle, windowHandle);
}
void DisplayMonitor::getMonitorContentScale(float& factorX, float& factorY) const noexcept {
  return _readDisplayMonitorContentScale(this->_handle, factorX, factorY);
}

// -- utilities --

bool DisplayMonitor::enableDpiAwareness() noexcept {
  return _enableDpiAwareness();
}

DisplayMonitor::Handle DisplayMonitor::getPrimaryMonitorHandle() noexcept {
  return _getPrimaryDisplayMonitorHandle();
}
