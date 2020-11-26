/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
--------------------------------------------------------------------------------
Description : Window management: Win32 implementation (Windows)
*******************************************************************************/
#pragma once

#ifdef _WINDOWS
# include <cstdint>
# include <cstddef>
# include <string>
# include <vector>
# include <system/api/windows_api.h>

# define _WIN_8_1_BLUE_BUILD 9600
# define _WIN_10_0_ANNIVERSARY_BUILD 14393

namespace pandora {
  namespace video {
    
    // -- OS version --
    
    // Verify Windows version on running system
    inline bool _isWindowsVersionGreaterEqual(DWORD major, DWORD minor, DWORD build) noexcept {
      OSVERSIONINFOEXW versionInfo = { sizeof(OSVERSIONINFOEXW), major, minor, build };
      ULONGLONG versionCheck = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
      versionCheck = VerSetConditionMask(versionCheck, VER_MINORVERSION, VER_GREATER_EQUAL);
      versionCheck = VerSetConditionMask(versionCheck, VER_BUILDNUMBER, VER_GREATER_EQUAL);
      return (RtlVerifyVersionInfo(&versionInfo, (VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER), versionCheck) == STATUS_SUCCESS);
    }
    
    
    // -- messages / errors --
    
    // Retrieve and format last error message
    inline std::string _formatLastError(const char prefix[]) noexcept {
      char error[256];
      error[255] = 0;
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 255, nullptr);
      return std::string(prefix) + error;
    }
    
    
    // -- hardware / adapters info --
        
    // List all active display devices on current system
    // output: - CHAR[32] DeviceName:    device name (ID)
    //         - CHAR[128] DeviceString: device context description string
    //         - DWORD StateFlags:       mask: DISPLAY_DEVICE_PRIMARY_DEVICE, DISPLAY_DEVICE_REMOVABLE, DISPLAY_DEVICE_MIRRORING_DRIVER, DISPLAY_DEVICE_VGA_COMPATIBLE
    inline std::vector<DISPLAY_DEVICEW> _listActiveDisplayDevices() noexcept {
      std::vector<DISPLAY_DEVICEW> devices;
      BOOL result = TRUE;
      for (DWORD index = 0; result != FALSE; ++index) {
        devices.emplace_back();
        BOOL result = EnumDisplayDevicesW(nullptr, index, &(devices.back()), 0);
        if (result == FALSE || (StateFlags & DISPLAY_DEVICE_ACTIVE) == 0)
          devices.pop_back();
      }
      return devices;
    }
    // List all available display modes of a display device
    // output: - DWORD dmPelsWidth:          display width (px)
    //         - DWORD dmPelsHeight:         display height (px)
    //         - DWORD dmDisplayFrequency:   refresh frequency (Hz)
    //         - POINTL dmPosition:          coordinates of device in reference to desktop area (px * px)
    //         - DWORD dmDisplayOrientation: values 0 (not configured), DMDO_DEFAULT, DMDO_90, DMDO_180, DMDO_270
    inline std::vector<DEVMODE> _listDisplayModes(LPCWSTR deviceName) noexcept {
      std::vector<DEVMODE> modes;
      BOOL result = TRUE;
      for (DWORD index = 0; result != FALSE; ++index) {
        modes.emplace_back();
        BOOL result = EnumDisplaySettingsExW(deviceName, index, &(modes.back()), 0);
        if (result == FALSE || info.dmPelsWidth == 0u || info.dmPelsHeight == 0u || info.dmPelsWidth == (DWORD)-1)
          modes.pop_back();
      }
      return modes;
    }
    
