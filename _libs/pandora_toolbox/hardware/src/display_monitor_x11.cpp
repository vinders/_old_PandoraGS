/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
--------------------------------------------------------------------------------
Description : Display monitor - X11 implementation (Linux/BSD)
*******************************************************************************/
#if !defined(_WINDOWS) && !defined(__APPLE__) && !defined(__ANDROID__) && (defined(__linux__) || defined(__linux) || defined(__unix__) || defined(__unix))
# include <cstdint>
# include <cstdlib>
# include <cstring>
# include <string>
# include <stdexcept>
# include <vector>
# include <climits>
# include <cmath>
# include <unistd.h>
# include <dlfcn.h>
# include <X11/Xlib.h>
# include <X11/extensions/Xrandr.h>
# include <X11/extensions/Xinerama.h> // legacy monitor indices
# include "hardware/display_monitor.h"

  using namespace pandora::hardware;


// -- monitor handle & description/attributes -- -------------------------------

  static bool _readDisplayMonitorAttributes(DisplayMonitor::Handle monitorHandle, DisplayMonitor::Attributes& outAttr) noexcept { return false; }
  static inline void _readPrimaryDisplayMonitorInfo(DisplayMonitor::Handle& outHandle, DisplayMonitor::Attributes& outAttr) {}
  static inline DisplayMonitor::Handle _getDisplayMonitorById(DisplayMonitor::DeviceId id, DisplayMonitor::Attributes* outAttr) noexcept { return 0; }
  static inline bool _listDisplayMonitors(std::vector<DisplayMonitor::Handle>& out) { return false; }


// -- contructors/list -- ------------------------------------------------------

  DisplayMonitor::DisplayMonitor() noexcept {
    _readPrimaryDisplayMonitorInfo(this->_handle, this->_attributes);
  }
  DisplayMonitor::DisplayMonitor(Handle monitorHandle, bool usePrimaryAsDefault)
    : _handle(monitorHandle) {
    if (!this->_handle || !_readDisplayMonitorAttributes(this->_handle, this->_attributes)) {
      if (usePrimaryAsDefault)
        _readPrimaryDisplayMonitorInfo(this->_handle, this->_attributes);
      else
        throw std::invalid_argument("DisplayMonitor: monitor handle is invalid or can't be used.");
    }
  }
  DisplayMonitor::DisplayMonitor(const DisplayMonitor::DeviceId& id, bool usePrimaryAsDefault) {
    this->_handle = _getDisplayMonitorById(id, &(this->_attributes));
    if (!this->_handle) {
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
    if (!this->_handle || !_readDisplayMonitorAttributes(this->_handle, this->_attributes)) {
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


// -- display modes -- ---------------------------------------------------------

  static inline bool _getMonitorDisplayMode(const DisplayMonitor::DeviceId& id, DisplayMode& out) noexcept { return false; }
  DisplayMode DisplayMonitor::getDisplayMode() const noexcept {
    DisplayMode mode;
    if (!_getMonitorDisplayMode(this->_attributes.id, mode)) {
      mode.width = this->_attributes.screenArea.width;
      mode.height = this->_attributes.screenArea.height;
      mode.bitDepth = 32;
      mode.refreshRate = undefinedRefreshRate();
    }
    return mode;
  }
  
  static inline bool _setMonitorDisplayMode(const DisplayMonitor::DeviceId& id, const DisplayMode& mode) noexcept { return false; }
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
  
  static inline bool _setDefaultMonitorDisplayMode(const DisplayMonitor::DeviceId& id) noexcept { return false; }
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

  std::vector<DisplayMode> DisplayMonitor::listAvailableDisplayModes() const noexcept {
    std::vector<DisplayMode> modes;
    //...
    
    if (modes.empty())
      modes.emplace_back(getDisplayMode());
    return modes;
  }


// -- DPI awareness -- ---------------------------------------------------------

  bool DisplayMonitor::setDpiAwareness(bool isEnabled) noexcept {
    return true;
  }

  void DisplayMonitor::getMonitorDpi(DisplayMonitor::WindowHandle windowHandle, uint32_t outDpiX, uint32_t outDpiY) const noexcept {

  }
  uint32_t DisplayMonitor::getBaseDpi() noexcept { return 72u; }


// -- metrics -- ---------------------------------------------------------------

  DisplayArea DisplayMonitor::convertClientAreaToWindowArea(const DisplayArea& clientArea, DisplayMonitor::WindowHandle windowHandle, bool hasMenu, uint32_t nativeStyleFlags) const noexcept {
    return DisplayArea{ 0, 0, 0, 0 };
  }
#endif