    // Get current resolution of display device
    // deviceName: set to nullptr to read current display device
    // readFromConfig = true: read original config value / false: read current active value
    // output: - DWORD dmPelsWidth:          display width (px)
    //         - DWORD dmPelsHeight:         display height (px)
    //         - DWORD dmDisplayFrequency:   refresh frequency (Hz)
    //         - POINTL dmPosition:          coordinates of device in reference to desktop area (px * px)
    //         - DWORD dmDisplayOrientation: values 0 (not configured), DMDO_DEFAULT, DMDO_90, DMDO_180, DMDO_270
    inline bool _getDisplayDevice(LPCWSTR deviceName, bool readFromConfig, DEVMODE& outInfo) noexcept {
      return (EnumDisplaySettingsExW(deviceName, readFromConfig ? ENUM_REGISTRY_SETTINGS : ENUM_CURRENT_SETTINGS, &outInfo, 0) != FALSE
             && info.dmPelsWidth > 0u && info.dmPelsHeight > 0u && info.dmPelsWidth != (DWORD)-1);
    }
    // Get current resolution of display device
    // deviceName: set to nullptr to read current display device
    // readFromConfig = true: read original config value / false: read current active value
    inline bool _getDisplayDeviceResolution(LPCWSTR deviceName, bool readFromConfig, uint32_t& outX, uint32_t& outY) noexcept {
      DEVMODE info;
      if (_getDisplayDevice(deviceName, readFromConfig, info)) {
        outX = info.dmPelsWidth;
        outY = info.dmPelsHeight;
        return true;
      }
      return false;
    }
    
    
    // -- hardware / adapters operations --
    
    // Initialize DPI awareness of the process (must be called before using any window or DPI-dependant resource)
    // warning: not supported below Windows 8.1
    inline bool _setDpiAwareness(bool isEnabled) noexcept {
#     if !defined(_P_MIN_WINDOWS_VERSION) || _P_MIN_WINDOWS_VERSION == 8 || _P_MIN_WINDOWS_VERSION == 7 || _P_MIN_WINDOWS_VERSION == 6
        if (_isWindowsVersionGreaterEqual(8, 1, _WIN_8_1_BLUE_BUILD))
          return (SetProcessDpiAwareness(isEnabled ? PROCESS_PER_MONITOR_DPI_AWARE : PROCESS_DPI_UNAWARE) == S_OK);
        else
          return (SetProcessDPIAware() != FALSE);
#     else
        return (SetProcessDpiAwareness(isEnabled ? PROCESS_PER_MONITOR_DPI_AWARE : PROCESS_DPI_UNAWARE) == S_OK);
#     endif
    }
    
    // Get primary monitor handle
    inline HMONITOR _getPrimaryMonitorHandle() noexcept {
      return MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
    }
 
    // Set fullscreen display resolution
    inline bool _setFullscreenDisplayMode(uint32_t width, uint32_t height, uint32_t bitDepth, DWORD refreshRate) noexcept {
      DEVMODEW screenMode;
      ZeroMemory(&screenMode, sizeof(DEVMODE));
      screenMode.dmSize = sizeof(DEVMODE);   
      screenMode.dmPelsWidth = width;
      screenMode.dmPelsHeight = height;    
      screenMode.dmBitsPerPel = bitDepth;
      screenMode.dmDisplayFrequency = refreshRate;
      screenMode.dmFields = (DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY);
      return (ChangeDisplaySettings(&screenMode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL);
    }
    // Reset display resolution to original value
    inline void _resetFullscreenDisplayMode() noexcept {
      ChangeDisplaySettings(nullptr, 0);
    }
    
#   if !defined(_P_MIN_WINDOWS_VERSION) || _P_MIN_WINDOWS_VERSION == 8 || _P_MIN_WINDOWS_VERSION == 7 || _P_MIN_WINDOWS_VERSION == 6
      inline int _getLogicalPixelsPerInch() noexcept {
        HDC hdc = GetDC(nullptr);
        int pixelsPerLogicInch = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(nullptr, hdc);
        return (pixelsPerLogicInch > 0) ? pixelsPerLogicInch : 96;
      }
      // Retro-compatible version of GetSystemMetricsForDpi
      inline int _getSystemMetricsForDpi(int nIndex, UINT dpi) noexcept { // UINT dpi = DpiForWindow(hwnd);
        int value = GetSystemMetrics(nIndex);
        int logPxPerInch = _getLogicalPixelsPerInch();
        return (dpi == logPxPerInch) ? value : MulDiv(value, dpi, logPxPerInch);
      }
#   else
      inline int _getSystemMetricsForDpi(int nIndex, UINT dpi) noexcept { return GetSystemMetricsForDpi(nIndex, dpi); }
#   endif

  }
}
#endif
